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

#include "mod.h"
#include <dlfcn.h>
#include <stdio.h>
#include "list.h"


/* 所有模块的链表 */
static DList *modules=NULL;

/* 不同回调函数的链表 */
static DList *accept_hooks = NULL;
static DList *recv_hooks = NULL;
static DList *exit_hooks = NULL;

/* 返回记录所有模块的链表 */
DList *all_modules(void) {
    return modules;
}

/* 调用相关的回调函数 */
int call_accept_hooks(Socket *socket) {
    DList *ptr=accept_hooks;
    while(ptr) {
        JacAcceptHook cb=(JacAcceptHook)dlist_data(ptr);
        if(!cb(socket)) {
            return 0;
        }
        ptr=dlist_next(ptr);
    }
    return 1;
}

int call_recv_hooks(Socket *socket, const void *buf, unsigned int len) {
    DList *ptr=recv_hooks;
    while(ptr) {
        JacRecvHook cb = (JacRecvHook)dlist_data(ptr);
        if(cb(socket, buf, len)<=0) {
            return 0;
        }
        ptr=dlist_next(ptr);
    }
    return 1;
}

void call_exit_hooks(void){
    DList *ptr=exit_hooks;
    while(ptr) {
        JacExitHook cb = (JacExitHook)dlist_data(ptr);
        cb();
        ptr=dlist_next(ptr);
    }
}

/*
 * 打开模块文件
 */
static inline void *dl_open(const char *filename);
static inline void dl_close(void *handle);
static inline void *dl_symbol(void *handle, const char *symbol);

/* 记录模块信息 */
static inline void register_module(JacModule *mod) {
    if(mod==NULL) {
        return;
    }
    modules = dlist_append(modules, mod);
    JacHook *hook=mod->hook;
    if(hook) {
        if(hook->accept) {
            accept_hooks=dlist_append(accept_hooks, hook->accept);
        }
        if(hook->recv) {
            recv_hooks=dlist_append(recv_hooks, hook->recv);
        }
        if(hook->exit){
            exit_hooks=dlist_append(exit_hooks, hook->exit);
        }
    }
}

/*
 * 载入指定模块
 */
JacModule *load_module(const char *filename) {
    void *handle = dl_open(filename);
    if(handle==NULL) {
        return NULL;
    }

    JacModule *mod=NULL;
    JacModuleInit *ptr=(JacModuleInit*)dl_symbol(handle, JACQUES_MODULE_NAME_STRING);
    if(ptr==NULL||(mod = (*ptr)())==NULL) {
        goto OUT;
    }
OUT:
    dl_close(handle);
    register_module(mod);
    return mod;
}

static inline void *dl_symbol(void *handle, const char *symbol) {
    dlerror();  /*  Clear any existing error */
    void *ptr=dlsym(handle, symbol);
    return ptr;
}

static inline void *dl_open(const char *filename) {
    int flags = RTLD_LAZY|RTLD_NODELETE;
    void *handle=dlopen(filename, flags);
    if(handle!=NULL) {
        return handle;
    }
    char buf[1024];
    snprintf(buf, sizeof(buf), "%s.so", filename);
    return dlopen(buf, flags);
}

static inline void dl_close(void *handle) {
    dlclose(handle);
}
