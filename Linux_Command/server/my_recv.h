/*************************************************************************
	> File Name: my_recv.h
	> Author: 
	> Mail: 
	> Created Time: 2017年08月07日 星期一 11时44分27秒
 ************************************************************************/

#ifndef _MY_RECV_H
#define _MY_RECV_H
    #define BUFSIZE 1024
    void my_err(const char*err_string , int line);
    int my_recv(int conn_fd , char *data_buf , int len);
#endif
