/*************************************************************************
	> File Name: 1_4.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月17日 星期一 10时36分10秒
 ************************************************************************/

#include<stdio.h>
int sum[10000];
int main ()
{
    int n,a;
    int s=0;
    while(scanf("%d",&n)!=EOF&&n!=0)
    {
        for(int i=0;i<n;i++)
        {
            scanf("%d",&a);
            sum[s]+=a; 
        }
        s++;
    }
    for(int i=0;i<s;i++)
        printf("%d\n",sum[i]);
    return 0;
}
