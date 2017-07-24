/*************************************************************************
	> File Name: 8_1.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月24日 星期一 08时23分24秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
int main ()
{
    char a[100000];
    int b[10]={0};
    char c[10]={"Bulbasr"};
    gets(a);
    int len=strlen(a);
    int j=0,t=1000000,m;
    for(int i=0;i<len;i++)
    {
        for(int j=0;j<7;j++)
        {
            if(c[j]==a[i])
            {
                b[j]++;
                break;
            }
        }
    }
    b[1]/=2;b[4]/=2;
    for(int i=0;i<7;i++)
    {
        //printf("%d\n",b[i]);
        if(t>b[i])
            t=b[i];
    }
    printf("%d\n",t);
    return 0;
}
