/*************************************************************************
	> File Name: 3_1.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月19日 星期三 09时07分35秒
 ************************************************************************/

#include<stdio.h>
int f(int n)
{
    int a=1,sum=0;
    if(n<=4)
        return 1;
    else
    {
        while(n-a>=3)
        {
            sum+=f(a);
            a++;
        }
    }
    return sum;
}
int main ()
{
    int n;
    while(scanf("%d",&n)!=EOF&&n!=0)
    {
        int sum=0;
        for(int i=1;i<=n;i++)
            sum+=f(i);
        printf("%d\n",sum);
    }
    return 0;
}
