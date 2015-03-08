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
    JConfParser *cfg;
    JLogger *normal_logger;
    JLogger *error_logger;

    JList *servers;

    int running;
} JacMaster;

#define jac_master_error(master,fmt,...) \
            j_logger_error((master)->error_logger,fmt,##__VA_ARGS__)
#define jac_master_info(master,fmt,...) \
            j_logger_verbose((master)->normal_logger,fmt,##__VA_ARGS__)
#define jac_master_debug(master,fmt,...) \
            j_logger_debug((master)->normal_logger,fmt,##__VA_ARGS__)
#define jac_master_warning(master,fmt,...) \
            j_logger_warning((master)->normal_logger,fmt,##__VA_ARGS__)


JacMaster *jac_master_start(JConfParser * cfg);


void jac_master_wait(JacMaster * master);


void jac_master_quit(JacMaster * master);


#endif
