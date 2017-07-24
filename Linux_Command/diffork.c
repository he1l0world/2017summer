/*************************************************************************
	> File Name: diffork.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月24日 星期一 15时24分25秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>

int globvar=5;
int main ()
{
    pid_t pid;
    int var=1,i;

    printf("fork is diffirent with vfork \n");

    pid = fork();
    //pid = vfork();
    switch(pid)
    {
        case 0:
            i=3;
            while(i--)
            {
                printf("Child process is running\n");
                globvar++;
                var++;
                sleep(1);
            }
            printf("Child's globvar = %d,var = %d\n",globvar,var);
            break;
        case -1:
            perror("Process Creation failed\n");
            exit(0);
        default:
            i=5;
            while(i--)
            {
                printf("Parent Process is running\n");
                globvar++;
                var++;
                sleep(1);
            }
            printf("Parent's globvar = %d,var = %d\n",globvar,var);
            break;
    }
    return 0;
}
