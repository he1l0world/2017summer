/*************************************************************************
	> File Name: 6_3.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月22日 星期六 09时22分01秒
 ************************************************************************/

#include<stdio.h>
int main ()
{
    int a,b,c,d,e;
    int sum=0,t=1;
    scanf("%d %d %d %d %d",&a,&b,&c,&d,&e);
    for(int i=b;;i+=d)
    {
        if(i>=c)
            i=c;
        sum+=i;
        if(sum>=a)
            break;
        sum-=e;
        t++;
    }
    printf("%d\n",t);
    return 0;
}
