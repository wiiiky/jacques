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

#include <jlib/jlib.h>
#include "socket.h"
#include <jlib/ji18n.h>

static jboolean jac_accept_callback(JSocket * server, JSocket * client,
                                    jpointer user_data);
static jboolean jac_receive_callback(JSocket * socket,
                                     const jchar * buffer, jint size,
                                     jpointer user_data);
static void jac_socket_send_callback(JSocket * socket, jint ret,
                                     jpointer user_data);

int main(int argc, const char *argv[]) {
    printf(_("hello world!\n"));
    JSocket *socket = jac_socket_listen("0.0.0.0", 23456);
    j_socket_accept_async(socket, jac_accept_callback, NULL);
    j_main();
    return 0;
}

static jint client_count = 0;

static jboolean jac_accept_callback(JSocket * server, JSocket * client,
                                    jpointer user_data) {
    if (client == NULL) {
        j_printf("accept connection error!\n");
        j_socket_unref(server);
        j_main_quit();
        return FALSE;
    } else {
        client_count++;
        JSocketAddress raddr;
        j_socket_get_remote_address(client, &raddr);
        jchar *remote = j_inet_socket_address_to_string(&raddr);
        j_printf("%s connected!\n", remote);
        j_socket_receive_async(client, jac_receive_callback, NULL);
        j_free(remote);
    }
    return TRUE;
}

static jboolean jac_receive_callback(JSocket * socket,
                                     const jchar * buffer, jint size,
                                     jpointer user_data) {
    JSocketAddress raddr;
    j_socket_get_remote_address(socket, &raddr);
    jchar *remote = j_inet_socket_address_to_string(&raddr);
    if (size <= 0) {
        j_socket_unref(socket);
        j_printf("%s closed!\n", remote);
        j_free(remote);
        return FALSE;
    }
    j_printf("%s => %.*s\n", remote, size, buffer);
    j_free(remote);
    j_socket_send_async(socket, buffer, size, jac_socket_send_callback,
                        NULL);
    return TRUE;
}

static void jac_socket_send_callback(JSocket * socket, jint ret,
                                     jpointer user_data) {
    if (ret <= 0) {
        j_printf("send error!\n");
    }
}
