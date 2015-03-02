/*
 * config.c
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
#include "config.h"
#include "i18n.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>


static JConfParser *gConfigParser = NULL;


/*
 * Checks to see if the JConfNode root has and only has a string arugment
 */
static inline int jac_config_check_directive_string(JConfNode * root,
                                                    const char *name)
{
    const char *str = jac_config_get_string(root, name,
                                            (const char *) 1);
    if (str == NULL) {
        printf(_("\033[31minvalid argument of %s\033[0m\n"), name);
        return 0;
    }
    return 1;
}

/*
 * Checks to see if the config is correct
 */
int jac_config_check(JConfParser * cfg)
{
    int ret = 1;
    JConfNode *root = j_conf_parser_get_root(cfg);
    JList *vs = j_conf_node_get_scope(root, JAC_VIRTUAL_SERVER_SCOPE);
    if (vs == NULL) {
        printf(_("\033[31mno <%s> found\033[0m\n"),
               JAC_VIRTUAL_SERVER_SCOPE);
    } else {
        JList *ptr = vs;
        while (ptr) {
            JConfNode *node = (JConfNode *) j_list_data(ptr);
            if (!jac_server_check_conf_virtualserver(node)) {
                ret = 0;
            }
            ptr = j_list_next(ptr);
        }
        j_list_free(vs);
    }

    ret =
        ret & jac_config_check_directive_string(root,
                                                JAC_LOG_DIRECTIVE) &
        jac_config_check_directive_string(root, JAC_ERROR_LOG_DIRECTIVE);


    return ret;
}

JConfParser *jac_config_parser(void)
{
    if (gConfigParser == NULL) {
        gConfigParser = j_conf_parser_new();
        j_conf_parser_add_env(gConfigParser, CONFIG_LOCATION);
        j_conf_parser_add_env(gConfigParser, ".");
        j_conf_parser_add_variable(gConfigParser,
                                   "LogLocation=" LOG_LOCATION);
        j_conf_parser_add_variable(gConfigParser,
                                   "RunLocation=" RUNTIME_LOCATION);
    }
    return gConfigParser;
}

const char *jac_config_get_string(JConfNode * root,
                                  const char *name, const char *def)
{
    JConfNode *node = j_conf_node_get_directive_last(root, name);
    if (node) {
        if (j_conf_node_get_arguments_count(node) != 1) {
            return NULL;
        }
        JConfData *data = j_conf_node_get_argument_first(node);
        return j_conf_data_get_raw(data);
    }
    return def;
}

int jac_config_get_integer(JConfNode * node, const char *name, int def)
{
    JConfNode *d = j_conf_node_get_directive_last(node, name);
    if (d) {
        JConfData *data = j_conf_node_get_argument_first(d);
        if (j_conf_node_get_arguments_count(d) != 1 ||
            !j_conf_data_is_int(data)) {
            return -1;
        }
        return j_conf_data_get_int(data);
    }
    return def;
}
