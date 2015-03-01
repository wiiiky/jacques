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
#include <jlib/jlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static const char *jac_server_get_conf_virtualserver_name(JConfNode * vs)
{
    JConfData *name_data = j_conf_node_get_argument_first(vs);
    if (name_data) {
        return j_conf_data_get_raw(name_data);
    }
    return _("unnamed");
}

static int jac_server_check_conf_virtualserver(JConfNode * vs)
{
    JConfNode *directive = j_conf_node_get_directive_last(vs,
                                                          LISTEN_PORT_DIRECTIVE);
    if (directive == NULL) {
        printf(_("\033[31m%s not found in %s\033[0m\n"),
               LISTEN_PORT_DIRECTIVE,
               jac_server_get_conf_virtualserver_name(vs));
        return 0;
    }
    JConfData *port_data = j_conf_node_get_argument_first(directive);
    if (j_conf_node_get_arguments_count(directive) != 1 ||
        port_data == NULL || !j_conf_data_is_int(port_data) ||
        j_conf_data_get_int(port_data) < 0 ||
        j_conf_data_get_int(port_data) > 65535) {
        printf(_("\033[31minvalid argument of %s in %s\033[0m\n"),
               LISTEN_PORT_DIRECTIVE,
               jac_server_get_conf_virtualserver_name(vs));
        return 0;
    }
    return 1;
}

/*
 * Checks to see if the config of servers are correct
 * If no <VirtualServer> found or directive error, returns 0
 */
int jac_server_check_conf(JConfParser * p)
{
    JConfNode *root = j_conf_parser_get_root(p);
    JList *vs = j_conf_node_get_scope(root, JAC_VIRTUAL_SERVER_SCOPE);
    if (vs == NULL) {
        printf(_("no <%s> found\n"), JAC_VIRTUAL_SERVER_SCOPE);
        return 0;
    }
    JList *ptr = vs;
    int ret = 1;
    while (ptr) {
        JConfNode *node = (JConfNode *) j_list_data(ptr);
        if (!jac_server_check_conf_virtualserver(node)) {
            ret = 0;
        }
        ptr = j_list_next(ptr);
    }
    j_list_free(vs);

    return ret;
}

static inline void jac_server_main(JacServer * server)
{
    while (1);
    exit(0);
}

JacServer *jac_server_start(const char *name, unsigned int port,
                            const char *normal, const char *error)
{
    int pid = fork();
    if (pid < 0) {
        return NULL;
    }
    jac_close_fds();

    JacServer *server = (JacServer *) j_malloc(sizeof(JacServer));
    server->name = j_strdup(name);
    server->listenport = port;
    server->pid = getpid();
    server->normal_logger = j_logger_open(normal, "%0 [%l]: %m");
    server->error_logger = j_logger_open(error, "%0 [%l]: %m");
    if (pid == 0) {
        set_proctitle(NULL, "jacques: server %s", name);
        j_logger_verbose(server->normal_logger, "server starts!!!!");
        jac_server_main(server);
    }
    return server;
}


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

JacServer *jac_server_start_from_conf(JConfNode * root, JConfNode * vs)
{
    JConfNode *directive = j_conf_node_get_directive_last(vs,
                                                          LISTEN_PORT_DIRECTIVE);
    const char *normal = jac_server_get_conf_log(root, vs);
    const char *error = jac_server_get_conf_err_log(root, vs);
    if (directive == NULL) {
        return NULL;
    }
    JConfData *port_data = j_conf_node_get_argument_first(directive);
    if (j_conf_node_get_arguments_count(directive) != 1 ||
        port_data == NULL || !j_conf_data_is_int(port_data) ||
        j_conf_data_get_int(port_data) < 0 ||
        j_conf_data_get_int(port_data) > 65535) {
        return NULL;
    }
    unsigned int port = j_conf_data_get_int(port_data);
    return jac_server_start(jac_server_get_conf_virtualserver_name(vs),
                            port, normal, error);
}
