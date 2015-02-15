/*
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
#ifndef __JAC_MODULE_H__
#define __JAC_MODULE_H__

/*
 * Something about module
 */

#include <jconf/jconf.h>


#define LOAD_MODULE_DIRECTIVE   "LoadModule"


/*
 * Loads a module from path
 */
int jac_load_module(const char *path);

/*
 * Loads modules from config
 */
int jac_load_modules(JConfParser * parser);


#endif
