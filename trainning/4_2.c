/*************************************************************************
	> File Name: 4_2.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月20日 星期四 09时09分02秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#define N 1000
int main ()
{
    int n;
    char a[N];
    scanf("%d",&n);
    getchar();
    while(n--)
    {
        gets(a);
        int len=strlen(a);
        int t=0;
        for(int i=0;i<=len;i++)
        {
            if(a[i]==' '||a[i]=='\0')
            {
                for(int j=i-1;j>=t;j--)
                    printf("%c",a[j]);
                printf("%c",a[i]);
                t=i+1;
            }
        }
        printf("\n");
    }
    return 0;
}
