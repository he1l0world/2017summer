/*************************************************************************
	> File Name: myshell.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月25日 星期二 08时04分01秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<dirent.h>
#include<signal.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<pwd.h>
#include<sys/utsname.h>
#define normal 0 //一般的命令
#define out_redirect 1 //输出重定向
#define in_redirect 2  //输入重定向
#define have_pipe 3 //命令中有管道
#define outt_redirect 4 //输出重定向>>

void print_prompt(); //打印提示符
void get_input(char *);  //得到输入的命令
void explain_input(char *,int *,char a[][256]);  //对输入命令解析
void do_cmd(int ,char a[][256]);  //执行命令
int find_command(char *);  //查找命令中的可执行程序
void handle_sight();
char *pre;
void handle_sight(int signo){
}

int main ()
{
    int i;
    signal(SIGINT, handle_sight);
    int argcount=0;
    char arglist[100][256];
    char **arg=NULL;
    char *buf=NULL;

    buf=(char*)malloc(256);
    if(buf==NULL)
    {
        perror("malloc failed");
        exit(1);
    }

    while(1)
    {
        //将buf空间清空
        memset(buf,0,256);
        print_prompt();
        get_input(buf);
        signal(SIGINT, handle_sight);
        //如果输入命令为exit或者logout推出程序
        if(strcmp(buf,"exit\n") ==0 ||strcmp(buf,"logout\n") == 0)
            break;
        if(strncmp(buf,"\n",2) == 0)
            continue;
        for(i=0;i<100;i++)
            arglist[i][0]='\0';
        argcount=0;
        explain_input(buf,&argcount,arglist);
        do_cmd(argcount,arglist);
    }
    if(buf!=NULL)
    {
        free(buf);
        buf=NULL;
    }
    exit(0);
}

void print_prompt()
{
    char buf[80];
    char a[20][40];
    int count=0;
    struct passwd *pw;
    struct utsname uts;

    pw=getpwuid(getuid());
    getcwd(buf,sizeof(buf));
    int j=0;
    for(int i=0;i<strlen(buf);i++)
    {
        if(buf[i]!='/')
            a[count][j++]=buf[i];
        if(buf[i]=='/')
        {
            a[count][j]='\0';
            count++;
            j=0;
        }
    }
    a[count][j]='\0';
    if(uname(&uts) < 0)
        printf("无法获取主机名");
    if(count==2)
        printf("[""\033[31;47m%s\033[0m""@%s ""\033[32;47m~\033[0m""]",pw->pw_name,uts.nodename);
    else
        printf("[""\033[31;47m%s\033[0m""@%s ""\033[32;47m%s\033[0m""]",pw->pw_name,uts.nodename,a[count]);
}
/*获取用户输入*/
void get_input(char *buf)
{
    char *a;
    /*int len=0;
    char ch;

        ch=getchar();
        while(len<256 && ch !='\n')
        {
            //printf("%c",ch);
            buf[len++]=ch;
            ch=getchar();
        }
        if(len == 256)
        {
            printf("command is too long \n");
            exit(-1);//命令过长
        }
        buf[len]='\n';
        buf[++len]='\0';*/
        a=readline("$ ");
        int len=strlen(a);
        if(len >= 256)
        {
            printf("command is too long\n");
            exit(-1);
        }
        a[len]='\n';
        a[++len]='\0';
        strncpy(buf,a,len);
        if(buf)
            add_history(buf);
}

/*解析buf的命令，将结果存入arglis，命令以回车结束*/
void explain_input(char *buf,int *argcount,char arglist[100][256])
{
    char *p=buf;
    char *q=buf;
    int number=0;

    while(1)
    {
        if(*p == '\n')
            break;
        if(*p == ' ')
        {
            p++;
        }
        else
        {
            q=p;
            number=0;
            while(*q !=' ' && *q != '\n')
            {
                number++;
                q++;
            }
            strncpy(arglist[*argcount],p,number+1);
            arglist[*argcount][number]='\0';
            (*argcount)++;
            p=q;
        }
    }
}


void do_cmd(int argcount,char arglist[100][256])
{
    int flag=0;
    int how=0;
    int background=0;
    int status;
    int i;
    int fd;
    char *arg[argcount+1];
    char *argnext[argcount+1];
    char *file;
    pid_t pid;

    //将命令取出
    for(i=0;i<argcount;i++)
        arg[i]=arglist[i];
    arg[argcount]=NULL;

    /*查看命令行是否有后台运行符*/
    for(i=0;i<argcount;i++)
    {
        if(strncmp(arg[i],"&",1) == 0)
        {
            if(i==argcount -1)
            {
                background=1;
                arg[argcount-1]=NULL;
                break;
            }
            else
            {
                printf("wrong command!\n");
                return ;
            }
        }
    }

    for(i=0;arg[i]!=NULL;i++)
    {
        if(strcmp(arg[i],">") == 0)
        {
            flag++;
            how = out_redirect;
            if(arg[i+1] == NULL)
                flag++;
        }
        if(strcmp(arg[i],"<") == 0)
        {
            flag++;
            how=in_redirect;
            if(i==0)
                flag++;
        }
        if(strcmp(arg[i],"|") == 0)
        {
            flag++;
            how=have_pipe;
            if(arg[i+1] == NULL)
                flag++;
            if(i==0)
                flag++;
        }
        if(strcmp(arg[i],">>") == 0)
        {
            flag++;
            how=outt_redirect;
            if(arg[i+1] == NULL)
                flag++;
        }
    }

    /*flag大于1，说明命令中含有多个>,<,|符号，所以是错误命令*/
    if(flag>1)
    {
        printf("wrong command");
        return;
    }
    
    if(how == out_redirect)
    {
        for(i=0;arg[i] != NULL;i++)
        {
            if(strcmp(arg[i],">") == 0)
            {
                file =arg[i+1];
                arg[i]=NULL;
            }
        }
    }
    if(how == outt_redirect)
    {
        for(i=0;arg[i]!=NULL;i++)
        {
            if(strcmp(arg[i],">>") == 0)
            {
                file =arg[i+1];
                arg[i]=NULL;
            }
        }
    }
    if(how==in_redirect)
    {
        for(i=0;arg[i] !=NULL;i++)
        {
            if(strcmp(arg[i],"<") == 0)
            {
                file=arg[i+1];
                arg[i]=NULL;
            }
        }
    }
    if(how==have_pipe)
    {
    /*把管道符号后门的部分放到argnext中，管道后面的部分是一个可执行的shell命令。*/
        for(i=0;arg[i] !=NULL;i++)
        {
            if(strcmp(arg[i],"|") == 0)
            {
                arg[i]=NULL;
                int j;
                for(j=i+1;arg[j] != NULL;j++)
                {
                    argnext[j-i-1] =arg[j];
                }
                argnext[j-i-1]=arg[j];
                break;
            }
        }
    }
    if((pid=fork()) < 0)
    {
        printf("fork error\n");
        return ;
    }

    switch(how)
    {
        case 0://命令不含>,<,|
        /*pid为零说明是子进程，在子进程中执行输入的命令*/
            if(pid == 0)
            {
                if(strcmp(arg[0],"cd") != 0&&!(find_command(arg[0])))
                {
                    printf("%s : command not found\n",arg[0]);
                    exit(0);
                }
                if(strcmp(arg[0],"cd") !=0)
                    execvp(arg[0],arg);
                exit(0);
            }
            else
            {
                if(strcmp(arg[0],"cd") == 0)
                {
                    if(arg[1]==NULL)
                    {
                        arg[1]="./";
                        pre=arg[1];
                    }
                    if(chdir(arg[1]) < 0)
                    {
                        printf("cd error\n");
                    }
                }
            }
            break;
        case 1://输入命令含有重定向>
            if(pid == 0)
            {
                if(!(find_command(arg[0])))
                {
                    printf("%s : command not found\n",arg[0]);
                    exit(0);
                }
                fd=open(file,O_RDWR|O_CREAT|O_TRUNC,0664);
                dup2(fd,1);
                execvp(arg[0],arg);
                exit(0);
            }
            break;
        case 2://输入命令含有输入重定向<
            if(pid == 0)
            {
                if(!(find_command(arg[0])))
                {
                    printf("%s :command not found!\n",arg[0]);
                    exit(0);
                }
                fd=open(file,O_WRONLY);
                dup2(fd,0);
                execvp(arg[0],arg);
                exit(0);
            }
        case 3://输入命令含有管道符|
            if(pid == 0)
            {
                int pid2;
                int status2;
                int fd2;

                if((pid2=fork()) < 0)
                {
                    printf("fork error\n");
                    return ;
                }
                else if (pid2 == 0)
                {
                    if(!(find_command(arg[0])))
                    {
                        printf("%s :command not found\n",arg[0]);
                        exit(0);
                    }
                    fd2 = open("/temp/youdonotknowfile",O_WRONLY|O_CREAT|O_TRUNC,0644);
                    dup2(fd2,1);
                    execvp(arg[0],arg);
                    exit(0);
                }

                if(waitpid(pid2,&status2,0) == -1 )
                    printf("wait for child process error\n");
                if(!(find_command(argnext[0])))
                {
                    printf("%s : command not found\n",argnext[0]);
                    exit(0);
                }
                fd2=open("/temp/youdonotknowfile",O_RDONLY);
                dup2(fd2,0);
                execvp(argnext[0],argnext);

                if(remove("/temp/youdonotknowfile"))
                    printf("remove error");
                exit(0);
            }
            break;
        case 4:
                if(pid == 0)
                {
                    if(!find_command(arg[0]))
                    {
                        printf("%s : comman not found\n",arg[0]);
                        exit(0);
                    }
                    fd=open(file,O_CREAT|O_APPEND|O_RDWR,0664);
                    //printf("%s\n",file);
                    if(fd== -1)
                        printf("无法打开\n");
                    dup2(fd,1);
                    execvp(arg[0],arg);
                    exit(0);
                }
                break;
        default:
            break;
    }

    /*如果命令有&，表示后台运行，父进程直接返回，不等待子进程结束*/
    if(background == 1)
    {
        printf("[process id %d]\n",pid);
        return ;
    }
    /*父进程等待子进程结束*/
    if(waitpid(pid,&status,0) == -1)
    {
        printf("wait for child process error\n");
    }
}

/*查找命令中可执行的程序*/
int find_command(char *command)
{
    DIR *dp;
    struct dirent *dirp;
    char *path[]={"./","/bin","/usr/bin",NULL};
    /*使当前目录下的程序可以运行，如命令"./fork"可以被正确解释和执行*/
    if(strncmp(command,"./",2) == 0)
        command+=2;

    /*分别在当前目录，/bin和/usr/bin目录查找执行的程序*/
    int i=0;
    while(path[i] !=NULL)
    {
        if((dp=opendir(path[i])) == NULL)
        {
            printf("can not open /bin \n");
        }
        while((dirp=readdir(dp)) != NULL)
        {
            if(strcmp(dirp->d_name,command) == 0)
            {
                closedir(dp);
                return 1;
            }
        }
        closedir(dp);
        i++;
    }
    return 0;
}
