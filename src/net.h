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
#ifndef __JAC_NET_H__
#define __JAC_NET_H__


#include <jio/jio.h>

/*
 * 接受消息的回调函数
 * 如果接受失败，则返回的data为NULL，长度为0
 */
typedef void (*JSocketRecvPackageNotify) (JSocket * sock,
                                          const void *data,
                                          unsigned int len,
                                          JSocketRecvResultType type,
                                          void *user_data);
void j_socket_recv_package(JSocket * sock, JSocketRecvPackageNotify notify,
                           void *user_data);


/*
 * 发送消息
 */

/*
 * count 为数据包的长度
 * len 为成功发送出去的长度
 * len == count 表示发送成功
 */
typedef void (*JSocketSendPackageNotify) (JSocket * sock, const char *data,
                                          unsigned int count,
                                          unsigned int len,
                                          void *user_data);
void j_socket_send_package(JSocket * sock, JSocketSendPackageNotify notify,
                           const void *data, unsigned int len,
                           void *user_data);

#endif
