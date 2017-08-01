/*************************************************************************
	> File Name: 10_1.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月26日 星期三 08时07分16秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define N 1000
int sum=0,k,n,s=0;
char a[N][N];
int b[N][N];
int t[N]={0},T[N]={0};
void dfs(int c,int d)
{
    if(s==k)
    {
        sum++;
        return ;
    }
    if(c==n &&d==n)
        return ;
    for(int i=c+1;i<n;i++)
    {
        for(int j=0;j<n;j++)
        {
            if(t[i]!=1&&T[j]!=1&&b[i][j]==1)
            {
                t[i]=1;T[j]=1;b[i][j]=0;s++;
                dfs(i,j);
                t[i]=0;T[j]=0;b[i][j]=1;s--;
            }
        }
    }
}   
int main ()
{
    while(scanf("%d %d",&n,&k)!=EOF&&(n!=-1&&k!=-1))
    {
        getchar();
        sum=0;s=0;
        memset(t,0,sizeof(t));
        memset(a,0,sizeof(a));
        memset(b,0,sizeof(b));
        memset(T,0,sizeof(T));
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<n;j++)
            {
                scanf("%c",&a[i][j]);
                if(a[i][j]=='#')
                    b[i][j]=1;
                else
                    b[i][j]=0;
            }
            getchar();
        }
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<n;j++)
            {
                if(b[i][j]==1)
                {
                    s++;
                    b[i][j]=0;
                    t[i]=1;T[j]=1;
                    dfs(i,j);
                    t[i]=0;T[j]=0;
                    b[i][j]=1;
                    s--;
                }
            }
        }
        printf("%d\n",sum);
    }
    return 0;
}
