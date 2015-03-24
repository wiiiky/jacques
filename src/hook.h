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
#ifndef __JAC_HOOK_H__
#define __JAC_HOOK_H__

#include <jmod/jmod.h>
#include "server.h"

/*
 * 接受客户端连接的回调函数
 */
int jac_accept_hooks(JSocket * conn, JacServer * server);
int jac_accept_error_hooks(void);

/*
 * 接受到客户端数据的回调函数
 */
int jac_recv_hooks(JSocket * conn, const void *data, unsigned int len,
                   JSocketRecvResultType type, JacServer * server);
int jac_recv_error_hooks(JSocket * conn, const void *data,
                         unsigned int len, JSocketRecvResultType type);

/*
 * 发送数据完成的回调函数
 */
int jac_send_hooks(JSocket * conn, const void *data, unsigned int count,
                   unsigned int len, JacServer * server);
int jac_send_error_hooks(JSocket * conn, const void *data,
                         unsigned int count, unsigned int len);

#endif
