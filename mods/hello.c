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
    j_mod_log(J_LOG_LEVEL_WARNING,"%s closed",j_socket_get_peer_name(conn));
}

void init(void)
{
    j_mod_log(J_LOG_LEVEL_INFO,"hello world!");
    j_mod_register_hook(J_HOOK_RECV,on_recv);
    j_mod_register_hook(J_HOOK_RECV_ERROR,on_recv_error);
}

JModule module_struct={
    "hello",
    init,
    NULL
};
