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
#ifndef __JAC_SERVER_H__
#define __JAC_SERVER_H__

#include "config.h"

typedef struct _Server Server;

/* 根据配置文件以及命令行选项载入服务设置 */
JList *load_servers(JConfRoot *root, CLOption *option);
/*
 * 读取配置文件、启动服务器
 */
void start_all(JList *servers);

/* 等待服务进程 */
void wait_all(JList *servers);

/* 输出服务设置 */
void dump_server(Server *server);

#endif
