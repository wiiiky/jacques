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
#include "server.h"
#include "client.h"
#include "signal_.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void accept_cb(struct ev_loop *loop, ev_io *w, int revents);

/* 保存客户端链接 */
static inline void save_client(Server *server, Client *client);

/* 创建一个监听套接字 */
Server *server_start(const char *ip, unsigned short port) {
    struct sockaddr_storage addr;
    socklen_t addrlen;
    int fd = socket_service(ip, port, &addr, &addrlen);
    if(fd<0) {
        return NULL;
    }
    Server *server = (Server*)malloc(sizeof(Server));
    server->clients=NULL;
    server->loop=EV_DEFAULT;
    SOCKET_INIT(server, fd, &addr, addrlen, EV_READ, accept_cb);
    ev_io_start(server->loop, (ev_io*)server);

    /* 捕获信号 */
    signal_init(server->loop);

    ev_run(server->loop, 0);
    return server;
}

void server_stop(Server *server) {
    SOCKET_RELEASE(server, server->loop);
    ev_unref(server->loop);
    free(server);
}

static void accept_cb(struct ev_loop *loop, ev_io *w, int revents) {
    Server *server=(Server*)w;
    Client *cli=client_accept(server);
    if(cli) {
        save_client(server, cli);
    }
}

/* 保存客户端链接 */
static inline void save_client(Server *server, Client *client) {
    server->clients=dlist_prepend(server->clients, client);
    client_start(client);
}
