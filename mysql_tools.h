
#ifndef _MYSQL_TOOLS_H
#define  _MYSQL_TOOLS_H
#include "mysql.h"
#include <windows.h>
#include"tools.h"
#include <stdio.h>
#include <stdlib.h>
#include "myBase_tools.h"
#define  MAXLENG_QBUF 500
#pragma comment (lib, "libmysql.lib")
#pragma comment (lib, "mysqlclient.lib")
//定义数据库操作的宏，也可以不定义留着后面直接写进代码


extern char host[];
extern char user[];
extern char passwd[];
extern char db[];
extern unsigned int port;


void initMysql(	
	char *thost,
	char *tuser,
	char *tpasswd,
	char *tdb,
	unsigned int tport);

void myToolConnet(MYSQL *mysql);

void init_insetPicture();
void init_insetLogo();
#endif