/*************************************************************************
	> File Name: chmod.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月20日 星期四 09时20分51秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
int main (int argc,char **argv)
{
    int mode;
    int mode_u;
    int mode_g;
    int mode_o;
    char *path;
    if(argc<3)//三个参数一个程序文件名一个mode一个目标文件
    {
        printf("%s<mode num> <target file> \n",argv[0]);
        exit(0);
    }
    mode=atoi(argv[1]);
    if ( mode > 777 || mode < 0 )
    {
        printf("mode num error");
        exit(0);
    }
    mode_u=mode/100;
    mode_g=(mode-(mode_u*100))/10;
    mode_o=mode%10;
    mode=(mode_u*8*8)+(mode_g*8)+mode_o;//转换为八进制
    //printf("%d %d %d",mode_u,mode_g,mode_o);//测试各个数字是否正确
    path = argv[2];

    if(chmod(path,mode)==-1)
    {
        perror("chmod error");
        exit(1);
    }


    return 0;
}
