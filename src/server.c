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
#include "signals.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/* 事件回调 */
static void ev_callback(struct ev_loop *loop, ev_io *io, int events);
/* 结束服务 */
static void jac_server_release(void *self);

/* 创建服务 */
JacServer *jac_server_new(const char *ip, unsigned short port){
    JacServer *server=(JacServer*)malloc(sizeof(JacServer));
    SphSocket *socket=jac_server_get_socket(server);
    sph_socket_init(socket, jac_server_release);
    sph_socket_reuse_addr(socket, 1);
    sph_socket_reuse_port(socket, 1);
    sph_socket_bind(socket, ip, port);
    sph_socket_listen(socket, 1024);
    return server;
}

/* 启动服务 */
void jac_server_run(JacServer *server){
    init_signals();
    SphSocket *socket=jac_server_get_socket(server);
    sph_socket_start(socket, NULL, ev_callback);
    run_evloop();
}

/* 事件回调 */
static void ev_callback(struct ev_loop *loop, ev_io *io, int events){
    JacServer *server=(JacServer*)io;
    SphSocket *socket=jac_server_get_socket(server);
    int fd = sph_socket_accept(socket);
    printf("accept %d\n", fd);
    close(fd);
}

/* 结束服务 */
static void jac_server_release(void *self){
}
