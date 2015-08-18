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
#include "server.h"

static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"verbose", no_argument, 0, 'v'},
    {"test", no_argument, 0, 't'},
    {"configuration", required_argument, 0, 'c'}
};

static inline void show_help(CLOption *option);
static inline void test_config(JConfRoot *root, JList *servers, CLOption *option);

int main(int argc, char *argv[]) {
    jint opt, long_index;

    CLOption OPTIONS = {
        FALSE,
        FALSE,
        FALSE,
        NULL
    };

    while((opt=getopt_long(argc, argv, "hvtc:",
                           long_options, &long_index))!=-1) {
        switch(opt) {
        case 'h':
            OPTIONS.help = TRUE;
            break;
        case 'v':
            OPTIONS.verbose=TRUE;
            break;
        case 'c':
            OPTIONS.config=j_strdup(optarg);
            break;
        case 't':
            OPTIONS.test=TRUE;
            break;
        default:
            break;
        }
    }
    /* 如果指定了--help，则显示帮助信息 */
    show_help(&OPTIONS);

    JConfRoot *root=config_load(OPTIONS.config);
    JList *servers=load_servers(root, &OPTIONS);

    /* 如果指定了--test，则执行配置文件的测试 */
    test_config(root, servers, &OPTIONS);

    /* 启动服务器 */
    start_all(servers);

    /* 等待服务进程结束 */
    wait_all(servers);
    return 0;
}


/*
 * 显示帮助信息
 */
static inline void show_help(CLOption *option) {
    if(!option->help) {
        return;
    }
    j_printf("%s %s\n", PACKAGE, VERSION);
    j_printf("Built on %s\n", __DATE__);
    j_printf("\n");
    j_printf("  --test\t-t\tTest configuration\n");
    j_printf("  --verbose\t-v\tverbose log\n");
    j_printf("  --config\t-c filename\n\t\t\tset configuration file (default: conf/%s.conf)", PACKAGE);
    j_printf("\n");
    j_printf("  --help\t-h\tShow this Help\n");
    j_printf("\n");
    exit(0);
}

static inline void test_config(JConfRoot *root, JList *servers, CLOption *option) {
    if(!option->test) {
        return;
    }
    if(root==NULL) {
        j_printf("%s\n", config_message());
        exit(-1);
    }
    jchar *data=j_conf_node_dump((JConfNode*)root);
    j_printf("%s\n",data);
    j_free(data);

    j_printf("==========================SERVERS==========================\n");
    JList *ptr=servers;
    while(ptr) {
        Server *server=(Server*)j_list_data(ptr);
        dump_server(server);
        ptr=j_list_next(ptr);
        j_printf("\n");
    }
    exit(0);
}
