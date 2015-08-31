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
#ifndef __JAC_MASTER_H__
#define __JAC_MASTER_H__

#include <jconf/jconf.h>
#include <jlib/jlib.h>
#include "config.h"

struct _Master {
    JConfLoader *config_loader;
    jchar *config_error;
    jchar *log;
    jint logfd;
    jchar *error_log;
    jint error_logfd;
    jint log_level;

    JList *mod_paths;

    JList *servers;
    const CLOption *option;
};

typedef struct _Master Master;

/* 初始化主进程 */
Master *create_master(const CLOption *option);

/* 开始执行主控进程 */
void run_master(Master *master);

#endif
