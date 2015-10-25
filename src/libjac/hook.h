/*
 * lib.h
 * Copyright (C) 2015 Wiky L <wiiiky@outlook.com>
 * 
 * libjac is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * libjac is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */

#ifndef __LIBJAC_HOOK_H__
#define __LIBJAC_HOOK_H__


#include "socket.h"

/*
 * 返回0关闭链接
 */
typedef int (*JacAcceptHook)(Socket *socket);
/*
 * 返回接受到的字节长度
 * 如果<=0则会关闭链接
 */
typedef int (*JacRecvHook)(Socket *socket, const void *buf, unsigned int len);


typedef struct {
    JacAcceptHook accept;
    JacRecvHook recv;
}JacHook;


#endif
