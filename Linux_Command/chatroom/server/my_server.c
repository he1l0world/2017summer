/*************************************************************************
	> File Name: my_server.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月08日 星期二 19时30分00秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<errno.h>
#include"server_base.h"
#define LISTENQ 20
#define INVALID_USERINFO 'n'
#define VALID_USERINFO 'y'


int main ()
{
   int sock_fd ,conn_fd;
    int optval;
    int ret;
    pthread_t *thread;
    socklen_t cli_len;
    struct sockaddr_in serv_addr;
    char recv_buf[128];
    sock_fd = socket(AF_INET , SOCK_STREAM , 0);//创建套接字
    if(sock_fd < 0 )
        my_err("socket" ,__LINE__);
    optval = 1;
    if(setsockopt(sock_fd , SOL_SOCKET,SO_REUSEADDR,(void*)&optval ,sizeof(int)) < 0)//设置套接字可复用
        my_err("setsockopt" ,__LINE__);

    //初始化服务端地址结构
    memset(&serv_addr , 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(4507);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sock_fd , (struct sockaddr*)&serv_addr,sizeof(struct sockaddr_in)) < 0)//绑定套接字到地址
        my_err("bind",__LINE__);
    
    //套接字转化为监听
    if(listen(sock_fd ,LISTENQ) < 0)
    {
        my_err("listen", __LINE__);
    }
    cli_len = sizeof(struct sockaddr_in);
    thread = (pthread_t*)malloc(sizeof(pthread_t)*65535);
    while(1)
    {
        int i = 0;
        conn_fd = accept(sock_fd , (struct sockaddr*)&serv_addr ,&cli_len);
        if(conn_fd < 0)
            my_err("accept" ,__LINE__);
        pthread_create(&thread[i],NULL,do_something,NULL);//创建子线程处理刚刚接收的请求
        pthread_join(thread[i],NULL);
        i++;
    }
    return 0;
}
