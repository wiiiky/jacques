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
#ifndef __JACQUES_CLIENT_H__
#define __JACQUES_CLIENT_H__

#include <sph.h>

typedef enum {
    PACKAGE_FLAG_SIZE=0,
    PACKAGE_FLAG_PAYLOAD=1,
} PackageFlag;

typedef struct {
    PackageFlag pflag;
    unsigned int plen;
} PackageData;


typedef struct {
    SphSocket parent;
    PackageData pdata;
} JacClient;
#define jac_client_socket(client)           ((SphSocket*)client)
#define jac_client_get_package_data(client) (&(client)->pdata)


JacClient *jac_client_new_from_fd(int fd);

/* 开始监听客户链接事件 */
void jac_client_start(JacClient *client);

#endif
