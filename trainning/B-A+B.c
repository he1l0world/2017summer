/*************************************************************************
	> File Name: B-A+B.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月17日 星期一 10时18分38秒
 ************************************************************************/

#include<stdio.h>
int main ()
{
    int n,a,b,sum[10000];
    scanf("%d",&n);
    int i=0;
    while(n--)
    {
        scanf("%d %d",&a,&b);
        sum[i++]=a+b;
    }
    for(int j=0;j<i;j++)
        printf("%d\n",sum[j]);
    return 0;
}
