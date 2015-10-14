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
#include "socket.h"
#include <unistd.h>
#include <string.h>

/* 使用文件描述符初始化 */
void socket_init(BaseSocket *sock, int fd, struct sockaddr *addr,
                 socklen_t addrlen, int events,
                 void(*callback)(struct ev_loop *, ev_io *, int)) {
    memcpy(&sock->addr, addr, addrlen);
    sock->addrlen = addrlen;
    ev_io_init((ev_io*)sock, callback, fd, events);
}

/* 关闭套接字 */
void socket_release(BaseSocket *sock, struct ev_loop *loop) {
    ev_io_stop(loop, (ev_io*)sock);
    close(((ev_io*)sock)->fd);
}

/*
 * 创建一个监听套接字
 */
int socket_service(const char *ip, unsigned short port,
                   struct sockaddr_storage *addr, socklen_t *addrlen) {
    struct sockaddr_in *addr4 = (struct sockaddr_in*)addr;
    struct sockaddr_in6 *addr6 = (struct sockaddr_in6*)addr;

    if(inet_pton(AF_INET, ip, &addr4->sin_addr.s_addr)) {
        addr4->sin_family = AF_INET;
        addr4->sin_port=htons(port);
        *addrlen = sizeof(*addr4);
    } else if(inet_pton(AF_INET6, ip, &addr6->sin6_addr.s6_addr)) {
        addr6->sin6_family=AF_INET6;
        addr6->sin6_port=htons(port);
        *addrlen = sizeof(*addr6);
    } else {
        return -1;
    }

    int fd = socket(addr->ss_family, SOCK_STREAM, 0);
    int flag=1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag))||
            bind(fd, (struct sockaddr*)addr, *addrlen)) {
        close(fd);
        return -1;
    }

    if(listen(fd, 4096)) {
        close(fd);
        return -1;
    }
    return fd;
}
