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

/* 一个服务相关的信息 */
struct _Server {
    JObject parent;
    jchar *name;
    jushort port;
    jchar *user;    /* 以哪个用户执行 */

    jchar *log;         /* 日志文件路径 */
    jchar *error_log;   /* 错误日志路径 */
    jint logfd;
    jint error_logfd;
    JLogLevelFlag log_level;    /* 日志等级 */

    JList *mod_paths;       /* 需要载入模块名 */

    /* 监听端口的套接字 */
    JSocket *socket;

    /* 进程ID */
    pid_t pid;
};

typedef struct _Server Server;


/* 根据配置文件以及命令行选项载入服务设置 */
JList *load_servers(JConfRoot *root, const CLOption *option);

jboolean start_server(Server *server);

/* 输出服务设置 */
void dump_server(Server *server);

#endif
