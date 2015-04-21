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

/*
 * 检查模块是否已经存在
 */
static inline int jac_module_check(JModule * mod)
{
    if (mod->name == NULL) {
        return 0;
    }
    JList *ptr = mods;
    while (ptr) {
        JModule *m = (JModule *) j_list_data(ptr);
        if (j_strcmp0(m->name, mod->name) == 0) {
            return 0;
        }
        ptr = j_list_next(ptr);
    }
    return 1;
}

static inline void jac_module_register(JConfNode * r, JConfNode * n,
                                       JModule * mod)
{
    if (!jac_module_check(mod)) {
        return;
    }
    mods = j_list_append(mods, mod);
    JModuleInit init = mod->init;
    if (init) {
        init(r, n);
    }
}

/*
 * Loads a module from path
 */
int jac_load_module(JConfRoot * root, JConfNode * node, const char *name)
{
    JModule *mod = j_mod_load(MODULE_LOCATION, name);
    if (mod == NULL) {
        return 0;
    }
    JConfNode *r = NULL, *n = NULL;
    if (root) {
        r = j_conf_root_get(root, name);
    }
    if (node) {
        n = j_conf_object_get(node, name);
    }
    jac_module_register(r, n, mod);
    return 1;
}

static inline int jac_load_modules_from_list(JConfRoot * r, JConfNode * n,
                                             JList * mods)
{
    JList *ptr = mods;
    while (ptr) {
        JConfNode *node = (JConfNode *) j_list_data(ptr);
        if (j_conf_node_is_string(node)) {
            if (!jac_load_module(r, n, j_conf_string_get(node))) {
                return 0;
            }
        } else if (j_conf_node_is_array(node)) {
            JList *children = j_conf_node_get_children(node);
            while (children) {
                JConfNode *node = (JConfNode *) j_list_data(children);
                if (j_conf_node_is_string(node)) {
                    if (!jac_load_module(r, n, j_conf_string_get(node))) {
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
    return 1;
}

/*
 * Loads modules from config
 */
int jac_load_modules(JConfRoot * root)
{
    JList *mods = j_conf_root_get_list(root, DIRECTIVE_LOAD_MODULE);
    jac_load_modules_from_list(root, NULL, mods);
    j_list_free(mods);
    return 1;
}

int jac_load_modules_from_node(JConfRoot * root, JConfNode * node)
{
    JList *mods = j_conf_object_get_list(node, DIRECTIVE_LOAD_MODULE);
    jac_load_modules_from_list(root, node, mods);
    j_list_free(mods);
    return 1;
}
