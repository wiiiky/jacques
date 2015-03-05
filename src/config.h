/*
 * config.h
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

#ifndef __JAC_CONFIG_H__
#define __JAC_CONFIG_H__


#include <jconf/jconf.h>

/*
 * Gets the static JConfParser
 */
JConfParser *jac_config_parser(void);


#define PROGRAME_NAME   "jacques"
#define PROGRAME_VERSION    "0.01"

#define JACQUES_USER "jacques-user"

/*********************** /etc *****************************/
#ifndef CONFIG_LOCATION
#define CONFIG_LOCATION "/etc/" PROGRAME_NAME
#endif


#ifndef CONFIG_FILENAME
#define CONFIG_FILENAME PROGRAME_NAME ".conf"
#endif


#define CONFIG_FILEPATH CONFIG_LOCATION "/" CONFIG_FILENAME


#ifndef MODULE_LOCATION
#define MODULE_LOCATION CONFIG_LOCATION "/modules"
#endif


/********************* /var/run **************************/
#ifndef RUNTIME_LOCATION
#define RUNTIME_LOCATION "/var/run/" PROGRAME_NAME
#endif


#ifndef PID_FILENAME
#define PID_FILENAME PROGRAME_NAME ".pid"
#endif

#define PID_FILEPATH RUNTIME_LOCATION "/" PID_FILENAME


/********************* /var/log ****************************/
#ifndef LOG_LOCATION
#define LOG_LOCATION "/var/log/" PROGRAME_NAME
#endif


#ifndef LOG_FILENAME
#define LOG_FILENAME PROGRAME_NAME ".log"
#endif

#ifndef ERR_FILENAME
#define ERR_FILENAME PROGRAME_NAME ".err"
#endif

#define LOG_FILEPATH LOG_LOCATION "/" LOG_FILENAME
#define ERR_FILEPATH LOG_LOCATION "/" ERR_FILENAME


/******************* Directives ****************************/
#define JAC_LOG_DIRECTIVE   "NormalLog"
#define JAC_ERROR_LOG_DIRECTIVE "ErrorLog"


/* functions */

int jac_config_check(JConfParser * cfg);

const char *jac_config_get_string(JConfNode * root,
                                  const char *name, const char *def);
int jac_config_get_integer(JConfNode * node, const char *name, int def);


#endif
