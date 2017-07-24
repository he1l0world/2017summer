/*************************************************************************
	> File Name: fork3.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月24日 星期一 15时11分00秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main ()
{
    pid_t pid;

    pid = fork();
    switch(pid)
    {
        case 0:
            while(1)
            {
                printf("A backgroud process ,PID :%d,ParentID :%d\n",getpid(),getppid());
                sleep(3);
            }
            break;
        case -1:
            perror("Process Creation failed\n");
            break;
        default:
            printf("I am parent process my pid is %d\n",getpid());
    }
    return 0;
}
