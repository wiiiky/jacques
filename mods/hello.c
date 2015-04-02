/*
 * 测试用模块
 * the module for testing
 */
#include <jmod/jmod.h>
#include <jio/jio.h>
#include <stdio.h>

static void on_recv(JSocket *conn,const void *data,unsigned int len,
                    JModuleRecv *r)
{
    JByteArray *array=j_module_recv_get_byte_array(r);
    j_byte_array_append(array,data,len);
    j_module_recv_set_action(r,J_MODULE_RECV_SEND);
    j_mod_log(J_LOG_LEVEL_INFO,"hello echoing");
}

static void on_recv_error(JSocket *conn,const void *data, unsigned int len)
{
    j_mod_log(J_LOG_LEVEL_WARNING,"recv error from: %s",j_socket_get_peer_name(conn));
}

void init(void)
{
    j_mod_log(J_LOG_LEVEL_INFO,"hello world!");
    j_mod_register_hook(J_HOOK_RECV,on_recv);
    j_mod_register_hook(J_HOOK_RECV_ERROR,on_recv_error);
}

void config_init(void)
{
    j_mod_log(J_LOG_LEVEL_WARNING,"config init");
}

void config_summary(void)
{
    j_mod_log(J_LOG_LEVEL_DEBUG,"config summary");
}

void config_load(const char *scope,JList *sargs,const char *directive,JList *dargs)
{
    j_mod_log(J_LOG_LEVEL_WARNING,"%s/%s",scope?scope:"NULL",directive);
}

static char *directives[]={"Hello","World",NULL};

JModuleConfigHandler config_handler={
    directives,
    NULL,
    config_init,
    config_load,
    config_summary
};

JModule module_struct={
    "hello",
    &config_handler,
    init,
    NULL
};
