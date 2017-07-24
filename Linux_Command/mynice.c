/*************************************************************************
	> File Name: mynice.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月24日 星期一 21时53分35秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/resource.h>
#include<sys/wait.h>
#include<sys/types.h>

int main ()
{
    pid_t pid;
    int stat_val=0;
    int  oldpri,newpri;

    printf("nice study\n");

    pid=fork();
    switch(pid)
    {
        case 0:
            printf("Child is running,Curpid is %d,Parentpid is %d\n",pid,getpid());
            oldpri=getpriority(PRIO_PROCESS,0);
            printf("old priority = %d\n",oldpri);
            newpri = nice(2);
            printf("New priority = %d\n",newpri);
            exit(0);
        case -1:
            perror("Process Creation failed\n");
            break;
        default:
            printf("Parent is running ,Childpid is %d ,Parentpid is %d\n",pid,getpid());
            break;
    }
    wait(&stat_val);
    exit(0);
}
