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

#include <stdlib.h>
#include <jlib/jlib.h>
#include <getopt.h>
#include "socket.h"
#include "config.h"

static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"verbose", no_argument, 0, 'v'},
};

static jboolean OPT_VERBOSE=FALSE;


static inline void show_help(jboolean show);

int main(int argc, char *argv[]) {
    jint opt, long_index;

    jboolean help=FALSE;
    while((opt=getopt_long(argc, argv, "hv",
                           long_options, &long_index))!=-1) {
        switch(opt) {
        case 'h':
            help = TRUE;
            break;
        case 'v':
            OPT_VERBOSE=TRUE;
            break;
        default:
            break;
        }
    }

    show_help(help);
    return 0;
}


/*
 * 显示帮助信息
 */
static inline void show_help(jboolean show) {
    if(!show) {
        return;
    }
    j_printf("%s %s\n", PACKAGE, VERSION);
    j_printf("\t--help\t-h\tShow this Help\n");
    j_printf("\n");
    exit(0);
}
