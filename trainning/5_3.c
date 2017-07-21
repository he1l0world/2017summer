/*************************************************************************
	> File Name: 5_3.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月21日 星期五 10时05分49秒
 ************************************************************************/

#include<stdio.h>
int main ()
{
    int n,a;
    scanf("%d",&n);
    while(n--)
    {
        int m=3;
        scanf("%d",&a);
        for(int i=1;i<=a;i++)
            m--,m*=2;
        printf("%d\n",m);
    }
    return 0;
}
