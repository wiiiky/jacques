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
    JModuleAction *act =
        j_module_action_new_with_type(J_MODULE_ACTION_RECV);
    JList *hooks = j_mod_get_hooks(J_HOOK_ACCEPT);

    while (hooks) {
        JModuleAcceptHook hook = (JModuleAcceptHook) j_list_data(hooks);
        hook(conn, act);
        hooks = j_list_next(hooks);
    }

    int ret = 1;
    unsigned int type = j_module_action_get_type(act);
    if (type & J_MODULE_ACTION_DROP || type & J_MODULE_ACTION_IGNORE) {
        j_socket_close(conn);
        ret = 0;
    } else {
        j_socket_set_error_notify(conn, on_keep_socket, server);
        if (type & J_MODULE_ACTION_SEND) {
            j_socket_send_package(conn, on_send_package,
                                  on_send_package_error,
                                  j_module_action_get_data(act),
                                  j_module_action_get_len(act), server);
        }
        if (type & J_MODULE_ACTION_RECV) {
            j_socket_recv_package(conn, on_recv_package,
                                  on_recv_package_error, server);
        }
    }
    j_module_action_free(act);
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
                   JacServer * server)
{
    JModuleAction *act =
        j_module_action_new_with_type(J_MODULE_ACTION_IGNORE);
    JList *hooks = j_mod_get_hooks(J_HOOK_RECV);

    while (hooks) {
        JModuleRecvHook hook = (JModuleRecvHook) j_list_data(hooks);
        hook(conn, data, len, act);
        hooks = j_list_next(hooks);
    }
    int ret = 1;
    unsigned int type = j_module_action_get_type(act);
    if (type & J_MODULE_ACTION_DROP) {
        j_socket_close(conn);
        ret = 0;
    } else {
        if (type & J_MODULE_ACTION_RECV) {
            j_socket_recv_package(conn, on_recv_package,
                                  on_recv_package_error, server);
        }
        if (type & J_MODULE_ACTION_SEND) {
            j_socket_send_package(conn, on_send_package,
                                  on_send_package_error,
                                  j_module_action_get_data(act),
                                  j_module_action_get_len(act), server);
        }
    }
    j_module_action_free(act);
    return ret;
}

int jac_recv_error_hooks(JSocket * conn, const void *data,
                         unsigned int len)
{
    JList *hooks = j_mod_get_hooks(J_HOOK_RECV_ERROR);
    while (hooks) {
        JModuleRecvErrorHook hook =
            (JModuleRecvErrorHook) j_list_data(hooks);
        hook(conn, data, len);
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
                   JacServer * server)
{
    JModuleAction *act =
        j_module_action_new_with_type(J_MODULE_ACTION_IGNORE);
    JList *hooks = j_mod_get_hooks(J_HOOK_SEND);

    while (hooks) {
        JModuleSendHook hook = (JModuleSendHook) j_list_data(hooks);
        hook(conn, data, count, act);
        hooks = j_list_next(hooks);
    }

    int ret = 1;
    unsigned int type = j_module_action_get_type(act);
    if (type & J_MODULE_ACTION_DROP) {
        j_socket_close(conn);
        ret = 0;
    } else {
        if (type & J_MODULE_ACTION_SEND) {
            j_socket_send_package(conn, on_send_package,
                                  on_send_package_error,
                                  j_module_action_get_data(act),
                                  j_module_action_get_len(act), server);
        }
        if (type & J_MODULE_ACTION_RECV) {
            j_socket_recv_package(conn, on_recv_package,
                                  on_recv_package_error, server);
        }
    }
    j_module_action_free(act);
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
