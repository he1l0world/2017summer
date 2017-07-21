/*************************************************************************
	> File Name: 1_6.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月17日 星期一 10时52分16秒
 ************************************************************************/

#include<stdio.h>
int main ()
{
    int n,m,sum[10000]={0},a,s=0;
    while(scanf("%d",&m)!=EOF)
    {
        for(int i=0;i<m;i++)
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

