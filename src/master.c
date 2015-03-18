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
#include <jlib/jlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "i18n.h"
#include "server.h"


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

    JLogger *normal_logger = j_logger_open(normal, "%0 [%l]: %m");
    JLogger *error_logger = j_logger_open(error, "%0 [%l]: %m");

    if (!pid) {
        j_logger_error(error_logger, _("fail to daemonize"));
        goto ERROR;
    }

    if (!jac_save_pid(pid)) {
        j_logger_error(error_logger,
                       _("fail to start jacques, is it already running?"));
        goto ERROR;
    }

    JacMaster *master = (JacMaster *) j_malloc(sizeof(JacMaster));
    master->pid = pid;
    master->cfg = cfg;
    master->normal_logger = normal_logger;
    master->error_logger = error_logger;
    master->servers = NULL;
    master->running = 1;

    jac_master_info(master, _("jacques daemon starts"));

    set_proctitle(NULL, "jacques: master");

    jac_master_fork_servers(master);

    return master;
  ERROR:
    j_logger_close(normal_logger);
    j_logger_close(error_logger);
    return NULL;
}

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
        }
        ptr = j_list_next(ptr);
    }
    j_list_free(virtualservers);
}

static JacMaster *running_master = NULL;

void jac_master_wait(JacMaster * master)
{
    running_master = master;
    signal(SIGINT, signal_handler);
    while (master->running) {
        if (wait(NULL) < 0 && errno == ECHILD) {
            break;
        }
    }
    jac_master_quit(master);
}

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

void jac_master_quit(JacMaster * master)
{
    jac_master_info(master, _("jacques MASTER quits"));
    j_conf_parser_free(master->cfg);
    j_logger_close(master->normal_logger);
    j_logger_close(master->error_logger);
    j_list_free_full(master->servers, (JListDestroy) jac_server_free);
    j_free(master);
    exit(0);
}
