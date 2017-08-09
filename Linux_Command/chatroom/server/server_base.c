/*************************************************************************
	> File Name: server_base.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月08日 星期二 19时43分45秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"server_base.h"


void my_err(const char *err_string ,int line)
{
    fprintf(stderr , "line %d ",line);
    perror(err_string);
    exit(1);
}

int check_account(struct node Buf)
{
    int i;
    if(Buf.users.username == NULL || Buf.users.password == NULL)
    {
        printf("in find_name, NULL pointer");
        return -2;
    }
    /*连接myqsl查找用户名密码 */
}


void send_data(int conn_fd , struct node Buf)
{
    
}
void *do_something(void *arg)
{
    
}
