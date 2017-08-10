/*************************************************************************
	> File Name: my_client.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月08日 星期二 14时15分24秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<unistd.h>
#include<libgen.h>
#include<string.h>
#include<pthread.h>
#include<readline/history.h>
#include<readline/readline.h>
#define BUFSIZE 128
#define SIGN_IN 0
#define SIGN_UP 1
#define UPDATE_FILE 2
#define DOWNLOAD_FILE 3
#define CHAT_ADD 4
#define CHAT_PRI 5
#define CHAT_CREATE 6
#define CHAT_JOIN 7
#define INVALID_USERINFO  'n'
#define VALID_USERINFO  'y'

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
    char filename[BUFSIZE];
    char send[BUFSIZE];
    char recv[BUFSIZE];
    int id;
};
void update_file();
void download_file();
void chat_com();
void chat_pri();
void my_err(const char *err_string ,int line);
void get_input(char *buf);
void sign_in(int conn_fd ,struct node Buf ,struct sockaddr_in serv_addr);
void sign_up(int conn_fd  ,struct node Buf,struct sockaddr_in serv_addr);
int input_userinfo(int conn_fd ,struct node Buf);
void main_UI();


/**********************************************************************/
char *port = "4507";
char *adr = "127.0.0.1";
struct sockaddr_in serv_addr;
struct node *Buf;
int conn_fd ;
int main ()
{
    int serv_port = atoi(port);
    Buf = (struct node*)malloc(sizeof(struct node));
    conn_fd = socket(AF_INET , SOCK_STREAM , 0);//创建套接字
    memset(&serv_addr , 0 ,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(adr,&serv_addr.sin_addr);
    if(conn_fd < 0)
        my_err("socket" , __LINE__);
    char ch;
    while(1)
    {
        do
        {
            fflush(stdin);
            system("clear");
            printf("-----------\n");
            printf(" a:登录\n");
            printf(" b:注册\n");
            printf(" q:退出\n");
            printf("-----------\n");
            printf(" 请选择:  ");
            ch = getchar();
        }while(ch != 'q'&& ch!='a'&&ch!='b');
        if(ch == 'q' || ch == 'Q') 
            break;
        switch(ch)
        {
            case 'a':
            case 'A':
                printf("helloworld\n");
                sign_in(conn_fd,*Buf,serv_addr);
                break;
            case 'b':
            case 'B':
                sign_up(conn_fd ,*Buf,serv_addr);
                break;
            default:
                break;
        }
    }
    close(conn_fd);
    return 0;
}


/********************************************************************/
void update_file()
{
    struct node *post_buf , *recv_buf;
    int fd , j;
    char ch;
    char *filename , *name;
    char content[128];
    post_buf = (struct node *)malloc(sizeof(struct node));
    recv_buf = (struct node *)malloc (sizeof(struct node));
    do
    {
        system("clear");
        fflush(stdin);
        j = 0;
        memset(post_buf , 0 ,sizeof(struct node));
        memset(content, 0 ,sizeof(content));
       /* printf("请输入文件路径:(绝对路径)    ");
        scanf("%s",filename);*/
        filename = readline("请输入文件路径:   ");
        printf("filename %s\n",filename);
        post_buf->type = UPDATE_FILE;
        for(int i = 0 ; filename[i] !='\0'  ; i++)
        {
            if(filename[i] == ' ')
                continue;
            post_buf->filename[j] = filename[i];
            j++;
            if(filename[i] == '/')
            {
                j = 0;
            }
        }
        post_buf->filename[j] = '\0';
        if(send(conn_fd ,post_buf ,sizeof(struct node) , 0) <0 )
            my_err("send",__LINE__);

        printf("filename = %s\n",filename);
        if((fd = open(filename,O_RDONLY)) < 0)
            my_err("open" , __LINE__);
        
        while(read(fd,content,sizeof(content)) > 0)
        {
            if(send(conn_fd , content , sizeof(content) , 0) < 0)
                my_err("send" , __LINE__);
        }
        close(fd);
        memset(content , 0 ,128);
        char *end = "#end";
        send(conn_fd , end , sizeof(end) , 0);
        if(recv(conn_fd , recv_buf , sizeof(struct node) , 0) < 0)
            my_err("recv",__LINE__);
        if(strcmp(recv_buf->buf , "success!") == 0)
            printf("上传成功!\n");
        else
            printf("上传失败!\n");
        printf("是否继续上传?(y/n)   ");
        ch = getchar();
        ch = getchar();
    }while(ch != 'n');
}
void download_file()
{
    int fd;
    char ch;
    int ret;
    char content[128];
    struct node *recv_buf;
    recv_buf = (struct node*)malloc(sizeof(struct node));
    do
    {
        system("clear");
        fflush(stdin);
        memset(recv_buf , 0 ,sizeof(struct node));
        memset(content ,0 , 128);
        fflush(stdin);
        system("clear");
        printf("请输入想要下载文件ID:  ");
        scanf("%d",recv_buf->id);
        recv_buf->type = DOWNLOAD_FILE;
        /*----------------不全-------------*/
        if((ret =recv(conn_fd , recv_buf ,sizeof(struct node) , 0)) < 0)
            my_err("recv",__LINE__);
        if(fd = open(recv_buf->filename , O_CREAT|O_WRONLY|O_TRUNC,0644) <0)
            my_err("open",__LINE__);
        recv(conn_fd , content , sizeof(content),0);
        while(strcmp(content,"#end") != 0)
        {
            if(strcmp(content , "#end") != 0)
            {
                write(fd ,content , sizeof(content));
                memset(content , 0 ,sizeof(content));
            }
            if(recv(conn_fd , content ,sizeof(content), 0) < 0)
                my_err("recv", __LINE__);
        }
        printf("是否继续下载?(y/n)");
        ch = getchar();
    }while(ch != 'n');
}
void chat_pri()
{
    char ch;
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    while(1)
    {
        do
        {
            system("clear");
            printf("---------------------------\n");
            printf("       a加好友\n");
            printf("       b选择好友聊天\n");
            printf("       q退出  \n");
            printf("---------------------------\n");
            printf("请选择:   ");
            ch = getchar();
        }while(ch!='a' && ch != 'b' && ch != 'q');
        if(ch == 'q')
            break;
        if(ch == 'a')
        {
            post_buf->type = CHAT_ADD;
        }
        else
        {
            post_buf->type = CHAT_PRI;
        }
    }
}
void chat_com()
{
    char ch;
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    while(1)
    {
        do
        {
            printf("---------------------------\n");
            printf("       a创建群聊\n");
            printf("       b加入群聊\n");
            printf("       q退出\n");
            printf("---------------------------\n");
            printf("请选择: \n");
            ch = getchar();
        }while(ch != 'a' && ch != 'b' && ch != 'q');
        if(ch == 'q')
            break;
        if(ch == 'a')
        {
            post_buf->type = CHAT_CREATE;
        }
        else
        {
            post_buf->type = CHAT_JOIN;
        }
    }
}
void my_err(const char *err_string ,int line)
{
    fprintf(stderr , "line :%d",line);
    perror(err_string);
    exit(1);
}


void sign_up(int conn_fd , struct node Buf , struct sockaddr_in serv_addr)
{
    while(1)
    {
        memset(&Buf,0,sizeof(struct node));
        system("clear");
        printf("请输入帐号:  ");
        scanf("%s",Buf.users.username);
        printf("请输入密码:  ");
        scanf("%s",Buf.users.password);
        Buf.type = 1;
        if(connect(conn_fd ,(struct sockaddr*)&serv_addr , sizeof(struct sockaddr_in) ) < 0 )
            my_err("connect" , __LINE__);
        if(send(conn_fd , &Buf,sizeof(Buf) , 0) < 0)
            my_err("send", __LINE__);
        if(recv(conn_fd ,&Buf , sizeof(struct node),0 ) < 0 )
            my_err("recv",__LINE__);
        if(Buf.buf[0] == 'y')
        {
            printf("注册成功\n");
            printf("输入任意键继续\n");
            getchar();
            break;
        }
        else
        {
            printf("注册失败\n");
            printf("输入任意键继续\n");
            getchar();
        }
    }
    if(Buf.buf[0] == 'y')
        main_UI();
}


int input_userinfo(int conn_fd , struct node Buf)
{
    struct  node recv_buf;
    char flag_userinfo;
    int len;
    if(send(conn_fd , &Buf , sizeof(struct node) , 0) < 0)
        my_err("send",__LINE__);
    if((len = recv(conn_fd ,&recv_buf ,sizeof(struct node), 0)) < 0)
    {
        printf("data is too long\n");
        exit(1);
    }
    if(recv_buf.buf[0] == VALID_USERINFO)
        return 1;
    return 0;
        
}

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
            printf("-------------------------------------------\n");
            printf("请输入你的选择:   ");
            ch = getchar();
        }while(ch != 'q' && ch !='a' && ch!='b'&&ch!='c'&&ch!='d');
        if(ch == 'q' )
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

void sign_in(int conn_fd ,struct node Buf , struct sockaddr_in serv_addr)
{
    int ret,i;
    int flag = 1;
    do
    {
        memset(&Buf , 0 ,sizeof(struct node));
        flag = 1;
        system("clear");
        printf("请输入帐号:  ");
        scanf("%s",Buf.users.username);
        printf("请输入密码:  ");
        scanf("%s",Buf.users.password);
        printf("conn_fd : %d addr %s\n",conn_fd , inet_ntoa(serv_addr.sin_addr));
        if(connect(conn_fd , (struct sockaddr*)&serv_addr , sizeof(struct sockaddr_in)) < 0)
            my_err("connect" , __LINE__);
        Buf.type = SIGN_IN;
        if(input_userinfo(conn_fd ,Buf ) == 0)
            flag = 0;
        if(flag == 0)
        {
            printf("帐号或者密码错误请重新输入！\n");
            printf("请输入任意键继续！\n");
            getchar();
        } 
    }while(flag == 0);
    main_UI();
    return ;
}



