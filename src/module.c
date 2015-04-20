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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */
#include "module.h"
#include "i18n.h"
#include <stdio.h>
#include <jio/jio.h>
#include <jmod/jmod.h>


static JList *mods = NULL;

JList *jac_all_modules(void)
{
    return mods;
}

static inline void jac_module_register(JModule * mod)
{
    mods = j_list_append(mods, mod);
    JModuleInit init = mod->init;
    if (init) {
        init();
    }
}

/*
 * Loads a module from path
 */
int jac_load_module(const char *name)
{
    JModule *mod = j_mod_load(MODULE_LOCATION, name);
    if (mod == NULL) {
        return 0;
    }
    jac_module_register(mod);
    return 1;
}

static inline int jac_load_modules_from_list(JList * mods)
{
    JList *ptr = mods;
    while (ptr) {
        JConfNode *node = (JConfNode *) j_list_data(ptr);
        if (j_conf_node_is_string(node)) {
            if (!jac_load_module(j_conf_string_get(node))) {
                return 0;
            }
        } else if (j_conf_node_is_array(node)) {
            JList *children = j_conf_node_get_children(node);
            while (children) {
                JConfNode *node = (JConfNode *) j_list_data(children);
                if (j_conf_node_is_string(node)) {
                    if (!jac_load_module(j_conf_string_get(node))) {
                        return 0;
                    }
                } else {
                    return 0;
                }
                children = j_list_next(children);
            }
        } else {
            return 0;
        }
        ptr = j_list_next(ptr);
    }
}

/*
 * Loads modules from config
 */
int jac_load_modules(JConfRoot * root)
{
    JList *mods = j_conf_root_get_list(root, DIRECTIVE_LOAD_MODULE);
    jac_load_modules_from_list(mods);
    j_list_free(mods);
    return 1;
}

int jac_load_modules_from_node(JConfNode * node)
{
    JList *mods = j_conf_object_get_list(node, DIRECTIVE_LOAD_MODULE);
    jac_load_modules_from_list(mods);
    j_list_free(mods);
    return 1;
}
