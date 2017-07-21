/*************************************************************************
	> File Name: 2_2.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月18日 星期二 08时58分01秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
int main ()
{
    float n;
    while(scanf("%f",&n)!=EOF)
    {
        float sum=0;
        float *a;
        float max=0,min=100;
        a=(float *)malloc(sizeof(float)*n);
        for(int i=0;i<n;i++)
        {
            scanf("%f",&a[i]);
            sum+=*(a+i);
            if(max<*(a+i))
                max=*(a+i);
            if(min>*(a+i))
                min=*(a+i);
        }
        printf("%.2f\n",(sum-max-min)/(n-2));
    }
    return 0;
}
