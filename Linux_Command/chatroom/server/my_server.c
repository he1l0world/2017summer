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
#include<time.h>
#include<signal.h>
#include<sys/stat.h>
#include<mysql/mysql.h>
#include<dirent.h>
#include<fcntl.h>
#include<errno.h>
#define LISTENQ 20
#define INVALID_USERINFO 'n'
#define VALID_USERINFO 'y'
#define BUFSIZE 128
struct user
{
    char username[32];
    char password[32];
};
struct message
{
    char from_who[32];
    char to_who[32];
    char group[32];
    char content[128];
    char filename[BUFSIZE];
};

struct node
{
    struct user users;
    char buf[BUFSIZE];
    int type;
    int id;
    struct message mes;
};
struct con_fd
{
    int stat;
    char name[32];
    int conn_fd;
};
int check_account(struct node Buf);
void my_err(char * err_string ,int line);
void sign_handle(int signo);
void send_data(int conn_fd ,struct node Buf );
void *do_something(void *arg);
void update_file(int con_fd ,struct node Buf);
void download_file(int con_fd , struct node Buf);
void add_his(int type , struct node Buf);
int check_username(struct node Buf);
void chat_add(int con_fd,struct node Buf);
void quit();
void send_file(int con_fd,struct node Buf);
void check_info();
int check_group(struct node Buf);
int check_fri(struct node Buf);
void chat_pri(int con_fd,struct node Buf);
void chat_del(int con_fd , struct node Buf);
void chat_join(int con_fd,struct node Buf);
void chat_create(int con_fd,struct node Buf);
void his_pri(int con_fd , struct node Buf);
void his_com(int con_fd , struct node Buf);
void chat_can(int con_fd , struct node Buf);
void my_friends(int con_fd , struct node Buf);
void chat_inv(int con_fd , struct node Buf);
int find_fd(char *);


/*********************************************************************/
struct con_fd userinfo[1000];
int i = 0;
char *err_his = ".err_history.txt";
int num = 0;
MYSQL mysql;
int main ()
{ 
    signal(SIGPIPE,SIG_IGN);
    mysql_init(&mysql);//初始化mysql
    if(!mysql_real_connect(&mysql,"47.93.198.31","root","Mr.chen583627195","chatroom",3306,NULL,0))
    {
        mysql_close(&mysql);
        my_err("mysql",__LINE__);
    }
    mysql_query(&mysql , "set names utf8");

    int optval;
    int ret;
    int sock_fd ;
    pthread_t *thread;
    struct sockaddr_in serv_addr , cli_addr;
    socklen_t cli_len;
    int conn_fd1;
    char *addr = "127.0.0.1";
    //初始化服务端地址结构
    memset(&serv_addr , 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9999);
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
    int con;
    cli_len = sizeof(struct sockaddr_in);
    thread = (pthread_t*)malloc(sizeof(pthread_t)*65535);
    while(1)
    {
        //signal(SIGINT , sign_handle);
        conn_fd1 = accept(sock_fd , (struct sockaddr*)&cli_addr ,&cli_len );
        if(conn_fd1 < 0)
            my_err("accept" , __LINE__);
        else
        {
            printf("conn_fd %d addr %s\n",conn_fd1 ,inet_ntoa(serv_addr.sin_addr));
            pthread_create(&thread[i],NULL,do_something,(void*)conn_fd1);//创建子线程处理刚刚接收的请求
            //conn_fd1 = 0;
            //pthread_join(thread[i],NULL);
        }
    }
}

/*********************************************************************/

void message_box(struct node Buf)
{
    MYSQL_ROW row;
    MYSQL_RES *result;
    char sql[128];
    int ret;
    sprintf(sql,"insert into message values(%d,'%s','%s','%s','%s');",Buf.type,Buf.mes.from_who,Buf.mes.to_who,Buf.mes.content,Buf.mes.group);
    printf("sql = %s\n",sql);

    ret = mysql_query(&mysql,sql);
    if(ret)
        my_err("query",__LINE__);
}
void addhis(struct node Buf)
{
    MYSQL_ROW row;
    MYSQL_RES *result;
    char sql[128];
    int ret;
    if(Buf.type == 9)//私聊
    {
        sprintf(sql,"insert into his_pri values ('%s','%s','%s',now());",Buf.mes.from_who,Buf.mes.to_who,Buf.mes.content);
        ret = mysql_query(&mysql,sql);
        if(ret)
            my_err("query",__LINE__);
        memset(sql,0,sizeof(sql));

    }
    if(Buf.type == 11) //群聊消息
    {
        memset(sql,0,sizeof(sql));
        sprintf(sql,"insert into his_com values ('%s','%s','%s',now());",Buf.mes.group,Buf.mes.from_who,Buf.mes.content);
        ret = mysql_query(&mysql,sql);
        if(ret)
            my_err("query",__LINE__);
    }
}
void send_message(int con_fd,struct node Buf)
{
    char sql[128];
    int ret;
    MYSQL_ROW row;
    MYSQL_RES *result;
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    sprintf(sql,"select * from message where to_who='%s';",Buf.users.username);
    ret = mysql_query(&mysql,sql);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        memset(post_buf,0,sizeof(struct node));
        post_buf->type = atoi(row[0]) ;
        printf("post_buf->type %d\n",post_buf->type);
        strcpy(post_buf->mes.group,row[4]);
        strcpy(post_buf->mes.from_who,row[1]);
        strcpy(post_buf->mes.content,row[3]);
        strcpy(post_buf->mes.to_who,row[2]);
        strcpy(post_buf->mes.from_who,row[1]);
        if(send(con_fd,post_buf,sizeof(struct node), 0) < 0)
            my_err("send",__LINE__);
    }
    memset(sql,0,sizeof(sql));
    sprintf(sql," delete from message where to_who='%s';",Buf.users.username);
    mysql_query(&mysql,sql);
}
void add_his(int type , struct node Buf)
{
    time_t *timep =(time_t*)malloc(sizeof(time_t));
    time(timep);
    char *s = ctime(timep);
    int fd;
    //fd = open(err_his ,O_WRONLY);
    printf("time %s",s);
    switch(type)
    {
        case 000://用户登录成功
        case 001://用户登录失败
        case 100://用户注册成功
        case 102://用户注册失败
        case 101://用户存在
        default:
            break;
    }
    close(fd);
}

void sign_handle(int signo)
{
    
}
void my_err(char *err_string ,int line)
{
    fprintf(stderr , "line %d ",line);
    perror(err_string);
}

int check_account(struct node Buf)
{
    MYSQL_ROW row;
    MYSQL_RES *result;
    char sql[128];
    if(Buf.users.username == NULL || Buf.users.password == NULL)
    {
        printf("in find_name, NULL pointer");
        return -2;
    }
    sprintf(sql,"select * from userinfo where username='%s' and password='%s';",Buf.users.username,Buf.users.password);
    /*连接myqsl查找用户名密码 */
    
    int res = mysql_query(&mysql,sql);//正确返回0
    if(res)
        return 0;
    else
    {
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);
        if(row == NULL)
            return 0;
        if(strcmp(row[0],Buf.users.username) == 0 && strcmp(row[1],Buf.users.password) == 0)
        {
            return 1;
        }



    }
}


int check_fri(struct node Buf)
{
    MYSQL_ROW row;
    MYSQL_RES *result;

    char sql[128];
    sprintf(sql,"select * from friends where username='%s' and friend='%s';",Buf.mes.from_who,Buf.mes.to_who);
    printf("sql %s",sql);
    printf("from %s to %s\n",Buf.mes.from_who,Buf.mes.to_who);
    /*连接myqsl查找好友 */
    int res = mysql_query(&mysql,sql);//正确返回0
    printf("res %d\n",res);
    if(res)
        return 0;
    else
    {
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);
        printf("friend %s %s\n",row[0],row[1]);
        //printf("row %s\n",row);
        if(row == NULL)
            return 0;
        return 1;
    }

}
int check_username(struct node Buf)
{   
    MYSQL_ROW row;
    MYSQL_RES *result;
    int res;
    char sql[128];
    if(Buf.type == 1)
    {
        
        sprintf(sql,"select * from userinfo where username='%s';",Buf.users.username);
        res = mysql_query(&mysql,sql);
        if(res)
            return 0;
        else
        {
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            if(row == NULL)
                return 0;
            else
                return 1;
        }
    }
    else
    {
        sprintf(sql,"select * from userinfo where username='%s';",Buf.mes.to_who);
        /*连接myqsl查找用户名 */
        res = mysql_query(&mysql,sql);//正确返回0
        if(res)
            return 0;
        else
        {
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            if(row == NULL)
                return 0;
            else
                return 1;
        }
    }
}
int check_group(struct node Buf)
{
    int ret;
    MYSQL_ROW row;
    MYSQL_RES *result;
    char sql[128];
    sprintf(sql,"select * from chatgroup where groupname='%s';",Buf.mes.group);
    ret = mysql_query(&mysql,sql);
    if(ret)
        return 0;
    else
    {
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);
        if(row==NULL)
            return 0;
        else
            return 1;
    }
}
int find_fd(char *name)
{
    for(int j = 0 ; j < i;j++)
    {
        if(strcmp(name,userinfo[j].name) == 0 && userinfo[j].stat == 1)
        {
            return userinfo[j].conn_fd;
        }
    }
    return 0;
}
char* check_own(struct node Buf)
{
    int ret;
    MYSQL_ROW row;
    MYSQL_RES *result;
    char sql[128];
    sprintf(sql,"select owner from chatgroup where groupname='%s';",Buf.mes.group);
    ret = mysql_query(&mysql,sql);
    if(ret)
        return NULL;
    else
    {
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);
        if(row == NULL)
            return NULL;
        else
        {
           return row[0];
        }
    }
}
void sign_in(int con_fd , struct node Buf)
{
    struct node *post_buf;
    printf("name:%s password:%s\n",Buf.users.username,Buf.users.password);
    post_buf = (struct node*)malloc(sizeof(struct node));
    if(check_account(Buf) == 1)
    {
        post_buf->type = 000; //成功登录
        add_his(post_buf->type,Buf);
        strcpy(userinfo[i].name,Buf.users.username);
        userinfo[i].stat = 1;
        userinfo[i].conn_fd = con_fd;
        i++;
        if(send(con_fd,post_buf,sizeof(struct node) , 0) < 0)
            my_err("send",__LINE__);
        send_message(con_fd,Buf);
        add_his(post_buf->type , Buf);
    }
    else
    {
        post_buf->type = 001; //帐号或密码错误
        add_his(post_buf->type,Buf);
        if(send(con_fd , post_buf , sizeof(struct node ), 0) < 0)
            my_err("send",__LINE__);
    }
}
void sign_up(int con_fd , struct node Buf)
{
    char sql[128];
    MYSQL_ROW row;
    int ret;
    MYSQL_RES *result;
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    if(check_username(Buf) == 0)
    {
        sprintf(sql,"insert into userinfo values('%s' , '%s');",Buf.users.username,Buf.users.password);
        ret = mysql_query(&mysql,sql);
        if(ret)
        {
            post_buf->type = 102;//注册失败
            add_his(post_buf->type,Buf);
        }
        else
        {
            strcpy(userinfo[i].name,Buf.users.username);
            userinfo[i].stat = 1;
            userinfo[i].conn_fd = con_fd;
            i++;
            post_buf->type = 100;//注册成功
            add_his(100,Buf);
            if(send(con_fd,post_buf,sizeof(struct node), 0) < 0)
                my_err("send",__LINE__);
            send_message(con_fd,Buf);
        }
    }
    else
    {
        post_buf->type =101;//帐号存在
        add_his(101,Buf);
    }   
    if(send(con_fd,post_buf,sizeof(struct node), 0) <0)
        my_err("send",__LINE__);
}

void chat_create(int con_fd , struct node Buf)
{
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    int ret;
    char sql[128];
    MYSQL_ROW row;
    MYSQL_RES *result;
    sprintf(sql,"insert into chatgroup values('%s' , '%s' ,'%s') ;",Buf.mes.group,Buf.mes.from_who,Buf.mes.from_who);
    if(check_group(Buf) == 1)
    {
        post_buf->type = 202;//已存在群聊
        if(send(con_fd,post_buf,sizeof(struct node), 0) < 0)
            my_err("send",__LINE__);
    }
    else
    {
        ret = mysql_query(&mysql,sql);
        if(ret)
        {
            post_buf->type = 201;//状态非0就是失败
            if(send(con_fd,post_buf,sizeof(struct node), 0)< 0)
                my_err("send",__LINE__);
        }
        else
        {
            post_buf->type = 200;//执行成功
            if(send(con_fd,post_buf,sizeof(struct node), 0)< 0)
                my_err("send",__LINE__);
        }
    }
}
void chat_join(int con_fd ,struct node Buf)
{
    char *owner;
    int fd;
    char sql[128];
    int ret;
    struct node *post_buf;
    post_buf = (struct node *)malloc(sizeof(struct node));
    MYSQL_ROW row;
    MYSQL_RES *result;
    if(check_group(Buf) == 0)
    {
        post_buf->type = 301;//没有此群聊
        if(send(con_fd ,post_buf,sizeof(struct node), 0 )< 0)
            my_err("send",__LINE__);
    }
    else
    {
        Buf.type = 303;
        owner = check_own(Buf);
        strcpy(post_buf->mes.to_who,owner);
        printf("owner : %s\n",owner);
        fd = find_fd(owner);
        if(fd != 0)
            if(send(fd ,&Buf ,sizeof(struct node),0 )< 0)
                my_err("send",__LINE__);
        if(fd == 0)
            message_box(Buf);
    }
    
}

void chat_can(int con_fd , struct node Buf)
{
    MYSQL_ROW row;
    MYSQL_RES *result;
    int ret;
    char sql[128];
    char *owner;
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    owner = check_own(Buf);
    if(strcmp(Buf.mes.from_who,owner) == 0)
    {
        sprintf(sql,"delete from chatgroup where groupname='%s';",Buf.mes.group);
        ret = mysql_query(&mysql,sql);
        if(ret)
            post_buf->type = 402;//执行失败
        else
            post_buf->type = 400;//执行成功
    }
    else
        post_buf->type = 401;//非群主
    if(send(con_fd,post_buf,sizeof(struct node), 0) < 0)
        my_err("send",__LINE__);
}
void chat_inv(int con_fd , struct node Buf)
{
    int fd;
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    if(check_username(Buf) == 0)
    {
        post_buf->type = 501;//没有此用户
        if(send(con_fd ,&Buf,sizeof(struct node), 0) < 0)
            my_err("send",__LINE__);
    }
    else if(check_group(Buf) == 0)
    {
        Buf.type = 504;//没有此群聊
        if(send(con_fd,&Buf,sizeof(struct node), 0) < 0)
            my_err("send",__LINE__);
    }
    else
    {
        Buf.type = 500;
        fd = find_fd(Buf.mes.to_who);
        if(fd )
            if(send(fd, &Buf,sizeof(struct node), 0 ) < 0)
                my_err("send",__LINE__);
        if(fd == 0)
            message_box(Buf);//离线消息
    }
}

void chat_add(int con_fd , struct node Buf)
{
    int fd;
    struct node *post_buf ;
    post_buf = (struct node *)malloc(sizeof(struct node));
    if(check_username(Buf) == 1)
    {
        Buf.type = 600;
        fd = find_fd(Buf.mes.to_who);
        if(fd)
            if(send(fd , &Buf,sizeof(struct node), 0) < 0)
                my_err("send",__LINE__);
        if(fd == 0)
            message_box(Buf);
    }
    else
    {
        post_buf->type = 601;//没有此用户   
        if(send(con_fd,post_buf,sizeof(struct node), 0)< 0)
            my_err("send",__LINE__);
    }
}
void chat_del(int con_fd ,struct node Buf)
{
    MYSQL_ROW row;
    MYSQL_RES *result;
    int fd ;
    int ret;
    char sql[128];
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    if(check_fri(Buf) == 1)
    {
        sprintf(sql,"delete from friends where username='%s' and friend='%s';",Buf.mes.from_who,Buf.mes.to_who);
        ret = mysql_query(&mysql,sql);
        if(ret)
        {
            post_buf->type = 702;
            if(send(con_fd ,post_buf,sizeof(struct node), 0)< 0)
                my_err("send",__LINE__);
        }
        else
        {
            sprintf(sql,"delete from friends where friend='%s' and username='%s';",Buf.mes.from_who,Buf.mes.to_who);
        }
        ret = mysql_query(&mysql,sql);
    }
    else
    {
        post_buf->type = 701;//没有此好友
        if(send(con_fd , post_buf , sizeof(struct node), 0) < 0)
            my_err("send",__LINE__);
    }
}
void my_friends(int con_fd , struct node Buf)
{
    MYSQL_ROW row;
    MYSQL_RES *result;
    int ret;
    int fd ;
    char sql[128];
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    sprintf(sql,"select friend from friends where username='%s';",Buf.mes.from_who);
    ret = mysql_query(&mysql,sql);
    if(ret)
    {
        post_buf->type=801;//失败
        if(send(con_fd,post_buf,sizeof(struct node), 0) < 0)
            my_err("send",__LINE__);
    }
    else
    {
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);
        if(row ==NULL)
        {
            post_buf->type = 802;//没有好友
            if(send(con_fd,post_buf,sizeof(struct node), 0) , 0)
                my_err("send",__LINE__);
        }
        else
        {
            strcpy(post_buf->mes.content,row[0]);
            fd = find_fd(post_buf->mes.content);
            if(fd)
                post_buf->type = 800;//在线
            else
                post_buf->type = 803;//离线
            if(send(con_fd,post_buf,sizeof(struct node), 0) < 0)
                my_err("send",__LINE__);
            while(row = mysql_fetch_row(result))
            {
                strcpy(post_buf->mes.content,row[0]);
                fd = find_fd(post_buf->mes.content);
                if(fd)
                    post_buf->type = 800;
                else
                    post_buf->type = 803;
                if(send(con_fd,post_buf,sizeof(struct node), 0)< 0)
                    my_err("send",__LINE__);
            }
        }
    }
}
void chat_pri(int con_fd , struct node Buf)
{
    int fd;
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    if(check_fri(Buf) == 1)
    {
        addhis(Buf);
        Buf.type = 900;
        fd = find_fd(Buf.mes.to_who);
        if(fd)
            if(send(fd,&Buf,sizeof(struct node), 0) < 0)
                my_err("send",__LINE__);
        if(fd == 0)
        {
            printf("-----------hera\n");
            message_box(Buf);
        }
    }
    else
    {
        post_buf->type = 901;//没有此好友
        if(send(con_fd,post_buf,sizeof(struct node), 0 )< 0)
            my_err("send",__LINE__);
    }
}
void his_pri(int con_fd , struct node Buf)
{
    MYSQL_ROW row;
    char sql[256];
    char content[BUFSIZE];
    int ret;
    MYSQL_RES *result;
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    int fd;
    if(check_fri(Buf) == 1)
    {
        sprintf(sql,"select username,history,chattime from his_pri where username='%s' and friend='%s' union select username ,history,chattime from his_pri where username='%s' and friend='%s' order by chattime;",Buf.mes.from_who,Buf.mes.to_who,Buf.mes.to_who,Buf.mes.from_who);
        ret = mysql_query(&mysql,sql);
char *ope_his = ".ope_history.txt";
        if(ret)
        {
            post_buf->type = 1002;//执行失败
            if(send(con_fd,post_buf,sizeof(struct node), 0) < 0)
                my_err("send",__LINE__);
        }
        else
        {
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            if(row == NULL)
            {
                post_buf->type = 1003;//没有历史记录
                if(send(con_fd,post_buf,sizeof(struct node), 0)< 0)
                my_err("send",__LINE__);
            }
            else
            {
                sprintf(content,"%s: %s %s",row[0],row[1],row[2]);
                strcpy(post_buf->mes.content,content);
                post_buf->type = 1000;
                send(con_fd,post_buf,sizeof(struct node), 0);
                while(row = mysql_fetch_row(result))
                {
                    memset(content,0,sizeof(content));
                    sprintf(content,"%s: %s %s",row[0],row[1],row[2]);
                    strcpy(post_buf->mes.content,content);
                    send(con_fd,post_buf,sizeof(struct node), 0);
                }
            }
        }
    }
    else
    {
        post_buf->type = 1001;//没有此好友
        if(send(con_fd,post_buf,sizeof(struct node) , 0)<0)
            my_err("send",__LINE__);
    }
}
void chat_com(int con_fd,struct node Buf)
{
    MYSQL_ROW row;
    MYSQL_RES *result;
    char sql[128];
    int ret,fd;
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    if(check_group(Buf) == 1)
    {
        sprintf(sql,"select member from chatgroup where groupname='%s';",Buf.mes.group);
        ret = mysql_query(&mysql,sql);
        if(ret)
        {
            post_buf->type = 1102;//执行失败
            if(send(con_fd,post_buf,sizeof(struct node), 0)< 0)
                my_err("send",__LINE__);
        }
        else
        {
            result = mysql_store_result(&mysql);
            addhis(Buf);
            while(row = mysql_fetch_row(result))
            {
                Buf.type = 1100;//成功
                fd = find_fd(row[0]); 
                strcpy(Buf.mes.to_who,row[0]);
                if(fd)
                    if(send(fd ,&Buf,sizeof(struct node), 0)< 0)
                        my_err("send",__LINE__);
                if(fd == 0)
                    message_box(Buf);
            }
            
        }
    }
    else
    {
        post_buf->type = 1101;//没有这个群
        if(send(con_fd,post_buf,sizeof(struct node), 0)< 0)
            my_err("send",__LINE__);
    }
}

void his_com(int con_fd , struct node Buf)
{
    MYSQL_ROW row;
    MYSQL_RES *result;
    char sql[128];
    int ret;
    char content[BUFSIZE];
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    int fd;
    if(check_group(Buf) == 1)
    {
        sprintf(sql,"select * from chatgroup where groupname='%s' and member='%s';",Buf.mes.group,Buf.mes.from_who);
        ret = mysql_query(&mysql,sql);
        if(ret)
        {
            post_buf->type = 1202;//执行失败
            if(send(con_fd,post_buf,sizeof(struct node), 0) < 0)
                my_err("send",__LINE__);
        }
        else
        {
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            if(row == NULL)
            {
                post_buf->type = 1204;//不是群成员
                if(send(con_fd,post_buf,sizeof(struct node), 0)< 0)
                    my_err("send",__LINE__);
            }
            else
            {
                memset(sql,0,sizeof(sql));
                sprintf(sql,"select username,history,chattime from his_com where groupname='%s';",Buf.mes.group);
                mysql_query(&mysql,sql);
                result = mysql_store_result(&mysql);
                row = mysql_fetch_row(result);


                sprintf(content,"%s: %s %s",row[0],row[1],row[2]);
                printf("content =%s\n",content);
                printf("-----------------hekzah\n");
                strcpy(post_buf->mes.content,content);
                post_buf->type = 1200;
                send(con_fd,post_buf,sizeof(struct node), 0);
                while(row = mysql_fetch_row(result))
                {
                    memset(content,0,sizeof(content));
                    sprintf(content,"%s: %s %s",row[0],row[1],row[2]);
                    strcpy(post_buf->mes.content,content);
                    send(con_fd,post_buf,sizeof(struct node), 0);
                }
            }
        }
    }
    else
    {
        post_buf->type = 1201;//没有此群聊
        if(send(con_fd,post_buf,sizeof(struct node) , 0)<0)
            my_err("send",__LINE__);
    }
   
}
void send_file(int con_fd , struct node Buf)
{
    int fd1;
    char filename[128],temp[128];
    int j = 0;
    int len = strlen(Buf.mes.filename);
    printf("接收到的filename %s\n",Buf.mes.filename);
    for(int i = len-1 ; Buf.mes.filename[i] != '/' ; i--)
    {
        temp[j++] = Buf.mes.filename[i];
    }
    temp[j] = '\0';
    len = strlen(temp);
    j = 0;
    printf("解析出来的temp %s\n",temp);
    for(int i = len-1 ; i >= 0 ; i--)
        filename[j++] = temp[i];
    filename[j] = '\0';
    printf("解析出来的filename %s\n",filename);
    if((fd1 = open(filename,O_CREAT|O_TRUNC|O_WRONLY,0644)) < 0)
        my_err("open",__LINE__);
    /*发送文件不论同不同意先发到服务端*/
    struct node *post_buf;
    int fd;
    post_buf = (struct node*)malloc(sizeof(struct node));
    if(check_fri(Buf) == 1)
    {
        fd = find_fd(Buf.mes.to_who);
        if(fd)
        {
            Buf.type = 1300;//询问是否同意发送文件
            printf("filename =%s\n",filename);
            strcpy(Buf.mes.filename,filename);
            if(send(fd,&Buf,sizeof(struct node), 0 )< 0)
                my_err("send",__LINE__);
            while(strcmp(Buf.mes.content,"#end") != 0)
            {
               write(fd1,Buf.mes.content,128); 
                memset(Buf.mes.content,0,128);
                if(recv(con_fd,&Buf,sizeof(struct node), 0) < 0)
                    my_err("recv",__LINE__);
            }
            if(strcmp(Buf.mes.content,"#end") == 0)
                close(fd1);
        }
        else
        {
            Buf.type = 1300;
            message_box(Buf);
        
        }
    }
    else
    {
        post_buf->type = 1301;//没有此好友
        if(send(con_fd ,post_buf,sizeof(struct node) , 0)< 0)
            my_err("send",__LINE__);
    }
}
/*void check_info(int con_fd ,struct node Buf)
{
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    post_buf->type = 14;
   
}*/
void sen_gro(int con_fd,struct node Buf)
{
    MYSQL_ROW row;
    MYSQL_RES *result;
    char sql[128];
    int ret  ,fd;
    if(check_group(Buf) == 0)
    {
        Buf.type = 1501;//没有此群聊
        if(send(con_fd,&Buf,sizeof(struct node),0) < 0)
            my_err("send",__LINE__);
        return;
    }
    sprintf(sql,"select * from chatgroup where member='%s' and groupname='%s';",Buf.mes.from_who,Buf.mes.group);
    mysql_query(&mysql,sql);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(row == NULL)
    {
        Buf.type = 1502;//不是本群成员
        if(send(con_fd,&Buf,sizeof(struct node), 0) < 0)
            my_err("send",__LINE__);
        return ;
    }
    else
    {
        memset(sql,0,sizeof(sql));
        sprintf(sql,"select member from chatgroup where groupname='%s';",Buf.mes.group);
        ret =  mysql_query(&mysql,sql);
        printf("sql %s\n",sql);
        printf("ret %d\n",ret);
        if(ret)
            my_err("query",__LINE__);
        else
        {
            result = mysql_store_result(&mysql);
            while(row = mysql_fetch_row(result))
            {
                strcpy(Buf.mes.content,row[0]);
                fd = find_fd(Buf.mes.content);
                if(fd )
                {
                    Buf.type = 1500;//在线
                }
                else
                {
                    Buf.type = 1503;//离线
                }
                if(send(con_fd,&Buf,sizeof(struct node), 0) < 0)
                    my_err("send",__LINE__);
            }
        }
    }
}
void quit(int con_fd , struct node Buf)
{
    close(con_fd);
    /*下线状态改变*/
    for(int j = 0; j < i ; j++)
    {
        if(con_fd == userinfo[j].conn_fd)
        {
            userinfo[j].stat = 0;
        }
    }
}

void *do_something(void* arg)
{   
    MYSQL_ROW row;
    int fd;
    int fd1;
    MYSQL_RES *result;
    char sql[128];
    int con_fd =(int)arg ;
    printf("con_fd = %d\n",con_fd);
    int flag ;
    char *owner;
    int ret, how;
    struct node *recv_buf , *post_buf;
    recv_buf = (struct node*)malloc(sizeof(struct node));
    post_buf =(struct node*)malloc(sizeof(struct node));
    while(1)
    {
        memset(recv_buf,0,sizeof(struct node));
        memset(post_buf,0,sizeof(struct node));
        how = -1;
        ret = recv(con_fd , recv_buf , sizeof(struct node) , 0);
        if(ret == 0)
            close(con_fd);
        if(ret < 0)
        {
            my_err("recv" ,__LINE__);
        }
        how = recv_buf->type;
        switch(how)
        {
            case 0:
                //sign_in
                sign_in(con_fd , *recv_buf);
                break;
            case 1:
                //sign_up
                sign_up(con_fd , *recv_buf);
                break; 
            case 2:
                //创建群聊
                chat_create(con_fd ,*recv_buf);
                break;
            case 3://加入群聊
                chat_join(con_fd , *recv_buf);
                break;
            case 300://群主同意
                memset(sql,0,sizeof(sql));
                owner = check_own(*recv_buf); 
                sprintf(sql,"insert into chatgroup values('%s','%s','%s');",recv_buf->mes.group,recv_buf->mes.from_who,owner);
                printf("sql =%s\n",sql);
                mysql_query(&mysql,sql);
                recv_buf->type = 300;
                fd = find_fd(recv_buf->mes.from_who);
                if(fd)
                {
                    if(send(fd , recv_buf ,sizeof(struct node), 0) < 0)
                        my_err("send",__LINE__);
                }
                else
                    message_box(*recv_buf);
                break;
            case 302: //群主拒绝
                recv_buf->type = 302;
                fd = find_fd(recv_buf->mes.from_who);
                if(fd)
                {
                    if(send(fd,recv_buf,sizeof(struct node ), 0) < 0)
                        my_err("send",__LINE__);
                }
                else
                    message_box(*recv_buf);
                break;
            case 4:
                //解散群
                chat_can(con_fd ,*recv_buf);
                break;
            case 5:
                //邀请好友
                chat_inv(con_fd , *recv_buf);
                break;
            case 502://邀请同意
                recv_buf->type = 502;
                memset(sql,0,sizeof(sql));
                owner = check_own(*recv_buf);
                printf("owner %s\n",owner);
                sprintf(sql,"insert into chatgroup values ('%s','%s','%s');",recv_buf->mes.group,recv_buf->mes.to_who,owner);
                mysql_query(&mysql,sql);
                fd = find_fd(recv_buf->mes.from_who);
                if(fd)
                {
                    if(send(fd ,recv_buf,sizeof(struct node), 0)< 0)
                        my_err("send",__LINE__);
                }
                else
                    message_box(*recv_buf);
                break;
            case 503://拒绝邀请
                recv_buf-> type = 503;
                fd = find_fd(recv_buf->mes.from_who);
                if(fd)
                {
                   if(send(fd,recv_buf,sizeof(struct node), 0) < 0) 
                        my_err("send",__LINE__);
                }
                else
                    message_box(*recv_buf);
                break;
            case 6:
                //加好友
                chat_add(con_fd , *recv_buf);
                break; 
            case 603://同意加好友
                memset(sql,0,sizeof(sql));
                sprintf(sql,"insert into friends values ('%s','%s');",recv_buf->mes.from_who,recv_buf->mes.to_who);
               mysql_query(&mysql,sql);
                memset(sql,0,sizeof(sql));
                sprintf(sql,"insert into friends values ('%s','%s');",recv_buf->mes.to_who,recv_buf->mes.from_who);
               mysql_query(&mysql,sql); 
                recv_buf->type = 603;
                fd = find_fd(recv_buf->mes.from_who);
                if(fd)
                {
                    if(send(fd , recv_buf ,sizeof(struct node), 0) < 0)
                        my_err("send",__LINE__);
                }
                else
                    message_box(*recv_buf);
                break;
            case 604://拒绝加好友
                recv_buf->type = 604;
                fd = find_fd(recv_buf->mes.from_who);
                if(fd)
                {
                    if(send(fd,recv_buf,sizeof(struct node), 0) < 0)
                        my_err("send",__LINE__);
                }
                else
                    message_box(*recv_buf);
                break;
            case 7:
                //删除好友
                chat_del(con_fd,*recv_buf);
                break;
            case 8:
                //查看我的好友
                my_friends(con_fd,*recv_buf);
                break;
            case 9://私聊
                chat_pri(con_fd , *recv_buf);
                break;
            case 10://查看好友的聊天记录
                his_pri(con_fd , *recv_buf);
                break;
            case 11://群聊
                chat_com(con_fd , *recv_buf);
                break;
            case 12://查看群聊天记录
                his_com(con_fd , *recv_buf);
                break;
            case 13://发送文件
                send_file(con_fd,*recv_buf);
                break;
            case 1303://发送文件同意
                fd = find_fd(recv_buf->mes.from_who);
                if(fd)
                {
                    if(send(fd , recv_buf,sizeof(struct node), 0) < 0)
                        my_err("send",__LINE__);
                }
                else
                    message_box(*recv_buf);
                printf("filename %s\n",recv_buf->mes.filename);
                if((fd1 = open(recv_buf->mes.filename,O_RDONLY))< 0)
                    my_err("open",__LINE__);
                strcpy(post_buf->mes.filename,recv_buf->mes.filename);
                post_buf->type = 1305;//发送文件名字
                while(read(fd1,post_buf->mes.content,128) > 0)
                {
                    if(send(con_fd,post_buf,sizeof(struct node), 0) < 0)
                        my_err("send",__LINE__);
                    memset(post_buf->mes.content,0,128);
                    post_buf->type = 1306;//发送文件内容
                }
                close(fd1);
                strcpy(post_buf->mes.content,"#end");
                if(send(con_fd,post_buf,sizeof(struct node), 0) < 0)
                    my_err("send",__LINE__);
                break;
            case 1304://发送文件拒绝
                fd = find_fd(recv_buf->mes.from_who);
                if(fd)
                {
                    if(send(fd,recv_buf,sizeof(struct node), 0) < 0)
                        my_err("send",__LINE__);
                }
                else
                    message_box(*recv_buf);
                break;
            /*case 14://未读消息
                check_info(con_fd,*recv_buf);
                break;*/
            case 15://群成员
                sen_gro(con_fd,*recv_buf);
                break;
            case 16://退出
                quit(con_fd ,*recv_buf);
                break;
            default:
                break;;
        }
    }
}

