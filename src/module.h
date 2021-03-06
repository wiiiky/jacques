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

#ifndef __JACQUES_MODULE_H__
#define __JACQUES_MODULE_H__

#include <sph.h>

/* 根据模块名载入模块，失败返回NULL */
JacModule *jac_module_load_by_name(const char *name);

/* 调用模块的回调函数，会按载入顺序调用所有模块的回调函数，直到某个模块出错返回 */
int jac_module_accept(SphSocket *socket);
int jac_module_recv(SphSocket *socket, void *data, unsigned int len);
void jac_module_finalize(void);


#endif
