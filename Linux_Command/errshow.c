/*************************************************************************
	> File Name: errshow.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月01日 星期二 20时33分03秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>

FILE *open_file (char *filename)
{
    FILE *stream;
    errno = 0;

    stream = fopen(filename , "r");
    if(stream == NULL)
    {
        printf("can not open the file %s. reason: %s",filename,strerror(errno));
        exit(1);
    }
    else
        return stream;
}

int main ()
{
    char *filename = "test";

    open_file (filename);
    return 0;
}
