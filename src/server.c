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
#include "net.h"
#include "module.h"
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
static void on_recv_package(JSocket * sock,
                            const void *data,
                            unsigned int len,
                            JSocketRecvResultType type, void *user_data);
static void on_send_package(JSocket * sock, int res, void *user_data);
static int on_accept_client(JSocket * listen, JSocket * client,
                            void *data);

/*
 * the log function for modules
 */
static void jac_server_module_log(JLogLevel level, const char *fmt,
                                  va_list ap);

const char *jac_server_get_conf_virtualserver_name(JConfNode * vs)
{
    JConfData *name_data = j_conf_node_get_argument_first(vs);
    if (name_data) {
        return j_conf_data_get_raw(name_data);
    }
    return _("unnamed");
}

int jac_server_check_conf_virtualserver(JConfNode * vs)
{
    int port = jac_config_get_integer(vs, LISTEN_PORT_DIRECTIVE, -1);
    if (port <= 0 || port > 65535) {
        printf(_("\033[31minvalid argument of %s in %s\033[0m\n"),
               LISTEN_PORT_DIRECTIVE,
               jac_server_get_conf_virtualserver_name(vs));
        return 0;
    }
    return 1;
}

static JacServer *running_server = NULL;

/*
 * 初始化服务进程
 */
static inline void jac_server_init(JacServer * server,
                                   JConfNode * root,
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
    j_mod_set_log_func(jac_server_module_log);
    if (!jac_load_modules_from_scope(vs)) { /* 载入服务内的模块 */
        jac_server_warning(server,
                           _
                           ("error occurs while loading server specified modules"));
    }

    signal(SIGINT, signal_handler);
    set_proctitle(NULL, "jacques: server %s", jac_server_get_name(server));

    jac_server_info(server, _("starts!"));
}

/*
 * Callback of accepting connections
 * 接受到客户端连接
 */
static int on_accept_client(JSocket * listen, JSocket * client, void *data)
{
    JacServer *server = (JacServer *) data;
    if (client) {
        jac_server_info(server, "client %s accepted!",
                        j_socket_get_peer_name(client));
        j_socket_recv_package(client, on_recv_package, server);
    } else {
        jac_server_warning(server, "accept error");
    }
    return 1;
}

/*
 * 收到客户端数据
 */
static void on_recv_package(JSocket * sock,
                            const void *data,
                            unsigned int len,
                            JSocketRecvResultType type, void *user_data)
{
    JacServer *server = (JacServer *) user_data;

    if (type == J_SOCKET_RECV_ERR) {    /* 没有数据接受到或者出现了错误 */
        jac_server_info(server, "receive ERROR from %s",
                        j_socket_get_peer_name(sock));
        j_socket_close(sock);
        return;
    } else if (data == NULL || len == 0 || type == J_SOCKET_RECV_EOF) { /* 有数据接受到，但是客户端关闭了链接 */
        jac_server_info(server, "receive data with EOF from %s",
                        j_socket_get_peer_name(sock));
        j_socket_close(sock);
        return;
    }
    jac_server_info(server, "received data from %s; echoing",
                    j_socket_get_peer_name(sock));
    j_socket_send_package(sock, on_send_package, data, len, user_data);
}

/*
 * 给客户端发送数据
 */
static void on_send_package(JSocket * sock, int res, void *user_data)
{
    JacServer *server = (JacServer *) user_data;
    if (res) {
        j_socket_recv_package(sock, on_recv_package, user_data);
    } else {
        j_socket_close(sock);
        jac_server_warning(server, "send data to %s error",
                           j_socket_get_peer_name(sock));
    }
}

/*
 * The main loop of server
 * Server listens on the specified port and handle connections from client
 */
static inline void jac_server_main(JacServer * server)
{
    JSocket *listen_sock = jac_server_get_sock(server);

    /* main loop */
    j_socket_accept_async(listen_sock, on_accept_client, server);
    j_main();
    jac_server_end(server);
}

/*
 * Gets the log location of server
 */
static inline const char *jac_server_get_conf_log(JConfNode * root,
                                                  JConfNode * vs)
{
    const char *normal = jac_config_get_string(vs, JAC_LOG_DIRECTIVE,
                                               NULL);
    if (normal) {
        return normal;
    }
    normal = jac_config_get_string(root, JAC_LOG_DIRECTIVE, LOG_FILEPATH);
    return normal;
}


/*
 * Gets the error log location of server
 */
static inline const char *jac_server_get_conf_err_log(JConfNode * root,
                                                      JConfNode * vs)
{
    const char *error = jac_config_get_string(vs, JAC_ERROR_LOG_DIRECTIVE,
                                              NULL);
    if (error) {
        return error;
    }
    return jac_config_get_string(root, JAC_ERROR_LOG_DIRECTIVE,
                                 ERR_FILEPATH);
}


/*
 * 启动服务进程 Starts the server process
 */
JacServer *jac_server_start_from_conf(JConfNode * root, JConfNode * vs)
{
    int port = jac_config_get_integer(vs, LISTEN_PORT_DIRECTIVE, -1);
    if (port <= 0 || port > 65535) {
        return NULL;
    }
    const char *normal = jac_server_get_conf_log(root, vs);
    const char *error = jac_server_get_conf_err_log(root, vs);
    const char *name = jac_server_get_conf_virtualserver_name(vs);

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

static void signal_handler(int signum)
{
    if (running_server == NULL) {
        return;
    }
    if (signum == SIGINT) {
        jac_server_end(running_server);
    }
}


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
