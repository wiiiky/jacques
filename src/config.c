/*
 * Copyright (C) 2015 Wiky L
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */
#include "config.h"
#include <stdlib.h>

static JConfLoader *config_loader=NULL;
static jchar *config_error=NULL;

static void free_loader(void) {
    if(config_loader==NULL) {
        return;
    }
    j_conf_loader_unref(config_loader);
    j_free(config_error);
}


static inline JConfLoader *get_config_loader(void) {
    if(J_UNLIKELY(config_loader==NULL)) {
        config_loader = j_conf_loader_new();
        j_conf_loader_allow_unknown_variable(config_loader, FALSE);
        j_conf_loader_put_string(config_loader, "PROGRAM", PACKAGE);
        j_conf_loader_put_string(config_loader, "PROGRAM_VERSION", VERSION);
        j_conf_loader_put_string(config_loader, "LOGS_LOCATION", LOG_DIR);
        j_conf_loader_put_integer(config_loader, "DEBUG", 1);
        j_conf_loader_put_integer(config_loader, "INFO",2);
        j_conf_loader_put_integer(config_loader, "ERROR",3);
        j_conf_loader_put_integer(config_loader, "VERBOSE",4);
        atexit(free_loader);
    }
    return config_loader;
}

jchar* config_message(void) {
    JConfLoader *loader=get_config_loader();
    j_free(config_error);
    config_error= j_conf_loader_build_error_message(loader);
    return config_error;
}

/* 读取配置文件，出错返回NULL */
JConfRoot *config_load(const jchar *path) {
    if(path==NULL) {
        path = CONFIG_FILENAME;
    }
    JConfLoader *loader = get_config_loader();
    if(!j_conf_loader_loads(loader, path)) {
        return NULL;
    }
    JConfRoot *root=j_conf_loader_get_root(loader);
    return root;
}
