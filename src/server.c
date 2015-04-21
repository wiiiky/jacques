/*
 * Copyright (C) 2015 Wiky L <wiiiky@outlook.com>
 *
 * jacques is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jacques is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>."
 */

#include "server.h"
#include "i18n.h"
#include "utils.h"
#include "module.h"
#include "hook.h"
#include <jlib/jlib.h>
#include <jio/jio.h>
#include <jmod/jmod.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>


/* 日志输出函数 */
static inline void jac_server_log(JacServer * server, JLogLevel level,
                                  const char *fmt, ...);
#define jac_server_info(server,fmt,...) jac_server_log(server,J_LOG_LEVEL_INFO,fmt,##__VA_ARGS__)
#define jac_server_warning(server,fmt,...) jac_server_log(server,J_LOG_LEVEL_WARNING,fmt,##__VA_ARGS__)
#define jac_server_error(server,fmt,...) jac_server_log(server,J_LOG_LEVEL_ERROR,fmt,##__VA_ARGS__)
#define jac_server_debug(server,fmt,...) jac_server_log(server,J_LOG_LEVEL_DEBUG,fmt,##__VA_ARGS__)

static void signal_handler(int signum);


/*
 * the log function for modules
 */
static void jac_server_module_log(JLogLevel level, const char *fmt,
                                  va_list ap);

/*
 * 载入服务内模块，并完成模块的配置解析
 */
static inline void jac_server_load_modules(JacServer * server,
                                           JConfRoot * root,
                                           JConfNode * vs);

static JacServer *running_server = NULL;

/*
 * 初始化服务进程
 */
static inline void jac_server_init(JacServer * server,
                                   JConfRoot * root,
                                   JConfNode * vs,
                                   const char *normal, const char *error)
{
    server->pid = getpid();
    server->custom_logger = j_logger_open(normal, "%0 [%l]: %m");
    server->error_logger = j_logger_open(error, "%0 [%l]: %m");
    server->listen_sock = j_socket_listen_on(jac_server_get_port(server),
                                             1024);

    if (server->listen_sock == NULL ||
        !j_socket_set_blocking(server->listen_sock, 0)) {
        jac_server_error(server,
                         _("unable to listen on port %u"),
                         jac_server_get_port(server));
        jac_server_end(server);
    } else if (!set_procuser(JACQUES_USER, JACQUES_GROUP)) {
        jac_server_error(server,
                         _("unable to set user/group %s/%s"),
                         JACQUES_USER, JACQUES_GROUP);
        jac_server_end(server);
    }

    running_server = server;

    /* 载入服务内模块 */
    jac_server_load_modules(server, root, vs);

    signal(SIGINT, signal_handler);
    set_proctitle(NULL, "jacques: server %s", jac_server_get_name(server));

    jac_server_info(server, _("starts!"));
}

/*
 * Callback of accepting connections
 * 接受到客户端连接
 */
void on_accept_connection(JSocket * listen, JSocket * conn, void *data)
{
    jac_accept_hooks(conn, data);
}

/*
 * 收到客户端数据
 */
void on_recv_package(JSocket * sock,
                     const void *data, unsigned int len, void *user_data)
{
    JacServer *server = (JacServer *) user_data;
    jac_recv_hooks(sock, data, len, server);
}

void on_recv_package_error(JSocket * sock,
                           const void *data,
                           unsigned int len, void *user_data)
{
    jac_recv_error_hooks(sock, data, len);
}

/*
 * 给客户端发送数据
 */
void on_send_package(JSocket * sock, const char *data,
                     unsigned int count, void *user_data)
{
    JacServer *server = (JacServer *) user_data;
    jac_send_hooks(sock, data, count, server);
}

void on_send_package_error(JSocket * sock, const char *data,
                           unsigned int count,
                           unsigned int len, void *user_data)
{
    jac_send_error_hooks(sock, data, count, len);
}

void on_keep_socket(JSocket * conn, void *user_data)
{
    j_socket_close(conn);
}

/*
 * The main loop of server
 * Server listens on the specified port and handle connections from client
 */
static inline void jac_server_main(JacServer * server)
{
    JSocket *listen_sock = jac_server_get_sock(server);

    /* main loop */
    j_socket_accept_async(listen_sock, on_accept_connection, server);
    j_main();
    jac_server_end(server);
}

/*
 * 启动服务进程 Starts the server process
 */
JacServer *jac_server_start_from_conf(JConfRoot * root, JConfNode * vs)
{
    int64_t port = jac_config_get_int(root, vs, DIRECTIVE_LISTEN_PORT, -1);
    if (port <= 0 || port > 65535) {
        return NULL;
    }
    const char *normal = jac_config_get_string(root, vs,
                                               DIRECTIVE_LOGFILE,
                                               LOG_FILEPATH);
    const char *error = jac_config_get_string(root, vs,
                                              DIRECTIVE_ERROR_LOGFILE,
                                              ERR_FILEPATH);
    const char *name = jac_config_get_string(root, vs,
                                             DIRECTIVE_SERVER_NAME,
                                             _("unnamed"));

    int pid = fork();
    if (pid < 0) {
        return NULL;
    }
    JacServer *server = (JacServer *) j_malloc(sizeof(JacServer));
    server->name = j_strdup(name);
    server->listen_port = port;
    server->listen_sock = NULL;
    server->custom_logger = NULL;
    server->error_logger = NULL;
    if (pid == 0) {
        jac_server_init(server, root, vs, normal, error);
        jac_server_main(server);
    }
    server->pid = pid;
    return server;
}

void jac_server_free(JacServer * server)
{
    j_logger_close(server->custom_logger);
    j_logger_close(server->error_logger);
    if (server->listen_sock) {
        j_socket_close(server->listen_sock);
    }
    j_free(server->name);
    j_free(server);
}

/*
 * Server quits
 */
void jac_server_end(JacServer * server)
{
    jac_server_info(server, _("quits"));
    jac_server_free(server);
    exit(0);
}

/*
 * 处理信号
 */
static void signal_handler(int signum)
{
    if (running_server == NULL) {
        return;
    }
    if (signum == SIGINT) {
        jac_server_end(running_server);
    }
}


/*
 * 服务进程的日志函数
 */
static inline void jac_server_log(JacServer * server, JLogLevel level,
                                  const char *fmt, ...)
{
    JLogger *logger = server->custom_logger;
    JLogger *error = server->error_logger;
    char buf[1024];
    snprintf(buf, sizeof(buf) / sizeof(char), "SERVER %s: %s",
             jac_server_get_name(server), fmt);
    va_list ap;
    va_start(ap, fmt);
    switch (level) {
    case J_LOG_LEVEL_INFO:
        j_logger_vlog(logger, J_LOG_LEVEL_INFO, buf, ap);
        break;
    case J_LOG_LEVEL_WARNING:
        j_logger_vlog(logger, J_LOG_LEVEL_WARNING, buf, ap);
        break;
    case J_LOG_LEVEL_ERROR:
        j_logger_vlog(error, J_LOG_LEVEL_ERROR, buf, ap);
        break;
    default:
        j_logger_vlog(logger, J_LOG_LEVEL_DEBUG, buf, ap);
    }
    va_end(ap);
}

/*
 *  由模块调用的日志记录函数
 */
static void jac_server_module_log(JLogLevel level, const char *fmt,
                                  va_list ap)
{
    if (running_server == NULL) {
        return;
    }
    JLogger *logger = running_server->custom_logger;
    JLogger *error = running_server->error_logger;
    switch (level) {
    case J_LOG_LEVEL_INFO:
        j_logger_vlog(logger, J_LOG_LEVEL_INFO, fmt, ap);
        break;
    case J_LOG_LEVEL_WARNING:
        j_logger_vlog(logger, J_LOG_LEVEL_WARNING, fmt, ap);
        break;
    case J_LOG_LEVEL_ERROR:
        j_logger_vlog(error, J_LOG_LEVEL_ERROR, fmt, ap);
        break;
    default:
        j_logger_vlog(logger, J_LOG_LEVEL_DEBUG, fmt, ap);
    }
}



static inline void jac_server_load_modules(JacServer * server,
                                           JConfRoot * root,
                                           JConfNode * vs)
{
    j_mod_set_log_func(jac_server_module_log);
    jac_load_modules_from_node(root, vs);
}
