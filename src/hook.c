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
#include <jio/jio.h>

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
    if (j_module_accept_is_recv(acc)) {
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

int jac_accept_error_hooks(void)
{
    JList *hooks = j_mod_get_hooks(J_HOOK_ACCEPT_ERROR);
    while (hooks) {
        JModuleAcceptErrorHook hook =
            (JModuleAcceptErrorHook) j_list_data(hooks);
        hook();
        hooks = j_list_next(hooks);
    }
    return 0;
}

/*
 * 接受到客户端数据的回调函数
 * 如果出错，则链接一定会被关闭，而不管模块如何指定操作
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
    if (j_module_recv_is_recv(r)) {
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

int jac_recv_error_hooks(JSocket * conn, const void *data,
                         unsigned int len, JSocketRecvResultType type)
{
    JList *hooks = j_mod_get_hooks(J_HOOK_RECV_ERROR);
    while (hooks) {
        JModuleRecvErrorHook hook =
            (JModuleRecvErrorHook) j_list_data(hooks);
        hook(conn, data, len, type);
        hooks = j_list_next(hooks);
    }
    j_socket_close(conn);
    return 0;
}

/*
 * 发送数据完成的回调函数
 * 如果出错，则链接一定会被关闭，而不管模块如何指定操作
 */
int jac_send_hooks(JSocket * conn, const void *data, unsigned int count,
                   unsigned int len, JacServer * server)
{
    JModuleSend *s = j_module_send_new();
    JList *hooks = j_mod_get_hooks(J_HOOK_SEND);

    while (hooks) {
        JModuleSendHook hook = (JModuleSendHook) j_list_data(hooks);
        hook(conn, data, count, len, s);
        hooks = j_list_next(hooks);
    }

    int ret = 1;
    if (j_module_send_is_recv(s)) {
        j_socket_recv_package(conn, on_recv_package, server);
    } else if (j_module_send_is_send(s)) {
        j_socket_send_package(conn, on_send_package,
                              j_module_send_get_data(s),
                              j_module_send_get_len(s), server);
    } else {
        j_socket_close(conn);
        ret = 0;
    }
    j_module_send_free(s);
    return ret;
}

int jac_send_error_hooks(JSocket * conn, const void *data,
                         unsigned int count, unsigned int len)
{
    JList *hooks = j_mod_get_hooks(J_HOOK_SEND_ERROR);
    while (hooks) {
        JModuleSendErrorHook hook =
            (JModuleSendErrorHook) j_list_data(hooks);
        hook(conn, data, count, len);
        hooks = j_list_next(hooks);
    }
    j_socket_close(conn);
    return 0;
}
