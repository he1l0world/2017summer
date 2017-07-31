/*************************************************************************
	> File Name: jointhread.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月31日 星期一 14时19分23秒
 ************************************************************************/

#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

void assisthread(void *arg)
{
    printf("I am helping to do some jobs \n");
    sleep(3);
    pthread_exit(0);
}

int main (void )
{
    pthread_t assistthid;
    int status;

    pthread_create(&assistthid,NULL,(void *)assisthread,NULL);
    pthread_join(assistthid,(void *)&status);
    printf("assisthread's exit is caused %d\n",status);

    return 0;
}
