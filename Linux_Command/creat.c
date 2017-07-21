/*************************************************************************
	> File Name: creat.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月20日 星期四 11时07分40秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>

int main ()
{
    int fd;
    
    //if( (fd = open("test.txt",O_CREAT | O_EXCL,S_IRUSR | S_IWUSR )) == -1 )
    if((fd = creat("test.txt",S_IWUSR)) == -1 )
    {
        perror("open");
        exit(1);
    }
    else
        printf("Create file success!\n");
    close(fd);
    return 0;
}
