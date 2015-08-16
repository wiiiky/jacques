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

/* 创建一个监听端口port的TCP套接字 */
JSocket *socket_listen(const jchar * address, jushort port) {
    if(address==NULL) {
        address="0.0.0.0";
    }
    JSocketAddress addr;
    if (!j_inet_socket_address_init_from_string(&addr, address, port)) {
        return NULL;
    }

    JSocket *socket =
        j_socket_new(J_SOCKET_FAMILY_INET, J_SOCKET_TYPE_STREAM,
                     J_SOCKET_PROTOCOL_DEFAULT);
    if (socket == NULL) {
        return NULL;
    }

    if (!j_socket_bind(socket, &addr, TRUE)
            || !j_socket_listen(socket, 1024)) {
        j_socket_close(socket);
        return NULL;
    }
    return socket;
}
