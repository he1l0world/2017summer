/*************************************************************************
	> File Name: 13_1.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月29日 星期六 15时05分44秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define N 30
#define M 200
int a[N],b[N],y[M][N];
int n,count,u=0;
int prime(int m)
{
    if(m<2)
        return 0;
    if(m==2)
        return 1;
    for(int i=2;i*i<=m;i++)
    {
        if(m%i==0)
            return 0;
    }
    return 1;
}
void dfs(int m,int count)
{
    int flag=0;
    for(int i=2;i<=n;i++)
    {
        if(a[i]==1)
            flag=1;
        if(count==n&&a[i]==1)
        {
            flag=1;
            b[count]=i;
        }
    }
    if(count==n&&flag==1)
    {
        int t=1;
        for(int i=2;i<=n-1;i++)
        {
            if(prime(b[i]+b[i+1]) == 0 || prime(b[i] + b[i-1]) == 0)
                t=0;
        }
        if(prime(b[1]+b[n]) == 0)
            t=0;
        if(t==1)
        {
            for(int i=1;i<=n;i++)
            {
                printf("%d",b[i]);
                if(i<=n-1)
                    printf(" ");
            }
            printf("\n");
            return ;
        }
    }
    for(int i=2;i<=n;i++)
    {
        if(prime(m+i)&&a[i]==1)
        {
            a[i]=0;
            b[count++]=i;
            dfs(i,count);
            count--;
            a[i]=1;
        }
    }
    return ;
}
int main ()
{
    int x=1;
    while(scanf("%d",&n)!=EOF)
    {
        count=1;
        u=0;
        memset(a,0,sizeof(a));
        for(int i=1;i<=n;i++)
            a[i]=1;
        b[count++]=1;
        printf("Case %d:\n",x);
        dfs(1,count);
        count--;
        printf("\n");
        x++;
    }
    return 0;
}
