/*************************************************************************
	> File Name: 8_2.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月24日 星期一 12时05分50秒
 ************************************************************************/

#include<stdio.h>
int gcd(int m,int n)
{
    int b;
    b=1;
    while(b!=0)
    {
        b=m%n;
        if(b==0)
            break;
        m=n;
        n=b;
    }
    return n;
}
int main ()
{
    int n;
    long long a,b,c,d;
    scanf("%d",&n);
    while(n--)
    {
       scanf("%lld/%lld %lld/%lld",&a,&b,&c,&d);
        long long m=gcd(a,b);
        long long t=gcd(c,d);
        a/=m;b/=m;c/=t;d/=t;
        long long y=a/m*c;
        if(gcd(b,d)==1)
            printf("%lld\n",a/gcd(a,c)*c);
        else
            printf("%lld/%lld\n",a/gcd(a,c)*c,gcd(b,d));
    }
    return 0;
}
