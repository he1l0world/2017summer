/*************************************************************************
	> File Name: fork.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月24日 星期一 14时12分45秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main ()
{
    pid_t pid;
    
    printf("Process Creation Study\n");
    pid =fork();
    switch(pid)
    {
        case 0:
        printf("Child process is running,Curpid is %d,Parentpid is %d\n",pid,getppid());
        break;
        case -1:
            printf("Process Creation failed\n");
        break;
        default:
        printf("Parent process is running ,Childpid is %d,Parentpid is %d\n",pid,getpid());
        break;
    }
    return 0;
}
