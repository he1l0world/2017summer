/*************************************************************************
	> File Name: my_kill.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月02日 星期三 17时33分54秒
 ************************************************************************/

#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<sys/types.h>
#include<string.h>

int main (int argc,char **argv)
{
    int i,j;
    int signum = SIGTERM;//默认发送SIGTERM
    pid_t pid ;

    /*首先检查参数个数*/
    if(argc != 2 && argc != 4)
    {
        printf("Usage : ./my_kill <-s signum> [PID]\n");
        exit(0);
    }

    /*从命令行参数解析出信号编号*/
    for(i =1 ; i < argc ; i++)
    {
        if(!strcmp(argv[i] , "-s"))
        {
            signum = atoi(argv[i+1]);
            break;
        }
    }

    /*解析出进程号*/
    if(argc == 2)
    {
        pid = atoi(argv[i]);
    }
    else
    {
        for(j = 1 ; j < argc ;j++)
        {
            if(j != i && j != i+1)
            {
                pid = atoi(argv[j]);
                break;
            }
        }
    }

    if(kill(pid , signum) < 0)
    {
        perror("kill");
        exit(1);
    }
    return 0;
}
