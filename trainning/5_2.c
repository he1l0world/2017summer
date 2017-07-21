/*************************************************************************
	> File Name: 5_2.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月21日 星期五 09时17分23秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#define N 10000
int main ()
{
    int n;
    char a[N];
    char b[N];
    char c[N];
    scanf("%d%*c",&n);
    while(n--)
    {
        int len;
        int t=0;
        scanf("%s",a);
        len=strlen(a);
        //printf("%s",a);
        for(int i=0,j=len-1;i<j;i++,j--)
        {
            b[t]=a[i];
            c[t++]=a[j];
        }
        c[t]='\0';
        b[t]='\0';
        if(strcmp(b,c)==0)
            printf("yes\n");
        else
            printf("no\n");
    }
    return 0;
}
