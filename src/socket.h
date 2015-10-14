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

#ifndef __JAC_SOCKET_H__
#define __JAC_SOCKET_H__

#include <ev.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
    ev_io parent;
    struct sockaddr_storage addr;
    socklen_t addrlen;
} BaseSocket;

/* 使用文件描述符初始化 */
void socket_init(BaseSocket *sock, int fd, struct sockaddr *addr,
                 socklen_t addrlen, int events,
                 void(*callback)(struct ev_loop *, ev_io *, int));
#define SOCKET_INIT(sock, fd, addr, addrlen, events, cb)\
    socket_init((BaseSocket*)sock, fd, (struct sockaddr*)addr, addrlen, events, cb)

/* 关闭套接字 */
void socket_release(BaseSocket *sock, struct ev_loop *loop);
#define SOCKET_RELEASE(sock, loop)  socket_release((BaseSocket*)sock, loop)

/*
 * 创建一个监听套接字
 */
int socket_service(const char *ip, unsigned short port,
                   struct sockaddr_storage *addr, socklen_t *addrlen);


#endif
