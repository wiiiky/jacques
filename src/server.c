/*
 * Copyright (C) 2015 Wiky L
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */
#include "server.h"
#include "utils.h"
#include "socket.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <jmod/jmod.h>


/*
 * 全局变量，服务启动后生效
 */
static Server *g_server = NULL;


/* 创建服务，失败返回NULL */
static inline Server *create_server(const char *name,JConfObject *root, JConfObject *obj, const CLOption *option);

static inline void init_server(Server *server);
/* 服务的执行函数 */
static inline void run_server(Server *server);

/* 调用相关的回调函数 */
static inline void handle_init(const char *name, const char *host, unsigned short port);
static inline void handle_accept(JSocket *socket);
static inline void handle_recv(JSocket *socket, const char *buf, int size, void *user_data);
static inline void handle_send(JSocket *socket, int size, void *user_data);

/* 日志记录函数 */
static void server_log_handler(const char *domain,
                               JLogLevelFlag level,
                               const char *message,
                               void * user_data);
/* 信号处理函数 */
static void signal_handler(int signo);
#define server_debug(...) j_log(g_server->name, J_LOG_LEVEL_DEBUG, __VA_ARGS__)
#define server_info(...) j_log(g_server->name, J_LOG_LEVEL_INFO, __VA_ARGS__)
#define server_warning(...) j_log(g_server->name, J_LOG_LEVEL_WARNING, __VA_ARGS__)
#define server_error(...) j_log(g_server->name, J_LOG_LEVEL_ERROR, __VA_ARGS__)

static void stop_server(Server *server);

/* 读取配置，创建一个服务进程 */
static inline Server *create_server(const char *name,JConfObject *root, JConfObject *obj, const CLOption *option) {
    int64_t port = j_conf_object_get_integer(obj, "port", 0);
    if(port<=0||port>65536) {
        j_fprintf(stderr, "invalid port in server %s\n", name);
        return NULL;
    }
    /* 日志路径无效依然可以创建服务，只是日志功能失效 */
    Server *server=(Server*)j_malloc(sizeof(Server));
    J_OBJECT_INIT(server, stop_server);
    server->name=j_strdup(name);
    server->port=port;
    server->pid=-1;
    server->user=extract_user(root, obj);
    server->host = extract_host(root, obj);
    server->log=extract_log(root, obj, CONFIG_KEY_LOG, DEFAULT_LOG);
    server->error_log=extract_log(root, obj, CONFIG_KEY_ERROR_LOG, DEFAULT_ERROR_LOG);
    server->log_level=extract_loglevel(root, obj);
    server->logfd=-1;
    server->error_logfd=-1;
    server->socket=NULL;

    server->mod_paths=extract_modules(obj);

    return server;
}

JList *load_servers(JConfRoot *root, const CLOption *option) {
    JList *servers=NULL;
    JList *keys=j_conf_object_lookup((JConfObject*)root,
                                     "^server-[[:alpha:]][[:alnum:]]*",
                                     J_CONF_NODE_TYPE_OBJECT);
    JList *ptr=keys;
    while(ptr) {
        const char *key=(const char*)j_list_data(ptr);
        Server *server=create_server(key+7,(JConfObject*)root, j_conf_object_get((JConfObject*)root, key), option);
        if(server) {
            servers=j_list_append(servers, server);
        }
        ptr=j_list_next(ptr);
    }
    j_list_free(keys);

    return servers;
}

boolean start_server(Server *server) {
    server->pid=fork();
    if(server->pid<0) {
        return FALSE;
    } else if(server->pid>0) {
        /* 主进程 master */
        return TRUE;
    }
    /* 执行服务 */
    run_server(server);
    exit(0);
}

/* 关闭一个服务进程 */
static void stop_server(Server *server) {
    j_free(server->name);
    j_free(server->log);
    j_free(server->error_log);
    j_free(server->user);
    j_free(server->host);
    j_list_free_full(server->mod_paths, j_free);
    close(server->logfd);
    close(server->error_logfd);
    if(server->pid>0) {
        kill(server->pid, SIGINT);
    }
    if(server->socket) {
        j_socket_unref(server->socket);
    }
}

static inline boolean server_accept_cb(JSocket *socket, JSocket *cli, void * user_data);
static inline boolean server_recv_cb(JSocket *socket, const void *buffer, int size, void * user_data);
static inline void server_send_cb(JSocket *socket, int ret, void * user_data);

static inline void server_send_internal(JSocket *socket, const void *buf, unsigned int size, void *user_data);
static inline void server_send_multi_internal(const void *buf, unsigned int size, void *user_data, JacSocketFilter filter, void *filter_data);

/* 进入服务器主循环 */
static inline void run_server(Server *server) {
    init_server(server);

    server_info("listen on port %d", server->port);
    j_socket_accept_async(server->socket, server_accept_cb, NULL);
    j_main();
    server_info("exit");
}

/* 初始化服务进程 */
static inline void init_server(Server *server) {
    g_server=server;
    server->logfd=create_or_append(server->log);
    server->error_logfd=create_or_append(server->error_log);
    j_log_set_handler(server->name, server->log_level, server_log_handler, server);

    /* 载入模块 */
    JList *ptr=server->mod_paths;
    while(ptr) {
        const char *path=(const char*)j_list_data(ptr);
        JacModule *mod=jac_loads_module(path);
        if(mod==NULL) {
            server_error("fail to load module %s", path);
            exit(1);
        }
        server_info("load module %s successfully", mod->name);
        ptr=j_list_next(ptr);
    }

    if(!setuser_by_name(server->user)) {
        server_error("fail to set the process effective user '%s'", server->user);
        exit(1);
    }
    if((server->socket=socket_listen(server->host, server->port))==NULL) {
        server_error("unable to listen on port %d", server->port);
        exit(1);
    }

    signal(SIGINT, signal_handler);

    register_jac_send(server_send_internal);
    register_jac_send_multi(server_send_multi_internal);

    handle_init(server->name, server->host, server->port);
}

/* 调用服务初始化的回调函数 */
static inline void handle_init(const char *name, const char *host, unsigned short port) {
    JList *ptr=get_server_init_hooks();
    while(ptr) {
        ServerInit func=(ServerInit)j_list_data(ptr);
        func(name, host, port);
        ptr=j_list_next(ptr);
    }
}

static inline boolean server_accept_cb(JSocket *socket, JSocket *cli, void * user_data) {
    if(cli==NULL) {
        server_error("socket accept error");
        return FALSE;
    }
    server_info("establish connection with %s", j_socket_get_remote_address_string(cli));
    j_socket_receive_async(cli, server_recv_cb, NULL);
    j_socket_unref(cli);
    handle_accept(cli);
    return TRUE;
}

/* 调用接收链接的回调函数 */
static inline void handle_accept(JSocket *client) {
    JList *ptr = get_client_accept_hooks();
    while(ptr) {
        AcceptClient func=(AcceptClient)j_list_data(ptr);
        func(client);
        ptr=j_list_next(ptr);
    }
}

static inline boolean server_recv_cb(JSocket *socket, const void *buffer, int size, void * user_data) {
    handle_recv(socket, buffer, size, user_data);
    if(size<=0) {
        server_info("client %s closed", j_socket_get_remote_address_string(socket));
        return FALSE;
    }
    return TRUE;
}

static inline void server_send_internal(JSocket *socket, const void *buf, unsigned int size, void *user_data) {
    j_socket_send_async(socket, buf, size, server_send_cb, user_data);
}

static inline void server_send_multi_internal(const void *buf, unsigned int size, void *user_data, JacSocketFilter filter, void *filter_data) {
    JList *sockets=j_main_get_sources("SocketSource");
    JList *ptr=sockets;
    printf("multi_internal, %d\n",j_list_length(ptr));
    while(ptr) {
        JSocketSource *src = (JSocketSource*)j_list_data(ptr);
        JSocket *socket=j_socket_source_get_socket(src);
        if(filter(socket, filter_data)) {
            server_send_internal(socket, buf, size, user_data);
        }
        ptr=j_list_next(ptr);
    }
    j_list_free(sockets);
}

/* 收到数据的回调函数 */
static inline void handle_recv(JSocket *socket, const char *buf, int size, void *user_data) {
    JList *ptr=get_client_recv_hooks();
    while(ptr) {
        RecvClient func=(RecvClient)j_list_data(ptr);
        func(socket, buf, size, user_data);
        ptr=j_list_next(ptr);
    }
}


static inline void server_send_cb(JSocket *socket, int ret, void * user_data) {
    handle_send(socket, ret, user_data);
}

/* 数据发送完成的回调函数 */
static inline void handle_send(JSocket *socket, int size, void *user_data) {
    JList *ptr=get_client_send_hooks();
    while(ptr) {
        SendClient func=(SendClient)j_list_data(ptr);
        func(socket, size, user_data);
        ptr=j_list_next(ptr);
    }
}


/* 日志处理函数 */
static void server_log_handler(const char *domain, JLogLevelFlag level, const char *message, void * user_data) {
    Server *server=(Server*)user_data;
    log_common(server->name, message, level, server->logfd, server->error_logfd);
}

static void signal_handler(int signo) {
    if(signo==SIGINT) {
        j_main_try_quit();
    }
    j_printf("server %d quit!\n",getpid());
}

/* 在终端输出服务的配置信息 */
void dump_server(Server *server) {
    j_printf("\033[34m%s\033[0m listens on port \033[32m%u\033[0m\n", server->name, server->port);
    j_printf("  log: \033[32m%s\033[0m\n",server->log);
    j_printf("  error_log: \033[32m%s\033[0m\n",server->error_log);
    j_printf("  log_level: ");
    if(server->log_level&J_LOG_LEVEL_INFO) {
        j_printf("\033[32mINFO\033[0m|");
    }
    if(server->log_level&J_LOG_LEVEL_DEBUG) {
        j_printf("\033[32mDEBUG\033[0m|");
    }
    if(server->log_level&J_LOG_LEVEL_ERROR) {
        j_printf("\033[32mERROR\033[0m|");
    }
    if(server->log_level&J_LOG_LEVEL_WARNING) {
        j_printf("\033[32mWARNING\033[0m");
    }
    j_printf("\n");
    j_printf("  user: \033[32m%s\033[0m\n", server->user);
    j_printf("  modules: \033[32m\n");
    JList *ptr=server->mod_paths;
    while(ptr) {
        j_printf("          %s\n", (char*)j_list_data(ptr));
        ptr=j_list_next(ptr);
    }
    j_printf("\033[0m\n");
}
