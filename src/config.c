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
#include "utils.h"
#include <stdlib.h>

JConfLoader *create_config_loader(void) {
    JConfLoader *loader = j_conf_loader_new();
    j_conf_loader_allow_unknown_variable(loader, FALSE);
    j_conf_loader_put_string(loader, "PROGRAM", PACKAGE);
    j_conf_loader_put_string(loader, "PROGRAM_VERSION", VERSION);
    j_conf_loader_put_integer(loader, "ERROR", J_LOG_LEVEL_ERROR);
    j_conf_loader_put_integer(loader, "INFO",J_LOG_LEVEL_INFO);
    j_conf_loader_put_integer(loader, "DEBUG",J_LOG_LEVEL_DEBUG);
    j_conf_loader_put_integer(loader, "CRITICAL",J_LOG_LEVEL_CRITICAL);
    j_conf_loader_put_integer(loader, "WARNING", J_LOG_LEVEL_MESSAGE);
    return loader;
}

/* 载入日志文件，同时创建目录（如果必要的话） */
jchar *load_log(JConfObject *root, JConfObject *node, const jchar *key, const jchar *def) {
    jchar *log=join_path_with_root(j_conf_object_get_string_priority(root, node, key, def), LOG_DIR);
    make_dir(log);
    return log;
}

/* 从配置中读取模块列表 */
JList *load_modules(JConfObject *obj) {
    JList *mods=j_conf_object_get_string_list(obj, CONFIG_KEY_MODULES);
    JList *ret=NULL;
    JList *ptr=mods;
    while(ptr) {
        ret=j_list_append(ret, join_path_with_root((jchar*)j_list_data(ptr), MOD_DIR));
        ptr=j_list_next(ptr);
    }
    j_list_free(mods);
    return ret;
}

/* 读取用户，服务进程将以该用户的身份执行 */
jchar *load_user(JConfObject *root, JConfObject *node) {
    return j_strdup(j_conf_object_get_string_priority(root, node, CONFIG_KEY_USER, DEFAULT_USER));
}

jint load_loglevel(JConfObject *root, JConfObject *node) {
    return j_conf_object_get_integer_priority(root, node, CONFIG_KEY_LOG_LEVEL, DEFAULT_LOG_LEVEL);
}
