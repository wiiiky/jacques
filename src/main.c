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
    {"test", no_argument, 0, 't'},
};

static jboolean OPT_HELP=FALSE;
static jboolean OPT_VERBOSE=FALSE;
static jboolean OPT_TEST=FALSE;

static inline void show_help(jboolean show);
static inline void test_config(jboolean test);

int main(int argc, char *argv[]) {
    jint opt, long_index;

    while((opt=getopt_long(argc, argv, "hvt",
                           long_options, &long_index))!=-1) {
        switch(opt) {
        case 'h':
            OPT_HELP = TRUE;
            break;
        case 'v':
            OPT_VERBOSE=TRUE;
            break;
        case 't':
            OPT_TEST=TRUE;
            break;
        default:
            break;
        }
    }
    show_help(OPT_HELP);
    test_config(OPT_TEST);
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

static inline void test_config(jboolean test) {
    if(!test) {
        return;
    }
    JConfRoot *root=config_load(NULL);
    if(root==NULL) {
        jchar *msg=config_message();
        j_printf("%s\n", msg);
        j_free(msg);
        exit(-1);
    }
    jchar *data=j_conf_node_dump((JConfNode*)root);
    j_printf("%s\n",data);
    j_free(data);
    j_conf_node_unref((JConfNode*)root);
    exit(0);
}
