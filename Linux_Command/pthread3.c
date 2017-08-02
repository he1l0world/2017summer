/*************************************************************************
	> File Name: pthread3.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月02日 星期三 09时17分08秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

pthread_key_t key;
void *thread3(void *arg)
{
    int thid = 5;
    pthread_setspecific(key,(void *)thid);
    printf("pthread ID %d return %d\n",pthread_self(),pthread_getspecific(key));
}

void *thread2(void *arg)
{
    int thid = 10;
    pthread_setspecific(key,(void *)thid);
    printf("Pthread ID %d return %d\n",pthread_self(),pthread_getspecific(key));
}
void *thread1(void *arg)
{
    int thid =15;
    pthread_setspecific(key,(void *)thid);
    printf("Pthread ID %d return %d\n",pthread_self(),pthread_getspecific(key));
}

int main ()
{
    pthread_t thid1,thid2,thid3;
    pthread_key_create(&key,NULL);
    pthread_create(&thid1,NULL,thread1,NULL);
    pthread_create(&thid2,NULL,thread2,NULL);
    pthread_create(&thid3,NULL,thread3,NULL);
    sleep(3);
    pthread_key_delete(key);
    return 0;
}
