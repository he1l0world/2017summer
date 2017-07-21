/*************************************************************************
	> File Name: 1_3.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月17日 星期一 10时30分15秒
 ************************************************************************/

#include<stdio.h>
int main ()
{
    int a,b;
    int i=0,sum[10000];
    while(scanf("%d %d",&a,&b)!=EOF&&(a!=0||b!=0))
         sum[i++]=a+b;
    for(int j=0;j<i;j++)
         printf("%d\n",sum[j]);
    return 0;
}
