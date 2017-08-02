/*************************************************************************
	> File Name: my_sigaction.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月02日 星期三 15时45分40秒
 ************************************************************************/

#include<stdio.h>
#include<signal.h>
#include<unistd.h>

int  temp = 0 ;

/*信号处理函数*/
void handler_sigint(int signo)
{
    printf("recv SIGINT \n");
    sleep(5);
    temp +=1;
    printf("the value of temp is :%d \n",temp);
    printf("in handler_sigint ,after sleep\n");
}

int main ()
{
    struct sigaction act;

    /*赋值act结构*/
    act.sa_handler = handler_sigint;
    act.sa_flags = SA_NOMASK;
    /*安装信号处理函数*/
    sigaction(SIGINT , &act , NULL);
    while(1);
    return 0;
}
