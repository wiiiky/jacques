/*
 * config.c
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
#include "config.h"
#include <stdlib.h>

JConfParser *jac_config_parser(void)
{
    static JConfParser *gConfigParser=NULL;
    if(gConfigParser==NULL){
        gConfigParser=j_conf_parser_new();
        j_conf_parser_add_env(gConfigParser,CONFIG_LOCATION);
        j_conf_parser_add_env(gConfigParser,".");
        j_conf_parser_add_variable(gConfigParser,
            "LogLocation=" LOG_LOCATION);
        j_conf_parser_add_variable(gConfigParser,
            "RunLocation=" RUNTIME_LOCATION);
    }
    return gConfigParser;
}
