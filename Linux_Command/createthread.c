/*************************************************************************
	> File Name: createthread.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月31日 星期一 11时28分54秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

int *thread(void*arg)
{
    pthread_t newthid;

    newthid = pthread_self();
    printf("this is a new thread , thread ID = %u\n",newthid);
    return NULL;
}

int main (void)
{
    pthread_t thid;

    printf("main thread , ID is %u\n",pthread_self());//打印主线程id
    if(pthread_create(&thid, NULL ,(void *)thread, NULL) !=0 )
    {
        printf("thread creation failed\n");
        exit(1);
    }
    sleep(1);
    exit(0);
}
