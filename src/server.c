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
#include "server.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "utils.h"
#include <sys/stat.h>
#include <fcntl.h>

/* 一个服务相关的信息 */
struct _Server {
    JObject parent;
    jchar *name;
    jushort port;

    jchar *log;         /* 日志文件路径 */
    jchar *error_log;   /* 错误日志路径 */
    JLogLevelFlag log_level;    /* 日志等级 */
    jint logfd;
    jint error_logfd;

    pid_t pid;
};

#define DEFAULT_LOG_LEVEL (J_LOG_LEVEL_ERROR|J_LOG_LEVEL_INFO|J_LOG_LEVEL_WARNING)

/* 创建服务器，失败返回NULL */
static inline Server *create_server(const jchar *name,JConfObject *root, JConfObject *obj, CLOption *option);
/* 启动服务器 */
static inline jboolean start_server(Server *server);
/* 关闭服务器 */
static void stop_server(Server *server);

static JList *all_servers=NULL;
static void stop_all(void);

void start_all(JList *servers) {
    JList *ptr=servers;
    while(ptr) {
        Server *server=(Server*)j_list_data(ptr);
        start_server(server);
        ptr=j_list_next(ptr);
    }
}

/* 读取配置，创建一个服务进程 */
static inline Server *create_server(const jchar *name,JConfObject *root, JConfObject *obj, CLOption *option) {
    jint64 port = j_conf_object_get_integer(obj, "port", 0);
    if(port<=0||port>65536) {
        j_fprintf(stderr, "invalid port in server %s\n", name);
        return NULL;
    }
    jint logfd=-1, error_logfd=-1;
    /* 日志路径无效依然可以创建服务，只是日志功能失效 */
    jchar *log=j_strdup(j_conf_object_get_string_priority(root, obj, "log", LOG_DIR "/" PACKAGE ".log"));
    if(!make_dir(log)||(logfd=append_file(log))<0) {
        j_fprintf(stderr, "fail to open %s\n", log);
    }
    jchar *error_log=j_strdup(j_conf_object_get_string_priority(root, obj, "error_log", LOG_DIR "/" PACKAGE ".err"));
    if(!make_dir(error_log)||(error_logfd=append_file(error_log))<0) {
        j_fprintf(stderr, "fail to open %s\n", error_log);
    }
    Server *server=(Server*)j_malloc(sizeof(Server));
    J_OBJECT_INIT(server, stop_server);
    server->name=j_strdup(name);
    server->port=port;
    server->pid=-1;
    server->log=log;
    server->error_log=error_log;
    server->log_level=j_conf_object_get_integer_priority(root, obj, "log_level", DEFAULT_LOG_LEVEL);
    server->logfd=logfd;
    server->error_logfd=error_logfd;

    return server;
}

JList *load_servers(JConfRoot *root, CLOption *option) {
    if(J_UNLIKELY(all_servers!=NULL)) {
        return all_servers;
    }
    JList *keys=j_conf_object_lookup((JConfObject*)root, "^server-[[:alpha:]][[:alnum:]]*", J_CONF_NODE_TYPE_OBJECT);
    JList *ptr=keys;
    while(ptr) {
        const jchar *key=(const jchar*)j_list_data(ptr);
        Server *server=create_server(key+7,(JConfObject*)root, j_conf_object_get((JConfObject*)root, key), option);
        if(server) {
            all_servers=j_list_append(all_servers, server);
        }
        ptr=j_list_next(ptr);
    }
    j_list_free(keys);
    atexit(stop_all);

    return all_servers;
}

static inline jboolean start_server(Server *server) {
    server->pid=fork();
    if(server->pid<0) {
        return FALSE;
    } else if(server->pid>0) {
        /* 主进程 */
        return TRUE;
    }
    j_usleep(5000000);
    exit(0);
}

/* 关闭一个服务进程 */
static void stop_server(Server *server) {
    j_free(server->name);
    j_free(server->log);
    j_free(server->error_log);
    close(server->logfd);
    close(server->error_logfd);
    if(server->pid>0) {
        kill(server->pid, SIGINT);
    }
}

static void stop_all(void) {
    JList *ptr=all_servers;
    while(ptr) {
        Server *server=(Server*)j_list_data(ptr);
        J_OBJECT_UNREF(server);
        ptr=j_list_next(ptr);
    }
    j_list_free(all_servers);
    all_servers=NULL;
}

void wait_all(JList *servers) {
    jint status;
    pid_t pid;
    while((pid=j_wait(&status))>0) {
        JList *ptr=servers;
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

void dump_server(Server *server) {
    j_printf("\"%s\" listens on port %u\n", server->name, server->port);
    j_printf("  log: %s\n",server->log);
    j_printf("  error_log: %s\n",server->error_log);
    j_printf("  log_level: ");
    if(server->log_level&J_LOG_LEVEL_INFO) {
        j_printf("INFO|");
    }
    if(server->log_level&J_LOG_LEVEL_DEBUG) {
        j_printf("DEBUG|");
    }
    if(server->log_level&J_LOG_LEVEL_ERROR) {
        j_printf("ERROR|");
    }
    if(server->log_level&J_LOG_LEVEL_WARNING) {
        j_printf("WARNING");
    }
    j_printf("\n");
}
