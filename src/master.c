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
#include "utils.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define MASTER_DOMAIN "master"

static Master *g_master=NULL;

static void quit_master(void);
/* 读取配置文件 */
static inline boolean load_config(Master *master);

/* 日志记录函数 */
static void master_log_handler(const char *domain, JLogLevelFlag level,
                               const char *message, void * user_data);
#define master_debug(...) j_log(MASTER_DOMAIN, J_LOG_LEVEL_DEBUG, __VA_ARGS__)
#define master_info(...) j_log(MASTER_DOMAIN, J_LOG_LEVEL_INFO, __VA_ARGS__)
#define master_warning(...) j_log(MASTER_DOMAIN, J_LOG_LEVEL_WARNING, __VA_ARGS__)
#define master_error(...) j_log(MASTER_DOMAIN, J_LOG_LEVEL_ERROR, __VA_ARGS__)

static inline void wait_servers(Master *master);
static inline boolean check_master(Master *master);

/* 信号处理函数 */
static void signal_handler(int signo);

Master *create_master(const CLOption *option) {
    if(g_master!=NULL) {
        return g_master;
    }
    g_master=(Master*)j_malloc(sizeof(Master));
    g_master->config_loader=create_config_loader();
    g_master->config_error=NULL;
    g_master->servers=NULL;
    g_master->option=option;
    g_master->log=NULL;
    g_master->error_log=NULL;
    g_master->logfd=-1;
    g_master->error_logfd=-1;
    g_master->running=FALSE;
    atexit(quit_master);

    if(!load_config(g_master)) {
        return g_master;
    }
    return g_master;
}

static void quit_master(void) {
    if(J_UNLIKELY(g_master==NULL)) {
        return;
    }
    j_conf_loader_unref(g_master->config_loader);
    j_free(g_master->config_error);
    j_free(g_master->log);
    close(g_master->logfd);
    j_list_free_full(g_master->servers, (JDestroyNotify)j_object_unref);
    j_list_free_full(g_master->mod_paths, (JDestroyNotify)j_free);
    j_free(g_master);
}

/* 信号处理函数 */
static void signal_handler(int signo) {
    j_printf("master signal: %d\n", signo);
    g_master->running=FALSE;
}

/* 开始执行主控进程 */
void run_master(Master *master) {
    if(!check_master(master)) {
        return;
    }
    j_log_set_handler(MASTER_DOMAIN,master->log_level, master_log_handler, master);
    JList *ptr=master->servers;
    while(ptr) {
        Server *server=(Server*)j_list_data(ptr);
        if(!start_server(server)) {
            master_error("fail to start server %s", server->name);
        } else {
            master_debug("server %s starts successfully", server->name);
        }
        ptr=j_list_next(ptr);
    }

    wait_servers(master);
}


static inline boolean load_config(Master *master) {
    const char *path=master->option->config;
    if(path==NULL) {
        path = CONFIG_FILENAME;
    }
    if(!j_conf_loader_loads(master->config_loader, path)) {
        master->config_error=j_conf_loader_build_error_message(master->config_loader);
        return FALSE;
    }
    JConfObject *root=(JConfObject*)j_conf_loader_get_root(master->config_loader);
    master->log=load_log(root,NULL, CONFIG_KEY_LOG, DEFAULT_LOG);
    master->logfd=append_file(master->log);
    master->error_log=load_log(root,NULL, CONFIG_KEY_ERROR_LOG, DEFAULT_ERROR_LOG);
    master->error_logfd=append_file(master->error_log);
    master->log_level=load_loglevel(root, NULL);
    master->mod_paths=load_modules(root);

    master->servers=load_servers((JConfRoot*)root, master->option);
    return TRUE;
}

static inline void wait_servers(Master *master) {
    int status;
    pid_t pid;

    master->running=TRUE;
    signal(SIGINT, signal_handler);
    while((pid=j_wait(&status))>0 && master->running) {
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
            master_warning("unknown server(PID=%d) exits with status code %d", pid, status);
        } else {
            master_info("server %s(PID=%d) exits with status code %d", server->name, pid, status);
            server->pid=-1;
        }
    }
}

static inline boolean check_master(Master *master) {
    if(master->config_error!=NULL) {
        j_fprintf(stderr, "configuration error: %s\n", master->config_error);
        return FALSE;
    } else if(master->logfd<0) {
        j_fprintf(stderr, "unable to open log %s\n", master->log);
        return FALSE;
    } else if(master->error_logfd<0) {
        j_fprintf(stderr, "unable to open error log %s\n", master->error_log);
        return FALSE;
    }
    return TRUE;

}

static void master_log_handler(const char *domain, JLogLevelFlag level,
                               const char *message, void * user_data) {
    Master *master=(Master*)user_data;
    log_internal(domain, message, level, master->logfd, master->error_logfd);
}
