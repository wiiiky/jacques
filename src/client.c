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
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>


static void ev_callback(struct ev_loop *loop, ev_io *io, int events);
static void jac_client_release(void *self);


JacClient *jac_client_new_from_fd(int fd) {
    if(fd<0) {
        return NULL;
    }
    JacClient *client=(JacClient*)malloc(sizeof(JacClient));
    SphSocket *socket=jac_client_socket(client);
    sph_socket_init_from_fd(socket, fd, jac_client_release);

    return client;
}


static void jac_client_release(void *self) {

}


/* 开始监听客户链接事件 */
void jac_client_start(JacClient *client) {
    SphSocket *socket=jac_client_socket(client);
    sph_socket_start(socket, NULL, ev_callback);
}

static void ev_callback(struct ev_loop *loop, ev_io *io, int events) {
    JacClient *client=(JacClient *)io;
    SphSocket *socket=jac_client_socket(client);
    if(events&EV_ERROR) {
        sph_socket_unref(socket);
        return;
    } else if(events&EV_READ) {
        char buf[4096];
        int n = sph_socket_recv(socket, buf, sizeof(buf), MSG_DONTWAIT);
        if(n<=0) {
            sph_socket_unref(socket);
            return;
        }
        sph_socket_send(socket, buf, n, 0);
    }
}
