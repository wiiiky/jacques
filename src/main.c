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

#include <ev.h>
#include <stdio.h>
#include "server.h"
#include "client.h"

int main(int argc, const char *argv[]) {
    if (!ev_default_loop (0)) {
        return -1;
    }
    Server *server=server_start("0.0.0.0", 13221);
    if(server==NULL) {
        return -2;
    }
    server_stop(server);

    return 0;
}

