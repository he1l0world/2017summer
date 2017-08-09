/*************************************************************************
	> File Name: my_client.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月08日 星期二 14时15分24秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include"client_base.h"
#include"base.h"

int main ()
{
    char *port = "4507";
    char *adr = "127.0.0.1";
    int serv_port = atoi(port);
    struct sockaddr_in serv_addr;
    int conn_fd ;
    USER *userinfo;
    memset(userinfo, 0 ,sizeof(USER));

    conn_fd = socket(AF_INET , SOCK_STREAM , 0);//创建套接字
    memset(&serv_addr , 0 ,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(adr,&serv_addr.sin_addr);
    if(conn_fd < 0)
        my_err("socket" , __LINE__);
    char ch;
    char choice[32];
    while(1)
    {
        do
        {
            fflush(stdin);
            system("clear");
            printf("-----------\n");
            printf(" a:登录\n");
            printf(" b:注册\n");
            printf(" q:退出\n");
            printf("-----------\n");
            printf(" 请选择:  ");
            ch = getchar();
        }while(ch== 'q' || ch == 'Q');
        if(ch == 'q' || ch== 'Q') 
            break;
        switch(ch)
        {
            case 'a':
            case 'A':
                sign_in(conn_fd,userinfo,serv_addr);
                strcpy(choice,"sign_in");
                send(conn_fd,choice,strlen(choice), 0);
                break;
            case 'b':
            case 'B':
                sign_up(conn_fd ,userinfo,serv_addr);
                strcpy(choice,"sign_up");
                send(conn_fd,choice,strlen(choice), 0);
                break;
            default:
                break;
        }
    }
    close(conn_fd);
    return 0;
}


