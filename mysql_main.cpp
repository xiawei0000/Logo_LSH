#include"mysql_main.h"
//extern const int MAXLENG_QBUF;
//��Ϊ��ֵ̬


//int   NUMBER_URLBASEINDEX =10;//һ�δ���5��urlbase

void  __main(int argc, char **argv) //char **argv �൱�� char *argv[]
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
{/* ֱ��д�� ƥ���logo ��ƥ���ͼid
 */
	MYSQL mysql,*sock;    //�������ݿ����ӵľ�����������ڼ������е�MySQL����
	MYSQL_RES *res;       //��ѯ��������ṹ����
	//	MYSQL_FIELD *fd ;     //�����ֶ���Ϣ�Ľṹ
	//	MYSQL_ROW row ;       //���һ�в�ѯ������ַ�������
	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���

	int k;

	//char һ�㽫\\\\ת��Ϊ\\  ��sql��buf��\\�ٴ�ת��Ϊ\  ;
	//char *srcIndex="D:\\\\360\\\\Workspace\\\\C++\\\\LAB_work\\\\babaoli_pictureLab\\\\NEW_08\\\\burberry\\\\";//���ص�logo ��ַ����
	char tempsrc[_MAX_PATH];
	int temp=0;
	char tempint[20];

	//int URLImage_idURLImage;
	//const int TortType_idTortType=2;
	//int AuthorityImage_idAuthorityImage;


	/////////////////��������
	mysql_init(&mysql);//��ʼ��
	sock=&mysql;
	myToolConnet(sock);

	/*
	if (!(sock = mysql_real_connect//��������   ����sockָ��
		(&mysql,"localhost","root","xiawei123","taobao",3306,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));//����ķ���
		perror("");
		//exit(1);
	}
*/
	///////////////////////////��ɲ���

	{
		sprintf(qbuf,"INSERT INTO `logotorts` (  `URLImage_idURLImage` ,`Logo_idLogo`)  \
					 VALUES ( \'%d\', \'%d\');"
			,URLImage_idURLImage,logoid);

		if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
		{
			fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		}

	}

	/////////////////////////////�ͷ�
	//mysql_free_result(res);
	mysql_close(sock);
//	system("pause");
//	exit(0);
	return ;   //. Ϊ�˼��ݴ󲿷ֵı������������


}




void mySqlInsertResult(int URLImage_idURLImage,double maxsim)
{/*���� logo�����index �� 
	ͼƬ�����index��
	д�룺ͼƬid �� ��Ȩ����2�� logo id
 */
	MYSQL mysql,*sock;    //�������ݿ����ӵľ�����������ڼ������е�MySQL����
	MYSQL_RES *res;       //��ѯ��������ṹ����
	//	MYSQL_FIELD *fd ;     //�����ֶ���Ϣ�Ľṹ
	//	MYSQL_ROW row ;       //���һ�в�ѯ������ַ�������
	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���

	int k;

	//char һ�㽫\\\\ת��Ϊ\\  ��sql��buf��\\�ٴ�ת��Ϊ\  ;
	//char *srcIndex="D:\\\\360\\\\Workspace\\\\C++\\\\LAB_work\\\\babaoli_pictureLab\\\\NEW_08\\\\burberry\\\\";//���ص�logo ��ַ����
	char tempsrc[_MAX_PATH];
	int temp=0;
	char tempint[20];

	//int URLImage_idURLImage;
	const int TortType_idTortType=2;
	//int AuthorityImage_idAuthorityImage;


	/////////////////��������
	mysql_init(&mysql);//��ʼ��
	sock=&mysql;
	myToolConnet(sock);

	/*
	if (!(sock = mysql_real_connect//��������   ����sockָ��
		(&mysql,"localhost","root","xiawei123","taobao",3306,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));//����ķ���
		perror("");
		//exit(1);
	}
*/
	///////////////////////////��ɲ���

	{
		sprintf(qbuf,"INSERT INTO `imagetorts` (  `URLImage_idURLImage` ,`TortType_idTortType`,`match`)  \
					 VALUES ( \'%d\', \'%d\',\'%lf\');"
			,URLImage_idURLImage,TortType_idTortType,maxsim);

		if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
		{
			fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		}

	}

	/////////////////////////////�ͷ�
	//mysql_free_result(res);
	mysql_close(sock);
//	system("pause");
//	exit(0);
	return ;   //. Ϊ�˼��ݴ󲿷ֵı������������


}




int mySqlGetLogoFeats(int idLogo,int Cheng, LogoFeatStruct *logoFeat)
{/*��ѯһ��logo�����feat�����ݿ�
 */
	MYSQL mysql,*sock;    //�������ݿ����ӵľ�����������ڼ������е�MySQL����
	MYSQL_RES *res;       //��ѯ��������ṹ����
	MYSQL_ROW row ;       //���һ�в�ѯ������ַ�������
	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���
	char *end;
	char *sql ;
	int tempsize;
	clock_t start, finish;//����ʱ����
	double duration;

	 	//start = clock();//��ʼ��ʱ

	mysql_init(&mysql);//��ʼ��
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
			 {//���ݿ�û��feats
				 mysql_free_result(res);
				 mysql_close(sock);
				 return 0;   

			 }
			logoFeat->numberOffeat= atoi( row[0]);

			tempsize=sizeof(struct feature ) *logoFeat->numberOffeat;
			if (logoFeat->numberOffeat ==0 )
			{//��ʾ ������û�У�ͼ̫С��
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
//	printf( " ����logo ��ʱ ��%lf   \n", duration );

	return 1;   
}

void mySqlInsertLogoFeats(int idLogo,int Cheng, LogoFeatStruct logoFeat)
{/*����һ��logo�����feat�����ݿ�
 */
	MYSQL mysql,*sock;    //�������ݿ����ӵľ�����������ڼ������е�MySQL����
	MYSQL_RES *res;       //��ѯ��������ṹ����
	//	MYSQL_FIELD *fd ;     //�����ֶ���Ϣ�Ľṹ
	//	MYSQL_ROW row ;       //���һ�в�ѯ������ַ�������
	//char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���
	char *end;
	char *sql ;
	int sizeOfBuff;


	sizeOfBuff=sizeof(char)*MAXLENG_QBUF + logoFeat.numberOffeat* sizeof(struct feature)*2;
	sql=(char*)malloc(sizeOfBuff  );
	//�����ĵ���mysql_real_escape_string ��Ҫ�ռ���� 2*�����ƿռ� +1 �Ĵ�С��
	/////////////////��������

	mysql_init(&mysql);//��ʼ��
	sock=&mysql;
	myToolConnet(sock);
 
	///////////////////////////��ɲ���

	{

		/*sprintf(qbuf,"INSERT INTO `LogoFeats` (idLogo,Cheng,NumberFeats,Feats) VALUES \
					 ( %d, %d,%d,%s);"
					 ,LogoId,Cheng,logoFeat.numberOffeat,(char*)logoFeat.feat1);
					 */
		sprintf( sql,"REPLACE INTO `LogoFeat` (idLogo,Cheng,NumberFeats,Feats) VALUES \
					 ( %d, %d,%d," ,idLogo,Cheng,logoFeat.numberOffeat);
 //����replace������
		end = sql + strlen(sql);
		*end++ = '\'';
		end += mysql_real_escape_string(&mysql, end,
			(char *)(logoFeat.feat1), logoFeat.numberOffeat* sizeof(struct feature));
		*end++ = '\'';
		*end++ = ')';

		printf("end - sql: %d\n", (unsigned int)(end - sql));

		if(mysql_real_query(&mysql, sql, (unsigned int)(end - sql)))
		{
			fprintf(stderr, "������������� Query failed (%s)\n", mysql_error(&mysql));		
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
{//��idproject ���Ҷ�Ӧ urlbase ��� Ȼ��ͳ���ܵ�row ��

	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���
	int k;
	//int numberOfRows;

	MYSQL_RES *resForurlbase,*res,*resForcount;  
	MYSQL_ROW row  ;
	MYSQL_ROW rowForcount  ;
	char  idURLBase[_MAX_PATH];

	int i;
	*numberOfRows=0;
	//��һ���� ��urlbase�� �ҵ�idproject��Ӧ�� idurlbase
	sprintf(qbuf,"SELECT   idURLBase  FROM  urlbase WHERE \
				 Project_idProject=%I64d AND State=1 ORDER BY AuditTime ASC;",idProgect);//��ʱ�������ѯ����


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
	

	row = mysql_fetch_row(resForurlbase);//���һ��
	if (row ==NULL)
	{
		printf("�Ѿ�������ϵĹ��� �� ���ݿ�URLBase�� û�� Project_idProject=%I64d ��ֵ��\n ",idProgect);
		mysql_close(sock);
		exit(1) ;
	}

	i=0;//�ڼ���urlbase
    do //�޸�Ϊֻ��һ��
 	{		
		strcpy((char*)idURLBase_Index[i],row[0]);
		i++;//����urlbase;


		strcpy(idURLBase,row[0]);

		/* ������  ���޸�
		//��������urlbase��stateΪ2
		sprintf(qbuf,"UPDATE urlbase SET State=2 WHERE \
					 idurlbase=\'%s\';",idURLBase);
		mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf));
		*/

		//��ʼ��ѯ
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

		rowForcount = mysql_fetch_row(resForcount);//�õ� row

		if (rowForcount==NULL)
		{
			;//����
		}
		else
			*numberOfRows  +=atoi(rowForcount[0]);//���ٸ���= picture��Ŀ

			mysql_free_result(resForcount);

			if (i >=  NUMBER_URLBASEINDEX)
			{//��ദ��5��
				break;
			}

 	} while  ( row = mysql_fetch_row(resForurlbase)) ;


	mysql_free_result(resForurlbase);

	return i;//���ص��״�����base

}


void mySqlGetPictureAddress
	(long long idProgect,
	MYSQL *sock, 
	srcIndexStruct **srcIndex,
	int *numberOfRows_ALL
	)
{//��idbrand ���Ҷ�Ӧ��picture

	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���
	int k;
	int i,numberof_urlbaseindex;//ʵ��ȡ�����ٸ�urlbase;
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


	//��ʼ������
	idURLBase_Index=(char **)malloc(sizeof( char *)*NUMBER_URLBASEINDEX);
	for (i=0; i<NUMBER_URLBASEINDEX; i++)
	{
		idURLBase_Index[i]=(char*) malloc(sizeof(char)* _MAX_PATH);
	}

	
	//   ***********��ֱ�Ӳ� ����Ŀ��
	numberof_urlbaseindex=mysqlGetAllRowsForPicture(idProgect,sock, numberOfRows_ALL, idURLBase_Index);

	if ( *numberOfRows_ALL == 0 )
	{//û�� ֱ���ͷ� ������
	
		for (i=0; i<NUMBER_URLBASEINDEX; i++)
		{//�ͷ� idURLBase_Index 
			free(idURLBase_Index[i]);// 
		}
		free(idURLBase_Index);

		return;
	
	}

	//  *********** ������ռ�
	//logo ��ַ���� char�������
	*srcIndex=(srcIndexStruct *)malloc(sizeof(srcIndexStruct )*(*numberOfRows_ALL));

	for (k=0;k < *numberOfRows_ALL; k++)
	{	
		(*srcIndex)[k].srcIndex=(char *)malloc(sizeof(char)*_MAX_PATH);
	}


	//
	
	/*
	//��һ���� ��urlbase�� �ҵ�idproject��Ӧ�� idurlbase
	sprintf(qbuf,"SELECT   idURLBase  FROM  urlbase WHERE \
				 Project_idProject=%I64d AND State=0 ORDER BY AuditTime ASC;",idProgect);//��ʱ�������ѯ����

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
	
	row = mysql_fetch_row(resForurlbase);//���һ��
	if (row ==NULL)
	{
		printf("���ݿ�URLBase�� û�� Project_idProject=%I64d ��ֵ ���Ѿ�������",idProgect);
		mysql_close(sock);
		exit(1) ;
	}
	*/

	 mysql_query(sock,"set names gbk");//�������ģ���ֹ��������

	 i=0;//��¼�����urlbase���
	//��һ�β�ѯ�� �õ��ܵ��У� �ڶ��β�ѯ��һ��������·��
	 for (i=0; i<numberof_urlbaseindex;i++)
	 {
			// ���Ƴ�urlbase
		strcpy(idURLBase,idURLBase_Index[i]);
	
		/*
		//��������urlbase ��stateΪ1
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
		}//��


		if (!(res=mysql_store_result(sock))) {
			fprintf(stderr,"Couldn't get result FROM %s\n", mysql_error(sock));
			mysql_close(sock);
			exit(1);
		}//��


		while (rowForpicture = mysql_fetch_row(res)) 
		{//�� ����urlbase������ ͼƬ����������
			strcpy( (*srcIndex)[tagNumberForAllRows].srcIndex,  rowForpicture[1]);
			(*srcIndex)[tagNumberForAllRows].id= atoi( rowForpicture[0]);
		

			/*  �����ã��޸���
			//��������urliamge �� stateΪ1
			sprintf(qbuf,"UPDATE urlimage SET State=1 WHERE \
						 idURLImage= %d;",(*srcIndex)[tagNumberForAllRows].id);

						
			mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf));
 */
			//�Ƿ��Ӱ��select�Ľ������ 
			tagNumberForAllRows++;
		} 
		mysql_free_result(res);//�ͷ�

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
{//��idbrand ���Ҷ�Ӧ��logo


	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���
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

		//�鵽���������ã���������������������
	if (!(res=mysql_store_result(sock))) {
		fprintf(stderr,"Couldn't get result FROM %s\n", mysql_error(sock));
			mysql_close(sock);
		exit(1);
	}

	*numberOfRows = mysql_num_rows(res);//���ٸ���= logo��Ŀ

	//logo ��ַ���� char�������
		
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
	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���
	int k;
	int idBrand;
	
	MYSQL_RES *res;  
	MYSQL_ROW row  ;
//	long *lengths;

	/*
	sprintf(qbuf,"SELECT   Brand_idBrand  FROM view_projectimage \
				 WHERE idProject=%d;",idProgect);
				 */
	//û������ ����ֱ��progect��
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
		printf("���ݿ� û�� %I64d ��idBrand ֵ",idProgect);
			mysql_close(sock);
		exit(0) ;
	}
	idBrand=atoi(row[0]);

	mysql_free_result(res);

	return idBrand;
}

void mySqlLogoSrc(long long idProgect,int*numberOfRows,srcIndexStruct **srcIndex)
{
	MYSQL mysql,*sock;    //�������ݿ����ӵľ�����������ڼ������е�MySQL����
	MYSQL_RES *res;       //��ѯ��������ṹ����
//	MYSQL_FIELD *fd ;     //�����ֶ���Ϣ�Ľṹ
//	MYSQL_ROW row ;       //���һ�в�ѯ������ַ�������
	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���
//	int numberOfRows;
	int k;
	int idBrand;
//	char **srcIndex;//���ص�logo ��ַ����

	/////////////////��������
	mysql_init(&mysql);//��ʼ��
	sock=&mysql;
	myToolConnet(sock);
	/*

	if (!(sock = mysql_real_connect//��������   ����sockָ��
		(&mysql,"localhost","root","xiawei123","taobao",3306,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));//����ķ���
		perror("");
	 	exit(1);
	}
	*/

	/*
	sprintf(qbuf,SELECT_QUERY,atoi(argv[1]));//SELECT_QUERY �Ǻ궨�� ���+%s
	if(mysql_query(sock,qbuf)) {//��ѯ  ��䱣����qbuf����
		fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		exit(1);
	}
	*/
	//"insert into T_man values('abb',112); ",
	//	(unsigned long)strlen("insert into T_man values('abb',112); ") ) )

	///////////////////////////��ɲ�ѯ

	//��һ������brandid

	idBrand=mySqlGetidBrand
		( idProgect ,
		sock);

	//��logo·��
	mySqlGetLogoAddress
		( idBrand,
		sock , 
		srcIndex
		,numberOfRows);


	/////////////////////////////�ͷ�
	//mysql_free_result(res);
	mysql_close(sock);
	//system("pause");
	return ;   //. Ϊ�˼��ݴ󲿷ֵı������������
}



void mySqlPictureSrc(long long idProgect,int*numberOfRows,srcIndexStruct **srcIndex)
{//���ͼƬ��src����
	MYSQL mysql,*sock;    //�������ݿ����ӵľ�����������ڼ������е�MySQL����
	MYSQL_RES *res;       //��ѯ��������ṹ����
//	MYSQL_FIELD *fd ;     //�����ֶ���Ϣ�Ľṹ
//	MYSQL_ROW row ;       //���һ�в�ѯ������ַ�������
	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���

	
	int k;
	int idBrand;
	//���ص�logo ��ַ����

	/////////////////��������
	
	mysql_init(&mysql);//��ʼ��
	sock=&mysql;
	myToolConnet(sock);

	/*
	mysql_init(&mysql);//��ʼ��
	if (!(sock = mysql_real_connect//��������   ����sockָ��
		(&mysql,"localhost","root","xiawei123","taobao",3306,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));//����ķ���
		perror("");
		exit(1);
	}
	*/

	///////////////////////////��ɲ�ѯ





	//��picture·��
	mySqlGetPictureAddress
		( idProgect,
		sock , 
		srcIndex
		,numberOfRows);

	/////////////////////////////�ͷ�
	mysql_close(sock);
	return ;   //. Ϊ�˼��ݴ󲿷ֵı������������
}