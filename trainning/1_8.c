/*************************************************************************
	> File Name: 1_8.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月17日 星期一 11时01分35秒
 ************************************************************************/

#include<stdio.h>
int main ()
{
    int n,m,sum[10000]={0},a,s=0;
    scanf("%d",&n);
    while(n--)
    {
        scanf("%d",&m);
        for(int i=0;i<m;i++)
        {
            scanf("%d",&a);
            sum[s]+=a;          
        }
        s++;
    }
    for(int i=0;i<s;i++)
    {
        printf("%d\n",sum[i]);
        if(i<s-1)
            printf("\n");
    }
        return 0;
}

