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

#include "module.h"
#include <limits.h>
#include <stdio.h>

static SphList *module_list = NULL;


/* 根据模块名载入模块，失败返回NULL */
JacModule *jac_module_load_by_name(const char *name) {
    char buf[PATH_MAX];
    snprintf(buf, sizeof(buf), "%s/%s", "./mod",name);
    JacModule *mod=jac_module_load(buf);
    if(mod) {
        if(mod->m_init()==0) {
            module_list = sph_list_append(module_list, mod);
        } else {
            printf("fail to init %s\n", name);
        }
    }
    return mod;
}

int jac_module_accept(SphSocket *socket) {
    int ret = 1;
    SphList *ptr=module_list;
    while(ptr) {
        JacModule *mod=(JacModule*)sph_list_data(ptr);
        if(mod->m_accept && (ret = mod->m_accept(socket))) {
            break;
        }
        ptr=sph_list_next(ptr);
    }
    return ret;
}

int jac_module_recv(SphSocket *socket, void *data, unsigned int len) {
    int ret = 1;
    SphList *ptr=module_list;
    while(ptr) {
        JacModule *mod=(JacModule*)sph_list_data(ptr);
        if(mod->m_recv && (ret = mod->m_recv(socket, data, len))) {
            break;
        }
        ptr=sph_list_next(ptr);
    }
    return ret;
}

void jac_module_finalize(void) {
    SphList *ptr=module_list;
    while(ptr) {
        JacModule *mod=(JacModule*)sph_list_data(ptr);
        if(mod->m_finalize) {
            mod->m_finalize();
        }
        ptr=sph_list_next(ptr);
    }
}
