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


static JConfRoot *root_config = NULL;
JConfRoot *jac_config_root(void)
{
    if (root_config == NULL) {
        root_config = j_conf_root_new(CONFIG_FILEPATH);
        j_conf_root_add_vars(root_config, "LogLocation", LOG_LOCATION,
                             "RunLocation", RUNTIME_LOCATION, NULL);
    }
    return root_config;
}

void jac_config_free(void)
{
    if (root_config != NULL) {
        j_conf_root_free(root_config);
        root_config = NULL;
    }
}

JConfNode *jac_config_get_last(JConfRoot * root, JConfNode * node,
                               const char *name)
{
    JConfNode *ret = NULL;
    if (node != NULL) {
        ret = j_conf_object_get(node, name);
    }
    if (ret == NULL && root != NULL) {
        ret = j_conf_root_get(root, name);
    }
    return ret;
}

int64_t jac_config_get_int(JConfRoot * root, JConfNode * node,
                           const char *name, int64_t def)
{
    JConfNode *n = jac_config_get_last(root, node, name);
    if (n == NULL || !j_conf_node_is_int(n)) {
        return def;
    }
    return j_conf_int_get(n);
}

const char *jac_config_get_string(JConfRoot * root, JConfNode * node,
                                  const char *name, const char *def)
{
    JConfNode *n = jac_config_get_last(root, node, name);
    if (n == NULL || !j_conf_node_is_string(n)) {
        return def;
    }
    return j_conf_string_get(n);
}


const char *jac_config_get_virtual_server_name(JConfNode * node)
{
    return jac_config_get_string(NULL, node, DIRECTIVE_SERVER_NAME,
                                 _("unname"));
}
