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


static void ev_callback(struct ev_loop *loop, ev_io *io, int events);
static void jac_client_release(void *self);

typedef enum {
    PACKAGE_FLAG_SIZE=0,
    PACKAGE_FLAG_PAYLOAD=1,
} PackageFlag;

typedef struct {
    PackageFlag pflag;
    unsigned int plen;
} ClientData;

static inline ClientData *client_data_new(void) {
    ClientData *data=(ClientData*)malloc(sizeof(ClientData));
    data->pflag=PACKAGE_FLAG_SIZE;
    return data;
}

static inline void client_data_free(ClientData *data) {
    free(data);
}


JacClient *jac_client_new_from_fd(int fd) {
    if(fd<0) {
        return NULL;
    }
    JacClient *client=(JacClient*)malloc(sizeof(JacClient));
    SphSocket *socket=jac_client_socket(client);
    sph_socket_init_from_fd(socket, fd, jac_client_release);
    sph_socket_set_user_data(socket, client_data_new());

    return client;
}


static void jac_client_release(void *self) {
    SphSocket *socket=jac_client_socket(self);
    client_data_free((ClientData*)sph_socket_get_user_data(socket));
}


/* 开始监听客户链接事件 */
void jac_client_start(JacClient *client) {
    SphSocket *socket=jac_client_socket(client);
    sph_socket_start(socket, NULL, ev_callback);
}

#define WOULDBLOCK()    (errno==EAGAIN||errno==EWOULDBLOCK)

static void ev_callback(struct ev_loop *loop, ev_io *io, int events) {
    int n;

    JacClient *client=(JacClient *)io;
    SphSocket *socket=jac_client_socket(client);
    SphBuffer *rbuf = sph_socket_get_rbuf(socket);
    SphBuffer *wbuf = sph_socket_get_wbuf(socket);
    if(events&EV_ERROR) {
        sph_socket_unref(socket);
        D("event error\n");
        return;
    }
    if(events&EV_READ) {
        char buf[4096];
        ClientData *cdata=(ClientData*)sph_socket_get_user_data(socket);
        if(cdata->pflag==PACKAGE_FLAG_SIZE) {
            if(sph_socket_recv(socket, buf, 4, MSG_DONTWAIT)!=4) {
                sph_socket_unref(socket);
                D("read package length error %d\n", n);
                return;
            }
            cdata->plen=decode_package_length(buf);
            cdata->pflag = PACKAGE_FLAG_PAYLOAD;
        }
        while(cdata->plen>0) {
            n = sph_socket_recv(socket, buf,
                                cdata->plen<sizeof(buf)?cdata->plen:sizeof(buf),
                                MSG_DONTWAIT);
            if(n<=0) {
                if(WOULDBLOCK()) {
                    break;
                }
                sph_socket_unref(socket);
                D("read package payload error %d\n", n);
                return;
            }
            cdata->plen -= n;
            sph_buffer_append(rbuf, buf, n);
        }
        if(cdata->plen==0) {
            /* 读取到一个完整的数据包 */
            D("read a good package with length %u\n", sph_buffer_get_length(rbuf));
            encode_package_length(buf, sph_buffer_get_length(rbuf));
            sph_buffer_append(wbuf, buf, 4);
            sph_buffer_append(wbuf, sph_buffer_get_data(rbuf), sph_buffer_get_length(rbuf));
            sph_buffer_clear(rbuf);
            cdata->pflag=PACKAGE_FLAG_SIZE;
        }
    }
    if(events&EV_WRITE) {
        SphBuffer *wbuf=sph_socket_get_wbuf(socket);
        if(sph_buffer_get_length(wbuf)>0) {
            n = sph_socket_send(socket, sph_buffer_get_data(wbuf),
                                sph_buffer_get_length(wbuf), MSG_DONTWAIT);
            if(n<0) {
                if(!WOULDBLOCK()) {
                    sph_socket_unref(socket);
                    D("write error %d, close client\n", n);
                    return;
                }
            } else {
                D("write %d/%u\n", n, sph_buffer_get_length(wbuf));
                sph_buffer_erase(wbuf, 0, n);
            }
        }
    }
}
