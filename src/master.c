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
#include "master.h"
#include "config.h"
#include "server.h"
#include <stdlib.h>
#include <unistd.h>


static Master *g_master=NULL;

static void quit_master(void);
/* 读取配置文件 */
static inline JConfRoot *load_config(Master *master);

Master *create_master(const CLOption *option) {
    if(g_master!=NULL) {
        return g_master;
    }
    g_master=(Master*)j_malloc(sizeof(Master));
    g_master->config_loader=create_config_loader();
    g_master->config_error=NULL;
    g_master->servers=NULL;
    g_master->option=option;
    atexit(quit_master);

    JConfRoot *root;;
    if((root=load_config(g_master))==NULL) {
        return g_master;
    }
    g_master->servers=load_servers(root, g_master->option);
    return g_master;
}

static void quit_master(void) {
    if(J_UNLIKELY(g_master==NULL)) {
        return;
    }
    j_conf_loader_unref(g_master->config_loader);
    j_free(g_master->config_error);
    j_list_free_full(g_master->servers, (JDestroyNotify)j_object_unref);
    j_free(g_master);
}

static inline void wait_servers(Master *master);

/* 开始执行主控进程 */
void run_master(Master *master) {
    if(master->config_error!=NULL) {
        j_fprintf(stderr, "configuration error: %s", master->config_error);
        return;
    }
    JList *ptr=master->servers;
    while(ptr) {
        Server *server=(Server*)j_list_data(ptr);
        if(!start_server(server)) {
            j_fprintf(stderr, "fail to start server %s\n", server->name);
        }
        ptr=j_list_next(ptr);
    }

    wait_servers(master);
}


static inline JConfRoot *load_config(Master *master) {
    const jchar *path=master->option->config;
    if(path==NULL) {
        path = CONFIG_FILENAME;
    }
    if(!j_conf_loader_loads(master->config_loader, path)) {
        master->config_error=j_conf_loader_build_error_message(master->config_loader);
        return NULL;
    }
    JConfRoot *root=j_conf_loader_get_root(master->config_loader);
    return root;
}

static inline void wait_servers(Master *master) {
    jint status;
    pid_t pid;
    while((pid=j_wait(&status))>0) {
        JList *ptr=master->servers;
        Server *server=NULL;
        while(ptr) {
            Server *s=((Server*)j_list_data(ptr));
            if(s->pid==pid) {
                server=s;
                break;
            }
            ptr=j_list_next(ptr);
        }
        if(J_UNLIKELY(server==NULL)) {
            j_printf("unknown server %d exits with status code %d\n", pid, status);
        } else {
            j_printf("server %s exits with status code %d\n", server->name, status);
            server->pid=-1;
        }
    }
}
