/*
 * 测试用模块
 * the module for testing
 */
#include <jmod/jmod.h>
#include <jio/jio.h>
#include <jconf/jconf.h>
#include <stdio.h>

static char *string=NULL;

static void on_recv(JSocket *conn,const void *data,unsigned int len,
                    JModuleAction *act)
{
    JByteArray *array=j_module_action_get_byte_array(act);
    j_byte_array_append(array,string,j_strlen(string));
    j_module_action_set_type(act,J_MODULE_ACTION_SEND|J_MODULE_ACTION_RECV);
    j_mod_log(J_LOG_LEVEL_INFO,"echo %s",string);
}

static void on_recv_error(JSocket *conn,const void *data, unsigned int len)
{
    j_mod_log(J_LOG_LEVEL_WARNING,"%s closed",j_socket_get_peer_name(conn));
}

static inline const char *get_config_string(JConfNode *node)
{
    if(j_conf_node_is_object(node)){
        return j_conf_object_get_string(node,"String","hello world!");
    }else if(j_conf_node_is_string(node)){
        return j_conf_string_get(node);
    }
    return "hello world!";
}

void init(JConfNode *r,JConfNode *n)
{
    string=j_strdup(get_config_string(n));
    j_mod_log(J_LOG_LEVEL_INFO,"hello world!");
    j_mod_register_hook(J_HOOK_RECV,on_recv);
    j_mod_register_hook(J_HOOK_RECV_ERROR,on_recv_error);
}

JModule module_struct={
    "hello",
    init,
};
