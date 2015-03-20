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
    }
    j_main_quit();
    j_socket_close(sock);
}

static int accept_callback(JSocket *sock,JSocket *conn,void *user_data)
{
    if(conn==NULL){
        printf("accept error\n");
        j_main_quit();
    }else{
        printf("accepted\n");
        j_socket_recv_package(conn,recv_callback,NULL);
    }
    return 0;
}

static void send_callback(JSocket *sock,const char *data,unsigned int count ,unsigned int len, void *user_data)
{
    if(count==len){
        printf("success!\n");
    }else{
        printf("fail!\n");
    }
}

int main(int argc, char *argv[])
{
    JSocket *listen = j_socket_listen_on(PORT,32);
    if(listen==NULL){
        printf("fail to listen on port %d\n",PORT);
        return 1;
    }
    j_socket_accept_async(listen,accept_callback,NULL);
    
    JSocket *client = j_socket_connect_to("127.0.0.1","22222");
    j_socket_send_package(client,send_callback,"hello world",11,NULL);
    
    j_main();
    
    j_socket_close(client);
    j_socket_close(listen);
    return 0;
}
