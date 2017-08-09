/*************************************************************************
	> File Name: client_base.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月08日 星期二 14时30分03秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include"UI.h"
#include"base.h"
#include"client_base.h"    
#define INVALID_USERINFO  'n'
#define VALID_USERINFO  'y'

void sign_up(int conn_fd , struct user *userinfo , struct sockaddr_in serv_addr)
{
    system("clear");
    printf("请输入帐号:  ");
    scanf("%s",userinfo->username);
    printf("请输入密码:  ");
    scanf("%s",userinfo->password);
    if(send(conn_fd , userinfo->username,strlen(userinfo->username) , 0) < 0)
        my_err("send", __LINE__);
    if(send(conn_fd , userinfo->password,strlen(userinfo->password) ,0) < 0)
        my_err("send" , __LINE__);
    return ;
}

int input_userinfo(int conn_fd , const char *string)
{
    char input_buf[32];
    char recv_buf[32];
    char flag_userinfo;
    if(send(conn_fd , string , strlen(string) , 0) < 0)
        my_err("send",__LINE__);
    if(my_recv(conn_fd , recv_buf ,sizeof(recv_buf)) < 0)
    {
        printf("data is too long\n");
        exit(1);
    }
    if(recv_buf[0] == VALID_USERINFO)
        return 1;
    else
        return 0;
        
}
void sign_in(int conn_fd ,struct user *userinfo , struct sockaddr_in serv_addr)
{
    char *buf,*rec_buf;
    int ret,i;
    int flag = 1;
    do
    {
        flag = 1;
        system("clear");
        printf("请输入帐号:  ");
        scanf("%s",userinfo->username);
        printf("请输入密码:  ");
        scanf("%s",userinfo->password);
        if(connect(conn_fd , (struct sockaddr*)&serv_addr , 0) < 0)
            my_err("connect" , __LINE__);
        if(input_userinfo(conn_fd , userinfo->username ) == 0)
            flag = 0;
        if(input_userinfo(conn_fd ,userinfo->password ) == 0)
            flag = 0;
        if(flag == 0)
        {
            printf("帐号或者密码错误请重新输入！\n");
        } 
    }while(flag);
    if((ret = my_recv(conn_fd ,buf , sizeof(buf))) < 0)
        my_err("recv", __LINE__);
    for(i = 0; i < ret ;i++)
    {
        printf("%c",buf[i]);
    }
    printf("\n");
    main_UI();
    return ;
}


