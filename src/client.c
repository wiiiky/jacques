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
#include "client.h"
#include <ev.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

/* 接受一个客户端连接 */
ClientSocket *client_accept(ServerSocket *server) {
    struct sockaddr_storage addr;
    socklen_t addrlen;
    int fd = accept(((ev_io*)server)->fd, (struct sockaddr*)&addr, &addrlen);
    if(fd<0) {
        return NULL;
    }
    ClientSocket *cli = malloc(sizeof(ClientSocket));
    SOCKET_INIT(cli, fd, &addr, addrlen, EV_READ, NULL);
    cli->server=server;
    cli->prev=NULL;
    cli->next=NULL;

    return cli;
}

void client_free(ClientSocket *cli) {
    SOCKET_RELEASE(cli, cli->server->loop);
    free(cli);
}
