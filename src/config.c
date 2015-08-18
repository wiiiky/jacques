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
#include "config.h"
#include <stdlib.h>

JConfLoader *create_config_loader(void) {
    JConfLoader *loader = j_conf_loader_new();
    j_conf_loader_allow_unknown_variable(loader, FALSE);
    j_conf_loader_put_string(loader, "PROGRAM", PACKAGE);
    j_conf_loader_put_string(loader, "PROGRAM_VERSION", VERSION);
    j_conf_loader_put_string(loader, "LOGS_LOCATION", LOG_DIR);
    j_conf_loader_put_integer(loader, "ERROR", J_LOG_LEVEL_ERROR);
    j_conf_loader_put_integer(loader, "INFO",J_LOG_LEVEL_INFO);
    j_conf_loader_put_integer(loader, "DEBUG",J_LOG_LEVEL_DEBUG);
    j_conf_loader_put_integer(loader, "CRITICAL",J_LOG_LEVEL_CRITICAL);
    j_conf_loader_put_integer(loader, "WARNING", J_LOG_LEVEL_MESSAGE);
    return loader;
}
