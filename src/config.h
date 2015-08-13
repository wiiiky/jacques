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
#ifndef __JAC_CONFIG_H__
#define __JAC_CONFIG_H__

#include <jconf/jconf.h>


#ifndef LISTEN_PORT
#define LISTEN_PORT 1601
#endif

#define CONFIG_FILENAME CONFIG_DIR "/jacques.conf"

/* 获取配置文件的错误信息 */
jchar* config_message(void);
/* 读取配置文件，出错返回NULL */
JConfRoot *config_load(const jchar *path);


#endif
