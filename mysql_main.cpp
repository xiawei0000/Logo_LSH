#include"mysql_main.h"
//extern const int MAXLENG_QBUF;
//作为静态值


//int   NUMBER_URLBASEINDEX =10;//一次处理5个urlbase

void  __main(int argc, char **argv) //char **argv 相当于 char *argv[]
{
	//init_insetLogo();
	//	init_insetPicture();
//	mySqlLogoSrc(1);
	int numberOfRows;
	srcIndexStruct *srcIndex;
	//mySqlPictureSrc(1,&numberOfRows,&srcIndex);

	system("pause");
	return ;
}
/*
void mySqlInsertResult(int )
{
	mySqlInsertResultFromId(int URLImage_idURLImage,int AuthorityImage_idAuthorityImage);

}

FromId
*/



void mySqlInsertResultWithLogoId(int URLImage_idURLImage,int  logoid)
{/* 直接写入 匹配的logo 和匹配的图id
 */
	MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
	MYSQL_RES *res;       //查询结果集，结构类型
	//	MYSQL_FIELD *fd ;     //包含字段信息的结构
	//	MYSQL_ROW row ;       //存放一行查询结果的字符串数组
	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串

	int k;

	//char 一层将\\\\转义为\\  而sql的buf将\\再次转义为\  ;
	//char *srcIndex="D:\\\\360\\\\Workspace\\\\C++\\\\LAB_work\\\\babaoli_pictureLab\\\\NEW_08\\\\burberry\\\\";//返回的logo 地址数组
	char tempsrc[_MAX_PATH];
	int temp=0;
	char tempint[20];

	//int URLImage_idURLImage;
	//const int TortType_idTortType=2;
	//int AuthorityImage_idAuthorityImage;


	/////////////////建立连接
	mysql_init(&mysql);//初始化
	sock=&mysql;
	myToolConnet(sock);

	/*
	if (!(sock = mysql_real_connect//建立连接   返回sock指针
		(&mysql,"localhost","root","xiawei123","taobao",3306,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));//错误的返回
		perror("");
		//exit(1);
	}
*/
	///////////////////////////完成插入

	{
		sprintf(qbuf,"INSERT INTO `logotorts` (  `URLImage_idURLImage` ,`Logo_idLogo`)  \
					 VALUES ( \'%d\', \'%d\');"
			,URLImage_idURLImage,logoid);

		if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
		{
			fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		}

	}

	/////////////////////////////释放
	//mysql_free_result(res);
	mysql_close(sock);
//	system("pause");
//	exit(0);
	return ;   //. 为了兼容大部分的编译器加入此行


}




void mySqlInsertResult(int URLImage_idURLImage,double maxsim)
{/*读入 logo数组的index ， 
	图片数组的index，
	写入：图片id ， 侵权类型2， logo id
 */
	MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
	MYSQL_RES *res;       //查询结果集，结构类型
	//	MYSQL_FIELD *fd ;     //包含字段信息的结构
	//	MYSQL_ROW row ;       //存放一行查询结果的字符串数组
	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串

	int k;

	//char 一层将\\\\转义为\\  而sql的buf将\\再次转义为\  ;
	//char *srcIndex="D:\\\\360\\\\Workspace\\\\C++\\\\LAB_work\\\\babaoli_pictureLab\\\\NEW_08\\\\burberry\\\\";//返回的logo 地址数组
	char tempsrc[_MAX_PATH];
	int temp=0;
	char tempint[20];

	//int URLImage_idURLImage;
	const int TortType_idTortType=2;
	//int AuthorityImage_idAuthorityImage;


	/////////////////建立连接
	mysql_init(&mysql);//初始化
	sock=&mysql;
	myToolConnet(sock);

	/*
	if (!(sock = mysql_real_connect//建立连接   返回sock指针
		(&mysql,"localhost","root","xiawei123","taobao",3306,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));//错误的返回
		perror("");
		//exit(1);
	}
*/
	///////////////////////////完成插入

	{
		sprintf(qbuf,"INSERT INTO `imagetorts` (  `URLImage_idURLImage` ,`TortType_idTortType`,`match`)  \
					 VALUES ( \'%d\', \'%d\',\'%lf\');"
			,URLImage_idURLImage,TortType_idTortType,maxsim);

		if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
		{
			fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		}

	}

	/////////////////////////////释放
	//mysql_free_result(res);
	mysql_close(sock);
//	system("pause");
//	exit(0);
	return ;   //. 为了兼容大部分的编译器加入此行


}




int mySqlGetLogoFeats(int idLogo,int Cheng, LogoFeatStruct *logoFeat)
{/*查询一个logo单层的feat到数据库
 */
	MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
	MYSQL_RES *res;       //查询结果集，结构类型
	MYSQL_ROW row ;       //存放一行查询结果的字符串数组
	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串
	char *end;
	char *sql ;
	int tempsize;
	clock_t start, finish;//测试时间用
	double duration;

	 	//start = clock();//开始计时

	mysql_init(&mysql);//初始化
	sock=&mysql;
	myToolConnet(sock);

	

		sprintf(qbuf,"SELECT   NumberFeats,  Feats  FROM LogoFeat \
					 WHERE idLogo=%d AND Cheng=%d ;",idLogo,Cheng);

		if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
		{
			fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
			mysql_close(sock);
			exit(1);
		}

		if (!(res=mysql_store_result(sock))) {
			fprintf(stderr,"Couldn't get result FROM %s\n", mysql_error(sock));
			mysql_close(sock);
			exit(1);
		}


		 row = mysql_fetch_row(res) ;
		{
			 if( row== NULL)
			 {//数据库没有feats
				 mysql_free_result(res);
				 mysql_close(sock);
				 return 0;   

			 }
			logoFeat->numberOffeat= atoi( row[0]);

			tempsize=sizeof(struct feature ) *logoFeat->numberOffeat;
			if (logoFeat->numberOffeat ==0 )
			{//表示 特征点没有，图太小了
				logoFeat->feat1=NULL;
			}
			else
			{
				logoFeat->feat1=(struct feature *)malloc
					(sizeof(struct feature ) *logoFeat->numberOffeat);

				memcpy(logoFeat->feat1,row[1],
					sizeof(struct feature ) *logoFeat->numberOffeat);
			} 
		} 

		mysql_free_result(res);

	mysql_close(sock);
//	finish = clock();
//	duration = (double)(finish - start) / CLOCKS_PER_SEC;
//	printf( " 单个logo 耗时 ：%lf   \n", duration );

	return 1;   
}

void mySqlInsertLogoFeats(int idLogo,int Cheng, LogoFeatStruct logoFeat)
{/*保存一个logo单层的feat到数据库
 */
	MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
	MYSQL_RES *res;       //查询结果集，结构类型
	//	MYSQL_FIELD *fd ;     //包含字段信息的结构
	//	MYSQL_ROW row ;       //存放一行查询结果的字符串数组
	//char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串
	char *end;
	char *sql ;
	int sizeOfBuff;


	sizeOfBuff=sizeof(char)*MAXLENG_QBUF + logoFeat.numberOffeat* sizeof(struct feature)*2;
	sql=(char*)malloc(sizeOfBuff  );
	//按照文档，mysql_real_escape_string 需要空间大于 2*二进制空间 +1 的大小。
	/////////////////建立连接

	mysql_init(&mysql);//初始化
	sock=&mysql;
	myToolConnet(sock);
 
	///////////////////////////完成插入

	{

		/*sprintf(qbuf,"INSERT INTO `LogoFeats` (idLogo,Cheng,NumberFeats,Feats) VALUES \
					 ( %d, %d,%d,%s);"
					 ,LogoId,Cheng,logoFeat.numberOffeat,(char*)logoFeat.feat1);
					 */
		sprintf( sql,"REPLACE INTO `LogoFeat` (idLogo,Cheng,NumberFeats,Feats) VALUES \
					 ( %d, %d,%d," ,idLogo,Cheng,logoFeat.numberOffeat);
 //采用replace来处理
		end = sql + strlen(sql);
		*end++ = '\'';
		end += mysql_real_escape_string(&mysql, end,
			(char *)(logoFeat.feat1), logoFeat.numberOffeat* sizeof(struct feature));
		*end++ = '\'';
		*end++ = ')';

		printf("end - sql: %d\n", (unsigned int)(end - sql));

		if(mysql_real_query(&mysql, sql, (unsigned int)(end - sql)))
		{
			fprintf(stderr, "插入特征点错误 Query failed (%s)\n", mysql_error(&mysql));		
			free(sql);
			exit(1);
		}
	}
	
	free(sql);
	mysql_close(sock);
	return ;   
}


int  mysqlGetAllRowsForPicture(long long idProgect,
	MYSQL *sock, 
	int *numberOfRows,
	char **idURLBase_Index//[][_MAX_PATH]
	)
{//从idproject 查找对应 urlbase 多个 然后统计总的row ，

	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串
	int k;
	//int numberOfRows;

	MYSQL_RES *resForurlbase,*res,*resForcount;  
	MYSQL_ROW row  ;
	MYSQL_ROW rowForcount  ;
	char  idURLBase[_MAX_PATH];

	int i;
	*numberOfRows=0;
	//第一步： 查urlbase表， 找到idproject对应的 idurlbase
	sprintf(qbuf,"SELECT   idURLBase  FROM  urlbase WHERE \
				 Project_idProject=%I64d AND State=1 ORDER BY AuditTime ASC;",idProgect);//按时间升序查询排序


	if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
	{
		fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
			mysql_close(sock);
		exit(1);
	}
	
	if (!(resForurlbase=mysql_store_result(sock))) {
		fprintf(stderr,"Couldn't get result FROM %s\n", mysql_error(sock));
			mysql_close(sock);
		exit(1);
	}
	

	row = mysql_fetch_row(resForurlbase);//查第一个
	if (row ==NULL)
	{
		printf("已经处理完毕的工程 或 数据库URLBase表 没有 Project_idProject=%I64d 的值！\n ",idProgect);
		mysql_close(sock);
		exit(1) ;
	}

	i=0;//第几个urlbase
    do //修改为只查一次
 	{		
		strcpy((char*)idURLBase_Index[i],row[0]);
		i++;//保存urlbase;


		strcpy(idURLBase,row[0]);

		/* 测试用  不修改
		//马上设置urlbase的state为2
		sprintf(qbuf,"UPDATE urlbase SET State=2 WHERE \
					 idurlbase=\'%s\';",idURLBase);
		mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf));
		*/

		//开始查询
		sprintf(qbuf,"SELECT count(*)  FROM  urlimage  WHERE \
					 URLBase_idURLBase=\'%s\'AND State=0 ;",idURLBase);



		if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
		{
			fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
				mysql_close(sock);
			exit(1);
		}


		if (!(resForcount=mysql_store_result(sock))) {
			fprintf(stderr,"Couldn't get result FROM %s\n", mysql_error(sock));
				mysql_close(sock);
			exit(1);
		}

		rowForcount = mysql_fetch_row(resForcount);//得到 row

		if (rowForcount==NULL)
		{
			;//不变
		}
		else
			*numberOfRows  +=atoi(rowForcount[0]);//多少个行= picture数目

			mysql_free_result(resForcount);

			if (i >=  NUMBER_URLBASEINDEX)
			{//最多处理5个
				break;
			}

 	} while  ( row = mysql_fetch_row(resForurlbase)) ;


	mysql_free_result(resForurlbase);

	return i;//返回到底处理几个base

}


void mySqlGetPictureAddress
	(long long idProgect,
	MYSQL *sock, 
	srcIndexStruct **srcIndex,
	int *numberOfRows_ALL
	)
{//从idbrand 查找对应的picture

	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串
	int k;
	int i,numberof_urlbaseindex;//实际取出多少个urlbase;
	int tagNumberForAllRows;
	//int numberOfRows;

	MYSQL_RES *resForurlbase,*res,*resForcount;  
	MYSQL_ROW row  ;
	MYSQL_ROW rowForpicture  ;
	char  **idURLBase_Index;//[NUMBER_URLBASEINDEX][_MAX_PATH];
	char  idURLBase[_MAX_PATH];
	int numberOfRows;

	*numberOfRows_ALL=0;
	tagNumberForAllRows=0;


	//初始化数组
	idURLBase_Index=(char **)malloc(sizeof( char *)*NUMBER_URLBASEINDEX);
	for (i=0; i<NUMBER_URLBASEINDEX; i++)
	{
		idURLBase_Index[i]=(char*) malloc(sizeof(char)* _MAX_PATH);
	}

	
	//   ***********先直接查 行数目：
	numberof_urlbaseindex=mysqlGetAllRowsForPicture(idProgect,sock, numberOfRows_ALL, idURLBase_Index);

	if ( *numberOfRows_ALL == 0 )
	{//没有 直接释放 ，走人
	
		for (i=0; i<NUMBER_URLBASEINDEX; i++)
		{//释放 idURLBase_Index 
			free(idURLBase_Index[i]);// 
		}
		free(idURLBase_Index);

		return;
	
	}

	//  *********** 再申请空间
	//logo 地址是由 char数组组成
	*srcIndex=(srcIndexStruct *)malloc(sizeof(srcIndexStruct )*(*numberOfRows_ALL));

	for (k=0;k < *numberOfRows_ALL; k++)
	{	
		(*srcIndex)[k].srcIndex=(char *)malloc(sizeof(char)*_MAX_PATH);
	}


	//
	
	/*
	//第一步： 查urlbase表， 找到idproject对应的 idurlbase
	sprintf(qbuf,"SELECT   idURLBase  FROM  urlbase WHERE \
				 Project_idProject=%I64d AND State=0 ORDER BY AuditTime ASC;",idProgect);//按时间升序查询排序

	if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
	{
		fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
			mysql_close(sock);
		exit(1);
	}
	
	if (!(resForurlbase=mysql_store_result(sock))) {
		fprintf(stderr,"Couldn't get result FROM %s\n", mysql_error(sock));
			mysql_close(sock);
		exit(1);
	}
	
	row = mysql_fetch_row(resForurlbase);//查第一个
	if (row ==NULL)
	{
		printf("数据库URLBase表 没有 Project_idProject=%I64d 的值 或已经处理完",idProgect);
		mysql_close(sock);
		exit(1) ;
	}
	*/

	 mysql_query(sock,"set names gbk");//设置中文，防止中文乱码

	 i=0;//记录处理的urlbase编号
	//第一次查询， 得到总的列， 第二次查询才一个个复制路径
	 for (i=0; i<numberof_urlbaseindex;i++)
	 {
			// 复制出urlbase
		strcpy(idURLBase,idURLBase_Index[i]);
	
		/*
		//马上设置urlbase 的state为1
		sprintf(qbuf,"UPDATE urlbase SET State=1 WHERE \
					 idurlbase=\'%s\';",idURLBase);
		mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf));
		*/
		sprintf(qbuf,"SELECT  idURLImage ,ImageAddress  FROM urlimage \
					 WHERE URLBase_idURLBase=\'%s\'AND State=0 ORDER BY UploadDate ASC ;",idURLBase);

		if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
		{
			fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
			mysql_close(sock);
			exit(1);
		}//查


		if (!(res=mysql_store_result(sock))) {
			fprintf(stderr,"Couldn't get result FROM %s\n", mysql_error(sock));
			mysql_close(sock);
			exit(1);
		}//存


		while (rowForpicture = mysql_fetch_row(res)) 
		{//将 单个urlbase的所有 图片都存入数组
			strcpy( (*srcIndex)[tagNumberForAllRows].srcIndex,  rowForpicture[1]);
			(*srcIndex)[tagNumberForAllRows].id= atoi( rowForpicture[0]);
		

			/*  测试用，修改了
			//马上设置urliamge 的 state为1
			sprintf(qbuf,"UPDATE urlimage SET State=1 WHERE \
						 idURLImage= %d;",(*srcIndex)[tagNumberForAllRows].id);

						
			mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf));
 */
			//是否会影响select的结果？？ 
			tagNumberForAllRows++;
		} 
		mysql_free_result(res);//释放

	 }
	// } while  (row = mysql_fetch_row(resForurlbase)) ;


	//mysql_free_result(resForurlbase);

	 for (i=0; i<NUMBER_URLBASEINDEX; i++)
	 {
		 free(idURLBase_Index[i]);// 
	 }
	 free(idURLBase_Index);
	return ;
}



void mySqlGetLogoAddress
	(int idBrand,
	MYSQL *sock, 
	srcIndexStruct **srcIndex,int* numberOfRows
	)
{//从idbrand 查找对应的logo


	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串
	int k;
//	int numberOfRows;

	MYSQL_RES *res;  
	MYSQL_ROW row  ;

	sprintf(qbuf,"SELECT   idLogo,  logoAddress  FROM logo \
				 WHERE brand_idbrand=%d;",idBrand);
	if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
	{
		fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
			mysql_close(sock);
		exit(1);
	}

		//查到后，如何排序好？？？？？？？？？？？
	if (!(res=mysql_store_result(sock))) {
		fprintf(stderr,"Couldn't get result FROM %s\n", mysql_error(sock));
			mysql_close(sock);
		exit(1);
	}

	*numberOfRows = mysql_num_rows(res);//多少个行= logo数目

	//logo 地址是由 char数组组成
		
	*srcIndex=(srcIndexStruct *)malloc(sizeof(srcIndexStruct )*(*numberOfRows));

	for (k=0;k < *numberOfRows; k++)
	{	
		(*srcIndex)[k].srcIndex=(char *)malloc(sizeof(char)*_MAX_PATH);
	}

	k=0;
	while (row = mysql_fetch_row(res)) 
	{
		strcpy( (*srcIndex)[k].srcIndex,  row[1]);
		(*srcIndex)[k].id= atoi( row[0]);
		k++;
	} 


	mysql_free_result(res);

	return ;
}


int mySqlGetidBrand
(long long idProgect ,
MYSQL *sock)
{
	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串
	int k;
	int idBrand;
	
	MYSQL_RES *res;  
	MYSQL_ROW row  ;
//	long *lengths;

	/*
	sprintf(qbuf,"SELECT   Brand_idBrand  FROM view_projectimage \
				 WHERE idProject=%d;",idProgect);
				 */
	//没有数据 ，先直接progect找
	sprintf(qbuf,"SELECT   Brand_idBrand  FROM project \
				 WHERE idProject=%I64d;",idProgect);//%ld

	if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
	{
		fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		mysql_close(sock);
		exit(1);
	}


	if (!(res=mysql_store_result(sock))) {
		fprintf(stderr,"Couldn't get result FROM %s\n", mysql_error(sock));
			mysql_close(sock);
		exit(1);
	}

	//	printf("number of fields returned: %d\n",mysql_num_fields(res));
	//fd=mysql_fetch_field(res);


	//lengths = mysql_fetch_lengths(res);
	row = mysql_fetch_row(res);
	if (row ==NULL)
	{
		printf("数据库 没有 %I64d 的idBrand 值",idProgect);
			mysql_close(sock);
		exit(0) ;
	}
	idBrand=atoi(row[0]);

	mysql_free_result(res);

	return idBrand;
}

void mySqlLogoSrc(long long idProgect,int*numberOfRows,srcIndexStruct **srcIndex)
{
	MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
	MYSQL_RES *res;       //查询结果集，结构类型
//	MYSQL_FIELD *fd ;     //包含字段信息的结构
//	MYSQL_ROW row ;       //存放一行查询结果的字符串数组
	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串
//	int numberOfRows;
	int k;
	int idBrand;
//	char **srcIndex;//返回的logo 地址数组

	/////////////////建立连接
	mysql_init(&mysql);//初始化
	sock=&mysql;
	myToolConnet(sock);
	/*

	if (!(sock = mysql_real_connect//建立连接   返回sock指针
		(&mysql,"localhost","root","xiawei123","taobao",3306,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));//错误的返回
		perror("");
	 	exit(1);
	}
	*/

	/*
	sprintf(qbuf,SELECT_QUERY,atoi(argv[1]));//SELECT_QUERY 是宏定义 语句+%s
	if(mysql_query(sock,qbuf)) {//查询  语句保存在qbuf里面
		fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		exit(1);
	}
	*/
	//"insert into T_man values('abb',112); ",
	//	(unsigned long)strlen("insert into T_man values('abb',112); ") ) )

	///////////////////////////完成查询

	//第一步：查brandid

	idBrand=mySqlGetidBrand
		( idProgect ,
		sock);

	//查logo路径
	mySqlGetLogoAddress
		( idBrand,
		sock , 
		srcIndex
		,numberOfRows);


	/////////////////////////////释放
	//mysql_free_result(res);
	mysql_close(sock);
	//system("pause");
	return ;   //. 为了兼容大部分的编译器加入此行
}



void mySqlPictureSrc(long long idProgect,int*numberOfRows,srcIndexStruct **srcIndex)
{//获得图片的src数组
	MYSQL mysql,*sock;    //定义数据库连接的句柄，它被用于几乎所有的MySQL函数
	MYSQL_RES *res;       //查询结果集，结构类型
//	MYSQL_FIELD *fd ;     //包含字段信息的结构
//	MYSQL_ROW row ;       //存放一行查询结果的字符串数组
	char qbuf[MAXLENG_QBUF];      //存放查询sql语句字符串

	
	int k;
	int idBrand;
	//返回的logo 地址数组

	/////////////////建立连接
	
	mysql_init(&mysql);//初始化
	sock=&mysql;
	myToolConnet(sock);

	/*
	mysql_init(&mysql);//初始化
	if (!(sock = mysql_real_connect//建立连接   返回sock指针
		(&mysql,"localhost","root","xiawei123","taobao",3306,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));//错误的返回
		perror("");
		exit(1);
	}
	*/

	///////////////////////////完成查询





	//查picture路径
	mySqlGetPictureAddress
		( idProgect,
		sock , 
		srcIndex
		,numberOfRows);

	/////////////////////////////释放
	mysql_close(sock);
	return ;   //. 为了兼容大部分的编译器加入此行
}