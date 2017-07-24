/*************************************************************************
	> File Name: git2.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月24日 星期一 14时35分41秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main ()
{
    pid_t pid;
    char *msg;
    int k=8;

    printf("Process Creation Study\n");
    pid=fork();
    switch(pid)
    {
        case 0:
            msg="Child process is running";
            k= 3;
            break;
        case -1:
            perror("Process Creation failed\n");
            break;
        default:
            msg="Parent process is running";
            k=5;
            break;
    }
    while(k--)
    {
        puts(msg);
        sleep(1);
    }
    return 0;
}
