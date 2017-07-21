/*************************************************************************
	> File Name: test_umask.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月21日 星期五 10时53分35秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main ()
{
    umask(0);//没有屏蔽任何权限
    if(creat("test1.txt",S_IRWXU|S_IRWXG|S_IRWXO) < 0)
    {
        perror("creat");
        exit(1);
    }
    umask(S_IRWXO);//屏蔽其他用户的所有权限
    if(creat("test2.txt",S_IRWXU|S_IRWXG|S_IRWXO) < 0)
    {
        perror("creat");
        exit(1);
    }
    return 0;
}
