/*************************************************************************
	> File Name: procwrite.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月06日 星期日 11时55分43秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#define FIFO_NAME "myfifo"
#define BUF_SIZE 1024

int main ()
{
    int fd;
    char buf [BUF_SIZE] = "Hello procread , I come from process named procwrite!";
    umask(0);

    if(mkfifo(FIFO_NAME , 0010000 | 0666) == -1)
    /*S_IFIFO 等价于0010000 */
    {
        perror("mkfifo error!");
        exit(1);
    }

    if((fd = open(FIFO_NAME , O_WRONLY)) == -1)/*以写方式打开FIFO*/
    {
        perror("fopen error!");
        exit(1);
    }
    write(fd , buf , strlen(buf)+1);/*向FIFO写数据*/

    close(fd);
    exit(0);
}
