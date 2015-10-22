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
#include "list.h"
#include <ev.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/* 客户端事件的回调函数 */
static void client_cb(struct ev_loop *loop, ev_io *w, int revents);

void client_start(Client *cli) {
    ev_io_start(cli->server->loop, (ev_io*)cli);
}

/* 接受一个客户端连接 */
Client *client_accept(Server *server) {
    struct sockaddr_storage addr;
    socklen_t addrlen;
    int fd = accept(((ev_io*)server)->fd, (struct sockaddr*)&addr, &addrlen);
    if(fd<0) {
        return NULL;
    }
    Client *cli = malloc(sizeof(Client));
    SOCKET_INIT(cli, fd, &addr, addrlen, EV_READ, client_cb);
    cli->server=server;

    return cli;
}

void client_free(Client *cli) {
    SOCKET_RELEASE(cli, cli->server->loop);
    free(cli);
}

static void client_cb(struct ev_loop *loop, ev_io *w, int revents) {
    Client *cli = (Client*)w;
    if(EV_READ & revents) {
        char buf[4096];
        int n;
        while((n=client_recv(cli, buf, sizeof(buf), 0))>0) {
            client_send(cli, buf, n, 0);
        }
        if(n==0) {
            client_close(cli);
        } else if(errno!=EAGAIN&&errno!=EWOULDBLOCK) {
            client_close(cli);
        }
    }
}

/* 关闭链接 */
void client_close(Client *cli) {
    cli->server->clients=dlist_remove(cli->server->clients, cli);
    client_free(cli);
}
