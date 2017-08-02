/*************************************************************************
	> File Name: pthread1.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月02日 星期三 08时08分53秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#define N 10000

int a=0;
pthread_mutex_t mutex;
void *thread3(void *arg)
{
    int n = N ;
    while(n--)
    {
        pthread_mutex_lock(&mutex);
        a++;
        pthread_mutex_unlock(&mutex);
        printf("thread3 : %d\n",a);
    }
}

void *thread2(void *arg)
{
    int n = N ;
    while(n--)
    {   
        pthread_mutex_lock(&mutex);
        a++;
        pthread_mutex_unlock(&mutex);
        printf("thread2 : %d\n",a);
    }
}

void *thread1(void *arg)
{
    int n = N;
    while(n--)
    {
        pthread_mutex_lock(&mutex);
        a++;
        pthread_mutex_unlock(&mutex);
        printf("thread1 : %d\n",a);
    }
}
void thread_create()
{
    pthread_mutex_init(&mutex,NULL);
    pthread_t thid1,thid2,thid3;
    pthread_create(&thid1,NULL,thread1,NULL);
    pthread_create(&thid2,NULL,thread2,NULL);
    pthread_create(&thid3,NULL,thread3,NULL);
    sleep(3);
}

int main ()
{
    thread_create();
    printf("final end : %d\n",a);
}
