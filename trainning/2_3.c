/*************************************************************************
	> File Name: 2_1.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月18日 星期二 08时22分57秒
 ************************************************************************/

#include<stdio.h>
int main()
{
    int a,b;
    while(scanf("%d %d",&a,&b)!=EOF)
    {
        int c=a/b;
        int d=a%b;
        if(a<=b)
        {
            printf("%d\n",(2+2*a)/2);
            continue;
        }
        if(d==0)
        {
            int m=2;
            for(int i=1;i<=c;i++)
            {
                printf("%d",(2*m+2*b-2)/2);
                m+=2*b;
                if(i<=c-1)
                    printf(" ");
            }
            printf("\n");
            continue;
        }
        int m=2;
        for(int i=1;i<=c;i++)
        {
            printf("%d ",(2*m+2*b-2)/2);
            m=m+2*b;
        }
        printf("%d\n",(2*m+2*d-2)/2);
    }
    return 0;
}
