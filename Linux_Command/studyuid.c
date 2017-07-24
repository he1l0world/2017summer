/*************************************************************************
	> File Name: studyuid.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月24日 星期一 18时02分49秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>

extern int errno;

int main ()
{
    int fd;

    printf("uid study: \n");
    printf("Process's uid = %d,euid= %d\n",getuid(),geteuid());
    //strerror函数获取错误码信息
    if((fd=open("test.txt",O_RDWR)) == -1)
    {
        printf("Open failure,errno is %d : %s\n",errno,strerror(errno));
        exit(1);
    }
    printf("Open successfully\n");
    close(fd);
    exit(0);
}
