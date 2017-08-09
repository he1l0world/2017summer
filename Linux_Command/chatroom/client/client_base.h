/*************************************************************************
	> File Name: client_base.h
	> Author: 
	> Mail: 
	> Created Time: 2017年08月08日 星期二 14时28分15秒
 ************************************************************************/
#include"base.h"
#ifndef _CLIENT_BASE_H
#define _CLIENT_BASE_H

    void sign_in(int conn_fd ,USER *userinfo ,struct sockaddr_in serv_addr);
    void sign_up(int conn_fd  ,USER *userinfo,struct sockaddr_in serv_addr);
    int input_userinfo(int conn_fd ,const char *string);
#endif
