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
#ifndef __JAC_SERVER_H__
#define __JAC_SERVER_H__

#include "libjac.h"
#include "list.h"

typedef struct {
    Socket parent;
    struct ev_loop *loop;

    DList *clients;
} Server;


/* 创建一个监听套接字 */
Server *server_start(const char *ip, unsigned short port);
/* 结束服务 */
void server_stop(Server *server);


#endif
