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
#ifndef __JAC_DL_H__
#define __JAC_DL_H__


#include <libjac.h>
#include "list.h"

/*
 * 载入指定模块
 */
JacModule *load_module(const char *filename);

/* 返回记录所有模块的链表 */
DList *all_modules(void);

/* 调用相关的回调函数 */
int call_accept_hooks(Socket *socket);
int call_recv_hooks(Socket *socket, const void *buf, unsigned int len);
void call_exit_hooks(void);


#endif
