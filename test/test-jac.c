#include <stdio.h>
#include <stdlib.h>
#include <jlib/jlib.h>
#include <jio/jio.h>

#define PORT 22222

static void recv_callback (JSocket * sock, const void *data, unsigned int len,
                            void *user_data)
{
    char *buf =j_strndup((const char*)data,len);
    printf("%s\n",buf);
    j_free(buf);
    j_main_quit();
}

static void recv_error_callback(JSocket *sock, const void *data,unsigned int len, void *user_data)
{
    printf("recv error!\n");
    j_main_quit();
}

static void send_callback(JSocket *sock,const char *data, unsigned int count, void *user_data)
{
        printf("success!\n");
        j_socket_recv_package(sock,recv_callback,recv_error_callback,user_data);
}

static void send_error_callback(JSocket *sock,const char *data, unsigned int count,unsigned int len, void *user_data)
{
    printf("fail!\n");
    j_main_quit();
}

int main(int argc, char *argv[])
{
    JSocket *client = j_socket_connect_to("127.0.0.1","23456");
    if(client==NULL){
        printf("fail to connect to server!\n");
        return -1;
    }
    j_socket_send_package(client,send_callback,send_error_callback,"hello world",11,NULL);
    
    j_main();
    
    j_socket_close(client);
    return 0;
}
