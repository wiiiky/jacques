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
#include <jmod/jmod.h>
#include <jio/jio.h>


static void init_hook(const char *name, const char *host, unsigned short port);
static void accept_hook(JSocket *socket);
static void recv_hook(JSocket *socket, const char *buffer, int size, void *user_data);
static void send_hook(JSocket *socket, int ret, void *user_data);


static boolean send_filter(JSocket *socket, void *data);


JacHook hooks= {
    init_hook,
    accept_hook,
    recv_hook,
    send_hook,
};


JacModule mod = {
    "Test Module",
    &hooks
};

JACQUES_MODULE(mod);


static void init_hook(const char *name, const char *host, unsigned short port){
}

static void accept_hook(JSocket *socket){
}

static void recv_hook(JSocket *socket, const char *buffer, int size, void *user_data){
    jac_send_multi(buffer, size, NULL, send_filter, NULL);
}

static void send_hook(JSocket *socket, int ret, void *user_data){
    
}

static boolean send_filter(JSocket *socket, void *data){
    return TRUE;
}
