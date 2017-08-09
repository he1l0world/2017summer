/*************************************************************************
	> File Name: base.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月08日 星期二 11时21分02秒
 ************************************************************************/
#define base_c
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include"base.h"

void my_err(const char *err_string ,int line)
{
    fprintf(stderr , "line :%d",line);
    perror(err_string);
    exit(1);
}

int my_recv(int conn_fd , struct node Buf ,int len)
{
    static char  recv_buf[BUFSIZE];
    static char *pread;
    static int len_remain = 0;
    int i;
    if(len_remain <= 0)
    {
        if((len_remain = recv(conn_fd , recv_buf , sizeof(recv_buf) , 0)) < 0)
            my_err("recv" , __LINE__);   
        else if(len_remain == 0)
            return 0; //目的计算机端口关闭
         pread = recv_buf;//初始化pread指针
    }

    //从缓存中读取数据
    for(i = 0 ; *pread != '\0' ; i++)
    {
        if(i > len)
            return -1; //防止越界
        Buf.buf[i] = *pread++;

        len_remain--;
    }
    len_remain--;
    pread++;
}

void send_data(int conn_fd , struct node buf , int len)
{
    switch(buf.type)
    {
        
    }
    if(send(conn_fd , buf ,sizeof(buf) , 0) < 0)
        my_err("send" , __LINE__);
}
