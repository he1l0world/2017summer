/*************************************************************************
	> File Name: my_server.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月08日 星期二 19时30分00秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<signal.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#define LISTENQ 20
#define INVALID_USERINFO 'n'
#define VALID_USERINFO 'y'
#define BUFSIZE 128
#define SIGN_IN 0
#define SIGN_UP 1
#define UPDATE_FILE 2
#define DOWNLOAD_FILE 3
#define CHAT_ADD 4
#define CHAT_PRI 5
#define CHAT_CREATE 6
#define CHAT_JOIN 7
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
    char filename[BUFSIZE];
    char send[BUFSIZE];
    char recv[BUFSIZE];
    int id;
};
struct con_fd
{
    int stat;
    int fd;
    char name[32];
};
int check_account(struct node Buf);
void my_err(const char * err_string ,int line);
void sign_handle(int signo);
void send_data(int conn_fd ,struct node Buf );
void *do_something(void *arg);
void update_file(int con_fd ,struct node Buf);
void download_file(int con_fd , struct node Buf);

/*********************************************************************/
int sock_fd ;
int optval;
int ret;
struct con_fd conn_fd[1000];
pthread_t *thread;
socklen_t cli_len;
int i = 0;
struct sockaddr_in serv_addr , cli_addr;

int main ()
{
    int conn_fd1;
    char *addr = "127.0.0.1";
    //初始化服务端地址结构
    memset(&serv_addr , 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(4507);
    //serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_aton(addr , &serv_addr.sin_addr);

    sock_fd = socket(AF_INET , SOCK_STREAM , 0);//创建套接字
    if(sock_fd < 0 )
        my_err("socket" ,__LINE__);
    optval = 1;
    if(setsockopt(sock_fd , SOL_SOCKET,SO_REUSEADDR,(void*)&optval ,sizeof(int)) < 0)//设置套接字可复用
            my_err("setsockopt" ,__LINE__);
    
    if(bind(sock_fd , (struct sockaddr*)&serv_addr,sizeof(struct sockaddr_in)) < 0)//绑定套接字到地址
        my_err("bind",__LINE__);
    
    //套接字转化为监听
    if(listen(sock_fd ,LISTENQ) < 0)
    {
        my_err("listen", __LINE__);
    }
    cli_len = sizeof(struct sockaddr_in);
    thread = (pthread_t*)malloc(sizeof(pthread_t)*65535);
    while(1)
    {
        //signal(SIGINT , sign_handle);
        conn_fd1 = accept(sock_fd , (struct sockaddr*)&cli_addr ,&cli_len );
        if(conn_fd1 < 0)
            my_err("accept" , __LINE__);
    
        conn_fd[i].fd = conn_fd1;
        printf("conn_fd %d addr %s\n",conn_fd1 ,inet_ntoa(serv_addr.sin_addr));
        pthread_create(&thread[i],NULL,do_something,NULL);//创建子线程处理刚刚接收的请求
        //conn_fd1 = 0;
        //pthread_join(thread[i],NULL);
    }
}

/*********************************************************************/


void sign_handle(int signo)
{

}
void my_err(const char *err_string ,int line)
{
    fprintf(stderr , "line %d ",line);
    perror(err_string);
    exit(1);
}

int check_account(struct node Buf)
{
    int i;
    if(Buf.users.username == NULL || Buf.users.password == NULL)
    {
        printf("in find_name, NULL pointer");
        return -2;
    }
    char input_buf[32];
    /*连接myqsl查找用户名密码 */
}
void update_file(int con_fd ,struct node Buf)
{
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    int fd;
    char content[128];
    fd = open(Buf.filename ,O_CREAT|O_WRONLY|O_TRUNC,0644);
    recv(con_fd , content ,sizeof(content), 0);
    while(strcmp(content,"#end") != 0)
    {
        if(strcmp(content,"#end") != 0)
        {
            write(fd , content,sizeof(content));
        }
        if(recv(con_fd ,content,sizeof(content), 0) < 0)
            my_err("recv",__LINE__);
    }
    close(fd);
    strcpy(post_buf->buf , "success!");
    if(send(con_fd , post_buf , sizeof(struct node), 0) < 0)
        my_err("send",__LINE__);
}
void download_file(int con_fd , struct node Buf)
{
    ;
}
void *do_something(void* arg)
{   
    int con_fd =conn_fd[i].fd ;
    int t = i;
    i++;
    int ret, how;
    struct node *Buf ,*recv_buf;
    Buf = (struct node *)malloc(sizeof(struct node));
    recv_buf = (struct node*)malloc(sizeof(struct node));
    printf("accept a new client ,ip %s\n",inet_ntoa(cli_addr.sin_addr));
    while(1)
    {
        how = -1;
        if((ret = recv(con_fd ,  recv_buf , sizeof(struct node) , 0)) < 0)
        {
            my_err("recv" ,__LINE__);
        }
        how = recv_buf->type;
        switch(how)
        {
            case 0:
                //sign_in
                
                conn_fd[t].stat = 1;
                strcpy(conn_fd[t].name,recv_buf->users.username);
                strcpy(Buf->buf,"y");
                send(con_fd ,Buf,sizeof(struct node),0 );
                printf("%s %s\n",recv_buf->users.username,recv_buf->users.password);
                break;
            case 1:
                //sign_up
                conn_fd[t].stat = 1;
                strcpy(conn_fd[t].name,recv_buf->users.username);
                strcpy(Buf->buf,"y");
                send(con_fd , Buf,sizeof(struct node) , 0);
                break; 
            case 2:
                //update_file
                update_file(con_fd ,*recv_buf);
                break;
            case 3:
                //download_file
                download_file(con_fd ,*recv_buf);
                break;
            case 4:
                //chat_add
                break;
            case 5:
                //chat_pri
                break;
            case 6:
                //chat_create
                break;
            case 7:
                //chat_join
                break;
            default:
                break;;
        }
    }
}

