//#define _WIN32_WINNT 0x0502  // Designed to run on Windows XP SP2 and up
//#define WINVER 0x0502
//#include <windows.h>
//#pragma comment(lib,"Kernel32.lib")
#include "main.h"
int CMDORLOG=0;//�����cmd����log�汾
 int  NUMBER_URLBASEINDEX=5;

int main( int argc, char** argv )
{
	 //   _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	//���̲߳���
	HANDLE * handle; 
	struct tempStructForThread_EachPicture *tempForThread_EachPicturemen;

	//���ݿ⣺����
		//�����
	long long idProgect;
	int numberOfLogo;//numberOfRows;
	srcIndexStruct *logo_srcIndex;//logo�ĵ�ַ+id ��

	int numberOfPicture;//numberOfRows;
	srcIndexStruct *Picture_srcIndex;// �ĵ�ַ+id ��


	char filePath[300];// ��һ��logo���ļ��� ="D:\\360\\Workspace\\C++\\LAB_work\\babaoli_pictureLab\\NEW_08\\";
	//char filePath[300]="D:\\360\\Workspace\\C++\\LAB_work\\babaoli_pictureLab\\NEW_08\\";
	//char filePath[300];
	//char srcLogoPath[300];
	//char srcTestPath[300];
	char dstTestPicturePath[_MAX_PATH];

	clock_t start, finish;
	double duration;

	int i,j=1,k;

	int numberOfTestPicture;
	int tempLeng;

	LogoFeatStruct ** logofeat;//���ڱ���һ�μ����logofeat

	CvMat  ***LogoMatrix;
	int realchipNumber;

	/*MYSQL ������
	const char *host,
	const char *user,
	const char *passwd,
	const char *db,
	unsigned int port,
	*/
	//��������
	char host[_MAX_PATH];
	char user[_MAX_PATH];
	char passwd[_MAX_PATH];
	char db[_MAX_PATH];
	unsigned int port;

	srcIndexInterlocked=-1;
	

	/*
	if (argc != 8)
	{
		printf("��������\n" );
		fclose(stdout);//�ر��ļ� 
		exit(1);
	}

	//��������	 
	YOUHUA_TYPE=atoi(argv[1]);//�ò����Ż�
	strcpy(host,argv[2]);
	strcpy(user,argv[3]);
	strcpy(passwd,argv[4]);
	strcpy(db,argv[5]);

	port=atoi(argv[6]);
	idProgect=CharToLongLong(argv[7]);//�����β�����a to int  Ҫ�� a to long?????????
	*/

	arg_parse(  argc,  argv ,	
		 &YOUHUA_TYPE, host,
		user, passwd,db, &port,
		 &idProgect,&chipNumber,
		 &NUMBER_URLBASEINDEX,&CMDORLOG);


	//���ԣ�  ���̼߳���
	chipNumber=4;

	if (CMDORLOG == 1 )
	{//ʹ��log
		myInitLogFile(dstTestPicturePath);//��ʼ�� ��־
	    freopen( dstTestPicturePath, "a", stdout );//�޸�Ϊ�ļ���_log//time_.log
	}
	printf("��ʼ��� idProject=%I64d �Ĺ���\n",idProgect);

	initMysql(	host,user,passwd,db,port);


	start = clock();//��ʼ��ʱ

	mutex  = CreateMutex(NULL, FALSE, NULL);  
	//SrcIndexmutex = CreateMutex(NULL, FALSE, NULL);  
	FilePathmutex= CreateMutex(NULL, FALSE, NULL);  

	printf("��ʼ  ��ѯlogo\n");
	mySqlLogoSrc( idProgect,&numberOfLogo,&logo_srcIndex);//���logo����
	printf("�õ�logo%d��\n",numberOfLogo);

	if (numberOfLogo ==0)
	{//��ʾ���
		fprintf(stderr,"û�ж�Ӧlogo ,�����logo\n");

		//���ٻ�����  
		
		//CloseHandle(SrcIndexmutex );  
		CloseHandle(mutex );  
		CloseHandle(FilePathmutex );  
		fclose(stdout);//�ر��ļ� 
		return 1;
	}

	//���� =============
	numberOfLogo=10;


	//getFatherPath(filePath,logo_srcIndex[0].srcIndex);
	//ClearFile( filePath);
	//saveTime(0);
	
	numberForMachedPicture=0;

		/**/

	printf("��ʼ  ��ѯpicture\n");
	//�ڶ����������ݿ��picture
	mySqlPictureSrc( idProgect,&numberOfPicture,&Picture_srcIndex);//��� picture����
	//��ѯ�ٶ�ԶԶ���ڲ����ٶ�

		printf("�õ�picture%d��\n",numberOfPicture);
	if (numberOfPicture ==0)
	{//��ʾ���
		fprintf(stderr,"û�ж�ӦͼƬ ,����� ��ƷͼƬ\n");
		//�ͷ�logo ·���ڴ�
		for (i=0; i< numberOfLogo; i++)
		{
			free(logo_srcIndex[i].srcIndex);
		}
		free(logo_srcIndex);
		//���ٻ�����  
	//	CloseHandle(SrcIndexmutex );  
		CloseHandle(mutex );  
		CloseHandle(FilePathmutex );  
		fclose(stdout);//�ر��ļ� 
		return 0;
	}

//	SrcIndexFormutex=(int *)malloc (sizeof(int )*numberOfPicture);//����ռ�

/*	for (i=0; i<numberOfPicture ;i++)
	{
		SrcIndexFormutex[i]=0;//��ʼΪδ����
	}
	*/
	//�Ȳ����� ������÷��أ��ٴ���

	//������뻻�ɾ��� ����
		IplImage*** allImgOfLogo;
		
		////cheshi ֻ��һ��logo
		//numberOfLogo=1;

	CaculateLogoFeatFirst(  logo_srcIndex, &logofeat, numberOfLogo, NUMBEROF_CENG,&LogoMatrix ,&allImgOfLogo);

	//�����������ھ��� +lsh����

	PRNearNeighborStructT nnStruct[GRAY_INDEX_NUMBER];

	struct collectionPointsOfIndex collectionOfIndex[GRAY_INDEX_NUMBER];

	myCaculateIndex(numberOfLogo,NUMBEROF_CENG,&logofeat,nnStruct,collectionOfIndex);


	printIndex( nnStruct[0]);
	myGetMaxLogoSize(logo_srcIndex, numberOfLogo);//����logo���ֵ  �޸�ȫ�ֱ���


	//���ԣ�  ���̼߳���
	chipNumber=1;

	//��� ��picture������ƥ��
	realchipNumber =chipNumber< numberOfPicture? chipNumber: numberOfPicture;

	//���̲߳�����
	handle=(HANDLE *) malloc(sizeof(HANDLE)*realchipNumber);
	tempForThread_EachPicturemen=(struct tempStructForThread_EachPicture *)
		malloc(sizeof(struct tempStructForThread_EachPicture)*realchipNumber);


	for(j=0;j<realchipNumber;j++)
	{	//ʧ�ܣ���Ϊ���߳�ʱ�� kdtreee���޸�feat��data�����Զ���߳�ͬʱ�õ�ʱ�򣬾����ˡ�
		init_tempStructForThread_EachPicture(
			&tempForThread_EachPicturemen[j],
			Picture_srcIndex,
			 logo_srcIndex,
			numberOfPicture,
			 numberOfLogo,
			 logofeat,
			 LogoMatrix,
			 j,
			 nnStruct,
			 collectionOfIndex,
			 allImgOfLogo);
		handle[j] = (HANDLE)_beginthreadex(NULL, 0, MyThreadFun_EachPicture,
			& tempForThread_EachPicturemen[j], 0, NULL);  
	}

	WaitForMultipleObjects(realchipNumber, handle, TRUE, INFINITE);  

	//���ٻ����� 

	CloseHandle(mutex );  
	CloseHandle(FilePathmutex );  
	
	

	for(j=0;j<realchipNumber;j++)
	{
		CloseHandle(handle[j]);
	}

	free(handle);
	free(tempForThread_EachPicturemen);

	//�ͷ� ·���ڴ�

	for (i=0; i< numberOfLogo; i++)
	{
		free(logo_srcIndex[i].srcIndex);
	}
		free(logo_srcIndex);

		//ceshiyong numberOfPicture
		for (i=0; i<numberOfPicture ; i++)
		{
			free(Picture_srcIndex[i].srcIndex);
		}
		free(Picture_srcIndex);


	//�ͷ�mat
	myreleaseLogoMatrix(LogoMatrix,numberOfLogo,NUMBEROF_CENG);

	myreleaseFeatStruct(&logofeat, numberOfLogo, NUMBEROF_CENG );
	//saveResult_Matched(numberForMachedPicture);
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC/60;


	//saveTime(-1);
	//saveConsumeTime(duration);
	printf( "\n �ܺ�ʱ ��%lf ���� \n", duration );
	printf( "һ����⵽��%d �� ͼƬ\n",numberForMachedPicture);
	printf("������\n");
   		//    _CrtDumpMemoryLeaks();
		 //  system("pause");	
	fclose(stdout);//�ر��ļ� 
		  return 0;
}




/************************** Function Definitions *****************************/

// print usage for this program
static void usage( char* name )
{
 
  fprintf(stderr, "�����������£�\n");
  fprintf(stderr, "Options:\n");
//"l:L:u:U:w:W:d:D:p:P:i:I:Y:y:c:C:N:01
  fprintf(stderr, " -h			����\n");
  fprintf(stderr, " -L -l		�������ݿ����ӵ�ַ��Ĭ��Ϊlocalhost\n");
  fprintf(stderr, " -U -u		���ݿ��û���\n");
  fprintf(stderr, " -W -w		���ݿ��passsword \n");
  fprintf(stderr, " -D -d		db������\n");
  fprintf(stderr, " -p -P		���ݿ�Ķ˿�port��Ĭ��Ϊ3306\n");
  fprintf(stderr, " -I -i		����idprojectId�����������ݿ���ڵ� \n");
  fprintf(stderr, " -Y -y		�Ż��ȼ�������1��2��3������1��죬��ȷ��50%��2�ٶ��еȣ�3�ٶ����������׼\n");
  fprintf(stderr, " -C -c		�̵߳ĸ�����������ڵ���1\n");
  fprintf(stderr, " -N			һ�δ���urlbase�ĸ���\n");
  fprintf(stderr, " -0			ʹ��cmd��¼��� \n");
  fprintf(stderr, " -1			ʹ��log�ļ���¼���\n");
}



/*
  arg_parse() parses the command line arguments, setting appropriate globals.
  
  argc and argv should be passed directly from the command line
*/
static void arg_parse( int argc, char** argv ,	
	int *YOUHUA_TYPE, char *host,
	char *user,char * passwd,char *db,unsigned int *port,
	long long *idProgect,int *chipNumber,
	int * NUMBER_URLBASEINDEX,int *CMDORLOG
	)
{
	/*���룺-c1 -Y2 -l localhost -u root  -w xiawei123 -d taobao -p 3306  ddd
	�������ddddd��ʱ�� break�����������޷��ֱ������һ�������� ���������һ��������
	int arg = getopt( argc, argv, OPTIONS );
	if( arg == -1 )//���Ĳ�������Ҳ�����
	break;
	*/
		char *pname =  argv[0];
		int tagformust=0;//��Ǳ���Ĳ���
	//Ĭ�ϲ�����ֵ��

	//��������	 2 localhost root  xiawei123 taobao 3306  202
	
	strcpy(host,"localhost");
	strcpy(user,"root");
	strcpy(passwd,"123456");
	strcpy(db,"taobao");
	*port=3306;
	*YOUHUA_TYPE=3;//atoi(argv[1]);//�ò����Ż�
	*chipNumber=4;
	*NUMBER_URLBASEINDEX=5;//ÿ�δ�����ٸ�urlbase
	*CMDORLOG=1;//Ĭ��Ϊlog�ļ���¼���
//	*idProgect=CharToLongLong(argv[7]);//�����β�����a to int  Ҫ�� a to long?????????)

	if (argc < 2)
	{// exe ��1���� ���������и�projectid
		printf("��������\n" );
		fclose(stdout);//�ر��ļ� 
		exit(1);
	}

  //extract program name from command line (remove path, if present)
//  pname = basename( argv[0] );

  //parse commandline options
  while( 1 )
    {
      char* arg_check;
      int arg = getopt( argc, argv, OPTIONS );
      if( arg == -1 )//���Ĳ�������Ҳ�����
	break;

      switch( arg )
		{
	 case '0'://cmd
		 *CMDORLOG=0;
		  break;
	 case '1'://log
		 *CMDORLOG=1;
		  break;
	  case 'N'://*NUMBER_URLBASEINDEX=10;//ÿ�δ�����ٸ�urlbase
		  *NUMBER_URLBASEINDEX= atoi(optarg);
		  if (*NUMBER_URLBASEINDEX <=0)
		  {
			  fprintf( stderr, "NUMBER_URLBASEINDEX �������� \n" );
			  exit(1);
		  }
		  break;
	  case 'n'://*NUMBER_URLBASEINDEX=10;//ÿ�δ�����ٸ�urlbase
		  *NUMBER_URLBASEINDEX= atoi(optarg);
		  if (*NUMBER_URLBASEINDEX <=0)
		  {
			  fprintf( stderr, "NUMBER_URLBASEINDEX �������� \n" );
			  exit(1);
		  }
		  break;
	  case 'c'://chip��Ŀ
		  *chipNumber= atoi(optarg);
		  if (*chipNumber <=0)
		  {
			  fprintf( stderr, "chipNumber �������� \n" );
			    exit(1);
		  }
		  break;
	  case 'C':// 
		  *chipNumber= atoi(optarg);
		  if (*chipNumber <=0)
		  {
			  fprintf( stderr, "chipNumber �������� \n" );
			    exit(1);
		  }
		  break;
		  // catch unsupplied required arguments and exit
		case 'Y'://�Ż��ȼ�
			 *YOUHUA_TYPE= atoi(optarg);
			 if (*YOUHUA_TYPE==0)
			 {
				 fprintf( stderr, "YOUHUA_TYPE �������� ֻ���� 1 ��2�� 3\n" );
				   exit(1);
			 }
			 break;
		case 'y'://�Ż��ȼ�
			*YOUHUA_TYPE= atoi(optarg);
			if (*YOUHUA_TYPE==0)
			{
				fprintf( stderr, "YOUHUA_TYPE �������� ֻ���� 1 ��2�� 3\n" );
				  exit(1);
			}
			break;

		case 'l'://localhost����
	 		strcpy(host,optarg);
		  break;
		case 'L':
			strcpy(host,optarg);
			break;

		case 'u'://�������ݿ�user
			strcpy(user,optarg);
			break;
		case 'U':
			strcpy(user,optarg);
			break;

		case 'w'://�������ݿ�password
			strcpy(passwd,optarg);
			break;
		case 'W':
			strcpy(passwd,optarg);
			break;

		case 'd'://�������ݿ�db
			strcpy(db,optarg);
			break;
		case 'D':
			strcpy(db,optarg);
			break;
 
		case 'p'://�������ݿ�db
			*port=atoi(optarg);
			if (*port==0)
			{
				fprintf( stderr, "port �������� \n" );
				  exit(1);
			}
			break;
		case 'P':
			*port=atoi(optarg);
			if (*port==0)
			{
				fprintf( stderr, "port �������� \n" );
				  exit(1);
			}
			break;

		case 'i'://�������ݿ�db
			*idProgect=CharToLongLong(optarg);
			tagformust=1;//���Ϊ�Ѿ���ֵ
			if (*idProgect==0)
			{
				fprintf( stderr, "idProgect �������� \n" );
				  exit(1);
			}
			break;
		case 'I':
			*idProgect=CharToLongLong(optarg);
			tagformust=1;
			if (*idProgect==0)
			{
				fprintf( stderr, "idProgect �������� \n" );
				  exit(1);
			}
			break;
		  // user asked for help
		case 'h':
		  usage( pname );
		  exit(1);
		  break;

		  // catch invalid arguments
		default:
		 {
			 fprintf( stderr, "����Ĳ���%c\n",arg );
			 exit(1);
		 }
		}
    }//�����̴������


	//�ж��Ƿ��м�����
	if( argc - optind != 0 )
	{
		fprintf( stderr, "��������\n" );
		exit(1);
	}

	if( tagformust!= 1 )
	{
		fprintf( stderr, "��������,��������projectid\n" );
		exit(1);
	}
	


}
