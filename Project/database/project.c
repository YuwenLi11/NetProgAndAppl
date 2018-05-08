/********************************************************************
 * 标题：C语言链接mysql数据库，实现可以增删改查的角色权限登录系统
 *
 * 描述：本代码可在安装GCC编译环境和mysql集成环境下直接编译运行，根据不同使用者的mysql账户可修改代码前面的内容以成功连接mysql数据库
 * 编译命令：gcc project.c -lmysqlclient -o project
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
        int i = 1;//系统运行开关
        int id = 0;//role id
//登录使用的结构体
        struct Login {
        char name[24];
        char password[20];
        }login;
//操作使用的结构体
        struct Operation {
        char tables[24];
        char name[24];
        char passwd[20];
        int role;
        char prescription[20];
        char insurance[20];
        }ope;
//打印错误信息
        void print_mysql_error(const char *msg) {
        if(msg)
        printf("%s: %s\n",msg,mysql_error(g_conn));
        else
        puts(mysql_error(g_conn));
        }
        //执行sql语句，成功返回0，失败返回-1
        int executesql(const char * sql) {
        if(mysql_real_query(g_conn,sql,strlen(sql)))
        return -1;
        return 0;
        }
//初始化链接
        int init_mysql() {
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
        void create_database() {
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

        else {
        executesql("use project;");
        print_mysql_error(NULL);
        }
        }
//查看表格完整性
        void create_table() {
        //users表的检查与创建
        sprintf(sql,"show tables;");
        executesql(sql);
        g_res = mysql_store_result(g_conn);
        iNum_rows = mysql_num_rows(g_res);
        if(iNum_rows == 0)
        {
        puts("create users table");
        executesql("create table users(id_ smallint unsigned primary key auto_increment,role_id_ smallint unsigned,name_ varchar(24) not null unique,password_ char(20) not null,prescription_ varchar(200));");
         }

        mysql_free_result(g_res); // 释放结果集
        }
        //初始化管理员账户
        void init_Administrtor() {
//查询users表
        sprintf(sql,"select * from users where id_='1' and name_='root';");
        executesql(sql);
        g_res = mysql_store_result(g_conn);
        iNum_rows = mysql_num_rows(g_res);
        if(iNum_rows == 0)
        {
        puts("Init Administrtor User");
        //插入管理员用户
        sprintf(sql,"insert into users values(1,1,'admin','123','n/a');");
        executesql(sql);
        }
        mysql_free_result(g_res); // 释放结果集
        }

        //用户登录
        void user_login() {
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

        else {
        puts("!!!Login Failed!!! Check name or password!");
        while ((getchar()) != '\n');
        }
        }

        mysql_free_result(g_res); // 释放结果集
        }

//查询当前用户role_id
        void role_id() {
        sprintf(sql,"select role_id_ from users where name_='%s';",login.name);
        executesql(sql);
        g_res=mysql_store_result(g_conn);
        iNum_rows=mysql_num_rows(g_res);
        int iNum_fields=mysql_num_fields(g_res);
        while((g_row=mysql_fetch_row(g_res))) {
//通过当前用户的角色id查询该用户的权限id
        if(strcmp(g_row[0],"1")==0)
        id=1;
        if(strcmp(g_row[0],"2")==0)
        id=2;
        if(strcmp(g_row[0],"3")==0)
        id=3;
        if(strcmp(g_row[0],"4")==0)
        id=4;
        mysql_free_result(g_res); // 释放结果集
        }
        }


//judge target user role_id
        int judge(char u_id[20])
        {
        int target_id;

        //通过当前登录用户的id查询这个用户的角色id
        sprintf(sql,"select role_id_ from users where id_='%s';",u_id);
        executesql(sql);
        g_res=mysql_store_result(g_conn);
        iNum_rows=mysql_num_rows(g_res);
        if(iNum_rows == 0)
        {
        mysql_free_result(g_res);
        return 0;
        }
        else
        {
        int iNum_fields=mysql_num_fields(g_res);
        while((g_row=mysql_fetch_row(g_res))) {
//通过当前用户的角色id查询该用户的权限id
        if(strcmp(g_row[0],"1")==0)
        target_id=1;
        if(strcmp(g_row[0],"2")==0)
        target_id=2;
        if(strcmp(g_row[0],"3")==0)
        target_id=3;


        if(id < target_id)
        {
        mysql_free_result(g_res); // 释放结果集
        return 1;
        }
        else
        {
        mysql_free_result(g_res); // 释放结果集
        return 0;
        }
        }
        }
        }

        //查询函数
        void query_msg() {
        char u_id[20];
        system("clear");
        puts("!!!   enter id !!! ");
        printf("id：");scanf("%s",u_id);
        //在指定表中查询用户名相关信息
        if(judge(u_id) == 0){
        puts("!!!Insufficient permissions!!! ");
        while ((getchar()) != '\n');
        getchar();
        //权限不够，退出函数
        return ;
        }
        sprintf(sql,"select * from users where id_='%s';",u_id);
        executesql(sql);
        g_res = mysql_store_result(g_conn);
        iNum_rows = mysql_num_rows(g_res); // 得到记录的行数
        system("clear");

        int iNum_fields = mysql_num_fields(g_res); // 得到记录的列数
        puts("id_|role_id_ | name_ |password_|    prescription_ ");
        while((g_row=mysql_fetch_row(g_res)))
        printf("%s\t%s\t%s\t%s\t\t%s\n",g_row[0],g_row[1],g_row[2],g_row[3],g_row[4]);

        mysql_free_result(g_res);
        while ((getchar()) != '\n');
        getchar();
        }




//添加函数
        void add_msg() {
        char u_id[20];
        int o;
        if(id == 1)
        {
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
        sprintf(u_id,"%s",g_row[0]);
        }
        //备注
        printf("  Prescription：");scanf("%s",ope.prescription);
        //role
        printf(" ROLE:\n1: HEALTHY CARE PROVIDER\n2: PATIENT\n");scanf("%d",&o);
        switch(o)
        {
        case 1:
        ope.role=2;
        break;
        case 2:
        ope.role=3;
        break;
        default:
        puts("!!! enter right choice !!! ");

        while ((getchar()) != '\n');
        getchar();
        }
        //向用户表中插入一个新的用户的信息
        sprintf(sql,"insert into users values(%d,%d,'%s','%s','%s');",i,ope.role,ope.name,ope.passwd,ope.prescription);
        executesql(sql);
        puts("!!! success !!! ");
        while ((getchar()) != '\n');
        getchar();
        }
        else
        {
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
        sprintf(u_id,"%s",g_row[0]);
        }
        //备注
        printf("  Prescription：");scanf("%s",ope.prescription);
        //role
        ope.role=3;
        //向用户表中插入一个新的用户的信息
        sprintf(sql,"insert into users values(%d,%d,'%s','%s','%s');",i,ope.role,ope.name,ope.passwd,ope.prescription);
        executesql(sql);
        puts("!!! success !!! ");
        while ((getchar()) != '\n');
        getchar();
        }


        }



//改函数
        void alter_msg() {
        int o,op;
        char p;
        char ID[20];
        char u_id[20];

        system("clear");
        puts("!!!   enter id !!! ");
        printf("id：");scanf("%s",u_id);
        //在指定表中查询用户名相关信息
        if(judge(u_id) == 0){
        puts("!!!Insufficient permissions!!! ");
        while ((getchar()) != '\n');
        getchar();
        //权限不够，退出函数
        return ;
        }

        sprintf(sql,"select id_ from users where id_='%s';",u_id);
        executesql(sql);
        g_res = mysql_store_result(g_conn);
        iNum_rows = mysql_num_rows(g_res); // 得到记录的行数
        int iNum_fields = mysql_num_fields(g_res);
        while((g_row=mysql_fetch_row(g_res))){
        sprintf(ID,"%s",g_row[0]);
        }
        if (id ==1) {
        system("clear");
        puts("!!!    alt_msg    !!! ");
        puts("!!!  1:change  name   !!! ");
        puts("!!!  2:change passwd  !!! ");
        puts("!!!  3:change  role   !!! ");
        printf("!!!      choice：     !");scanf("%d",&o);
        switch(o)
        {
        case 1: system("clear");
        puts("!!!    alt_msg    !!! ");
        printf("!!!    enter name: ");scanf("%s",ope.name);
        //更新用户名
        sprintf(sql,"update users set name_='%s' where id_=%s;",ope.name,ID);
        executesql(sql);
        break;
        case 2: system("clear");
        puts("!!!    del_alt_msg    !!! ");
        printf("!!!    enter password: ");scanf("%s",ope.passwd);
        //更新密码
        sprintf(sql,"update users set password_='%s' where id_=%s;",ope.passwd,ID);
        executesql(sql);
        break;
        case 3: system("clear");
        puts("!!!    alt_msg    !!! ");
        puts("!!!      1.HEALTH CARE PROVIDER      !!! ");
        puts("!!!      2.PATIENT   !!! ");
        printf("!!!      choice：     !");scanf("%d",&op);
        switch(op)
        {
        case 1: //设置角色为doctor
        sprintf(sql,"update users set role_id_=1 where id_=%s;",ID);
        executesql(sql);
        break;
        case 2: //设置角色为patient
        sprintf(sql,"update users set role_id_=2 where id_=%s;",ID);
        executesql(sql);
        break;
default: puts("!!! enter right choice !!! ");
        while ((getchar()) != '\n');
        getchar();
        }

        break;

default :
        puts("!!! enter right choice !!! ");
        while ((getchar()) != '\n');
        getchar();
        }
        }

        else
        {
        system("clear");
        puts("!!!  change prescription  !!! ");
        printf("!!!    enter prescription: ");scanf("%s",ope.prescription);
        //更新备注
        sprintf(sql,"update users set prescription_='%s' where id_=%s;",ope.prescription,ID);
        executesql(sql);
        }

        puts("!!! success !!! ");
        mysql_free_result(g_res);
        while ((getchar()) != '\n');
        getchar();
        }

//删函数
        void delete_msg() {
        char p;
        char u_id[20],ID[20];

        system("clear");
        puts("!!!   enter id !!! ");
        printf("id：");scanf("%s",u_id);
        //在指定表中查询用户名相关信息
        if(judge(u_id) == 0){
        puts("!!!Insufficient permissions!!! ");
        while ((getchar()) != '\n');
        getchar();
        //权限不够，退出函数
        return ;
        }

        sprintf(sql,"select id_ from users where id_='%s';",u_id);
        executesql(sql);
        g_res = mysql_store_result(g_conn);
        iNum_rows = mysql_num_rows(g_res); // 得到记录的行数
        int iNum_fields = mysql_num_fields(g_res);
        while((g_row=mysql_fetch_row(g_res))){
        sprintf(ID,"%s",g_row[0]);
        }

        system("clear");
        puts("!!!    delete_msg    !!! ");
        printf("!!!    sure delete? (Y/N):");scanf("%s",&p);
        switch(p)
        {
        case 'Y': case 'y':
        //需要先删除用户角色表当中的信息，才可删除用户表中的信息
        sprintf(sql,"delete from users where id_=%s;",ID);
        executesql(sql);
        break;
        case 'N': case 'n':
        return;
        }
        }

//show self information
        void show_self() {
        sprintf(sql,"select * from users where name_='%s' and password_='%s';",login.name,login.password);
        executesql(sql);
        g_res=mysql_store_result(g_conn);
        iNum_rows=mysql_num_rows(g_res); // 得到记录的行数
        int iNum_fields=mysql_num_fields(g_res);
        //将该用户id取出来备用
        system("clear");
        puts("!!!  personal information  !!! \n");
        puts("id_|role_id_ | name_ |password_|    prescription_ ");
        while((g_row=mysql_fetch_row(g_res)))
        printf("%s\t%s\t%s\t%s\t\t%s\n",g_row[0],g_row[1],g_row[2],g_row[3],g_row[4]);
        mysql_free_result(g_res);
        while ((getchar()) != '\n');
        getchar();
        }

//alter self information
        void alter_self() {
        int o;
        char u_id[20];

        sprintf(sql,"select * from users where name_='%s' and password_='%s';",login.name,login.password);
        executesql(sql);
        g_res=mysql_store_result(g_conn);
        iNum_rows=mysql_num_rows(g_res); // 得到记录的行数
        int iNum_fields=mysql_num_fields(g_res);
        //将该用户id取出来备用
        while((g_row=mysql_fetch_row(g_res))){
        sprintf(u_id,"%s",g_row[0]);
        }

        system("clear");
        puts("!!!    alter_msg    !!! ");
        puts("!!!  1:change  name   !!! ");
        puts("!!!  2:change passwd  !!! ");
        printf("!!!      choice：     !");scanf("%d",&o);
        switch(o) {
        case 1: system("clear");
        puts("!!!    alter_msg    !!! ");
        printf("!!!    enter name: ");scanf("%s",ope.name);
        //更新用户名
        sprintf(sql,"update users set name_='%s' where id_=%s;",ope.name,u_id);
        executesql(sql);
        break;
        case 2: system("clear");
        puts("!!!    alter_msg    !!! ");
        printf("!!!    enter password: ");scanf("%s",ope.passwd);
        //更新密码
        sprintf(sql,"update users set password_='%s' where id_=%s;",ope.passwd,u_id);
        executesql(sql);
        break;

default: puts("!!! enter right choice !!! ");
        while ((getchar()) != '\n');
        getchar();
        }

        puts("!!! success !!! ");
        mysql_free_result(g_res);
        return;
        }


//显示所有用户及用户角色函数
        void display() {
//可以执行
        //查询users表
        sprintf(sql,"select * from users;");
        executesql(sql);
        g_res = mysql_store_result(g_conn);
        iNum_rows = mysql_num_rows(g_res); // 得到记录的行数
        int iNum_fields = mysql_num_fields(g_res); // 得到记录的列数
        system("clear");
        puts("!!!      users table   !!! \n");
        puts("id_|role_id_ | name_ |password_|    prescription_ ");
        while((g_row=mysql_fetch_row(g_res)))
        printf("%s\t%s\t%s\t%s\t\t%s\n",g_row[0],g_row[1],g_row[2],g_row[3],g_row[4]);
        mysql_free_result(g_res);
        while ((getchar()) != '\n');
        getchar();
        }
        //操作菜单
        void menu() {
        role_id();
        printf("id is : %d",id);
        switch(id)
        {
//admin
        case 1: {
        while(i)
        {
        int choice;
        system("clear");
        puts("!!!     choice：  !!! ");
        puts("!!! 1:query  user !!! ");
        puts("!!! 2:add  user !!! ");
        puts("!!! 3:alter user !!! ");
        puts("!!! 4:delete user !!! ");
        puts("!!! 5:display all !!! ");
        puts("!!! 6:show  self information !!! ");
        puts("!!! 7:alter self information !!! ");
        puts("!!! 8:exit  login !!! ");
        puts("!!! 0:exit system !!! ");
        scanf("%d",&choice);
        switch(choice)
        {
        case 1: query_msg();

        break;
        case 2: add_msg();

        break;
        case 3: alter_msg();

        break;
        case 4: delete_msg();

        break;
        case 5: display();

        break;
        case 6: show_self();
        break;
        case 7: alter_self();
        return;
        case 8: //退出登录
        return;
        case 0: puts("!!! thank you for using !!! ");
//退出系统
        i = 0;
        break;
default: puts("!!! enter right choice !!! ");
        while ((getchar()) != '\n');
        getchar();
        break;
        }
        }
        }


//doctor
        case 2: {
        while(i)
        {
        int choice;
        system("clear");
        puts("!!!     choice：  !!! ");
        puts("!!! 1:query patient !!! ");
        puts("!!! 2:add  patient !!! ");
        puts("!!! 3:alter patient's prescription !!! ");
        puts("!!! 4:delete paitent !!! ");
        puts("!!! 5:show  self information !!! ");
        puts("!!! 6:alter self information !!! ");
        puts("!!! 7:exit  login !!! ");
        puts("!!! 0:exit system !!! ");
        scanf("%d",&choice);
        switch(choice)
        {
        case 1: query_msg();

        break;
        case 2: add_msg();

        break;
        case 3: alter_msg();

        break;
        case 4: delete_msg();

        break;
        case 5: show_self();
        break;
        case 6: alter_self();
        return;
        case 7: //退出登录
        return;
        case 0: puts("!!! thank you for using !!! ");
//退出系统
        i = 0;
        break;
default: puts("!!! enter right choice !!! ");
        while ((getchar()) != '\n');
        getchar();
        break;
        }
        }
        }


//patient
        case 3: {
        while(i)
        {
        int choice;
        system("clear");
        puts("!!!     choice：  !!! ");
        puts("!!! 1:show  self information !!! ");
        puts("!!! 2:alter self information !!! ");
        puts("!!! 3:exit  login !!! ");
        puts("!!! 0:exit system !!! ");
        scanf("%d",&choice);
        switch(choice)
        {
        case 1: show_self();
        break;
        case 2: alter_self();
        return;

        case 3: //退出登录
//管理员权限开关
        return;
        case 0: puts("!!! thank you for using !!! ");
//退出系统
        i = 0;
        break;
default: puts("!!! enter right choice !!! ");
        while ((getchar()) != '\n');
        getchar();
        break;
        }
        }
        }

default :
        puts("!!! role id error !!! ");
        }
        }



//主函数
        int main(void) {
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
