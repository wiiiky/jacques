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

#define CONFIG_FILENAME CONFIG_DIR "/" PACKAGE ".conf"


#define CONFIG_KEY_PORT "port"

#define CONFIG_KEY_LOG  "log"
#define CONFIG_KEY_ERROR_LOG    "error_log"
#define CONFIG_KEY_LOG_LEVEL "log_level"
#define CONFIG_KEY_USER "user"
#define CONFIG_KEY_MODULES "modules"

#define DEFAULT_LOG LOG_DIR "/" PACKAGE ".log"
#define DEFAULT_ERROR_LOG LOG_DIR "/" PACKAGE ".err"
#define DEFAULT_LOG_LEVEL (J_LOG_LEVEL_ERROR|J_LOG_LEVEL_INFO|J_LOG_LEVEL_WARNING)
#define DEFAULT_USER    "wiky"

JConfLoader *create_config_loader(void);


/* 命令行参数 */
typedef struct {
    jboolean help;      /* --help */
    jboolean test;      /* --test */
    jboolean verbose;   /* --verbose */
    jchar *config;      /* --config filename*/
} CLOption;



#endif
