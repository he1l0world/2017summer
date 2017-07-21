/*************************************************************************
	> File Name: 3_3.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月19日 星期三 09时47分23秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<ctype.h>
#define N 10000
int main ()
{
    int n;
    scanf("%d",&n);
    char a[N];
    getchar();
    while(n--)
    {
        gets(a);
        int s=0;
        int len=strlen(a);
        for(int i=0;i<len;i++)
        {
            if(a[i]<0)
                s++;
        }
        printf("%d\n",s/3);
    }
    return 0;
}
