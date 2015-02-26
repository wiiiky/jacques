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
#include "i18n.h"
#include "server.h"


#define JAC_LOG_DIRECTIVE   "NormalLog"
#define JAC_ERROR_LOG_DIRECTIVE "ErrorLog"


static inline void jac_master_fork_servers(JacMaster * master);


JacMaster *jac_master_start(JConfParser * cfg)
{
    JConfNode *root = j_conf_parser_get_root(cfg);
    JConfNode *normal_node = j_conf_node_get_directive_last(root,
                                                            JAC_LOG_DIRECTIVE);
    JConfNode *error_node = j_conf_node_get_directive_last(root,
                                                           JAC_ERROR_LOG_DIRECTIVE);

    const char *normal = LOG_FILEPATH;
    const char *error = ERR_FILEPATH;
    if (normal_node) {
        if (j_conf_node_get_arguments_count(normal_node) != 1) {
            printf(_("invalid argument of %s\n"), JAC_LOG_DIRECTIVE);
            return NULL;
        }
        JConfData *data = j_conf_node_get_argument_first(normal_node);
        normal = j_conf_data_get_raw(data);
    }
    if (error_node) {
        if (j_conf_node_get_arguments_count(error_node) != 1) {
            printf(_("invalid argument of %s\n"), JAC_ERROR_LOG_DIRECTIVE);
            return NULL;
        }
        JConfData *data = j_conf_node_get_argument_first(error_node);
        error = j_conf_data_get_raw(data);
    }

    int pid = jac_daemonize();

    JLogger *normal_logger = j_logger_open(normal, "%0 [%l]: %m");
    JLogger *error_logger = j_logger_open(error, "%0 [%l]: %m");

    if (!pid) {
        j_logger_error(error_logger, _("fail to daemonize"));
        j_logger_close(normal_logger);
        j_logger_close(error_logger);
        return NULL;
    }

    if (!jac_save_pid(pid)) {
        j_logger_error(error_logger,
                       _("fail to start jacques, is it already running?"));
        j_logger_close(normal_logger);
        j_logger_close(error_logger);
        return NULL;
    }

    JacMaster *master = (JacMaster *) j_malloc(sizeof(JacMaster));
    master->pid = pid;
    master->cfg = cfg;
    master->normal_logger = normal_logger;
    master->error_logger = error_logger;
    master->servers = NULL;

    j_logger_verbose(normal_logger, _("jacques daemon starts"));

    set_proctitle(NULL, "jacques: master");

    jac_master_fork_servers(master);

    return master;
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

void jac_master_wait(JacMaster * master)
{
    while (1) {

    }
}

void jac_master_quit(JacMaster * master)
{
    j_conf_parser_free(master->cfg);
    j_logger_close(master->normal_logger);
    j_logger_close(master->error_logger);
    j_free(master);
    exit(0);
}