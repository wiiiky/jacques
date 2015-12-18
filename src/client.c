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
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include "debug.h"
#include "package.h"
#include "module.h"


static void ev_callback(struct ev_loop *loop, ev_io *io, int events);
static void jac_client_release(void *self);
static void jac_client_prepare_data(SphSocket *socket, const void *data, unsigned int len);


JacClient *jac_client_new_from_fd(int fd) {
    if(fd<0) {
        return NULL;
    }
    JacClient *client=(JacClient*)malloc(sizeof(JacClient));
    SphSocket *socket=jac_client_socket(client);
    sph_socket_init_from_fd(socket, fd);
    /* init hooks */
    socket->release = jac_client_release;
    socket->prepare = jac_client_prepare_data;
    client->pdata.pflag = PACKAGE_FLAG_SIZE;
    client->pdata.plen = 0;

    return client;
}


static void jac_client_release(void *self) {
    SphSocket *socket=jac_client_socket(self);
}


/* 开始监听客户链接事件 */
void jac_client_start(JacClient *client) {
    SphSocket *socket=jac_client_socket(client);
    sph_socket_start(socket, NULL, ev_callback);
}

#define WOULDBLOCK()    (errno==EAGAIN||errno==EWOULDBLOCK)

/* 从客户端读取数据，成功返回0，失败返回-1 */
static inline int jac_client_recv(JacClient *client);
/* 如果有未发送的数据，则发送，成功返回0，失败返回-1 */
static inline int jac_client_send(JacClient *client);

static void ev_callback(struct ev_loop *loop, ev_io *io, int events) {
    JacClient *client=(JacClient *)io;
    SphSocket *socket=jac_client_socket(client);
    if(events&EV_ERROR) {
        sph_socket_unref(socket);
        D("event error\n");
        return;
    }
    if(events&EV_READ) {
        if(jac_client_recv(client)) {
            return;
        }
    }
    if(events&EV_WRITE) {
        if(jac_client_send(client)) {
            return;
        }
    }
}

static void jac_client_prepare_data(SphSocket *socket, const void *data,
                                    unsigned int len) {
    uint8_t buf[4];
    SphBuffer *wbuf=sph_socket_get_wbuf(socket);
    encode_package_length(buf, len);
    sph_buffer_append(wbuf, buf, 4);
    sph_buffer_append(wbuf, data, len);
}

static inline int jac_client_recv(JacClient *client) {
    SphSocket *socket=jac_client_socket(client);
    SphBuffer *rbuf = sph_socket_get_rbuf(socket);
    SphBuffer *wbuf = sph_socket_get_wbuf(socket);

    int n;
    uint8_t buf[4096];
    PackageData *pdata=jac_client_get_package_data(client);

    if((n = sph_socket_recv(socket, buf, sizeof(buf), MSG_DONTWAIT))<=0) {
        if(!WOULDBLOCK()) {
            sph_socket_unref(socket);
            D("read error %d\n", n);
            return -1;
        }
        return 0;
    } else {
        sph_buffer_append(rbuf, buf, n);
    }

    do {
        if(pdata->pflag==PACKAGE_FLAG_SIZE && sph_buffer_get_length(rbuf)>=4) {
            pdata->plen=decode_package_length(sph_buffer_get_data(rbuf));
            pdata->pflag = PACKAGE_FLAG_PAYLOAD;
            sph_buffer_pop(rbuf, 4);
        }
        if(pdata->pflag==PACKAGE_FLAG_PAYLOAD && pdata->plen<=sph_buffer_get_length(rbuf)) {
            /* 回调给模块 */
            if(jac_module_recv(socket, sph_buffer_get_data(rbuf), pdata->plen)) {
                sph_socket_unref(socket);
                return -1;
            }
            sph_buffer_pop(rbuf, pdata->plen);
            D("package - %u\n", pdata->plen);
            pdata->pflag = PACKAGE_FLAG_SIZE;
        } else {
            break;
        }
    } while(1);
    return 0;
}

static inline int jac_client_send(JacClient *client) {
    SphSocket *socket=jac_client_socket(client);
    SphBuffer *rbuf = sph_socket_get_rbuf(socket);
    SphBuffer *wbuf = sph_socket_get_wbuf(socket);
    int n;

    if(sph_buffer_get_length(wbuf)>0) {
        n = sph_socket_send(socket, sph_buffer_get_data(wbuf),
                            sph_buffer_get_length(wbuf) ,MSG_DONTWAIT);
        if(n<0) {
            if(!WOULDBLOCK()) {
                sph_socket_unref(socket);
                D("write error %d, close client\n", n);
                return -1;
            }
            D("write would block!\n");
        } else {
            sph_buffer_pop(wbuf, n);
            D("write %d/%u\n", n, sph_buffer_get_length(wbuf));
        }
    }
    return 0;
}
