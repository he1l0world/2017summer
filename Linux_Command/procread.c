/*************************************************************************
	> File Name: procread.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月06日 星期日 11时50分11秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/types.h>
#include<stdlib.h>
#define FIFO_NAME "myfifo"
#define BUF_SIZE 1024

int main ()
{
    int fd;
    char buf[BUF_SIZE];

    umask(0);
    fd = open(FIFO_NAME , O_RDONLY);
    read(fd , buf , BUF_SIZE);
    printf("Read content :%s\n",buf);
    close(fd);
    exit(0);
}
