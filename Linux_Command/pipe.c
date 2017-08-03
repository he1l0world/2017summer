/*************************************************************************
	> File Name: pipe.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月03日 星期四 19时34分41秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
/*读管道*/
void read_from_pipe(int fd)
{
    char message[100];
    read(fd,message,100);
    printf("read from pipe :%s",message);
}

/*写管道*/
void write_to_pipe(int fd)
{
    char *message = "Hello,pipe!\n";
    write(fd,message,strlen(message)+1);
}

int main (void)
{
    int fd[2];
    pid_t pid;
    int sta_val;

    if(pipe(fd))
    {
        printf("create pipe failed!\n");
        exit(1);
    }
    
    pid = fork();
    switch (pid)
    {
        case -1:
            printf("fork error!\n");
            exit(1);
            break;
        case 0:
            /*子进程关闭fd[1]*/
            close(fd[1]);
            read_from_pipe(fd[0]);
            exit(0);
        default:
            /*父进程关闭fd[0]*/
            close(fd[0]);
            write_to_pipe(fd[1]);
            wait(&sta_val);
            exit(0);
    }
}
