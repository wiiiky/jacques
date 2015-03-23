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
#include "hook.h"
#include "net.h"

/*
 * 接受客户端连接的回调函数
 */
int jac_accept_hooks(JSocket * conn, JacServer * server)
{
    JModuleAccept *acc = j_module_accept_new();
    JList *hooks = j_mod_get_hooks(J_HOOK_ACCEPT);

    while (hooks) {
        JModuleAcceptHook hook = (JModuleAcceptHook) j_list_data(hooks);
        hook(conn, acc);
        hooks = j_list_next(hooks);
    }

    int ret = 1;
    if (conn == NULL) {
        ret = 0;
    } else if (j_module_accept_is_recv(acc)) {
        j_socket_recv_package(conn, on_recv_package, server);
    } else if (j_module_accept_is_send(acc)) {
        j_socket_send_package(conn, on_send_package,
                              j_module_accept_get_data(acc),
                              j_module_accept_get_len(acc), server);
    } else {
        j_socket_close(conn);
        ret = 0;
    }
    j_module_accept_free(acc);
    return ret;
}

/*
 * 接受到客户端数据的回调函数
 */
int jac_recv_hooks(JSocket * conn, const void *data, unsigned int len,
                   JSocketRecvResultType type, JacServer * server)
{
    JModuleRecv *r = j_module_recv_new();
    JList *hooks = j_mod_get_hooks(J_HOOK_RECV);

    while (hooks) {
        JModuleRecvHook hook = (JModuleRecvHook) j_list_data(hooks);
        hook(conn, data, len, type, r);
        hooks = j_list_next(hooks);
    }
    int ret = 1;
    if (data == NULL || len == 0 || type == J_SOCKET_RECV_ERR
        || type == J_SOCKET_RECV_EOF) {
        j_socket_close(conn);
        ret = 0;
    } else if (j_module_recv_is_recv(r)) {
        j_socket_recv_package(conn, on_recv_package, server);
    } else if (j_module_recv_is_send(r)) {
        j_socket_send_package(conn, on_send_package,
                              j_module_recv_get_data(r),
                              j_module_recv_get_len(r), server);
    } else {
        j_socket_close(conn);
        ret = 0;
    }
    j_module_recv_free(r);
    return ret;
}
