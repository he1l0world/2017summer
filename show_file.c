/*************************************************************************
	> File Name: show_file.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月21日 星期五 13时01分28秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>
#include<unistd.h>

int my_readir(const char * path)
{
    DIR *dir;
    struct dirent *ptr;

    if((dir=opendir(path)) == NULL)
    {
        perror("opendir");
        return -1;
    }
    while((ptr=readdir(dir)) != NULL)
        printf("file name :%s\n",ptr->d_name);
    closedir(dir);

    return 0;
}

int main (int argc,char** argv)
{
    if(argc<2)
    {
        printf("listfile <target path>\n");
        exit(1);
    }
    if(my_readir(argv[1]) < 0)
        exit(1);
    return 0;
}
