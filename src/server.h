/*
 * Copyright (C) 2015 Wiky L <wiiiky@outlook.com>
 *
 * jacques is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * jacques is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */
#ifndef __JAC_SERVER_H__
#define __JAC_SERVER_H__

#include "config.h"
#include <jio/jio.h>


#define DIRECTIVE_VIRTUAL_SERVER    "VirtualServer"
#define DIRECTIVE_LISTEN_PORT  "ListenPort"
#define DIRECTIVE_SERVER_NAME "Name"


typedef struct {
    int pid;
    char *name;

    JLogger *custom_logger;
    JLogger *error_logger;

    JSocket *listen_sock;
    unsigned short listen_port;
} JacServer;

#define jac_server_get_name(server) (server)->name
#define jac_server_get_sock(server) (server)->listen_sock
#define jac_server_get_port(server) (server)->listen_port

/*
 * Return a JacServer to master on success, NULL on error
 */
JacServer *jac_server_start_from_conf(JConfRoot * root, JConfNode * vs);


void jac_server_free(JacServer * server);
void jac_server_end(JacServer * server);


/*
 * callbacks
 */
void on_recv_package(JSocket * conn,
                     const void *data, unsigned int len, void *user_data);
void on_recv_package_error(JSocket * conn,
                           const void *data,
                           unsigned int len, void *user_data);
void on_send_package(JSocket * conn, const char *data,
                     unsigned int count, void *user_data);
void on_send_package_error(JSocket * sock, const char *data,
                           unsigned int count,
                           unsigned int len, void *user_data);
void on_accept_connection(JSocket * listen, JSocket * conn, void *data);

void on_keep_socket(JSocket * conn, void *user_data);


#endif
