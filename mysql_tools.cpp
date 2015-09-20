#include "mysql_tools.h"

//const int MAXLENG_QBUF=500;
char host[_MAX_PATH];
char user[_MAX_PATH];
char passwd[_MAX_PATH];
char db[_MAX_PATH];
unsigned int port;


void initMysql(	
	char *thost,
	char *tuser,
	char *tpasswd,
	char *tdb,
	unsigned int tport)
{//��ʼ������
	strcpy(host,thost);
	strcpy(user,tuser);
	strcpy(passwd,tpasswd);
	strcpy(db,tdb);
	port=tport;

}



void myToolConnet(MYSQL *mysql)
{
	if (!(mysql = mysql_real_connect//��������   ����sockָ��
		(mysql,host,user,passwd,db,port,NULL,0))) 
	{
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(mysql));//����ķ���
		perror("");
		exit(1);
	}
	/* �������ݿ�Ĭ���ַ��� */
/*	if ( mysql_set_character_set( mysql, "utf8" )) 
	{
		fprintf ( stderr , "����, %s/n" , mysql_error(  mysql) ) ;
	}
*/

/*
	if (!mysql_set_character_set(mysql, "gbk")) 
	{
		printf(  "New client character set: %s\n", mysql_character_set_name(mysql) );
	}
	*/

}



void init_insetPicture()
{
	MYSQL mysql,*sock;    //�������ݿ����ӵľ�����������ڼ������е�MySQL����
	MYSQL_RES *res;       //��ѯ��������ṹ����
	//	MYSQL_FIELD *fd ;     //�����ֶ���Ϣ�Ľṹ
	//	MYSQL_ROW row ;       //���һ�в�ѯ������ַ�������
	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���

	int k;
	int idBrand;

	//char һ�㽫\\\\ת��Ϊ\\  ��sql��buf��\\�ٴ�ת��Ϊ\  ;
	char *srcIndex="D:\\\\360\\\\Workspace\\\\C++\\\\LAB_work\\\\babaoli_pictureLab\\\\NEW_08\\\\burberry\\\\";//���ص�logo ��ַ����
	char tempsrc[_MAX_PATH];
	int temp=0;
	char tempint[20];
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

	///////////////////////////��ɲ���
	for (k=0;k<=16935;k++)
	{
		strcpy(tempsrc,srcIndex);
		intToChar(k,tempint);
		strcat(tempsrc,tempint);//(idLogo,logoName,logoAddress, brand_idbrand)
		strcat(tempsrc,".jpg");
		sprintf(qbuf,"insert into urlimage(idURLImage,ImageName,UploadDate,URLBase_idURLBase,ImageType_idImageType,ImageAddress)\
					 values (%d,\'%d \',\'2013-12-24 15:53:10\',1,2,\'%s\');",k,k,tempsrc);
		if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
		{
			fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		//	exit(1);
		}

	}

	/////////////////////////////�ͷ�
	//mysql_free_result(res);
	mysql_close(sock);
	//system("pause");
	//exit(0);
	return ;   //. Ϊ�˼��ݴ󲿷ֵı������������
}



void init_insetLogo()
{
	MYSQL mysql,*sock;    //�������ݿ����ӵľ�����������ڼ������е�MySQL����
	MYSQL_RES *res;       //��ѯ��������ṹ����
	//	MYSQL_FIELD *fd ;     //�����ֶ���Ϣ�Ľṹ
	//	MYSQL_ROW row ;       //���һ�в�ѯ������ַ�������
	char qbuf[MAXLENG_QBUF];      //��Ų�ѯsql����ַ���

	int k;
	int idBrand;

	//char һ�㽫\\\\ת��Ϊ\\  ��sql��buf��\\�ٴ�ת��Ϊ\  ;
	char *srcIndex="D:\\\\360\\\\Workspace\\\\C++\\\\LAB_work\\\\babaoli_pictureLab\\\\LOGO_ALL\\\\";//���ص�logo ��ַ����
	char tempsrc[_MAX_PATH];
	int temp=0;
	char tempint[20];
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

	///////////////////////////��ɲ���
	for (k=1;k<=29;k++)
	{
		strcpy(tempsrc,srcIndex);
		intToChar(k,tempint);
		strcat(tempsrc,tempint);
		strcat(tempsrc,".jpg");
		sprintf(qbuf,"insert into logo (idLogo,logoName,logoAddress, brand_idbrand)\
					 values (\'%d\',\'%d \',\'%s\',2);",k,k,tempsrc);
		if(mysql_real_query(sock,qbuf,(unsigned long)strlen(qbuf)))	
		{
			fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
			exit(1);
		}

	}

	/////////////////////////////�ͷ�
	//mysql_free_result(res);
	mysql_close(sock);
	//system("pause");
//	exit(0);
	return ;   //. Ϊ�˼��ݴ󲿷ֵı������������
}

