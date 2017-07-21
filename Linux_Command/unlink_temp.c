/*************************************************************************
	> File Name: unlink_temp.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月21日 星期五 12时09分13秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

void my_err(const char *err_string,int line)
{
    fprintf(stderr,"line:%d ",line);
    perror(err_string);
    exit(1);
}
int main ()
{
    int fd;
    char buf[32];

    if((fd=open("test.txt",O_CREAT|O_RDWR|O_TRUNC,S_IRWXU)) < 0)
        my_err("open",__LINE__);
    if(unlink("test.txt") < 0)
        my_err("unlink",__LINE__);
    if(write(fd,"temp",5) < 0)
        my_err("write",__LINE__);
    if(lseek(fd,0,SEEK_SET) == -1 )
        my_err("lseek",__LINE__);
    if(read(fd,buf,5) < 0)
        my_err("read",__LINE__);
    printf("%s\n",buf);
    return 0;
}
