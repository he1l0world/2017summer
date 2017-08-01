/*************************************************************************
	> File Name: 9_2.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月25日 星期二 08时54分44秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
int main ()
{
    int n,t=1;
    scanf("%d",&n);
    while(n--)
    {
        char a[1000],b[1000];
        int c[1005]={0} ;
        scanf("%s %s",a,b);
        //printf("%s %s\n",a,b);
        int len1=strlen(a);
        int len2=strlen(b);
        //len1--;len2--;
        int L1=len1,L2=len2;
        int x,y;
        int len3=len2>=len1?len2+1:len1+1;
        int f=len3;
        while(len1>0&&len2>0)
        {
            x=a[len1-1]-'0';
            y=b[len2-1]-'0';
           // printf("%d %d\n",x,y);
            c[len3-1]+=(x+y);
            c[len3-2]+=c[len3-1]/10;
            c[len3-1]%=10;
            //printf("---%d\n",c[len3]);
            len3--;len1--;len2--;
        }
        if(len1)
        {
            while(len1>0)
            {
                c[len3-1]+=(a[len1-1]-'0');
                c[len3-2]+=c[len3-1]/10;
                c[len3-1]%=10;
                len3--;len1--;
            }
        }
        if(len2)
        {
            while(len2>0)
            {
                c[len3-1]+=(b[len2-1]-'0');
                c[len3-2]+=c[len3-1]/10;
                c[len3-1]%=10;
                len3--;len2--;
            }
        }
        printf("Case %d:\n",t);
        t++;
        for(int i=0;i<L1;i++)
            printf("%c",a[i]);
        printf(" + ");
        for(int i=0;i<L2;i++)
            printf("%c",b[i]);
        printf(" = ");
        if(c[0]>0)
            printf("%d",c[0]);
        for(int i=1;i<f;i++)
            printf("%d",c[i]);
        printf("\n");
        if(n>=1)
            printf("\n");
    }
    return 0;
}
