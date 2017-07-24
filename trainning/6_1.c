/*************************************************************************
	> File Name: 6_1.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月22日 星期六 08时09分08秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
int main ()
{
    int n;
    scanf("%d",&n);
    getchar();
    while(n--)
    {
        char a;
        for(int i=0;i<11;i++)
        {
            scanf("%c",&a);
            if(i==5)
                printf("6");
            if(i>=6)
                printf("%c",a);
        }
        printf("\n");
        getchar();
    }
    return 0;
}
