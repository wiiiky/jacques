/*
 * main.c
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

#ifndef __JAC_MASTER_H__
#define __JAC_MASTER_H__


#include "config.h"
#include <jio/jio.h>
#include <jlib/jlib.h>


typedef struct {
    int pid;
    JLogger *custom_logger;
    JLogger *error_logger;

    JList *servers;

    int running;
} JacMaster;

/*
 * 日志输出
 */
void jac_master_log(JacMaster * master, JLogLevel level,
                    const char *fmt, ...);
#define jac_master_info(master,fmt,...) \
            jac_master_log(master,J_LOG_LEVEL_INFO,fmt,##__VA_ARGS__)
#define jac_master_debug(master,fmt,...) \
            jac_master_log(master,J_LOG_LEVEL_DEBUG,fmt,##__VA_ARGS__)
#define jac_master_warning(master,fmt,...) \
            jac_master_log(master,J_LOG_LEVEL_WARNING,fmt,##__VA_ARGS__)
#define jac_master_error(master,fmt,...) \
            jac_master_log(master,J_LOG_LEVEL_ERROR,fmt,##__VA_ARGS__)


JacMaster *jac_master_start();


void jac_master_wait(JacMaster * master);


void jac_master_quit(JacMaster * master);


#endif
