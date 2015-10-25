/*
 * mod.h
 * Copyright (C) 2015 Wiky L <wiiiky@outlook.com>
 * 
 * libjac is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * libjac is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */

#ifndef __LIBJAC_MOD_H__
#define __LIBJAC_MOD_H__


#include "hook.h"

typedef struct {
    JacHook *hook;
}JacModule;


#define JACQUES_MODULE_NAME __jacques_module__
#define STR(S)  #S
#define XSTR(S) STR(S)
#define JACQUES_MODULE_NAME_STRING  XSTR(JACQUES_MODULE_NAME)
#define JACQUES_MODULE(mod) JacModule* JACQUES_MODULE_NAME=&mod


#endif
