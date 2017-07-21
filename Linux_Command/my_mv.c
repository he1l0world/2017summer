/*************************************************************************
	> File Name: my_mv.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月21日 星期五 11时23分31秒
 ************************************************************************/

#include<stdio.h>

int main (int argc,char **argv)
{
    if(argc < 3)
    {
        printf("my_mv < old file> <new file>\n");
        exit(0);
    }
    if(rename(argv[1],argv[2]) < 0)
    {
        perror("rename");
        exit(1);
    }
    return 0;
}
