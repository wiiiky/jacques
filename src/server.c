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


/* 创建服务，失败返回NULL */
static inline Server *create_server(const jchar *name,JConfObject *root, JConfObject *obj, const CLOption *option);
/* 服务的执行函数 */
static inline void run_server(Server *server);

/* 日志记录函数 */
static void server_log_handler(const jchar *domain, JLogLevelFlag level,
                               const jchar *message, jpointer user_data);
#define server_debug(server, ...) j_log(server->name, J_LOG_LEVEL_DEBUG, __VA_ARGS__)
#define server_info(server, ...) j_log(server->name, J_LOG_LEVEL_INFO, __VA_ARGS__)
#define server_warning(server, ...) j_log(server->name, J_LOG_LEVEL_WARNING, __VA_ARGS__)
#define server_error(server, ...) j_log(server->name, J_LOG_LEVEL_ERROR, __VA_ARGS__)

static void stop_server(Server *server);

/* 读取配置，创建一个服务进程 */
static inline Server *create_server(const jchar *name,JConfObject *root, JConfObject *obj, const CLOption *option) {
    jint64 port = j_conf_object_get_integer(obj, "port", 0);
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
    server->user=load_user(root, obj);
    server->log=load_log(root, obj, CONFIG_KEY_LOG, DEFAULT_LOG);
    server->error_log=load_log(root, obj, CONFIG_KEY_ERROR_LOG, DEFAULT_ERROR_LOG);
    server->log_level=load_loglevel(root, obj);
    server->logfd=append_file(server->log);
    server->error_logfd=append_file(server->error_log);
    server->socket=NULL;

    server->mod_paths=load_modules(obj);

    return server;
}

JList *load_servers(JConfRoot *root, const CLOption *option) {
    JList *servers=NULL;
    JList *keys=j_conf_object_lookup((JConfObject*)root,
                                     "^server-[[:alpha:]][[:alnum:]]*",
                                     J_CONF_NODE_TYPE_OBJECT);
    JList *ptr=keys;
    while(ptr) {
        const jchar *key=(const jchar*)j_list_data(ptr);
        Server *server=create_server(key+7,(JConfObject*)root, j_conf_object_get((JConfObject*)root, key), option);
        if(server) {
            servers=j_list_append(servers, server);
        }
        ptr=j_list_next(ptr);
    }
    j_list_free(keys);

    return servers;
}

jboolean start_server(Server *server) {
    server->pid=fork();
    if(server->pid<0) {
        return FALSE;
    } else if(server->pid>0) {
        /* 主进程 */
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
        j_printf("          %s\n", (jchar*)j_list_data(ptr));
        ptr=j_list_next(ptr);
    }
    j_printf("\033[0m\n");
}

static inline jboolean server_accept(JSocket *socket, JSocket *cli, jpointer user_data);
static inline jboolean server_receive(JSocket *socket, const jchar *buffer, jint size, jpointer user_data);
static inline void server_send(JSocket *socket, jint ret, jpointer user_data);

/* 进入服务器主循环 */
static inline void run_server(Server *server) {
    j_log_set_handler(server->name, server->log_level, server_log_handler, server);

    if(!setuser(server->user)) {
        server_error(server, "fail to set the process effective user '%s'", server->user);
        exit(1);
    }
    server->socket=socket_listen("127.0.0.1", server->port);
    if(server->socket==NULL) {
        server_error(server, "unable to listen on port %d", server->port);
        exit(1);
    }
    server_info(server, "listen on port %d", server->port);
    j_socket_accept_async(server->socket, server_accept, server);
    j_main();
    server_info(server, "exit");
}

static inline jboolean server_accept(JSocket *socket, JSocket *cli, jpointer user_data) {
    Server *server=(Server*)user_data;
    if(cli==NULL) {
        server_error(server, "socket accept error");
        return FALSE;
    }
    server_info(server, "establish connection with %s", j_socket_get_remote_address_string(cli));
    j_socket_receive_async(cli, server_receive, server);
    j_socket_unref(cli);
    return TRUE;
}

static inline jboolean server_receive(JSocket *socket, const jchar *buffer, jint size, jpointer user_data) {
    Server *server=(Server*)user_data;
    if(size<=0) {
        server_info(server, "client %s closed", j_socket_get_remote_address_string(socket));
        return FALSE;
    }
    j_socket_send_async(socket, buffer, size,server_send ,server);
    return TRUE;
}

static inline void server_send(JSocket *socket, jint ret, jpointer user_data) {
    Server *server=(Server*)user_data;
    server_debug(server, "%d bytes sent to %s", ret, j_socket_get_remote_address_string(socket));
}


/* 日志处理函数 */
static void server_log_handler(const jchar *domain, JLogLevelFlag level, const jchar *message, jpointer user_data) {
    Server *server=(Server*)user_data;
    log_internal(server->name, message, level, server->logfd, server->error_logfd);
}
