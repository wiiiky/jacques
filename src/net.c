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
#include "net.h"
#include <jlib/jlib.h>

/*
 * data必须为四个字节
 * 解析四个字节的二进制数组
 * 返回长度
 */
static inline unsigned int parse_length(const char *data);

/*
 * 生成四个字节的数组，表示长度len
 */
static inline const void *generate_length(unsigned int len);

/* 中间数据 */
typedef struct {
    void *notify;
    void *user_data;
    unsigned int len;
} JSocketPackageData;

static inline JSocketPackageData
    * j_socket_package_data_new(void *notify, void *user_data)
{
    JSocketPackageData *data =
        (JSocketPackageData *) j_malloc(sizeof(JSocketPackageData));
    data->notify = notify;
    data->user_data = user_data;
    return data;
}

static inline void j_socket_package_data_free(JSocketPackageData * data)
{
    j_free(data);
}


/*
 * 获取消息内容的回调函数
 */
static void recv_package_callback(JSocket * sock, JSocketRecvResult * res,
                                  void *udata)
{
    JSocketPackageData *data = (JSocketPackageData *) udata;
    JSocketRecvPackageNotify notify =
        (JSocketRecvPackageNotify) data->notify;
    void *user_data = data->user_data;
    unsigned int len = data->len;

    if (res == NULL) {
        notify(sock, NULL, 0, J_SOCKET_RECV_ERR, user_data);
    } else if (j_socket_recv_result_get_len(res) != len
               || j_socket_recv_result_is_error(res)) {
        notify(sock, NULL, 0, j_socket_recv_result_get_type(res),
               user_data);
    } else {

        const void *buf = j_socket_recv_result_get_data(res);
        notify(sock, buf, len, j_socket_recv_result_get_type(res),
               user_data);
    }
    j_socket_package_data_free(data);
}

/*
 * 获取消息长度的回调函数
 */
static void recv_package_len_callback(JSocket * sock,
                                      JSocketRecvResult * res, void *udata)
{
    JSocketPackageData *data = (JSocketPackageData *) udata;
    JSocketRecvPackageNotify notify =
        (JSocketRecvPackageNotify) data->notify;
    void *user_data = data->user_data;
    if (res == NULL) {
        notify(sock, NULL, 0, J_SOCKET_RECV_ERR, user_data);
        j_socket_package_data_free(data);
        return;
    }
    const void *buf = j_socket_recv_result_get_data(res);
    unsigned int len = parse_length((const char *) buf);
    if (j_socket_recv_result_get_len(res) != 4
        || !j_socket_recv_result_is_normal(res) || len == 0) {
        notify(sock, NULL, 0, j_socket_recv_result_get_type(res),
               user_data);
        j_socket_package_data_free(data);
        return;
    }
    data->len = len;
    j_socket_recv_len_async(sock, recv_package_callback, len, data);
}

void j_socket_recv_package(JSocket * sock, JSocketRecvPackageNotify notify,
                           void *user_data)
{
    JSocketPackageData *data =
        j_socket_package_data_new(notify, user_data);
    j_socket_recv_len_async(sock, recv_package_len_callback, 4, data);
}


/********************** 发送消息 ************************************/

static void send_package_callback(JSocket * sock, const void *data,
                                  unsigned int count, unsigned int n,
                                  void *udata)
{
    JSocketPackageData *pdata = (JSocketPackageData *) udata;
    JSocketSendPackageNotify notify =
        (JSocketSendPackageNotify) pdata->notify;
    void *user_data = pdata->user_data;

    notify(sock, data, count, n, user_data);

    j_socket_package_data_free(pdata);
}

void j_socket_send_package(JSocket * sock, JSocketSendPackageNotify notify,
                           const void *data, unsigned int len,
                           void *user_data)
{
    if (len == 0) {
        return;
    }
    JSocketPackageData *pdata =
        j_socket_package_data_new(notify, user_data);
    const void *buf = generate_length(len);
    JByteArray *array = j_byte_array_new();
    j_byte_array_append(array, buf, 4);
    j_byte_array_append(array, data, len);
    j_socket_send_async(sock, send_package_callback,
                        j_byte_array_get_data(array),
                        j_byte_array_get_len(array), pdata);
    j_byte_array_free(array, 1);
}


static inline unsigned int parse_length(const char *data)
{
    unsigned int sum = 0;
    sum += data[0] << 24;
    sum += data[1] << 16;
    sum += data[2] << 8;
    sum += data[3];
    return sum;
}

static inline const void *generate_length(unsigned int len)
{
    static char buf[4];
    buf[0] = len / (1 << 24);
    buf[1] = len % (1 << 24) / (1 << 16);
    buf[2] = len % (1 << 16) / (1 << 8);
    buf[3] = len % (1 << 8);
    return (const void *) buf;
}
