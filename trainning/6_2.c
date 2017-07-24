/*************************************************************************
	> File Name: 6_2.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月22日 星期六 09时08分00秒
 ************************************************************************/

#include<stdio.h>
int main()
{
    int n,a,b;
    scanf("%d",&n);
    while(n--)
    {
        int sum;
        scanf("%d %d",&a,&b);
        a=a%100;
        b=b%100;;
        sum=a+b;
        sum=sum%100;
        printf("%d\n",sum);
    }
    return 0;
}
