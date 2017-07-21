/*************************************************************************
	> File Name: 1_7.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月17日 星期一 10时56分46秒
 ************************************************************************/

#include<stdio.h>
int main ()
{
    int a,b;
    int i=0,sum[10000]={0};
    while(scanf("%d %d",&a,&b)!=EOF)
        sum[i++]=a+b;
    for(int j=0;j<i;j++)
        printf("%d\n\n",sum[j]);
    return 0;
}
