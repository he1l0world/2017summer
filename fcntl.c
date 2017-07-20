/*************************************************************************
	> File Name: fcntl.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月20日 星期四 16时08分12秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>

void my_err(const char * err_string,int line)
{
    fprintf(stderr,"line:%d ",line);
    perror(err_string);
    exit(1);
}

int main()
{
    int ret;
    int access_mode;
    int fd;

    if((fd=open("test.txt",O_CREAT|O_TRUNC|O_RDWR,S_IRWXU)) == -1)
       my_err("open",__LINE__);
    if((ret=fcntl(fd,F_SETFL,O_APPEND)) < 0)
       my_err("fcntl",__LINE__);
    if((ret=fcntl(fd,F_GETFL,0)) < 0)
        my_err("fcntl",__LINE__);
    access_mode = ret & O_ACCMODE;
    if(access_mode == O_RDONLY)
        printf("test.txt access_mode : read only!");
    else if(access_mode == O_WRONLY)
       printf("test.txt access_mode : write only!");
    else if (access_mode == O_RDWR)
        printf("test.txt access_mode : write+read");

    if(ret & O_APPEND)
       printf(" ,append");
    if(ret & O_NONBLOCK)
        printf(" ,nonblock");
    if(ret & O_SYNC)
        printf(" ,sync");
    printf("\n");
    return 0;
}
