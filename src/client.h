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
#ifndef __JAC_CLIENT_H__
#define __JAC_CLIENT_H__

#include "server.h"

typedef struct {
    Socket parent;

    Server *server;
} Client;

void client_start(Client *cli);

/* 接受一个客户端连接 */
Client *client_accept(Server *server);

void client_free(Client *cli);

/* 关闭链接 */
void client_close(Client *cli);


#define client_recv(cli, buf, len, flags) socket_recv((Socket*)cli, buf, len, flags)
#define client_send(cli, buf, len, flags) socket_send((Socket*)cli, buf, len, flags)


#endif
