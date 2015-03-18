#include "../src/net.h"
#include <stdio.h>
#include <stdlib.h>
#include <jlib/jlib.h>

#define PORT 22222

static void recv_callback (JSocket * sock, const void *data, unsigned int len,
                           JSocketRecvResultType type, void *user_data)
{
    if(type!=J_SOCKET_RECV_ERR && len>0 && data!=NULL){
        char *buf =j_strndup((const char*)data,len);
        printf("%s\n",buf);
        j_free(buf);
    }else{
        printf("recv error!\n");
    }
    j_main_quit();
}

static void send_callback(JSocket *sock,int res, void *user_data)
{
    if(res){
        printf("success!\n");
        j_socket_recv_package(sock,recv_callback,user_data);
    }else{
        printf("fail!\n");
        j_main_quit();
    }
}

int main(int argc, char *argv[])
{
    JSocket *client = j_socket_connect_to("127.0.0.1","23456");
    if(client==NULL){
        printf("fail to connect to server!\n");
        return -1;
    }
    j_socket_send_package(client,send_callback,"hello world",11,NULL);
    
    j_main();
    
    j_socket_close(client);
    return 0;
}
