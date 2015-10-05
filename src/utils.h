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
#ifndef __JAC_UTILS_H__
#define __JAC_UTILS_H__

#include <jlib/jtypes.h>
#include <jio/jsocket.h>


/*
 * path是一个文件路径，该函数创建该文件所需要的目录
 * 路径存在或者创建成功，返回TRUE，否则返回FALSE
 */
boolean make_path(const char *path);


/* 以O_APPPEND打开文件 */
int append_file(const char *path);


void log_common(const char *domain,const char *message, int level, int fd, int errfd);

/* 根据用户名设置当前进程的用户ID */
boolean setuser(const char *user);


/*
 * 如果path是一个绝对路径，则返回该路径的一个副本
 * 否则返回parent/path的副本
 */
char *join_path_with_root(const char *path, const char *parent);


#endif
