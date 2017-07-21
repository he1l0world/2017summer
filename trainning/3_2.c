/*************************************************************************
	> File Name: 3_2.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月19日 星期三 09时39分21秒
 ************************************************************************/

#include<stdio.h>
long long  f(long long m,long long n)
{
    long long b=1,s=m*n;
    while(b>0)
    {
        b=m%n;
        if(b<=0)
            break;
        m=n;
        n=b;
    }
    return s/n;
}
int main ()
{
    long long n,a[10000];
    while(scanf("%lld",&n)!=EOF)
    {
        for(int i=0;i<n;i++)
        {
            scanf("%lld",&a[i]);
        }
        for(int i=0;i<n-1;i++)
        {
            a[i+1]=f(a[i],a[i+1]);
            //printf("%d\n",a[i+1]);
        }
        printf("%lld\n",a[n-1]);
    }
    return 0;
}
