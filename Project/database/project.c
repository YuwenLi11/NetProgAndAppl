#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mysql/mysql.h"

MYSQL		*g_conn;        /* mysql connection */
MYSQL_RES	*g_res;         /* mysql rocord set*/
MYSQL_ROW	g_row;          /* mysql rocord row*/

#define MAX_BUF_SIZE 1024       /* max buffer */
/*=================================================================*/
/**/ const char		*g_host_name	= "localhost";
/**/ const char		*g_user_name	= "root";
/**/ const char		*g_password	= "dees";
/**/ const char		*g_db_name	= "project";
/**/ const unsigned int g_db_port	= 3306;
/*=================================================================*/
char	sql[MAX_BUF_SIZE];
char	Time[MAX_BUF_SIZE];

int	iNum_rows	= 0;    /* row value aftern execute mysql */
int	i		= 1;    /* system switch */
int	id		= 0;    /* role id of user who is using*/
/* structure for login */
struct Login {
	char	name[24];
	char	password[20];
} login;
/* structure for operation */
struct Operation {
	char	tables[24];
	char	name[24];
	char	passwd[20];
	int	role;
	char	prescription[20];
} ope;

/* handle error message */
void print_mysql_error( const char *msg )
{
	if ( msg )
		printf( "%s: %s\n", msg, mysql_error( g_conn ) );
	else
		puts( mysql_error( g_conn ) );
}


/* execute mysql，success return 0，fail return -1 */
int executesql( const char * sql )
{
	if ( mysql_real_query( g_conn, sql, strlen( sql ) ) )
		return(-1);
	return(0);
}


/* initial mysql */
int init_mysql()
{
	/* init the database connection */
	g_conn = mysql_init( NULL );
	/* connection the database */
	if ( !mysql_real_connect( g_conn, g_host_name, g_user_name, g_password, g_db_name, g_db_port, NULL, 0 ) )
		return(-1);     /* connection failed */
	/* check if is ok to use */
	if ( executesql( "set names utf8" ) )
		return(-1);
	return(0);              /* success */
}


/*select database, if no such a database,create one */
void create_database()
{
	sprintf( sql, "use project" );
	if ( executesql( sql ) == -1 )
	{
		puts( "create database" );
		executesql( "create database project;" );
		print_mysql_error( NULL );
		puts( "choice database" );
		executesql( "use project;" );
		print_mysql_error( NULL );
		puts( "!!!Initialize the success!!!" );
	}else  {
		executesql( "use project;" );
		print_mysql_error( NULL );
	}
}


/* check the table */
void create_table()
{
	/* check/create user table */
	sprintf( sql, "show tables;" );
	executesql( sql );
	g_res		= mysql_store_result( g_conn );
	iNum_rows	= mysql_num_rows( g_res );
	if ( iNum_rows == 0 )
	{
		puts( "create users table" );
		executesql( "create table users(id_ smallint unsigned primary key auto_increment,role_id_ smallint unsigned,name_ varchar(24) not null unique,password_ char(20) not null,prescription_ varchar(200));" );
	}

	mysql_free_result( g_res ); /* free record */
}


/* initial admin */
void init_Administrtor()
{
/* check if there is an admin in user table */
	sprintf( sql, "select * from users where id_='1' and name_='root';" );
	executesql( sql );
	g_res		= mysql_store_result( g_conn );
	iNum_rows	= mysql_num_rows( g_res );
	if ( iNum_rows == 0 )
	{
		puts( "Init Administrtor User" );
		/* insert admin user */
		sprintf( sql, "insert into users values(1,1,'admin','123','n/a');" );
		executesql( sql );
	}
	mysql_free_result( g_res );
}


/* login */
void user_login()
{
	puts( "Init success! Please press any key to continue" );
	while ( 1 )
	{
		while ( (getchar() ) != '\n' )
			;
		system( "clear" );
		puts( "!!!Login System!!!" );
		/* user name and passwd */
		printf( "Name：" ); scanf( "%s", login.name );
		printf( "Passwd：" ); scanf( "%s", login.password );
		/* check user table if there is the user，login success */
		sprintf( sql, "select * from users where name_='%s' and password_='%s';", login.name, login.password );
		executesql( sql );
		g_res		= mysql_store_result( g_conn );
		iNum_rows	= mysql_num_rows( g_res );
		if ( iNum_rows != 0 )
		{
			puts( "!!! Login Success !!! " );
			while ( (getchar() ) != '\n' )
				;
			getchar();
			break;
		}else  {
			puts( "!!!Login Failed!!! Check name or password!" );
			while ( (getchar() ) != '\n' )
				;
		}
	}

	mysql_free_result( g_res );
}


/* get role_id of the user who is using now*/
void role_id()
{
	sprintf( sql, "select role_id_ from users where name_='%s';", login.name );
	executesql( sql );
	g_res		= mysql_store_result( g_conn );
	iNum_rows	= mysql_num_rows( g_res );
	int iNum_fields = mysql_num_fields( g_res );
	while ( (g_row = mysql_fetch_row( g_res ) ) )
	{
/* 1:admin, 2:doctor, 3:patient */
		if ( strcmp( g_row[0], "1" ) == 0 )
			id = 1;
		if ( strcmp( g_row[0], "2" ) == 0 )
			id = 2;
		if ( strcmp( g_row[0], "3" ) == 0 )
			id = 3;
		mysql_free_result( g_res );
	}
}


/* judge target user role_id */
int judge( char u_id[20] )
{
	int target_id;

	/* select user from user id */
	sprintf( sql, "select role_id_ from users where id_='%s';", u_id );
	executesql( sql );
	g_res		= mysql_store_result( g_conn );
	iNum_rows	= mysql_num_rows( g_res );
	if ( iNum_rows == 0 )
	{
		mysql_free_result( g_res );
		return(0);
	}else  {
		int iNum_fields = mysql_num_fields( g_res );
		while ( (g_row = mysql_fetch_row( g_res ) ) )
		{
/* get target user role id */
			if ( strcmp( g_row[0], "1" ) == 0 )
				target_id = 1;
			if ( strcmp( g_row[0], "2" ) == 0 )
				target_id = 2;
			if ( strcmp( g_row[0], "3" ) == 0 )
				target_id = 3;

/* determine the user role id with target user role id */
			if ( id < target_id )
			{
				mysql_free_result( g_res );
				return(1);//get permission 
			}else  {
				mysql_free_result( g_res );
				return(0);// rejected
			}
		}
	}
}


/* query */
void query_msg()
{
	char u_id[20];
	system( "clear" );
	puts( "!!!   enter id !!! " );
	printf( "id：" ); scanf( "%s", u_id );
	/* judge the permision */
	if ( judge( u_id ) == 0 )
	{
		puts( "!!!Insufficient permissions!!! " );
		while ( (getchar() ) != '\n' )
			;
		getchar();
		/* rejected */
		return;
	}
	sprintf( sql, "select * from users where id_='%s';", u_id );
	executesql( sql );
	g_res		= mysql_store_result( g_conn );
	iNum_rows	= mysql_num_rows( g_res ); 
	system( "clear" );

	int iNum_fields = mysql_num_fields( g_res );
	puts( "id_|role_id_ | name_ |password_|    prescription_ " );
	while ( (g_row = mysql_fetch_row( g_res ) ) )
		printf( "%s\t%s\t%s\t%s\t\t%s\n", g_row[0], g_row[1], g_row[2], g_row[3], g_row[4] );

	mysql_free_result( g_res );
	while ( (getchar() ) != '\n' )
		;
	getchar();
}


/* add */
void add_msg()
{
	char	u_id[20];
	int	o;
	//for admin user
	if ( id == 1 )
	{
		system( "clear" );
		puts( "!!!    Add_user   !!! " );
		/* user id each time puls 1 */
		sprintf( sql, "select id_ from users;" );
		executesql( sql );
		g_res		= mysql_store_result( g_conn );
		iNum_rows	= mysql_num_rows( g_res );
		int i = iNum_rows + 1;                          /* new id */
		/* user name and passwd */
		printf( "    Name：" ); scanf( "%s", ope.name );
		printf( "Password：" ); scanf( "%s", ope.passwd );
		executesql( sql );
		g_res		= mysql_store_result( g_conn );
		iNum_rows	= mysql_num_rows( g_res );
		int iNum_fields = mysql_num_fields( g_res );
		while ( (g_row = mysql_fetch_row( g_res ) ) )
		{
			sprintf( u_id, "%s", g_row[0] );
		}
		/* prescription */
		printf( "  Prescription：" ); scanf( "%s", ope.prescription );
		/* role */
		printf( " ROLE:\n1: HEALTHY CARE PROVIDER\n2: PATIENT\n" ); scanf( "%d", &o );
		switch ( o )
		{
		case 1:
			ope.role = 2;
			break;
		case 2:
			ope.role = 3;
			break;
		default:
			puts( "!!! enter right choice !!! " );

			while ( (getchar() ) != '\n' )
				;
			getchar();
		}
		/* insert a new user */
		sprintf( sql, "insert into users values(%d,%d,'%s','%s','%s');", i, ope.role, ope.name, ope.passwd, ope.prescription );
		executesql( sql );
		puts( "!!! success !!! " );
		while ( (getchar() ) != '\n' )
			;
		getchar();
	}
	//for doctor
	else  {
		system( "clear" );
		puts( "!!!    Add_user   !!! " );
		sprintf( sql, "select id_ from users;" );
		executesql( sql );
		g_res		= mysql_store_result( g_conn );
		iNum_rows	= mysql_num_rows( g_res );
		int i = iNum_rows + 1; 
		printf( "    Name：" ); scanf( "%s", ope.name );
		printf( "Password：" ); scanf( "%s", ope.passwd );
		executesql( sql );
		g_res		= mysql_store_result( g_conn );
		iNum_rows	= mysql_num_rows( g_res );
		int iNum_fields = mysql_num_fields( g_res );
		while ( (g_row = mysql_fetch_row( g_res ) ) )
		{
			sprintf( u_id, "%s", g_row[0] );
		}
		printf( "  Prescription：" ); scanf( "%s", ope.prescription );
		/* cannot choose a role, default:add a patient */
		ope.role = 3;
		sprintf( sql, "insert into users values(%d,%d,'%s','%s','%s');", i, ope.role, ope.name, ope.passwd, ope.prescription );
		executesql( sql );
		puts( "!!! success !!! " );
		while ( (getchar() ) != '\n' )
			;
		getchar();
	}
}


/* alter */
void alter_msg()
{
	int	o, op;
	char	p;
	char	ID[20];
	char	u_id[20];

	system( "clear" );
	puts( "!!!   enter id !!! " );
	printf( "id：" ); scanf( "%s", u_id );
	/* judge permission */
	if ( judge( u_id ) == 0 )
	{
		puts( "!!!Insufficient permissions!!! " );
		while ( (getchar() ) != '\n' )
			;
		getchar();
		/* rejected */
		return;
	}

	sprintf( sql, "select id_ from users where id_='%s';", u_id );
	executesql( sql );
	g_res		= mysql_store_result( g_conn );
	iNum_rows	= mysql_num_rows( g_res );
	int iNum_fields = mysql_num_fields( g_res );
	while ( (g_row = mysql_fetch_row( g_res ) ) )
	{
		sprintf( ID, "%s", g_row[0] );
	}
	//for admin user
	if ( id == 1 )
	{
		system( "clear" );
		puts( "!!!    alt_msg    !!! " );
		puts( "!!!  1:change  name   !!! " );
		puts( "!!!  2:change passwd  !!! " );
		puts( "!!!  3:change  role   !!! " );
		printf( "!!!      choice：     !" ); scanf( "%d", &o );
		switch ( o )
		{
		case 1: system( "clear" );
			puts( "!!!    alt_msg    !!! " );
			printf( "!!!    enter name: " ); scanf( "%s", ope.name );
			/* renew user name */
			sprintf( sql, "update users set name_='%s' where id_=%s;", ope.name, ID );
			executesql( sql );
			break;
		case 2: system( "clear" );
			puts( "!!!    del_alt_msg    !!! " );
			printf( "!!!    enter password: " ); scanf( "%s", ope.passwd );
			/* renew paddwd */
			sprintf( sql, "update users set password_='%s' where id_=%s;", ope.passwd, ID );
			executesql( sql );
			break;
		case 3: system( "clear" );
			puts( "!!!    alt_msg    !!! " );
			puts( "!!!      1.HEALTH CARE PROVIDER      !!! " );
			puts( "!!!      2.PATIENT   !!! " );
			printf( "!!!      choice：     !" ); scanf( "%d", &op );
			switch ( op )
			{
			case 1: /* set role to be a doctor */
				sprintf( sql, "update users set role_id_=2 where id_=%s;", ID );
				executesql( sql );
				break;
			case 2: /* set role to be a patient */
				sprintf( sql, "update users set role_id_=3 where id_=%s;", ID );
				executesql( sql );
				break;
			default: puts( "!!! enter right choice !!! " );
				while ( (getchar() ) != '\n' )
					;
				getchar();
			}

			break;

		default:
			puts( "!!! enter right choice !!! " );
			while ( (getchar() ) != '\n' )
				;
			getchar();
		}
	}
	//for doctor user
	else  {
		system( "clear" );
		puts( "!!!  change prescription  !!! " );
		printf( "!!!    enter prescription: " ); scanf( "%s", ope.prescription );
		/* renew presecription */
		sprintf( sql, "update users set prescription_='%s' where id_=%s;", ope.prescription, ID );
		executesql( sql );
	}

	puts( "!!! success !!! " );
	mysql_free_result( g_res );
	while ( (getchar() ) != '\n' )
		;
	getchar();
}


/*delete */
void delete_msg()
{
	char	p;
	char	u_id[20], ID[20];

	system( "clear" );
	puts( "!!!   enter id !!! " );
	printf( "id：" ); scanf( "%s", u_id );
	/* judge permission */
	if ( judge( u_id ) == 0 )
	{
		puts( "!!!Insufficient permissions!!! " );
		while ( (getchar() ) != '\n' )
			;
		getchar();
		/* rejected */
		return;
	}

	sprintf( sql, "select id_ from users where id_='%s';", u_id );
	executesql( sql );
	g_res		= mysql_store_result( g_conn );
	iNum_rows	= mysql_num_rows( g_res );
	int iNum_fields = mysql_num_fields( g_res );
	while ( (g_row = mysql_fetch_row( g_res ) ) )
	{
		sprintf( ID, "%s", g_row[0] );
	}

	system( "clear" );
	puts( "!!!    delete_msg    !!! " );
	printf( "!!!    sure delete? (Y/N):" ); scanf( "%s", &p );
	switch ( p )
	{
	case 'Y': case 'y':
		/* delete the user by id */
		sprintf( sql, "delete from users where id_=%s;", ID );
		executesql( sql );
		break;
	case 'N': case 'n':
		return;
	}
}


/* show self information */
void show_self()
{	/* get self by login name and passwd */
	sprintf( sql, "select * from users where name_='%s' and password_='%s';", login.name, login.password );
	executesql( sql );
	g_res		= mysql_store_result( g_conn );
	iNum_rows	= mysql_num_rows( g_res );
	int iNum_fields = mysql_num_fields( g_res );

	system( "clear" );
	puts( "!!!  personal information  !!! \n" );
	puts( "id_|role_id_ | name_ |password_|    prescription_ " );
	while ( (g_row = mysql_fetch_row( g_res ) ) )
		printf( "%s\t%s\t%s\t%s\t\t%s\n", g_row[0], g_row[1], g_row[2], g_row[3], g_row[4] );
	mysql_free_result( g_res );
	while ( (getchar() ) != '\n' )
		;
	getchar();
}


/* alter self information */
void alter_self()
{
	int	o;
	char	u_id[20];

	sprintf( sql, "select * from users where name_='%s' and password_='%s';", login.name, login.password );
	executesql( sql );
	g_res		= mysql_store_result( g_conn );
	iNum_rows	= mysql_num_rows( g_res );
	int iNum_fields = mysql_num_fields( g_res );
	while ( (g_row = mysql_fetch_row( g_res ) ) )
	{
		sprintf( u_id, "%s", g_row[0] );
	}

	system( "clear" );
	puts( "!!!    alter_msg    !!! " );
	puts( "!!!  1:change  name   !!! " );
	puts( "!!!  2:change passwd  !!! " );
	printf( "!!!      choice：     !" ); scanf( "%d", &o );
	switch ( o )
	{
	case 1: system( "clear" );
		puts( "!!!    alter_msg    !!! " );
		printf( "!!!    enter name: " ); scanf( "%s", ope.name );
		/* renew user name */
		sprintf( sql, "update users set name_='%s' where id_=%s;", ope.name, u_id );
		executesql( sql );
		break;
	case 2: system( "clear" );
		puts( "!!!    alter_msg    !!! " );
		printf( "!!!    enter password: " ); scanf( "%s", ope.passwd );
		/* renew passwd */
		sprintf( sql, "update users set password_='%s' where id_=%s;", ope.passwd, u_id );
		executesql( sql );
		break;

	default: puts( "!!! enter right choice !!! " );
		while ( (getchar() ) != '\n' )
			;
		getchar();
	}

	puts( "!!! success !!! " );
	mysql_free_result( g_res );
	return;
}


/* display */
void display()
{
//for admin to get all users'information
	sprintf( sql, "select * from users;" );
	executesql( sql );
	g_res		= mysql_store_result( g_conn );
	iNum_rows	= mysql_num_rows( g_res );
	int iNum_fields = mysql_num_fields( g_res );
	system( "clear" );
	puts( "!!!      users table   !!! \n" );
	puts( "id_|role_id_ | name_ |password_|    prescription_ " );
	while ( (g_row = mysql_fetch_row( g_res ) ) )
		printf( "%s\t%s\t%s\t%s\t\t%s\n", g_row[0], g_row[1], g_row[2], g_row[3], g_row[4] );
	mysql_free_result( g_res );
	while ( (getchar() ) != '\n' )
		;
	getchar();
}


/* menu */
void menu()
{
	role_id();
	printf( "id is : %d", id );
	switch ( id )
	{
/* admin */
	case 1: {
		while ( i )
		{
			int choice;
			system( "clear" );
			puts( "!!!     choice：  !!! " );
			puts( "!!! 1:query  user !!! " );
			puts( "!!! 2:add  user !!! " );
			puts( "!!! 3:alter user !!! " );
			puts( "!!! 4:delete user !!! " );
			puts( "!!! 5:display all !!! " );
			puts( "!!! 6:show  self information !!! " );
			puts( "!!! 7:alter self information !!! " );
			puts( "!!! 8:exit  login !!! " );
			puts( "!!! 0:exit system !!! " );
			scanf( "%d", &choice );
			switch ( choice )
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
			case 8: 
				return;
			case 0: puts( "!!! thank you for using !!! " );
				i = 0;
				break;
			default: puts( "!!! enter right choice !!! " );
				while ( (getchar() ) != '\n' )
					;
				getchar();
				break;
			}
		}
	}


/* doctor */
	case 2: {
		while ( i )
		{
			int choice;
			system( "clear" );
			puts( "!!!     choice：  !!! " );
			puts( "!!! 1:query patient !!! " );
			puts( "!!! 2:add  patient !!! " );
			puts( "!!! 3:alter patient's prescription !!! " );
			puts( "!!! 4:delete paitent !!! " );
			puts( "!!! 5:show  self information !!! " );
			puts( "!!! 6:alter self information !!! " );
			puts( "!!! 7:exit  login !!! " );
			puts( "!!! 0:exit system !!! " );
			scanf( "%d", &choice );
			switch ( choice )
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
			case 7:  
				return;
			case 0: puts( "!!! thank you for using !!! " );

				i = 0;
				break;
			default: puts( "!!! enter right choice !!! " );
				while ( (getchar() ) != '\n' )
					;
				getchar();
				break;
			}
		}
	}


/* patient */
	case 3: {
		while ( i )
		{
			int choice;
			system( "clear" );
			puts( "!!!     choice：  !!! " );
			puts( "!!! 1:show  self information !!! " );
			puts( "!!! 2:alter self information !!! " );
			puts( "!!! 3:exit  login !!! " );
			puts( "!!! 0:exit system !!! " );
			scanf( "%d", &choice );
			switch ( choice )
			{
			case 1: show_self();
				break;
			case 2: alter_self();
				return;

			case 3:

				return;
			case 0: puts( "!!! thank you for using !!! " );

				i = 0;
				break;
			default: puts( "!!! enter right choice !!! " );
				while ( (getchar() ) != '\n' )
					;
				getchar();
				break;
			}
		}
	}

	default:
		puts( "!!! role id error !!! " );
	}
}


/* 主函数 */
int main( void )
{
	while ( i )
	{
		puts( "!!!The system is initializing!!!" );
		/* 初始化链接 */
		if ( init_mysql() )
			print_mysql_error( NULL );
		create_database();
		create_table();
		/* 初始化管理员账户 */
		init_Administrtor();
		/* 用户登录 */
		user_login();
		/* 操作菜单 */
		menu();
	}
	/* 关闭链接 */
	mysql_close( g_conn );
	return(EXIT_SUCCESS);
}


