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
#include<signal.h>
#include<libgen.h>
#include<string.h>
#include<pthread.h>
#include<readline/history.h>
#include<readline/readline.h>
#include<mysql/mysql.h>
#define BUFSIZE 128
#define SIGN_IN 0
#define SIGN_UP 1
#define UPDATE_FILE 2
#define DOWNLOAD_FILE 3
#define CHAT_ADD 4
#define CHAT_PRI 5
#define CHAT_CREATE 6
#define CHAT_JOIN 7
#define CHAT_DEL 8
#define QUIT 9
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
    char name[BUFSIZE];
    char myname[BUFSIZE];
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
void *do_something(void *arg);
void main_UI();


/**********************************************************************/
char *port = "9999";
char *adr = "127.0.0.1";
struct sockaddr_in serv_addr;
struct node *Buf;
int conn_fd ;
char username[32];
MYSQL mysql;
MYSQL_ROW row;
MYSQL_RES *result;
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


    mysql_init(&mysql);
    if(mysql_real_connect(&mysql,"4793.198.31","root","Mr.chen583627195","chatroom",3306,NULL,0))
    {
        mysql_close(&mysql);
        my_err("mysql",__LINE__);
    }
    mysql_query(&mysql,"set name utf8");
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

int check_username(struct node recv_buf)
{
    char sql[128];
    sprintf(sql,"select * from userinfo where name='%s' and password='%s';",recv_buf.users.username,recv_buf.users.password);
    int res = mysql_query(&mysql,sql);
    if(res)
        return 0;
    else
        return 1;
}
void *do_something(void *arg)
{
    struct node *recv_buf ,*post_buf;
    char ch;
    recv_buf = (struct node*)malloc(sizeof(struct node));
    post_buf = (struct node*)malloc(sizeof(struct node));
    while(1)
    {
        if(recv(conn_fd , recv_buf ,sizeof(struct node) , 0) < 0)
            my_err("recv",__LINE__);
        switch(recv_buf->type)
        {
            case 4://chat_add
                if(strcmp(recv_buf->send,"request") == 0)
                {
                    printf("用户：%s  想要加你为好友\n",recv_buf->myname);
                    printf("验证信息:%s\n",recv_buf->buf);
                    getchar();
                    printf("是否同意?(y/n)    ");
                    ch = getchar(); getchar();
                    if(ch == 'y')
                    {
                        printf("你们已经成为好友！\n");
                        strcpy(post_buf->send,"yes");
                        /*mysql添加*/
                    }
                    else
                    {
                        printf("已拒绝\n");
                        strcpy(post_buf->send,"no");
                    }
                    post_buf->type = CHAT_ADD;
                    if(send(conn_fd ,post_buf ,sizeof(struct node) , 0) < 0)
                        my_err("send",__LINE__);
                }
                else
                {
                    if(strcmp(recv_buf->send,"yes") == 0)
                    {
                        printf("你们已经成为好友!\n");
                        /*mysql添加*/
                    }
                    else
                    {
                        printf("对方已拒绝\n");
                    }
                }
                break;
            case 5://chat_pri
                break;
            case 7://chat_join
                break;
            default:
                break;
        }
    }
}

void update_file()
{
    struct node *post_buf , *recv_buf;
    int fd , j;
    char ch;
    char *filename , *name;
    char *end = "#end";
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
        int i ;
        for(i = 0 ; filename[i] !='\0'  ; i++)
        {
            post_buf->filename[j] = filename[i];
            j++;
            if(filename[i] == '/')
            {
                j = 0;
            }
        }
        if(filename[i-1] == ' ')
            filename[i-1] = '\0';
        post_buf->filename[j] = '\0';
        if(post_buf->filename[j-1] == ' ')
            post_buf->filename[j-1] = '\0';
        if(send(conn_fd ,post_buf ,sizeof(struct node) , 0) <0 )
            my_err("send",__LINE__);

        if((fd = open(filename,O_RDONLY)) < 0)
        {
            send(conn_fd , end ,sizeof(end), 0);
            my_err("open" , __LINE__);
        }
        
        while(read(fd,content,sizeof(content)) > 0)
        {
            if(send(conn_fd , content , sizeof(content) , 0) < 0)
                my_err("send" , __LINE__);
        }
        close(fd);
        memset(content , 0 ,128);
        send(conn_fd , end , sizeof(end) , 0);
        if(recv(conn_fd , recv_buf , sizeof(struct node) , 0) < 0)
            my_err("recv",__LINE__);
        if(strcmp(recv_buf->buf , "success!") == 0)
            printf("上传成功!\n");
        else
            printf("上传失败!\n");
        printf("是否继续上传?(y/n)   ");
        getchar();
        ch = getchar();
        getchar();
    }while(ch != 'n');
}
void download_file()
{
    int fd, i , t;
    char ch;
    int ret;
    char content[2560];
    char content1[128];
    struct node *recv_buf , *post_buf;
    recv_buf = (struct node*)malloc(sizeof(struct node));
    post_buf = (struct node*)malloc(sizeof(struct  node));
    post_buf->type = DOWNLOAD_FILE;
    do
    {
        i = 1;
        system("clear");
        fflush(stdin);
        if(send(conn_fd , post_buf ,sizeof(struct node) , 0) < 0)
            my_err("send",__LINE__);
        memset(post_buf , 0 ,sizeof(struct node));
        memset(content ,0 , 128);

        if(recv(conn_fd , content , sizeof(content),0) < 0)
            my_err("recv", __LINE__);
        int len = strlen(content);
        printf("-----------------------------------------\n");
        printf("ID : 1    ");
        t = 2;
        for(int i = 0 ; i < len ;i++)
        {
            if(content[i] == ',')
            {
                printf("\n");
                if(i < len -1)
                    printf("ID : %d    ",t);
                t++;
            }
            else
                printf("%c",content[i]);
        }
        printf("-----------------------------------------\n");
        memset(content , 0 ,sizeof(content));
        printf("请输入想要下载文件ID:  ");
        scanf("%d",&post_buf->id);
        getchar();
        if(post_buf->id <=0 || post_buf->id >= t)
        {
            printf("ID is wrong!\n");
            break;
        }
        post_buf->type = DOWNLOAD_FILE;
        if(send(conn_fd , post_buf , sizeof(struct node) , 0) < 0)
            my_err("send",__LINE__);


        recv(conn_fd , recv_buf ,sizeof(struct node), 0);
        if(fd = open(recv_buf->filename,O_CREAT|O_TRUNC|O_WRONLY , 0644))
            my_err("open",__LINE__);
        if(recv(conn_fd , content1 ,sizeof(content1) , 0) < 0 )
            my_err("recv",__LINE__);
        while(strcmp(content1 , "#end") != 0)
        {
            if(strcmp(content1, "#end") != 0)
                write(fd , content1 ,sizeof(content1));
            if(recv(conn_fd , content1 ,sizeof(content1) , 0) < 0)
                my_err("recv",__LINE__);
        }
        close(fd);
        printf("下载成功!\n");
        printf("是否继续下载?(y/n)");
        ch = getchar();
    }while(ch != 'n');
}

void chat_add()
{
    system("clear");
    fflush(stdin);
    struct node *post_buf , *recv_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    recv_buf = (struct node*)malloc(sizeof(struct node));
    post_buf->type = CHAT_ADD;
    printf("\t\t请输入用户名:     \t");
    scanf("%s",post_buf->name);
    strcpy(post_buf->myname,username);
    strcpy(post_buf->send,"request");
    printf("\n\n\t\t填写验证信息：   \t");
    scanf("%s",post_buf->buf);
    if(send(conn_fd , post_buf ,sizeof(struct node) , 0) < 0)
        my_err("send",__LINE__);
    if(check_username(*post_buf) == 0)
        printf("没有此用户!\n");
    else
        printf("发送成功");
    printf("按任意键继续!\n");
    getchar();getchar();

}
void chat_pri()
{
    char ch;
    struct node *post_buf , *recv_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    recv_buf = (struct node*)malloc(sizeof(struct node));
    pthread_t thid ;
    while(1)
    {
        do
        {
            system("clear");
            printf("---------------------------\n");
            printf("       a加好友\n");
            printf("       b选择好友聊天\n");
            printf("       c删除好友\n");
            printf("       q退出  \n");
            printf("---------------------------\n");
            printf("请选择:   ");
            ch = getchar();
        }while(ch!='a' && ch != 'b' && ch != 'q' && ch != 'c');
        if(ch == 'q')
        {
            break;
        }

        if(ch == 'a')
        {
            chat_add();
        }
        else if(ch == 'b')
        {
            system("clear");
            fflush(stdin);
            post_buf->type = CHAT_PRI;
            printf("输入想要聊天的好友名称:      ");
            scanf("%s",post_buf->name);
            if(send(conn_fd , post_buf ,sizeof(struct node) , 0) < 0)
                my_err("send",__LINE__);
            printf("现在可以开始聊天:(输入#exit退出)");
            while(1)
            {
                memset(post_buf , 0 ,sizeof(struct node));
                post_buf->type = CHAT_PRI;
                strcpy(post_buf->myname,username);
                gets(post_buf->buf);
                if(send(conn_fd , post_buf , sizeof(struct node) , 0) < 0)
                    my_err("send",__LINE__);
                if(strcmp(post_buf->buf,"#exit") == 0)
                   break;
            }
        }
        else
        {
            system("clear");
            fflush(stdin);
            post_buf->type = CHAT_DEL;
            printf("请输入用户名:     ");
            scanf("%s",post_buf->name);
            printf("\n");
            if(send(conn_fd , post_buf ,sizeof(struct node), 0) < 0)
                my_err("send", __LINE__);
            printf("删除成功!\n");
        }
    }
}
void chat_com()
{
    char ch;
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    pthread_t thid;
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
            system("clear");
            fflush(stdin);
            post_buf->type = CHAT_CREATE;
            printf("请输入群名称:   ");
            scanf("%s",post_buf->name);
            if(send(conn_fd , post_buf , sizeof(struct node) , 0) < 0)
                my_err("send",__LINE__);
            printf("创建成功!\n");
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
            printf("            c:  私聊\n");
            printf("            d:  群聊\n");
            printf("            q:  退出\n");
            printf("-------------------------------------------\n");
            printf("请输入你的选择:   ");
            ch = getchar();
        }while(ch != 'q' && ch !='a' && ch!='b'&&ch!='c'&&ch!='d');
        if(ch == 'q' )
        {
            close(conn_fd);
            exit(0);
        }
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
    struct node *post_buf ,*recv_buf;
    post_buf = (struct node *)malloc(sizeof(struct node));
    recv_buf = (struct node*)malloc(sizeof(struct node));
    if(connect(conn_fd , (struct sockaddr*)&serv_addr , sizeof(struct sockaddr_in)) < 0)
        my_err("connect" , __LINE__);
   do
    {
        memset(recv_buf,0,sizeof(struct node));
        memset(post_buf , 0 ,sizeof(struct node));
        flag = 1;
        system("clear");
        printf("请输入帐号:  ");
        scanf("%s",post_buf->users.username);
        printf("请输入密码:  ");
        scanf("%s",post_buf->users.password);
        getchar();
        post_buf->type = SIGN_IN;
        if(send(conn_fd ,post_buf ,sizeof(struct node ) , 0) < 0)
            my_err("send", __LINE__);
        usleep(100);
        recv(conn_fd , recv_buf,sizeof(struct node), 0);
        if(strcmp(recv_buf->buf , "y") == 0)
            flag = 1;
        else
            flag = 0;
        if(flag == 0)
        {
            printf("帐号或者密码错误请重新输入！\n");
            printf("请输入任意键继续！\n");
            getchar();
        } 
    }while(flag == 0);
    strcpy(username,post_buf->users.username);
    main_UI();
    return ;
}



