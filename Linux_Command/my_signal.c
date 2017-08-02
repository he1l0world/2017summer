/*************************************************************************
	> File Name: my_signal.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月02日 星期三 15时30分16秒
 ************************************************************************/

#include<stdio.h>
#include<signal.h>

/*信号处理函数*/
void handler_sigint(int signo)
{
    printf(" recv SIGINT\n");
}

int main ()
{
    /*安装信号处理函数*/

        signal(SIGINT , handler_sigint);
    return 0;
}
