/*************************************************************************
	> File Name: my_client.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月08日 星期二 10时43分55秒
 ************************************************************************/


#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"base.h"
#include"UI.h"

void main_UI ()
{
    char ch = 'a';
    while(1)
    {
        do
        {
            fflush(stdin);
            system("clear");
            printf("-------------------------------------------\n");
            printf("            a:  上传文件\n");
            printf("            b:  下载文件\n");
            printf("            c:  群聊\n");
            printf("            d:  私聊\n");
            printf("            q:  退出\n");
            printf("请输入你的选择:   ");
            ch = getchar();
        }while(ch == 'q' || ch == 'Q');
        if(ch == 'q' || ch == 'Q')
            break;
        switch(ch)
        {
            case 'a':
            case 'A':
                update_file();
            break;
            case 'b':
            case 'B':
                download_file();
                break;
            case 'c':
            case 'C':
                chat_pri();
                break;
            case 'd':
            case 'D':
                chat_com();
                break;
            default:
                break;
        }
    }
}
