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
#include<signal.h>
#include<libgen.h>
#include<string.h>
#include<pthread.h>
#include<readline/history.h>
#include<readline/readline.h>
#include<mysql/mysql.h>
#define BUFSIZE 128
#define INVALID_USERINFO  'n'
#define VALID_USERINFO  'y'
typedef struct user
{
    char username[32];
    char password[32];
}USER;
struct message
{
    char from_who[32];
    char to_who[32];
    char group[32];
    char content[128];
};
struct node
{
    struct user  users;
    char buf[BUFSIZE];
    int type;
    char filename[BUFSIZE];
    int id;
    struct message mes;
};
struct info
{
    int type;
    struct message mes;
    struct info *next;
};
void update_file(char *filename);
void download_file(char *filename);
void chat_com();
void chat_pri();
void my_err(const char *err_string ,int line);
void get_input(char *buf);
void sign_in(int conn_fd ,struct node Buf );
void sign_up(int conn_fd  ,struct node Buf);
int input_userinfo(int conn_fd ,struct node Buf);
void *do_something(void *arg);
void main_UI();
void chat_del();
void send_file();
void check_info();
void quit();
void chat_join();
void chat_add();
void add_info(struct node Buf);


/**********************************************************************/
char *port = "9999";
char *adr = "127.0.0.1";
struct sockaddr_in serv_addr;
struct node *Buf;
int conn_fd ;
char username[32];
int fri = 0;//好友请求数
int file = 0;//发送文件数
int inv = 0;//邀请请求数
int join = 0;//加群请求数
char *end = "#end";
struct info *head;//存储消息盒子数据
int main ()
{
    head = (struct info*)malloc(sizeof(struct info));
    int serv_port = atoi(port);
    Buf = (struct node*)malloc(sizeof(struct node));
    conn_fd = socket(AF_INET , SOCK_STREAM , 0);//创建套接字
    memset(&serv_addr , 0 ,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serv_port);
    inet_aton(adr,&serv_addr.sin_addr);
    if(conn_fd < 0)
        my_err("socket" , __LINE__);
    if(connect(conn_fd , (struct sockaddr*)&serv_addr , sizeof(struct sockaddr_in)) < 0)
        my_err("connect",__LINE__);
    char ch;
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
    switch(ch)
    {
        case 'a':
        case 'A':
            sign_in(conn_fd,*Buf);
            break;
        case 'b':
        case 'B':
            sign_up(conn_fd ,*Buf);
            break;
        default:
            break;
    }
    close(conn_fd);
    return 0;
}


/********************************************************************/

void my_err(const char *err_string,int line)
{
    fprintf(stderr,"line %d",line);
    perror(err_string);
    exit(1);
}
void *do_something(void *arg)
{
    struct node *recv_buf ,*post_buf;
    char ch;
    int ret;
    recv_buf = (struct node*)malloc(sizeof(struct node));
    post_buf = (struct node*)malloc(sizeof(struct node));
    while(1)
    {
        ret = recv(conn_fd , recv_buf,sizeof(struct node) , 0); 
        if(ret< 0)
            my_err("recv",__LINE__);
        if(ret == 0)
        {
            close(conn_fd);
            pthread_exit(NULL);
            break;
        }
        switch(recv_buf->type)
        {
            case 200://创建群聊成功
                printf("创建群聊成功!\n");
                break;
            case 201://失败
                printf("创建群聊失败！\n");
                break;
            case 202://群聊存在
                printf("群聊已存在!\n");
            case 301://加入群聊失败
                printf("没有此群聊\n");
                break;
            case 300://加入群聊
                printf("加入  %s  群聊成功!\n",recv_buf->mes.group);
                break;
            case 302://群主拒绝
                printf("%s  群主拒绝您的申请\n",recv_buf->mes.group);
            case 303://有人想要加入群
                printf("有用户想要加入群，请及时查看\n");
                join++;
                add_info(*recv_buf);
                break;
            case 401://解散群失败
                printf("对不起 您不是群主不能解散该群\n");
                break;
            case 400://解散群成功
            printf("已解散!\n");
                break;
            case 402://解散失败
                printf("解散失败\n");
                break;
            case 501://没有此用户
                printf("没有此用户\n");
                break;
            case 500 ://有人邀请
                printf("用户 %s 邀请你加入 %s 群聊，请及时查看\n",recv_buf->mes.from_who,recv_buf->mes.group);
                inv++;
                break;
            case 601://没有此用户
                printf("没有此用户\n");
                break;
            case 600://有人想加好友
                printf("有人想要加你为好友,请及时查看\n");
                fri++;
                add_info(*recv_buf);
                break;
            case 603://对方同意
                printf("用户%s已同意您的好友请求，开始聊天吧\n",recv_buf->mes.to_who);
                break;
            case 604://对方拒绝
                printf("用户%s已拒绝您的好友请求\n",recv_buf->mes.to_who);
                break;
            case 701://没有此好友
                printf("没有此好友!\n");
                break;
            case 702://删除失败
                printf("删除失败!\n");
                break;
            case 801://查询失败
                printf("查询好友失败!\n");
                break;
            case 802://没有好友
                printf("还没有好友，快去加好友吧!\n");
                break;
            case 800://接收好友
                printf("\n好友:  %s\n",recv_buf->mes.content);
                break;
            case 900://私聊消息
                printf("(私聊消息)%s:  ",recv_buf->mes.from_who);
                printf("%s\n",recv_buf->mes.content);
                break;
            case 901://没有此好友
                printf("没有此好友\n");
                break;
            case 1001://没有此好友
                printf("没有此好友");
                break;
            case 1000://接收历史记录
                printf("%s\n",recv_buf->mes.content);
                break;
            case 1002://执行失败
                printf("查询历史失败");
                break;
            case 1003://没有记录
                printf("还没有聊天记录，快去聊天吧\n");
                break;
            case 1100://接收成功
                printf("(群聊消息) %s :%s\n",recv_buf->mes.from_who,recv_buf->mes.content);
                break;
            case 1101://没有此群聊
                printf("没有此群聊！\n");
                break;
            case 1202://执行失败
                printf("查找群聊记录失败\n");
                break;
            case 1201://没有此群聊
                printf("没有此群聊\n");
                break;
            case 1203://没有历史记录
                printf("没有此群聊的历史记录\n");
                break;
            case 1200://成功 
                printf("群聊 %s 记录 %s\n",recv_buf->mes.group,recv_buf->mes.content);
                break;
            case 1300://发送文件
                printf("您有一条发送文件请求，请及时查看 \n");
                file++;
                add_info(*recv_buf);
                break;
            case 1301://没有好友
                printf("没有此好友\n");
                break;
            default:
                break;
        }
    }
}


void add_info(struct node Buf)
{
    struct info *tail;
    tail = head;
    struct info *temp;
    temp = (struct info*)malloc(sizeof(struct info));
    temp->next = NULL;
    while(tail->next!=NULL)
        tail = tail->next;
    temp->type = Buf.type;
    strcpy(temp->mes.group,Buf.mes.group);
    strcpy(temp->mes.content,Buf.mes.content);
    strcpy(temp->mes.from_who,Buf.mes.from_who);
    strcpy(temp->mes.to_who,Buf.mes.to_who);
    tail->next = temp;
    tail = tail->next;
}

void sign_in(int conn_fd , struct node Buf)
{
    int flag;
    int ret;
    system("clear");
    fflush(stdin);
    char name[32];
    struct node *post_buf , *recv_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    recv_buf = (struct node*)malloc(sizeof(struct node));
    while(1)
    {
        memset(name,0,sizeof(name));
        memset(post_buf , 0 ,sizeof(struct node));
        post_buf->type = 0;
        printf("请输入用户名:           ");
        scanf("%s",name);
        strcpy(post_buf->users.username,name);
        printf("请输入密码:             ");
        scanf("%s",post_buf->users.password);
        getchar();
        if(send(conn_fd , post_buf , sizeof(struct node), 0) < 0)
            my_err("send",__LINE__);
        ret = recv(conn_fd , recv_buf ,sizeof(struct node),0);
        if(ret < 0 )
            my_err("recv",__LINE__);
        if(ret == 0)
            close(conn_fd );
        if(recv_buf->type == 000)
        {
            strcpy(username,name);
            main_UI();
        }
        else
        {
            printf("帐号或者密码错误!\n");
            printf("输入任意键继续\n");
            getchar();
        }
    }
}
void sign_up(int conn_fd ,struct node Buf)
{
    int flag;
    int ret;
    char name[32];
    system("clear");
    fflush(stdin);
    struct node *post_buf,*recv_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    recv_buf = (struct node*)malloc(sizeof(struct node));
    while(1)
    {
        memset(name,0,sizeof(name));
        memset(post_buf , 0 ,sizeof(struct node));
        printf("请输入帐号:        ");
        scanf("%s",name);
        strcpy(post_buf->users.username,name);
        printf("请输入密码:        ");
        scanf("%s",post_buf->users.password);
        getchar();
        post_buf->type = 1;
        if(send(conn_fd , post_buf,sizeof(struct node), 0 ) < 0)
            my_err("send",__LINE__);
        ret = recv(conn_fd , recv_buf ,sizeof(struct node),0);
        if(ret < 0 )
            my_err("recv",__LINE__);
        if(ret == 0)
            close(conn_fd );
        if(recv_buf->type == 100)
        {
            strcpy(username,name);
            main_UI();
        }
        else
        {
            printf("帐号已存在!\n");
            printf("按任意键继续\n");
            getchar();
        }
   }
}
void chat_create()
{
    system("clear");
    fflush(stdin);
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    printf("请输入群名:       ");
    scanf("%s",post_buf->mes.group);
    post_buf->type = 2;
    strcpy(post_buf->mes.from_who,username);
    if(send(conn_fd , post_buf,sizeof(struct node), 0) < 0 )
        my_err("send",__LINE__);
}
void chat_join()
{
    system("clear");
    fflush(stdin);
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    printf("请输入想要加入的群名:      ");
    scanf("%s",post_buf->mes.group);
    printf("请输入验证消息:        ");
    scanf("%s",post_buf->mes.content);
    post_buf->type = 3;
    strcpy(post_buf->mes.from_who,username);
    if(send(conn_fd , post_buf ,sizeof(struct node) , 0) < 0)
        my_err("send",__LINE__);
}
void chat_can()
{
    system("clear");
    fflush(stdin);
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    printf("请输入想要解散的群名:     ");
    scanf("%s",post_buf->mes.group);
    strcpy(post_buf->mes.from_who,username);
    post_buf->type = 4;
    if(send(conn_fd , post_buf ,sizeof(struct node) , 0 ) < 0)
        my_err("send",__LINE__);
}
void chat_inv()
{
    system("clear");
    fflush(stdin);
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    post_buf->type = 5;
    printf("想要邀请的好友名字:");
    scanf("%s",post_buf->mes.to_who);
    strcpy(post_buf->mes.from_who,username);
    if(send(conn_fd , post_buf ,sizeof(struct node), 0)< 0 )
        my_err("send",__LINE__);
}
void chat_add()
{
    system("clear");
    fflush(stdin);
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    post_buf->type = 6;
    printf("请输入想要添加的好友名:   ");
    scanf("%s",post_buf->mes.to_who);
    printf("请输入验证消息:         ");
    scanf("%s",post_buf->mes.content);
    strcpy(post_buf->mes.from_who,username);
    if(send(conn_fd, post_buf , sizeof(struct node), 0) < 0)
        my_err("send",__LINE__);

}
void chat_del()
{
    system("clear");
    fflush(stdin);
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    post_buf->type = 7;
    printf("请输入想要删除的好友名:        ");
    scanf("%s",post_buf->mes.to_who);
    strcpy(post_buf->mes.from_who,username);
    if(send(conn_fd , post_buf ,sizeof(struct node), 0 )< 0)
        my_err("send",__LINE__);
}
void check_fri()
{
    system("clear");
    fflush(stdin);
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    post_buf->type = 8;
    strcpy(post_buf->mes.from_who,username);
    if(send(conn_fd , post_buf ,sizeof(struct node), 0 ) < 0)
        my_err("send",__LINE__);
}

void chat_pri()
{   
    char content[BUFSIZE];
    system("clear");
    fflush(stdin);
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    post_buf->type = 9;
    printf("请输入想要私聊的好友名:   ");
    scanf("%s",post_buf->mes.to_who);
    strcpy(post_buf->mes.from_who,username);
    printf("---------------开始聊天---------------\n");
    printf("            输入‘#end’表示结束       \n");
    while(1)
    {
        memset(post_buf->mes.content,0,BUFSIZE);
        gets(content);
        if(strcmp(content,end) == 0)
            break;
        strcpy(post_buf->mes.content,content);
        if(send(conn_fd ,post_buf ,sizeof(struct node), 0) < 0)
            my_err("send",__LINE__);
        
    }
}
void his_pri()
{
    system("clear");
    fflush(stdin);
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    post_buf->type = 10;
    printf("请输入想要查看的好友记录:     ");
    scanf("%s",post_buf->mes.to_who);
    strcpy(post_buf->mes.from_who,username);
    if(send(conn_fd  , post_buf ,sizeof(struct node), 0) < 0)
        my_err("send",__LINE__);
}

void chat_com()
{
    char content[BUFSIZE];
    system("clear");
    fflush(stdin);
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    post_buf->type = 11;
    printf("请输入想要群聊的名字:  ");
    scanf("%s",post_buf->mes.group);
    strcpy(post_buf->mes.from_who,username);
    printf("---------------开始聊天------------\n");
    printf("            输入‘#end’表示结束    \n");
    while(1)
    {
        memset(post_buf->mes.content,0,BUFSIZE);
        gets(content);
        if(strcmp(content,end) == 0)
            break;
        strcpy(post_buf->mes.content,content);
        if(send(conn_fd ,post_buf ,sizeof(struct node), 0) < 0)
            my_err("send",__LINE__);
    }

}
void his_com()
{
    system("clear");
    fflush(stdin);
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    post_buf->type =12 ;
    printf("请输入想要看记录的群名:      ");
    scanf("%s",post_buf->mes.group);
    strcpy(post_buf->mes.from_who,username);
    if(send(conn_fd , post_buf,sizeof(struct node) , 0)< 0)
        my_err("send",__LINE__);
}

void send_file()
{
    system("clear");
    fflush(stdin);
    char *filename;
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    post_buf->type = 13;
    printf("请输入想要发送的好友:      ");
    scanf("%s",post_buf->mes.to_who);
    filename = readline("请输入文件路径：");
    strcpy(post_buf->filename ,filename); 
    if(send(conn_fd ,post_buf ,sizeof(struct node), 0) < 0)
        my_err("send",__LINE__);
}
void check_info()
{
    int ch,num,cho;
    struct info *tail;
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    if(send(conn_fd , post_buf,sizeof(struct node), 0) < 0)
        my_err("send",__LINE__);
        do
        {
            system("clear");
            fflush(stdin);
            printf("---------------------------------\n");
            printf("------     1好友请求(%d)---------\n",fri);
            printf("------     2加群请求(%d)---------\n",join);
            printf("------     3邀请加群(%d)---------\n",inv);
            printf("------     4文件请求(%d)---------\n",file);
            printf("------     5退出        ---------\n");
            printf("---------------------------------\n");
            printf("请输入你的选择:     ");
            scanf("%d",&ch);
        }while(ch < 1 || ch > 5);
        if(ch == 5)
            break;
        switch (ch)
        {
            case 1:
                num = 1;
                tail = head;
                while(tail->next!=NULL)
                {
                    if(tail-> type == 603)
                    {
                        printf("%d  用户%s 请求加你为好友",num,tail->mes.from_who);
                        num++;
                    }
                }
                printf("请输入想要处理的信息序号:   ");
                scanf("%d",&cho);
                fri = 0;
                break;
            case 2:
                num =1;
                tail = head;
                while(tail->next!=NULL)
                {
                    if(tail->type == 303)
                    {
                        printf("%d  用户%s请求加入群聊%s",num,tail->mes.from_who,tail->mes.group);
                        num++;
                    }
                }
                printf("请输入想要处理的信息序号:   ");
                scanf("%d",&cho);
                join = 0;
                break;
            case 3:
                num =1;
                tail = head;
                while(tail->next != NULL)
                {
                    if(tail->type == 500)
                    {
                        printf("%d  用户%s 邀请你加入群聊%s",num,tail->mes.from_who,tail->mes.group);
                        num++;
                    }
                }
                printf("请输入想要处理的信息序号:   ");
                scanf("%d",&cho);
                inv = 0;
                break;
            case 4:
                num = 1;
                tail = head;
                while(tail->next != NULL)
                {
                    if(tail->type == 1300)
                    {
                        printf("%d 用户%s 想要给你发送文件",num,tail->mes.from_who);
                       num++; 
                    }
                }
                printf("请输入想要处理的信息序号:   ");
                scanf("%d",&cho);
                file = 0;
                break;
        }
}
void quit()
{
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    post_buf->type = 15;
    if(send(conn_fd , post_buf,sizeof(struct node), 0) < 0)
        my_err("send",__LINE__);
    close(conn_fd);
}
void main_UI ()
{
    pthread_t thid;
    pthread_create(&thid,NULL,do_something,NULL);
    int ch;
    while(1)
    {
        do
        {
            fflush(stdin);
            system("clear");
            printf("-------------------------------------------\n");
            printf("            2创建群聊\n");
            printf("            3加入群聊\n");
            printf("            4解散群\n");
            printf("            5邀请好友\n");
            printf("            6加好友\n");
            printf("            7删除好友\n");
            printf("            8查看我的好友\n");
            printf("            9:私聊\n");
            printf("            10查看好友聊天记录\n");
            printf("            11:群聊\n");
            printf("            12查看群聊天记录\n");
            printf("            13:发送文件\n");
            printf("            14:未读信息\n");
            printf("            15:退出\n");
            printf("-------------------------------------------\n");
            printf("请输入你的选择:   ");
            scanf("%d",&ch);
        }while(ch < 2 || ch > 15);
    int type;
        switch(ch)
        {
            case 15://退出
                quit();
                exit(0);
                break;
            case 2://创建群
                chat_create();
                break;
            case 3://加入群聊
                chat_join();
                break;
            case 4://解散群
                chat_can();
                break;
            case 5://邀请好友
                chat_inv();
                break;
            case 6://加好友
                chat_add();
                break;
            case 7://删除好友
                chat_del();
                break;
            case 8://查看我的好友
                check_fri();
                break;
            case 9://私聊
                chat_pri();
                break;
            case 10://查看好友聊天记录
                his_pri();
                break;
            case 11://群聊
                chat_com();
                break;
            case 12://查看群聊天记录
                his_com();
                break;
            case 13://发送文件
                send_file();
                break;
            case 14://未读信息
                check_info();
                break;
        }
    }
}



