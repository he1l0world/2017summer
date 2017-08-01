/*************************************************************************
	> File Name: 9_1.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月25日 星期二 08时29分37秒
 ************************************************************************/

#include<stdio.h>
int f(int m,int n)
{
    int b=1;
    while(b!=0)
    {
        b=m%n;
        if(b<=0)
            break;
        m=n;
        n=b;
    }
    return n;
}
int main ()
{
    int n,a,b,c;
    scanf("%d",&n);
    while(n--)
    {
        scanf("%d %d",&a,&b);
        for(int i=2*b;i<=a;i+=b)
            if(f(a,i)==b)
            {c=i;break;}
        printf("%d\n",c);
    }
    return 0;
}
