/*************************************************************************
	> File Name: my_cd.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月21日 星期五 12时36分37秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<linux/limits.h>

void my_err(const char *err_string,int line)
{
    fprintf(stderr,"line:%d ",line);
    perror("err_string");
    exit(1);
}

int main (int argc,char** argv)
{
    char buf[PATH_MAX +1];

    if(argc<2)
    {
        printf("my_cd <target path>\n");
        exit(1);
    }

    if(chdir(argv[1]) < 0)
        my_err("chdir",__LINE__);
    if(getcwd(buf,512) < 0)
        my_err("getcwd",__LINE__);
    printf("%s\n",buf);

    return 0;
}
