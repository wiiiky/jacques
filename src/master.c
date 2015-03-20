/*
 * main.c
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */

#include "master.h"
#include "utils.h"
#include "module.h"
#include "i18n.h"
#include "server.h"
#include <jlib/jlib.h>
#include <jmod/jmod.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <errno.h>


static JacMaster *running_master = NULL;


/*
 * 日志输出
 */
static inline void jac_master_log(JacMaster * master, JLogLevel level,
                                  const char *fmt, ...);
#define jac_master_info(master,fmt,...) \
            jac_master_log(master,J_LOG_LEVEL_INFO,fmt,##__VA_ARGS__)
#define jac_master_debug(master,fmt,...) \
            jac_master_log(master,J_LOG_LEVEL_DEBUG,fmt,##__VA_ARGS__)
#define jac_master_warning(master,fmt,...) \
            jac_master_log(master,J_LOG_LEVEL_WARNING,fmt,##__VA_ARGS__)
#define jac_master_error(master,fmt,...) \
            jac_master_log(master,J_LOG_LEVEL_ERROR,fmt,##__VA_ARGS__)

static void jac_master_module_log(JLogLevel level, const char *fmt,
                                  va_list ap);

static inline void jac_master_fork_servers(JacMaster * master);
static void signal_handler(int signum);


JacMaster *jac_master_start(JConfParser * cfg)
{
    JConfNode *root = j_conf_parser_get_root(cfg);

    const char *normal = jac_config_get_string(root,
                                               JAC_LOG_DIRECTIVE,
                                               LOG_FILEPATH);
    if (normal == NULL) {
        printf(_("invalid argument of %s\n"), JAC_LOG_DIRECTIVE);
        return NULL;
    }
    const char *error =
        jac_config_get_string(root, JAC_ERROR_LOG_DIRECTIVE,
                              ERR_FILEPATH);
    if (error == NULL) {
        printf(_("invalid argument of %s\n"), JAC_ERROR_LOG_DIRECTIVE);
        return NULL;
    }

    /* daemonize */
    int pid = jac_daemonize();

    JLogger *custom_logger = j_logger_open(normal, "%0 [%l]: %m");
    JLogger *error_logger = j_logger_open(error, "%0 [%l]: %m");

    if (!pid) {
        j_logger_error(error_logger, _("fail to daemonize"));
        goto ERROR;
    }

    if (!jac_save_pid(pid)) {
        j_logger_error(error_logger,
                       _("fail to start, is it already running?"));
        goto ERROR;
    }

    JacMaster *master = (JacMaster *) j_malloc(sizeof(JacMaster));
    master->pid = pid;
    master->cfg = cfg;
    master->custom_logger = custom_logger;
    master->error_logger = error_logger;
    master->servers = NULL;
    master->running = 1;
    running_master = master;

    /* 设置模块的输出函数，并载入模块 */
    j_mod_set_log_func(jac_master_module_log);
    if (!jac_load_modules(cfg)) {
        jac_master_warning(master,
                           _("error occurs while loading modules"));
    }

    jac_master_info(master, _("starts"));

    set_proctitle(NULL, "jacques: master");

    jac_master_fork_servers(master);

    return master;
  ERROR:
    j_logger_close(custom_logger);
    j_logger_close(error_logger);
    return NULL;
}

/*
 * 创建服务进程
 */
static inline void jac_master_fork_servers(JacMaster * master)
{
    JConfNode *root = j_conf_parser_get_root(master->cfg);
    JList *virtualservers =
        j_conf_node_get_scope(root, JAC_VIRTUAL_SERVER_SCOPE);
    JList *ptr = virtualservers;
    while (ptr) {
        JConfNode *vs = (JConfNode *) j_list_data(ptr);
        JacServer *server = jac_server_start_from_conf(root, vs);
        if (server) {
            master->servers = j_list_append(master->servers, server);
        } else {
            jac_master_error(master,
                             _
                             ("unable to start SERVER %s. Check configuration!"),
                             jac_server_get_conf_virtualserver_name(vs));
        }
        ptr = j_list_next(ptr);
    }
    j_list_free(virtualservers);
}


/*
 * 主进程等待信号的发生，
 * 可能是管理员发送了相关的控制信号，而可能是服务进程结束产生的信号
 * FIXME
 */
void jac_master_wait(JacMaster * master)
{
    signal(SIGINT, signal_handler);
    while (master->running) {
        if (wait(NULL) < 0 && errno == ECHILD) {
            break;
        }
    }
    jac_master_quit(master);
}

/*
 * 信号处理函数
 */
static void signal_handler(int signum)
{
    if (running_master == NULL) {
        return;
    }
    JList *ptr = running_master->servers;
    while (ptr) {
        JacServer *server = (JacServer *) j_list_data(ptr);
        kill(server->pid, SIGINT);  /* Sends signal SIGINT to server */
        ptr = j_list_next(ptr);
    }
    while (wait(NULL) > 0);
    running_master->running = 0;
}

/*
 * 主进程退出，在此之前做一些清理
 */
void jac_master_quit(JacMaster * master)
{
    jac_master_info(master, _("quits"));
    j_conf_parser_free(master->cfg);
    j_logger_close(master->custom_logger);
    j_logger_close(master->error_logger);
    j_list_free_full(master->servers, (JListDestroy) jac_server_free);
    j_free(master);
    exit(0);
}

static inline void jac_master_log(JacMaster * master, JLogLevel level,
                                  const char *fmt, ...)
{
    JLogger *normal = master->custom_logger;
    JLogger *error = master->error_logger;

    char buf[1024];
    snprintf(buf, sizeof(buf) / sizeof(char), "MASTER: %s", fmt);
    va_list ap;
    va_start(ap, fmt);
    switch (level) {
    case J_LOG_LEVEL_INFO:
        j_logger_vlog(normal, J_LOG_LEVEL_INFO, buf, ap);
        break;
    case J_LOG_LEVEL_WARNING:
        j_logger_vlog(normal, J_LOG_LEVEL_WARNING, buf, ap);
        break;
    case J_LOG_LEVEL_ERROR:
        j_logger_vlog(error, J_LOG_LEVEL_ERROR, buf, ap);
        break;
    default:
        j_logger_vlog(normal, J_LOG_LEVEL_DEBUG, buf, ap);
        break;
    }
    va_end(ap);
}

static void jac_master_module_log(JLogLevel level, const char *fmt,
                                  va_list ap)
{
    if (running_master == NULL) {
        return;
    }
    JLogger *normal = running_master->custom_logger;
    JLogger *error = running_master->error_logger;

    switch (level) {
    case J_LOG_LEVEL_INFO:
        j_logger_vlog(normal, J_LOG_LEVEL_INFO, fmt, ap);
        break;
    case J_LOG_LEVEL_WARNING:
        j_logger_vlog(normal, J_LOG_LEVEL_WARNING, fmt, ap);
        break;
    case J_LOG_LEVEL_ERROR:
        j_logger_vlog(error, J_LOG_LEVEL_ERROR, fmt, ap);
        break;
    default:
        j_logger_vlog(normal, J_LOG_LEVEL_DEBUG, fmt, ap);
        break;
    }
}
