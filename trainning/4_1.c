/*************************************************************************
	> File Name: 4_1.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月20日 星期四 08时19分05秒
 ************************************************************************/

#include<stdio.h>
int f(int m)
{
    int a,b,c;
    a=m%10;b=m/10%10;c=m/100%10;
    if(a*a*a+b*b*b+c*c*c==m)
        return 1;
    return 0;
}
int main ()
{
    int m,n;
    while(scanf("%d %d",&m,&n)!=EOF)
    {
        int flag=0;
        for(int i=m;i<=n;i++)
        {
            if(flag==0&&f(i))
            {
                printf("%d",i);
                flag=1;
                continue;
            }
            if(flag&&f(i))
                printf(" %d",i);
        }
        if(flag==0)
                printf("no");
        printf("\n");
    }
    return 0;
}
