/*************************************************************************
	> File Name: base.h
	> Author: 
	> Mail: 
	> Created Time: 2017年08月08日 星期二 11时19分38秒
 ************************************************************************/

#ifndef _BASE_H
#define _BASE_H
    #define BUFSIZE 128
    #define sign_in 0
    #define sign_up 1
    #define update_file 2
    #define download_file 3
    #define chat_com 4
    #define chat_pri 5
    typedef struct user
    {
        char username[32];
        char password[32];
    }USER;
    struct node
    {
        struct user  users;
        char buf[BUFSIZE];
        int type;
        char path[BUFSIZE];
        char send[BUFSIZE];
        char recv[BUFSIZE];
    };
    void update_file();
    void download_file();
    void chat_com();
    void chat_pri();
    void my_err(const char *err_string ,int line);
    int my_recv(int conn_fd , struct node Buf ,int len);
    void send_data(int conn_fd ,struct node buf ,int len);
#endif
