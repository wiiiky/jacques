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
#include <libjac.h>
#include <stdlib.h>

static int accept_hook(Socket *socket);
static int recv_hook(Socket *socket, const void *buf, unsigned int len);

static JacHook hook = {
    accept_hook,
    recv_hook
};

static JacModule mod = {
    &hook
};

static JacModule *init(){
    return &mod;
}

JACQUES_MODULE_INIT(init);


static int accept_hook(Socket *socket) {
    return 1;
}

static int recv_hook(Socket *socket, const void *buf, unsigned int len) {
    if(len>0) {
        socket_send(socket, buf, len, 0);
    }
    return len;
}
