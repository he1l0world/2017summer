/*************************************************************************
	> File Name: 12_1.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月28日 星期五 14时50分26秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define N 101
int a[N][N]={0};
int b[8][2]={{0,1},{0,-1},{1,1},{1,-1},{1,0},{-1,0},{-1,-1},{-1,1}};
int n,m,sum;
void dfs(int c,int d)
{
    if(c<0||c>n||d<0||d>m)
        return ;
    for(int i=0;i<8;i++)
    {
        if(a[c+b[i][0]][d+b[i][1]]==1)
        {
            a[c+b[i][0]][d+b[i][1]]=sum;
            dfs(c+b[i][0],d+b[i][1]);
        }
    }
    return ;
}
int main ()
{
    char ch;
    while(scanf("%d %d",&n,&m)!=EOF&&m!=0)
    {
        sum=1;
        memset(a,0,sizeof(a));
        getchar();
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<m;j++)
            {
                scanf("%c",&ch);
                if(ch=='*')
                    a[i][j]=0;
                else
                    a[i][j]=1;
            }
            getchar();
        }
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<m;j++)
            {
                if(a[i][j]==1)
                {
                    sum++;
                    a[i][j]=sum;
                    dfs(i,j);
                }
            }
        }
        printf("%d\n",sum-1);
    }
    return 0;
}
