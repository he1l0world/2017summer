/*************************************************************************
	> File Name: my_ls.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月21日 星期五 14时43分53秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h> 
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<linux/limits.h>
#include<dirent.h>
#include<grp.h>
#include<pwd.h>
#include<errno.h>

#define PARAM_NONE 0//无参数
#define PARAM_A 1//-a 显示所有文件
#define PARAM_L 2//-l 一行只显示一个文件的详细信息
#define PARAM_R 4//-R 显示目录下的所有文件
#define MAXROWLEN 80//一行显示最到字符数

int g_leave_len=MAXROWLEN;//一行剩余长度，用于输出对齐
int g_maxlen;//存放某目录下最长的文件名的长度
//char max[10000][MAXROWLEN];
int t=0;


void display(int flag,char *pathname);


void my_err(const char *err_string,int line)
{
    fprintf(stderr,"line:%d ",line);
    perror(err_string);
    exit(1);
}

/*获取文件属性并打印*/
void display_attribute(struct stat buf,char *name)
{
    char buf_time[32];
    struct passwd *psd;//从该结构体获取文件所有者的用户名
    struct group *grp;//从该结构体获取文件所属组的组名

    /*获取并打印文件类型*/
    if(S_ISLNK(buf .st_mode))
        printf("l");
    else if(S_ISREG(buf.st_mode))
        printf("-");
    else if(S_ISDIR(buf.st_mode))
        printf("d");
    else if (S_ISCHR(buf.st_mode))
        printf("c");
    else if(S_ISBLK(buf.st_mode))
        printf("b");
    else if(S_ISFIFO(buf.st_mode))
        printf("f");
    else if(S_ISSOCK(buf.st_mode))
        printf("s");

    /*获取并打印文件所有者的权限*/
    if(buf.st_mode & S_IRUSR)
        printf("r");
    else
        printf("-");
    if(buf.st_mode & S_IWUSR)
        printf("w");
    else
        printf("-");
    if(buf.st_mode & S_IXUSR)
        printf("x");
    else
        printf("-");

    /*获取并打印文件所有者同组的用户对该文件的操作权限*/
    if(buf.st_mode & S_IRGRP)
        printf("r");
    else
        printf("-");
    if(buf.st_mode & S_IWGRP)
        printf("w");
    else
        printf("-");
    if(buf.st_mode & S_IXGRP)
        printf("x");
    else
        printf("-");

    /*获取并打印其他用户对该文件的操作权限*/
    if(buf.st_mode & S_IROTH)
        printf("r");
    else
        printf("-");
    if(buf.st_mode & S_IWOTH)
        printf("w");
    else
        printf("-");
    if(buf.st_mode & S_IXOTH)
        printf("x");
    else
        printf("-");
    printf("   ");
    /*根据uid与gid获取文件所有者的用户名和组名*/

    psd= getpwuid(buf.st_uid);
    grp=getgrgid(buf.st_gid);
    printf("%4d ",buf.st_nlink);
    printf("%-8s",psd->pw_name);
    printf("%-8s",grp->gr_name);

    printf("%6d",buf.st_size);
    strcpy(buf_time,ctime(&buf.st_mtime));
    buf_time[strlen(buf_time) - 1] = '\0';
    printf("   %s",buf_time);
}

/*在没有使用-l时，打印一个文件名，打印时上下行对齐*/
void display_single(char *name)
{
    int i,len;

    //如果本行不足以打印一个文件名则换行
    if(g_leave_len < g_maxlen)
    {
        printf("\n");
        g_leave_len = MAXROWLEN;
    }
    len=strlen(name);
    len=g_maxlen -len;
    printf("%s",name);

    for(i=0;i<len;i++)
        printf(" ");
    printf("  ");
    //下面的2只是2个空格
    g_leave_len -= (g_maxlen + 2);
}

/* 根据命令行参数和完整路径名显示目标文件
 * 参数flag： 命令行参数
 * 参数pathname： 包含了文件名的路径名
 */

 void display_dir(int flag_param,char*path)
 {
     DIR *dir;
     struct stat buf;
     struct dirent *ptr,*p;
     int count=0;
     char *filenames1;
    //获取该目录下文件总数和最长文件名
     dir=opendir(path);
     if(dir == NULL)
        my_err("opendir",__LINE__);
     while((ptr=readdir(dir)) != NULL)
     {
        if(g_maxlen<strlen(ptr->d_name))
            g_maxlen=strlen(ptr->d_name);
        count++;
    }
     closedir(dir);

     if(count>256)
        my_err("too many files under this dir",__LINE__);


     int i,j,len=strlen(path);
     dir=opendir(path);
     for(i=0;i<count;i++)
     {
         ptr=readdir(dir);
         if(ptr==NULL)
            my_err("readdir",__LINE__);
         if(flag_param==4)
         {
             int len1=strlen(ptr->d_name);
             filenames1=(char*)malloc(sizeof(char)*len1);
             strcpy(filenames1,ptr->d_name);
             //filenames1='\0';
             display(flag_param,filenames1);
             free(filenames1);
         }
     }
     closedir(dir);
     dir=opendir(path);
     if(flag_param!=4)
     {
        char filenames[256][PATH_MAX+1],temp[PATH_MAX+1];
        for(i=0;i<count;i++)
         {
             ptr=readdir(dir);
            strncpy(filenames[i],path,len);
            filenames[i][len]='\0';
            strcat(filenames[i],ptr->d_name);
            filenames[i][len+strlen(ptr->d_name)]='\0';
         }
       for(i=0;i<count-1;i++)
        {
            for(j=0;j<count-1-i;j++)
            {
                if(strcmp(filenames[j],filenames[j+1]) > 0)
                {
                    strcpy(temp,filenames[j+1]);
                    temp[strlen(filenames[j+1])]='\0';
                    strcpy(filenames[j+1],filenames[j]);
                    filenames[j+1][strlen(filenames[j])]='\0';
                    strcpy(filenames[j],temp);
                    filenames[j][strlen(temp)]='\0';
                }
            }
        }
        for(i=0;i<count;i++)
            display(flag_param,filenames[i]);
    }
    closedir(dir);
    dir=opendir(path);
     if((flag_param & PARAM_L) == 0)
        printf("\n\n");

    //printf("     ");
    //printf("--%s--\n",filenames[i]);
        //printf("%d\n",flag_param);
    if(flag_param==4)
    {
        while((ptr=readdir(dir))!=NULL)
        {
            char *temp1;
            int len2=strlen(path),len3=strlen(ptr->d_name);
            temp1=(char*)malloc(sizeof(char)*(len2+len3+2));
            sprintf(temp1,"%s/%s",path,ptr->d_name);
            //printf("--%s--\n",path);
            stat(temp1,&buf);
            //printf("%d\n",S_ISDIR(buf.st_mode));
            if(S_ISDIR(buf.st_mode)==0)
                continue;
            if(S_ISDIR(buf.st_mode)==1&&ptr->d_name[0]!='.')
                printf("%s :\n\n",temp1);
            if(S_ISDIR(buf.st_mode)==1&&ptr->d_name[0]!='.')
            {
                display_dir(flag_param,temp1);
                free(temp1);
            }
        }
    }
    closedir(dir);
    return ;
 }

void display(int flag,char *pathname)
 {
     int i,j;
     struct stat buf;
     char name[NAME_MAX + 1];
     /*从路径中解析出文件名*/
     for(i=0,j=0;i<strlen(pathname);i++)
     {
         if(pathname[i] == '/')
         {
             j=0;
             continue;
         }
         name[j++]=pathname[i];
     }
     name[j]='\0';
     /*用lstat而不是stat以方便解析链接文件*/
     if(flag!=PARAM_R)
        if(lstat(pathname,&buf) == -1 )
            my_err("stat",__LINE__);

     switch(flag)
     {
         case PARAM_NONE://没有-l和-a选项
            if(name[0] !='.')
                display_single(name);
        break;
         case PARAM_A://-a：显示包括隐藏文件在内的所有文件
            display_single(name);
         break;
         case PARAM_L://-l：每个文件单独占一行，显示文件的详细信息
            if(name[0] != '.')
            {
                display_attribute(buf,name);
                printf("   %-s\n",name);
            }
        break;
         case PARAM_R:
            if(name[0]!='.')
                display_single(name);
         break;
         case PARAM_A + PARAM_L:
            display_attribute(buf,name);
            printf("   %-s",name);
         break;
        default:
         break;
    }
 }


 int main (int argc,char **argv)
 {
     int i,j,k,num;
     int flag_param = PARAM_NONE;
     char path[PATH_MAX+1];
     char param[32];//保存命令行参数，目标文件名和目录名不再此列
     struct stat buf;//参数种类，即是否有-l和-a选项

     /*命令行参数的解析，分析-l，-a，-al，-la选项*/
     j=0;
     num=0;
     for(i=0;i<argc;i++)
     {
         if(argv[i][0] == '-')
         {
             for(k=1;k<strlen(argv[i]);k++,j++)
             {
                 param[j]=argv[i][k];
             }
         num++;
         }
     }

     /*只支持参数a和l和R，如果含有其他选项就报错*/
     for(i=0;i<j;i++)
     {
         if(param[i]=='a')
         {
             flag_param |=PARAM_A;
             continue;
         }
         else if(param[i] == 'l')
         {
             flag_param |=PARAM_L;
             continue;
         }
         else if(param[i] == 'R')
         {
             flag_param |=PARAM_R;
         }
         else
         {
             printf("my_ls:%d invalid option -%c\n",__LINE__,param[i]);
             exit(1);
         }
     }
     param[j]='\0';

     //如果没有输入文件名或者目录，就显示当前目录
     if((num+1) == argc)
     {
         strcpy(path,"./");
         path[2]='\0';
        display_dir(flag_param,path);
         return 0;
     }
     
     i=1;
     do{
        //如果不是目标文件或目录，解析下一个命令行参数
         if(argv[i][0] == '-')
         {
             i++;
             continue;
         }
         else
         {
             strcpy(path,argv[i]);
             //如果目标文件或目录不存在，解析下一个命令行参数
             if(stat(path,&buf) == -1)
                my_err("stat",__LINE__);
             if(S_ISDIR(buf.st_mode))
             {
                 if(path[strlen(argv[i])-1] != '/')
                 {
                     path[strlen(argv[i])]='/';
                     path[strlen(argv[i])+1]='\0';
                 }
                 else
                    path[strlen(argv[i])]= '\0';
                 display_dir(flag_param,path);
                 i++;
             }
             else
             {
                 display(flag_param,path);
                 i++;
             }
         }
     }while(i<argc);
 
 return 0;
 }
