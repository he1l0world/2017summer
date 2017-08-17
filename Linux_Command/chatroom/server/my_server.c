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
};

struct node
{
    struct user users;
    char buf[BUFSIZE];
    int type;
    char filename[BUFSIZE];
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
void add_his(char *err_string ,int line);
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
char *ope_his = ".ope_history.txt";
int num = 0;
MYSQL mysql;
int main ()
{ 
    mysql_init(&mysql);//初始化mysql
    if(!mysql_real_connect(&mysql,"47.93.198.31","root","Mr.chen583627195","chatroom",3306,NULL,0))
    {
        mysql_close(&mysql);
        my_err("mysql",__LINE__);
    }
    mysql_query(&mysql , "set name utf8");

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
    if(Buf.type = 9)//私聊
    {
        sprintf(sql,"insert into his_pri values ('%s','%s','%s',now());",Buf.mes.from_who,Buf.mes.to_who,Buf.mes.content);
        ret = mysql_query(&mysql,sql);
        if(ret)
            my_err("query",__LINE__);
        memset(sql,0,sizeof(sql));

    }
    else //群聊消息
    {
        sprintf(sql,"insert into his_com values ('%s','%s','%s','%s');");
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
    sprintf(sql,"select * from message ;");
    ret = mysql_query(&mysql,sql);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        if(strcmp(Buf.users.username,row[2]) == 0)
        {
            memset(post_buf,0,sizeof(struct node));
            post_buf->type = atoi(row[0]) ;
            switch(post_buf->type)
            {
                case 300://加入群成功
                    strcpy(post_buf->mes.group,row[4]);
                    break;
                case 302://群主拒绝
                    strcpy(post_buf->mes.group,row[4]);
                    break;
                case 303://申请加群
                    strcpy(post_buf->mes.from_who,row[1]);
                    strcpy(post_buf->mes.content,row[3]);
                    break;
                case 500://邀请加入群
                    strcpy(post_buf->mes.from_who,row[1]);
                    strcpy(post_buf->mes.group,row[4]);
                    break;
                case 600://申请好友
                    strcpy(post_buf->mes.from_who,row[1]);
                    strcpy(post_buf->mes.content,row[3]);
                    break;
                case 604://拒绝好友
                    strcpy(post_buf->mes.to_who,row[2]);
                    break;
                case 603://加好友成功
                    strcpy(post_buf->mes.to_who,row[2]);
                    break;
                case 900://私聊消息
                    strcpy(post_buf->mes.content,row[3]);
                    strcpy(post_buf->mes.from_who,row[1]);
                    break;
                case 1100://群聊消息
                    strcpy(post_buf->mes.content,row[3]);
                    strcpy(post_buf->mes.from_who,row[1]);
                    break;
                case 1300://文件请求
                    strcpy(post_buf->mes.from_who,row[1]);
                    break;
                default:
                    printf("type = %d\n",post_buf->type);
                    break;
            }
            if(send(con_fd,post_buf,sizeof(struct node), 0) < 0)
                my_err("send",__LINE__);
        }
    }
}
void add_his(char *err_string ,int line)
{
    time_t *timep =(time_t*)malloc(sizeof(time_t));
    time(timep);
    char *s = ctime(timep);
    sprintf(err_string," ",s,"\0");
    int fd;
    fd = open(err_his ,O_WRONLY);
    write(fd , err_string ,sizeof(err_string));
    close(fd);
}

void sign_handle(int signo)
{
    
}
void my_err(char *err_string ,int line)
{
    fprintf(stderr , "line %d ",line);
    perror(err_string);
    add_his(err_string ,line);
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
            return 1;
    }



}


int check_fri(struct node Buf)
{
    MYSQL_ROW row;
    MYSQL_RES *result;

    char sql[128];
    sprintf(sql,"select * from friends where username='%s' and friend='%s';",Buf.mes.to_who,Buf.mes.from_who,Buf.mes.to_who);
    /*连接myqsl查找好友 */
    int res = mysql_query(&mysql,sql);//正确返回0
    if(res)
        return 0;
    else
    {
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);
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
        strcpy(userinfo[i].name,Buf.users.username);
        userinfo[i].stat = 1;
        userinfo[i].conn_fd = con_fd;
        i++;
        send_message(con_fd,Buf);
    }
    else
        post_buf->type = 001; //帐号或密码错误
    if(send(con_fd , post_buf , sizeof(struct node ), 0) < 0)
        my_err("send",__LINE__);
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
            post_buf->type = 102;//注册失败
        else
        {
            strcpy(userinfo[i].name,Buf.users.username);
            userinfo[i].stat = 1;
            userinfo[i].conn_fd = con_fd;
            i++;
            post_buf->type = 100;//注册成功
        }
    }
    else
        post_buf->type =101;//帐号存在
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
        else
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
        if(send(con_fd ,post_buf,sizeof(struct node), 0) < 0)
            my_err("send",__LINE__);
    }
    else
    {
        Buf.type = 500;
        fd = find_fd(Buf.mes.to_who);
        if(fd )
            if(send(fd, &Buf,sizeof(struct node), 0 ) < 0)
                my_err("send",__LINE__);
        else
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
        else
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
            post_buf->type = 800;
            strcpy(post_buf->mes.content,row[0]);
            if(send(con_fd,post_buf,sizeof(struct node), 0) < 0)
                my_err("send",__LINE__);
            while(row = mysql_fetch_row(result))
            {
                strcpy(post_buf->mes.content,row[0]);
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
        Buf.type = 900;
        fd = find_fd(Buf.mes.to_who);
        if(fd)
            if(send(fd,&Buf,sizeof(struct node), 0) < 0)
                my_err("send",__LINE__);
        else
            message_box(Buf);
    }
    else
    {
        printf("name = %s\n",Buf.mes.to_who);
        post_buf->type = 901;//没有此好友
        if(send(con_fd,post_buf,sizeof(struct node), 0 )< 0)
            my_err("send",__LINE__);
    }
}
void his_pri(int con_fd , struct node Buf)
{
    MYSQL_ROW row;
    char sql[128];
    char content[BUFSIZE];
    int ret;
    MYSQL_RES *result;
    struct node *post_buf;
    post_buf = (struct node*)malloc(sizeof(struct node));
    int fd;
    if(check_fri(Buf) == 1)
    {
        sprintf(sql,"select chattime,history from his_pri where username='%s' and friend='%s';",Buf.mes.from_who,Buf.mes.to_who);
        ret = mysql_query(&mysql,sql);
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
                sprintf(content,"%s %s",row[0],row[1]);
                strcpy(post_buf->mes.content,content);
                post_buf->type = 1000;
                send(con_fd,post_buf,sizeof(struct node), 0);
                while(row = mysql_fetch_row(result))
                {
                    memset(content,0,sizeof(content));
                    sprintf(content,"%s %s",row[0],row[1]);
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
            while(row = mysql_fetch_row(result))
            {
                Buf.type = 1100;//成功
                fd = find_fd(row[0]); 
                strcpy(Buf.mes.to_who,row[0]);
                if(fd)
                    if(send(fd ,&Buf,sizeof(struct node), 0)< 0)
                        my_err("send",__LINE__);
                else
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
        sprintf(sql,"select chattime,history from his_com where username='%s' and groupname='%s';",Buf.mes.from_who,Buf.mes.group);
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
                post_buf->type = 1203;//没有历史记录
                if(send(con_fd,post_buf,sizeof(struct node), 0)< 0)
                my_err("send",__LINE__);
            }
            else
            {
                sprintf(content,"%s %s",row[0],row[1]);
                strcpy(post_buf->mes.content,content);
                post_buf->type = 1200;
                send(con_fd,post_buf,sizeof(struct node), 0);
                while(row = mysql_fetch_row(result))
                {
                    memset(content,0,sizeof(content));
                    sprintf(content,"%s %s",row[0],row[1]);
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
            if(send(fd,&Buf,sizeof(struct node), 0 )< 0)
                my_err("send",__LINE__);
        }
        else
            message_box(Buf);
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
    MYSQL_RES *result;
    char sql[128];
    int con_fd =(int)arg ;
    printf("con_fd = %d\n",con_fd);
    int flag ;
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
                sprintf(sql,"insert into chatgroup values('%s','%s','%s');",recv_buf->mes.group,recv_buf->mes.from_who,recv_buf->mes.to_who);
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
            case 1300://发送文件同意
                
                break;
            /*case 14://未读消息
                check_info(con_fd,*recv_buf);
                break;*/
            case 15://退出
                quit(con_fd ,*recv_buf);
                break;
            default:
                break;;
        }
    }
}

