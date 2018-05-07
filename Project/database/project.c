/********************************************************************
* 标题：C语言链接mysql数据库，实现可以增删改查的角色权限登录系统
*
* 描述：本代码可在安装GCC编译环境和mysql集成环境下直接编译运行，根据不同使用者的mysql账户可修改代码前面的内容以成功连接mysql数据库
* 编译命令：gcc mysqltest.c -lmysqlclient -o mysqltest
********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mysql/mysql.h"

MYSQL *g_conn;//mysql 链接
MYSQL_RES *g_res;//mysql 记录集
MYSQL_ROW g_row;//字符串数组，mysql 记录行

#define MAX_BUF_SIZE 1024 //缓冲区最大字节数
/*=================================================================*/
/**/const char *g_host_name = "localhost";
/**/const char *g_user_name = "root";
/**/const char *g_password = "dees";
/**/const char *g_db_name = "project";
/**/const unsigned int g_db_port = 3306;
/*=================================================================*/
char sql[MAX_BUF_SIZE];
char Time[MAX_BUF_SIZE];

int iNum_rows = 0;//mysql语句执行结果返回行数赋初值
int flag = 0;//管理员权限开关
int i = 1;//系统运行开关
int id = 0;//role id
//登录使用的结构体
struct Login
{
    char name[24];
    char password[20];
}login;
//操作使用的结构体
struct Operation
{
    char tables[24];
    char name[24];
    char passwd[20];
    char role[24];
    char prescription[20];
    char insurance[20];
}ope;
//打印错误信息
void print_mysql_error(const char *msg)
{
    if(msg)
        printf("%s: %s\n",msg,mysql_error(g_conn));
    else
        puts(mysql_error(g_conn));
}
    //执行sql语句，成功返回0，失败返回-1
int executesql(const char * sql)
{
    if(mysql_real_query(g_conn,sql,strlen(sql)))
        return -1;
        return 0;
}
//初始化链接
int init_mysql()
{
    //init the database connection
    g_conn = mysql_init(NULL);
    //connection the database
    if(!mysql_real_connect(g_conn,g_host_name,g_user_name,g_password,g_db_name,g_db_port,NULL,0))
        return -1;//链接失败
    //检查是否可以使用
    if(executesql("set names utf8"))
        return -1;
    return 0;//返回成功
}
//选择数据库，没有的时候创建
void create_database()
{
    sprintf(sql,"use project");
    if(executesql(sql) == -1)
    {
        puts("create database");
        executesql("create database project;");
        print_mysql_error(NULL);
        puts("choice database");
        executesql("use project;");
        print_mysql_error(NULL);
        puts("!!!Initialize the success!!!");
    }
    else
    {
        executesql("use project;");
        print_mysql_error(NULL);
    }
}
//查看表格完整性
void create_table()
{
    //users表的检查与创建
    sprintf(sql,"show tables;");
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res);
    if(iNum_rows == 0)
    {
        puts("create users table");
        executesql("create table users(id_ smallint unsigned primary key auto_increment,name_ varchar(24) not null unique,password_ char(20) not null,prescription_ varchar(200),insurance_ varchar(200));");
        puts("create roles table");
        executesql("create table roles(id_ smallint unsigned primary key auto_increment,name_ varchar(24) not null unique,role_ varchar(200));");
        puts("create authoritys table");
        executesql("create table authoritys(id_ smallint unsigned primary key auto_increment,name_ varchar(24) not null unique,remark_ varchar(200));");
        puts("create roleAuthority table");
        executesql("create table roleAuthority(role_id_ smallint unsigned,authority_id_ smallint unsigned,primary key(role_id_,authority_id_),foreign key(role_id_ ) references roles(id_ ),foreign key(authority_id_) references authoritys(id_));");
        puts("create userRole table");
        executesql("create table userRole(user_id_ smallint unsigned,role_id_ smallint unsigned,primary key(user_id_,role_id_),foreign key(user_id_) references users(id_),foreign key(role_id_ ) references roles(id_));");
    }
    mysql_free_result(g_res); // 释放结果集
}
    //初始化管理员账户
void init_Administrtor()
{
    //查询users表
    sprintf(sql,"select * from users where id_='1' and name_='root';");
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res);
    if(iNum_rows == 0)
    {
        puts("Init Administrtor User");
        //插入管理员用户
        sprintf(sql,"insert into users values(1,'root','123','n/a','n/a');");
        executesql(sql);
    }
    mysql_free_result(g_res); // 释放结果集
    //查询role
    sprintf(sql,"select * from roles;");
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res);
    if(iNum_rows < 3)
    {
        puts("Init System Role");
        //插入系统角色
        sprintf(sql,"insert into roles values(1,'ROOT','root');");
        executesql(sql);
        sprintf(sql,"insert into roles values(2,'ADMINISTRITOR','administritor');");
        executesql(sql);
        sprintf(sql,"insert into roles values(3,'HEALTHY CARE PROVIDER','healthy care provider');");
        executesql(sql);
        sprintf(sql,"insert into roles values(4,'INSURANCE COMPANY','insurance company');");
        executesql(sql);
        sprintf(sql,"insert into roles values(5,'PATIENT','patient');");
        executesql(sql);
    }
    mysql_free_result(g_res); // 释放结果集
    //查询authoritys表
    sprintf(sql,"select * from authoritys;");
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res);
    if(iNum_rows < 3)
    {
        puts("Init System Authority");
        //插入系统权限
        sprintf(sql,"insert into authoritys values(1,'QUERY','query');");
        executesql(sql);
        sprintf(sql,"insert into authoritys values(2,'ADD','addmsg');");
        executesql(sql);
        sprintf(sql,"insert into authoritys values(3,'ADD&QUERY','query and addmsg');");
        executesql(sql);
        sprintf(sql,"insert into authoritys values(4,'DEL&ALT','delete and alter');");
        executesql(sql);
        sprintf(sql,"insert into authoritys values(5,'QUERY&DEL&ALT','query and delete and alter');");
        executesql(sql);
        sprintf(sql,"insert into authoritys values(6,'ADD&DEL&ALT','addmsg and delete and alter');");
        executesql(sql);
        sprintf(sql,"insert into authoritys values(7,'QUERY&ADD&DEL&ALT','query and addmsg and delete and alter');");
        executesql(sql);
    }
    mysql_free_result(g_res); // 释放结果集
    //查询roleAuthority表
    sprintf(sql,"select * from roleAuthority;");
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res);
    if(iNum_rows < 3)
    {
        puts("Init RoleAuthority");
        //插入角色权限
        sprintf(sql,"insert into roleAuthority values(1,7);");
        executesql(sql);
        sprintf(sql,"insert into roleAuthority values(2,7);");
        executesql(sql);
        sprintf(sql,"insert into roleAuthority values(3,7);");
        executesql(sql);
        sprintf(sql,"insert into roleAuthority values(4,7);");
        executesql(sql);
        sprintf(sql,"insert into roleAuthority values(5,1);");
        executesql(sql);
    }
    mysql_free_result(g_res); // 释放结果集
    //查询userRole表
    sprintf(sql,"select * from userRole where user_id_='1' and role_id_='1';");
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res);
    if(iNum_rows == 0)
    {
        puts("Init User Role");
        //插入管理员用户
        sprintf(sql,"insert into userRole values(1,1);");
        executesql(sql);
    }
    mysql_free_result(g_res); // 释放结果集
}

    //用户登录
void user_login()
{
    puts("Init success! Please press any key to continue");
    while(1)
    {
        while ((getchar()) != '\n');
        system("clear");
        puts("!!!Login System!!!");
        //输入账户和密码
        printf("Name：");scanf("%s",login.name);
        printf("Passwd：");scanf("%s",login.password);
        //在数据库中查询，可查询到信息即表明users表中有账号信息，登录成功
        sprintf(sql,"select * from users where name_='%s' and password_='%s';",login.name,login.password);
        executesql(sql);
        g_res = mysql_store_result(g_conn);
        iNum_rows = mysql_num_rows(g_res);
        if(iNum_rows != 0)
        {
            puts("!!! Login Success !!! ");
            while ((getchar()) != '\n');
            getchar();
            break;
        }
        else
        {
            puts("!!!Login Failed!!! Check name or password!");
            while ((getchar()) != '\n');
        }
    }
    mysql_free_result(g_res); // 释放结果集
}

//查询函数
void query_msg()
{
    while(1){
        int i,j;
        int choice;
        system("clear");
        puts("!!!    Query_msg    !!! ");
        puts("!!!   choice table  !!! ");
        puts("!!! 1:    users     !!! ");
        puts("!!! 2:    roles     !!! ");
        puts("!!! 3:    userRole  !!! ");
        puts("!!! 0:    Return    !!! ");
        scanf("%d",&choice);
        switch(choice)
        {
            case 1:
            sprintf(ope.tables,"users");
            system("clear");
            puts("!!!   enter name  !!! ");
            printf("Name：");scanf("%s",ope.name);
            //在指定表中查询用户名相关信息
            sprintf(sql,"select * from %s where name_='%s';",ope.tables,ope.name);
            executesql(sql);
            g_res = mysql_store_result(g_conn);
            iNum_rows = mysql_num_rows(g_res); // 得到记录的行数
            system("clear");
            if(iNum_rows == 0)
            puts("No such person!");
            else
            {
            int iNum_fields = mysql_num_fields(g_res); // 得到记录的列数
            puts("id_  | name_ |password_| prescription_ |  insurance  ");
            while((g_row=mysql_fetch_row(g_res)))
            printf("%s\t%s\t%s\t\t%s\t\t%s\n",g_row[0],g_row[1],g_row[2],g_row[3],g_row[4]);
            }
            mysql_free_result(g_res);
            while ((getchar()) != '\n');
            getchar();
            break;
            case 2:
            sprintf(ope.tables,"roles");
            system("clear");
            //查询角色表
            sprintf(sql,"select * from %s;",ope.tables);
            executesql(sql);
            g_res = mysql_store_result(g_conn);
            iNum_rows = mysql_num_rows(g_res); // 得到记录的行数
            system("clear");
            if(iNum_rows == 0)
            puts("No such person!");
            else
            {
            int iNum_fields = mysql_num_fields(g_res); // 得到记录的列数
            puts(" id_  | name_         |   role_      ");
            while((g_row=mysql_fetch_row(g_res)))
            printf("%s\t%s\t\t%s\n",g_row[0],g_row[1],g_row[2]);
            }
            mysql_free_result(g_res);
            while ((getchar()) != '\n');
            getchar();
            break;
            case 3:
            sprintf(ope.tables,"userRole");
            system("clear");
            //查询用户角色表
            sprintf(sql,"select * from %s;",ope.tables);
            executesql(sql);
            g_res = mysql_store_result(g_conn);
            iNum_rows = mysql_num_rows(g_res); // 得到记录的行数
            system("clear");
            if(iNum_rows == 0)
            puts("No such person!");
            else
            {
            int iNum_fields = mysql_num_fields(g_res); // 得到记录的列数
            puts(" user_id_ | role_id_ ");
            while((g_row=mysql_fetch_row(g_res)))
            printf("\t%s\t%s\n",g_row[0],g_row[1]);
            }
            mysql_free_result(g_res);
            while ((getchar()) != '\n');
            getchar();
            break;
            case 0:
            return ;
            default :
            puts("!!! enter right choice !!! ");
            while ((getchar()) != '\n');
            getchar();
            break;
        }
    }
}


//权限判定函数
int judge(int authority)
{
    int auth = 0;
    //通过当前登录的用户名字获取用户id
    sprintf(sql,"select id_ from users where name_='%s';",login.name);
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res);
    int iNum_fields = mysql_num_fields(g_res);
    while((g_row=mysql_fetch_row(g_res))){
    //通过当前登录用户的id查询这个用户的角色id
    sprintf(sql,"select role_id_ from userRole where user_id_='%s';",g_row[0]);
    }
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res);
    iNum_fields = mysql_num_fields(g_res);
    while((g_row=mysql_fetch_row(g_res))){
    //通过当前用户的角色id查询该用户的权限id
    sprintf(sql,"select authority_id_ from roleAuthority where role_id_='%s';",g_row[0]);
    }
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res);
    iNum_fields = mysql_num_fields(g_res);
    while((g_row=mysql_fetch_row(g_res))){
    //通过当前用户的权限id查询权限级别
    sprintf(sql,"select name_ from authoritys where id_='%s';",g_row[0]);
    }
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res);
    iNum_fields = mysql_num_fields(g_res);
    while((g_row=mysql_fetch_row(g_res))){
    if(strcmp(g_row[0],"QUERY") == 0)
    auth = 1;
    if(strcmp(g_row[0],"ADD") == 0)
    auth = 2;
    if(strcmp(g_row[0],"ADD&QUERY") == 0)
    auth = 3;
    if(strcmp(g_row[0],"DEL&ALT") == 0)
    auth = 4;
    if(strcmp(g_row[0],"QUERY&DEL&ALT") == 0)
    auth = 5;
    if(strcmp(g_row[0],"ADD&DEL&ALT") == 0)
    auth = 6;
    if(strcmp(g_row[0],"QUERY&ADD&DEL&ALT") == 0)
    auth = 7;
    if(auth >= 4)
    flag = 1;//管理员权限开关
    //判断该用户权限级别能否执行将要进行的操作
    if(auth < authority)
    {
    mysql_free_result(g_res); // 释放结果集
    return 0;
    }
    else
    {
    mysql_free_result(g_res); // 释放结果集
    return 1;
    }
    }
}

void role_id()
{
    sprintf(sql,"select id_ from users where name_='%s';",login.name);
    executesql(sql);
    g_res=mysql_store_result(g_conn);
    iNum_rows=mysql_num_rows(g_res);
    int iNum_fields=mysql_num_fields(g_res);
    while((g_row=mysql_fetch_row(g_res))){
    //通过当前登录用户的id查询这个用户的角色id
    sprintf(sql,"select role_id_ from userRole where user_id_='%s';",g_row[0]);
    }
    executesql(sql);
    g_res=mysql_store_result(g_conn);
    iNum_rows=mysql_num_rows(g_res);
    iNum_fields=mysql_num_fields(g_res);
    while((g_row=mysql_fetch_row(g_res)))
    {
        //通过当前用户的角色id查询该用户的权限id
        if(strcmp(g_row[0],"1")==0)
        id=1;
        if(strcmp(g_row[0],"2")==0)
        id=2;
        if(strcmp(g_row[0],"3")==0)
        id=3;
        if(strcmp(g_row[0],"4")==0)
        id=4;
        if(strcmp(g_row[0],"5")==0)
        id=5;
        mysql_free_result(g_res); // 释放结果集
    }
}

//添加函数
void add_msg()
{
    int o;
    char ID[20];
    //通过权限判定函数的返回值决定是否可以进行添加用户操作
    if(judge(2) == 0){
    puts("!!!Insufficient permissions!!! ");
    while ((getchar()) != '\n');
    getchar();
    //权限不够，退出函数
    return ;
    }
    //可以执行
    system("clear");
    puts("!!!    Add_user   !!! ");
    //根据当前已有用户的行数判断，新建的用户id应为行数+1
    sprintf(sql,"select id_ from users;");
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res); // 得到记录的行数
    int i = iNum_rows + 1;//新用户id
    //输入账户和密码
    printf("    Name：");scanf("%s",ope.name);
    printf("Password：");scanf("%s",ope.passwd);
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res);
    int iNum_fields = mysql_num_fields(g_res);
    while((g_row=mysql_fetch_row(g_res))){
    sprintf(ID,"%s",g_row[0]);
    }
    //备注
    printf("  Prescription：");scanf("%s",ope.prescription);
    //insurance
    printf(" Insurance Status: ");scanf("%s",ope.insurance);
    //向用户表中插入一个新的用户的信息
    sprintf(sql,"insert into users values(%d,'%s','%s','%s','%s');",i,ope.name,ope.passwd,ope.prescription,ope.insurance);
    executesql(sql);
    //管理员用户设定权限
    if(flag)
    {
    while(1){
    system("clear");
    puts("!!!    ROLE   !!! ");
    puts("!!! 2: ADMINISTRITOR   !!! ");
    puts("!!! 3: HEALTHY CARE PROVIDER !!! ");
    puts("!!! 4: INSURANCE COMPANY !!! ");
    puts("!!! 5: PATIENT   !!! ");
    printf("Choice the Role of user %s：",ope.name);scanf("%d",&o);
    switch(o){
    case 2:
    sprintf(ope.role,"ADMINISTRITOR");
    break;
    case 3:
    sprintf(ope.role,"HEALTHY CARE PROVIDER");
    break;
    case 4:
    sprintf(ope.role,"INSURANCE COMPANY");
    break;
    case 5:
    sprintf(ope.role,"PATIENT");
    break;
    default :
    puts("!!! enter right choice !!! ");
    while ((getchar()) != '\n');
    getchar();
    }
    break;
    }
    //通过角色名找到角色id
    sprintf(sql,"select id_ from roles where name_='%s';",ope.role);
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res);
    iNum_fields = mysql_num_fields(g_res);
    while((g_row=mysql_fetch_row(g_res))){
    //将用户id和角色id写入用户角色表
    sprintf(sql,"insert into userRole values(%d,%s);",i,g_row[0]);
    executesql(sql);
    }
    }
    //如果没有管理员权限，默认添加的用户角色为other
    else{
    sprintf(sql,"insert into userRole values(%d,3);",i);
    executesql(sql);
    }
    flag = 0;//管理员权限开关
    puts("!!! success !!! ");
    while ((getchar()) != '\n');
    getchar();
}


//删改函数
void alter_msg()
{
    int o,op;
    char p;
    char ID[20];
    //通过权限判定函数的返回值决定是否可以进行删改用户操作
    if(judge(4) == 0){
    puts("!!!Insufficient permissions!!! ");
    while ((getchar()) != '\n');
    getchar();
    //权限不够，退出函数
    return ;
    }
    //可以执行
    system("clear");
    puts("!!!     alter_msg  !!! ");
    printf("    Name：");scanf("%s",ope.name);
    //判断要进行删改的用户是不是管理员用户，禁止对管理员用户进行删改操作
    if(strcmp(ope.name,"root") == 0)
    {
    puts("ROOT user deletion is prohibited");
    while ((getchar()) != '\n');
    getchar();
    return;
    }
    //通过用户名和密码查看用户表中是否有该用户
    sprintf(sql,"select id_ from users where name_='%s';",ope.name);
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res); // 得到记录的行数
    int iNum_fields = mysql_num_fields(g_res);
    //将该用户id取出来备用
    while((g_row=mysql_fetch_row(g_res))){
    sprintf(ID,"%s",g_row[0]);
    }
    //没有查到
    if(iNum_rows == 0)
    {
    puts("No such person!");
    puts("!!! enter right choice !!! ");
    while ((getchar()) != '\n');
    getchar();
    }
    //进入删改选择
    else
    {
    system("clear");
    puts("!!!    del_alt_msg    !!! ");
    puts("!!!  1:change  name   !!! ");
    puts("!!!  2:change passwd  !!! ");
    puts("!!!  3:change  role   !!! ");
    puts("!!!  4:change prescription  !!! ");
    puts("!!!  5:change insurance status !!! ");
    printf("!!!      choice：     !");scanf("%d",&o);
    switch(o)
    {
    case 1:
    system("clear");
    puts("!!!    del_alt_msg    !!! ");
    printf("!!!    enter name: ");scanf("%s",ope.name);
    //更新用户名
    sprintf(sql,"update users set name_='%s' where id_=%s;",ope.name,ID);
    executesql(sql);
    break;
    case 2:
    system("clear");
    puts("!!!    del_alt_msg    !!! ");
    printf("!!!    enter password: ");scanf("%s",ope.passwd);
    //更新密码
    sprintf(sql,"update users set password_='%s' where id_=%s;",ope.passwd,ID);
    executesql(sql);
    break;
    case 3:
    system("clear");
    puts("!!!    del_alt_msg    !!! ");
    puts("!!!      1.ADMINISTRITOR       !!! ");
    puts("!!!      2.HEALTH CARE PROVIDER      !!! ");
    puts("!!!      3.INSURANCE COMPANY   !!! ");
    puts("!!!      4.PATIENT             !!! ");
    printf("!!!      choice：     !");scanf("%d",&op);
    switch(op)
    {
    case 1:
    //设置角色为admin
    sprintf(sql,"update userRole set role_id_=2 where user_id_=%s;",ID);
    executesql(sql);
    break;
    case 2:
    //设置角色为doctor
    sprintf(sql,"update userRole set role_id_=3 where user_id_=%s;",ID);
    executesql(sql);
    break;
    case 3:
    //设置角色insurace
    sprintf(sql,"update userRole set role_id_=4 where user_id_=%s;",ID);
    executesql(sql);
    break;
    case 4:
    //设置角色为patient
    sprintf(sql,"update userRole set role_id_=5 where user_id_=%s;",ID);
    executesql(sql);
    break;
    default :
    puts("!!! enter right choice !!! ");
    while ((getchar()) != '\n');
    getchar();
    }
    break;
    case 4:
    system("clear");
    puts("!!!    del_alt_msg    !!! ");
    printf("!!!    enter prescription: ");scanf("%s",ope.prescription);
    //更新备注
    sprintf(sql,"update users set description_='%s' where id_=%s;",ope.prescription,ID);
    executesql(sql);
    break;

    case 5:
    system("clear");
    puts("!!!    del_alt_msg    !!! ");
    printf("!!!    enter insurance status: ");scanf("%s",ope.insurance);
    //更新insurance
    sprintf(sql,"update users set insurance_='%s' where id_=%s;",ope.insurance,ID);
    executesql(sql);
    break;

    default :
    puts("!!! enter right choice !!! ");
    while ((getchar()) != '\n');
    getchar();
    }
    puts("!!! success !!! ");
    mysql_free_result(g_res);
    while ((getchar()) != '\n');
    getchar();
    }
}

void delete_msg()
{
    int o,op;
    char p;
    char ID[20];
    //通过权限判定函数的返回值决定是否可以进行删改用户操作
    if(judge(4) == 0){
        puts("!!!Insufficient permissions!!! ");
        while ((getchar()) != '\n');
        getchar();
        //权限不够，退出函数
        return ;
    }
    //可以执行
    system("clear");
    puts("!!!     alter_msg  !!! ");
    printf("    Name：");scanf("%s",ope.name);
    //判断要进行删改的用户是不是管理员用户，禁止对管理员用户进行删改操作
    if(strcmp(ope.name,"root") == 0)
    {
        puts("ROOT user deletion is prohibited");
        while ((getchar()) != '\n');
        getchar();
        return;
    }
    //通过用户名和密码查看用户表中是否有该用户
    sprintf(sql,"select id_ from users where name_='%s';",ope.name);
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res); // 得到记录的行数
    int iNum_fields = mysql_num_fields(g_res);
    //将该用户id取出来备用
    while((g_row=mysql_fetch_row(g_res))){
        sprintf(ID,"%s",g_row[0]);
    }
    //没有查到
    if(iNum_rows == 0)
    {
        puts("No such person!");
        puts("!!! enter right choice !!! ");
        while ((getchar()) != '\n');
        getchar();
    }
    else
    {
        system("clear");
        puts("!!!    delete_msg    !!! ");
        printf("!!!    sure delete? (Y/N):");scanf("%s",&p);
        switch(p)
        {
            case 'Y':
            case 'y':
            //需要先删除用户角色表当中的信息，才可删除用户表中的信息
            sprintf(sql,"delete from userRole where user_id_=%s;",ID);
            executesql(sql);
            sprintf(sql,"delete from users where id_=%s;",ID);
            executesql(sql);
            break;
            case 'N':
            case 'n':
            return;
        }
    }
}
    //显示所有用户及用户角色函数
void display()
{
    //通过权限判定函数的返回值决定是否可以进行显示所有用户操作，other用户不可使用此功能
    if(judge(2) == 0){
        puts("!!!Insufficient permissions!!! ");
        while ((getchar()) != '\n');
        getchar();
        //权限不够，退出函数
        return ;
    }
    //可以执行
    //查询users表
    sprintf(sql,"select * from users;");
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res); // 得到记录的行数
    int iNum_fields = mysql_num_fields(g_res); // 得到记录的列数
    system("clear");
    puts("!!!      users table   !!! \n");
    puts("id_  | name_ |password_|    prescription_    |insurance_  ");
    while((g_row=mysql_fetch_row(g_res)))
    printf("%s\t%s\t%s\t\t%s\t\t%s\n",g_row[0],g_row[1],g_row[2],g_row[3],g_row[4]);
    //查询roles表
    sprintf(sql,"select * from roles;");
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res); // 得到记录的行数
    iNum_fields = mysql_num_fields(g_res); // 得到记录的列数
    puts("\n\n!!!      roles table   !!! \n");
    puts(" id_  | name_                           | role__      ");
    while((g_row=mysql_fetch_row(g_res)))
    printf("%s\t%s\t\t%s\n",g_row[0],g_row[1],g_row[2]);
    //查询userRole表
    sprintf(sql,"select * from userRole;");
    executesql(sql);
    g_res = mysql_store_result(g_conn);
    iNum_rows = mysql_num_rows(g_res); // 得到记录的行数
    iNum_fields = mysql_num_fields(g_res); // 得到记录的列数
    puts("\n\n!!!    userRole table  !!! \n");
    puts(" user_id_ | role_id_ ");
    while((g_row=mysql_fetch_row(g_res)))
    printf("\t%s\t%s\n",g_row[0],g_row[1]);
    mysql_free_result(g_res);
    while ((getchar()) != '\n');
    getchar();
}
    //操作菜单
void menu()
{
    while(i)
    {   role_id();
        printf("id is : %d",id);
        int choice;
        system("clear");
        puts("!!!     choice：  !!! ");
        puts("!!! 1:query   msg !!! ");
        puts("!!! 2:add  user !!! ");
        puts("!!! 3:alter msg !!! ");
        puts("!!! 4:delete msg !!! ");
        puts("!!! 5:display all !!! ");
        puts("!!! 6:exit  login !!! ");
        puts("!!! 0:exit system !!! ");
        scanf("%d",&choice);
        switch(choice)
        {
            case 1:
            query_msg();//无需判断权限，所有角色均可使用此查询
            break;
            case 2:
            add_msg();//添加用户操作
            break;
            case 3:
            alter_msg();//改用户操作
            break;
            case 4:
            delete_msg();//删用户操作
            break;
            case 5:
            display();//显示所有用户及用户角色
            break;
            case 6:
            //退出登录
            flag = 0;//管理员权限开关
            return;
            case 0:
            puts("!!! thank you for using !!! ");//退出系统
            i = 0;
            break;
            default :
            puts("!!! enter right choice !!! ");
            while ((getchar()) != '\n');
            getchar();
            break;
        }
    }
}



//主函数
int main(void)
{
    while(i)
    {
        puts("!!!The system is initializing!!!");
        //初始化链接
        if(init_mysql())
        print_mysql_error(NULL);
        create_database();
        create_table();
        //初始化管理员账户
        init_Administrtor();
        //用户登录
        user_login();
        //操作菜单
        menu();
    }
    // 关闭链接
    mysql_close(g_conn);
    return EXIT_SUCCESS;
}