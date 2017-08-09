/*************************************************************************
	> File Name: server_base.h
	> Author: 
	> Mail: 
	> Created Time: 2017年08月08日 星期二 14时43分33秒
 ************************************************************************/

#ifndef _SERVER_BASE_H
#define _SERVER_BASE_H
    #define BUFSIZE 128
    #define sign_in 0
    #define sign_up 1
    #define update_file 2
    #define download_file 3
    #define chat_com 4
    #define chat_pri 5
    struct user
    {
        char username[32];
        char password[32];
    };
    struct node
    {
        struct user users;
        char buf[BUFSIZE];
        int type;
        char path[BUFSIZE];
        char send[BUFSIZE];
        char recv[BUFSIZE];
    };
    int check_account(struct node Buf);
    void my_err(const char * err_string ,int line);
    void send_data(int conn_fd ,struct node Buf );
    void *do_something(void *arg);
#endif
