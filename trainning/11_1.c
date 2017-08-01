/*************************************************************************
	> File Name: 11_1.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月27日 星期四 08时10分04秒
 ************************************************************************/

#include<stdio.h>
#include<math.h>
#define N 15
int main ()
{
    long long n,x,m,y;
    long long sum=0,s=0;
    long long a[N]={0},b[N]={0};
    scanf("%lld %lld",&n,&x);
    for(int i=0;i<n;i++)
        scanf("%lld",&a[i]);
    scanf("%lld %lld",&m,&y);
    for(int i=0;i<m;i++)
        scanf("%lld",&b[i]);
    int len=n>=m?n:m;
    if(x>y&&n>=m)
    {
        printf(">\n");
        return 0;
    }
    if(y>x&&m>=n)
    {
        printf("<\n");
        return 0;
    }
    if(x>=10&&y>=10&&n==m)
    {
        int i;
        for(i=0;i<len;i++)
        {
            if(a[i]>b[i])
            {
                printf(">\n");
                return 0;
            }
            if(a[i]<b[i])
            {
                printf("<\n");
                return 0;
            }
        }
        if(i==len)
        {
            printf("=\n");
            return 0;
        }
    }
    int j=0;
    for(int i=n-1;i>=0;i--)
    {
        a[i]*=pow(x,j);
        j++;
    }
    j=0;
    for(int i=m-1;i>=0;i--)
    {
        b[i]*=pow(y,j);
        j++;
    }
    for(int i=0;i<len;i++)
        sum+=(a[i]-b[i]);
    if(s==sum)
        printf("=\n");
    if(sum>s)
        printf(">\n");
    if(sum<s)
        printf("<\n");
    return 0;
}
