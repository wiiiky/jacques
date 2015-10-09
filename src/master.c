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
#include <jmod/jmod.h>

#define MASTER_DOMAIN "master"

static Master *g_master=NULL;

static void quit_master(void);
/* 读取配置文件 */
static inline boolean load_config(Master *master);
/* 载入模块 */
static inline boolean load_modules(Master *master);


/* 日志记录函数 */
static void master_log_handler(const char *domain, JLogLevelFlag level,
                               const char *message, void * user_data);
#define master_debug(...) j_log(MASTER_DOMAIN, J_LOG_LEVEL_DEBUG, __VA_ARGS__)
#define master_info(...) j_log(MASTER_DOMAIN, J_LOG_LEVEL_INFO, __VA_ARGS__)
#define master_warning(...) j_log(MASTER_DOMAIN, J_LOG_LEVEL_WARNING, __VA_ARGS__)
#define master_error(...) j_log(MASTER_DOMAIN, J_LOG_LEVEL_ERROR, __VA_ARGS__)


/* 初始化Master，打开日志，载入模块等 */
static inline boolean init_master(Master *master);
/* 启动服务器 */
static inline void start_servers(Master *master);
static inline void wait_servers(Master *master);
static inline boolean check_master(Master *master);

/* 信号处理函数 */
static void signal_handler(int signo, siginfo_t *sinfo, void *unused);

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
    close(g_master->error_logfd);
    j_list_free_full(g_master->servers, (JDestroyNotify)j_object_unref);
    j_list_free_full(g_master->mod_paths, (JDestroyNotify)j_free);
    j_free(g_master);
}

/* 信号处理函数 */
static void signal_handler(int signo, siginfo_t *sinfo, void *unused) {
    if(signo==SIGINT) {
        g_master->running=FALSE;
    } else if(signo==SIGCHLD) {
        if(sinfo->si_code!=CLD_EXITED) {
            return;
        }
        /* 子进程退出 */
        int status;
        pid_t pid=sinfo->si_pid;
        waitpid(sinfo->si_pid, &status, 0);

        JList *ptr=g_master->servers;
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

/* 初始化Master，打开日志，载入模块等 */
static inline boolean init_master(Master *master) {
    master->logfd = create_or_append(master->log);
    master->error_logfd = create_or_append(master->error_log);
    j_log_set_handler(MASTER_DOMAIN,master->log_level, master_log_handler, master);

    if(!check_master(master)||!load_modules(master)) {
        return FALSE;
    }

    j_daemonize();

    master_debug("master pid %d\n",getpid());
    return TRUE;
}

/* 启动服务器 */
static inline void start_servers(Master *master) {
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
}

/* 开始执行主控进程 */
void run_master(Master *master) {
    if(!init_master(master)) {
        return;
    }
    start_servers(master);
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
    master->log=extract_log(root,NULL, CONFIG_KEY_LOG, DEFAULT_LOG);
    master->logfd=-1;
    master->error_log=extract_log(root,NULL, CONFIG_KEY_ERROR_LOG, DEFAULT_ERROR_LOG);
    master->error_logfd=-1;
    master->log_level=extract_loglevel(root, NULL);
    master->mod_paths=extract_modules(root);

    master->servers=load_servers((JConfRoot*)root, master->option);
    return TRUE;
}


/*
 * 等待信号
 */
static inline void wait_servers(Master *master) {
    master->running=TRUE;
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = signal_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    sigset_t sigmask;
    sigfillset(&sigmask);
    sigdelset(&sigmask, SIGINT);
    sigdelset(&sigmask, SIGCHLD);
    while(master->running) {
        sigsuspend(&sigmask);
    }
}

/*
 * 检查主进程的配置是否没有问题
 * 如果没有问题则返回True
 * 否则返回False
 */
static inline boolean check_master(Master *master) {
    if(master->config_error!=NULL) {
        j_fprintf(stderr, "configuration error: %s\n", master->config_error);
        return FALSE;
    }
    return TRUE;
}

/* 载入模块，失败返回FALSE，成功返回TRUE */
static inline boolean load_modules(Master *master) {
    JList *ptr=master->mod_paths;
    while(ptr) {
        const char *path = (const char*)j_list_data(ptr);
        JacModule *mod=jac_loads_module(path);
        if(mod==NULL) {
            fprintf(stderr, "fail to load module %s\n", path);
            return FALSE;
        }
        master_info("load module %s successfully\n", path);
        ptr=j_list_next(ptr);
    }
    return TRUE;
}


/* 日志输出函数 */
static void master_log_handler(const char *domain, JLogLevelFlag level,
                               const char *message, void * user_data) {
    Master *master=(Master*)user_data;
    log_common(domain, message, level, master->logfd, master->error_logfd);
}
