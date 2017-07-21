/*************************************************************************
	> File Name: 2_1.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月18日 星期二 09时13分54秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#define N 100
int main ()
{
    char a[N];
    while(scanf("%s",a)!=EOF)
    {
        int len=strlen(a);
        int flag=1;
        int t=0;
        int m=0,n=0;
        for(int i=0;i<len;i++)
        {
            if(a[i]=='@')
            {
                t++;
                m=i;
                if(a[i+1]=='.'||a[i-1]=='.')
                    flag=0;
            }
            if(a[0]=='@'||a[0]=='.'||a[len-1]=='@'||a[len-1]=='.')
                flag=0;
            if(a[i]=='.')
                n=i;
        }
        if(t!=1)
            flag=0;
        if(n<m)
            flag=0;
        if(flag)
            printf("YES\n");
        else
            printf("NO\n");
    }
    return 0;
}
