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
    init();
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

/*
 * Loads modules from config
 */
int jac_load_modules(JConfParser * parser)
{
    JConfNode *root = j_conf_parser_get_root(parser);
    return jac_load_modules_from_scope(root);
}

int jac_load_modules_from_scope(JConfNode * root)
{
    int ret = 1;
    JList *mods = j_conf_node_get_directive(root, LOAD_MODULE_DIRECTIVE);
    JList *ptr = mods;
    while (ptr) {
        JConfNode *node = (JConfNode *) j_list_data(ptr);
        JList *args = j_conf_node_get_arguments(node);
        while (args) {
            JConfData *arg = (JConfData *) j_list_data(args);
            const char *name = j_conf_data_get_raw(arg);
            if (!jac_load_module(name)) {
                printf(_("\033[31mfail to load module %s\033[m\n"), name);
                ret = 0;
            }
            args = j_list_next(args);
        }
        ptr = j_list_next(ptr);
    }
    j_list_free(mods);
    return ret;
}
