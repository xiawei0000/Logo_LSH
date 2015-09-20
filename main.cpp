//#define _WIN32_WINNT 0x0502  // Designed to run on Windows XP SP2 and up
//#define WINVER 0x0502
//#include <windows.h>
//#pragma comment(lib,"Kernel32.lib")
#include "main.h"
int CMDORLOG=0;//标记是cmd还是log版本
 int  NUMBER_URLBASEINDEX=5;

int main( int argc, char** argv )
{
	 //   _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	//多线程参数
	HANDLE * handle; 
	struct tempStructForThread_EachPicture *tempForThread_EachPicturemen;

	//数据库：参数
		//传入的
	long long idProgect;
	int numberOfLogo;//numberOfRows;
	srcIndexStruct *logo_srcIndex;//logo的地址+id 表

	int numberOfPicture;//numberOfRows;
	srcIndexStruct *Picture_srcIndex;// 的地址+id 表


	char filePath[300];// 第一个logo的文件夹 ="D:\\360\\Workspace\\C++\\LAB_work\\babaoli_pictureLab\\NEW_08\\";
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

	LogoFeatStruct ** logofeat;//用于保存一次计算的logofeat

	CvMat  ***LogoMatrix;
	int realchipNumber;

	/*MYSQL 参数：
	const char *host,
	const char *user,
	const char *passwd,
	const char *db,
	unsigned int port,
	*/
	//参数解析
	char host[_MAX_PATH];
	char user[_MAX_PATH];
	char passwd[_MAX_PATH];
	char db[_MAX_PATH];
	unsigned int port;

	srcIndexInterlocked=-1;
	

	/*
	if (argc != 8)
	{
		printf("参数有误\n" );
		fclose(stdout);//关闭文件 
		exit(1);
	}

	//参数处理	 
	YOUHUA_TYPE=atoi(argv[1]);//用不用优化
	strcpy(host,argv[2]);
	strcpy(user,argv[3]);
	strcpy(passwd,argv[4]);
	strcpy(db,argv[5]);

	port=atoi(argv[6]);
	idProgect=CharToLongLong(argv[7]);//长整形不能用a to int  要用 a to long?????????
	*/

	arg_parse(  argc,  argv ,	
		 &YOUHUA_TYPE, host,
		user, passwd,db, &port,
		 &idProgect,&chipNumber,
		 &NUMBER_URLBASEINDEX,&CMDORLOG);


	//测试：  多线程加速
	chipNumber=4;

	if (CMDORLOG == 1 )
	{//使用log
		myInitLogFile(dstTestPicturePath);//初始化 日志
	    freopen( dstTestPicturePath, "a", stdout );//修改为文件夹_log//time_.log
	}
	printf("开始检测 idProject=%I64d 的工程\n",idProgect);

	initMysql(	host,user,passwd,db,port);


	start = clock();//开始计时

	mutex  = CreateMutex(NULL, FALSE, NULL);  
	//SrcIndexmutex = CreateMutex(NULL, FALSE, NULL);  
	FilePathmutex= CreateMutex(NULL, FALSE, NULL);  

	printf("开始  查询logo\n");
	mySqlLogoSrc( idProgect,&numberOfLogo,&logo_srcIndex);//查好logo数据
	printf("得到logo%d个\n",numberOfLogo);

	if (numberOfLogo ==0)
	{//提示输出
		fprintf(stderr,"没有对应logo ,请添加logo\n");

		//销毁互斥量  
		
		//CloseHandle(SrcIndexmutex );  
		CloseHandle(mutex );  
		CloseHandle(FilePathmutex );  
		fclose(stdout);//关闭文件 
		return 1;
	}

	//测试 =============
	numberOfLogo=10;


	//getFatherPath(filePath,logo_srcIndex[0].srcIndex);
	//ClearFile( filePath);
	//saveTime(0);
	
	numberForMachedPicture=0;

		/**/

	printf("开始  查询picture\n");
	//第二部：查数据库的picture
	mySqlPictureSrc( idProgect,&numberOfPicture,&Picture_srcIndex);//查好 picture数据
	//查询速度远远快于插入速度

		printf("得到picture%d个\n",numberOfPicture);
	if (numberOfPicture ==0)
	{//提示输出
		fprintf(stderr,"没有对应图片 ,请添加 商品图片\n");
		//释放logo 路径内存
		for (i=0; i< numberOfLogo; i++)
		{
			free(logo_srcIndex[i].srcIndex);
		}
		free(logo_srcIndex);
		//销毁互斥量  
	//	CloseHandle(SrcIndexmutex );  
		CloseHandle(mutex );  
		CloseHandle(FilePathmutex );  
		fclose(stdout);//关闭文件 
		return 0;
	}

//	SrcIndexFormutex=(int *)malloc (sizeof(int )*numberOfPicture);//申请空间

/*	for (i=0; i<numberOfPicture ;i++)
	{
		SrcIndexFormutex[i]=0;//初始为未处理
	}
	*/
	//先查数据 ：出错好返回，再处理

	//这里必须换成矩阵， 返回
		IplImage*** allImgOfLogo;
		
		////cheshi 只用一个logo
		//numberOfLogo=1;

	CaculateLogoFeatFirst(  logo_srcIndex, &logofeat, numberOfLogo, NUMBEROF_CENG,&LogoMatrix ,&allImgOfLogo);

	//这里完成最近邻聚类 +lsh索引

	PRNearNeighborStructT nnStruct[GRAY_INDEX_NUMBER];

	struct collectionPointsOfIndex collectionOfIndex[GRAY_INDEX_NUMBER];

	myCaculateIndex(numberOfLogo,NUMBEROF_CENG,&logofeat,nnStruct,collectionOfIndex);


	printIndex( nnStruct[0]);
	myGetMaxLogoSize(logo_srcIndex, numberOfLogo);//计算logo最大值  修改全局变量


	//测试：  多线程加速
	chipNumber=1;

	//最后： 用picture数组来匹配
	realchipNumber =chipNumber< numberOfPicture? chipNumber: numberOfPicture;

	//多线程参数：
	handle=(HANDLE *) malloc(sizeof(HANDLE)*realchipNumber);
	tempForThread_EachPicturemen=(struct tempStructForThread_EachPicture *)
		malloc(sizeof(struct tempStructForThread_EachPicture)*realchipNumber);


	for(j=0;j<realchipNumber;j++)
	{	//失败：因为多线程时， kdtreee会修改feat的data，所以多个线程同时用的时候，就乱了。
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

	//销毁互斥量 

	CloseHandle(mutex );  
	CloseHandle(FilePathmutex );  
	
	

	for(j=0;j<realchipNumber;j++)
	{
		CloseHandle(handle[j]);
	}

	free(handle);
	free(tempForThread_EachPicturemen);

	//释放 路径内存

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


	//释放mat
	myreleaseLogoMatrix(LogoMatrix,numberOfLogo,NUMBEROF_CENG);

	myreleaseFeatStruct(&logofeat, numberOfLogo, NUMBEROF_CENG );
	//saveResult_Matched(numberForMachedPicture);
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC/60;


	//saveTime(-1);
	//saveConsumeTime(duration);
	printf( "\n 总耗时 ：%lf 分钟 \n", duration );
	printf( "一共检测到：%d 个 图片\n",numberForMachedPicture);
	printf("检测完毕\n");
   		//    _CrtDumpMemoryLeaks();
		 //  system("pause");	
	fclose(stdout);//关闭文件 
		  return 0;
}




/************************** Function Definitions *****************************/

// print usage for this program
static void usage( char* name )
{
 
  fprintf(stderr, "参数包括如下：\n");
  fprintf(stderr, "Options:\n");
//"l:L:u:U:w:W:d:D:p:P:i:I:Y:y:c:C:N:01
  fprintf(stderr, " -h			帮助\n");
  fprintf(stderr, " -L -l		配置数据库连接地址，默认为localhost\n");
  fprintf(stderr, " -U -u		数据库用户名\n");
  fprintf(stderr, " -W -w		数据库的passsword \n");
  fprintf(stderr, " -D -d		db的名字\n");
  fprintf(stderr, " -p -P		数据库的端口port，默认为3306\n");
  fprintf(stderr, " -I -i		工程idprojectId，必须是数据库存在的 \n");
  fprintf(stderr, " -Y -y		优化等级：包括1、2、3。其中1最快，正确率50%，2速度中等，3速度最慢，但最精准\n");
  fprintf(stderr, " -C -c		线程的个数，必须大于等于1\n");
  fprintf(stderr, " -N			一次处理urlbase的个数\n");
  fprintf(stderr, " -0			使用cmd记录输出 \n");
  fprintf(stderr, " -1			使用log文件记录输出\n");
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
	/*输入：-c1 -Y2 -l localhost -u root  -w xiawei123 -d taobao -p 3306  ddd
	读到最后ddddd的时候， break出来，所以无法分别是最后一个出错了 ，还是最后一个结束。
	int arg = getopt( argc, argv, OPTIONS );
	if( arg == -1 )//读的参数错误也会结束
	break;
	*/
		char *pname =  argv[0];
		int tagformust=0;//标记必须的参数
	//默认参数赋值：

	//参数处理	 2 localhost root  xiawei123 taobao 3306  202
	
	strcpy(host,"localhost");
	strcpy(user,"root");
	strcpy(passwd,"123456");
	strcpy(db,"taobao");
	*port=3306;
	*YOUHUA_TYPE=3;//atoi(argv[1]);//用不用优化
	*chipNumber=4;
	*NUMBER_URLBASEINDEX=5;//每次处理多少个urlbase
	*CMDORLOG=1;//默认为log文件记录结果
//	*idProgect=CharToLongLong(argv[7]);//长整形不能用a to int  要用 a to long?????????)

	if (argc < 2)
	{// exe 是1个， 两个必须有个projectid
		printf("参数有误\n" );
		fclose(stdout);//关闭文件 
		exit(1);
	}

  //extract program name from command line (remove path, if present)
//  pname = basename( argv[0] );

  //parse commandline options
  while( 1 )
    {
      char* arg_check;
      int arg = getopt( argc, argv, OPTIONS );
      if( arg == -1 )//读的参数错误也会结束
	break;

      switch( arg )
		{
	 case '0'://cmd
		 *CMDORLOG=0;
		  break;
	 case '1'://log
		 *CMDORLOG=1;
		  break;
	  case 'N'://*NUMBER_URLBASEINDEX=10;//每次处理多少个urlbase
		  *NUMBER_URLBASEINDEX= atoi(optarg);
		  if (*NUMBER_URLBASEINDEX <=0)
		  {
			  fprintf( stderr, "NUMBER_URLBASEINDEX 参数错误 \n" );
			  exit(1);
		  }
		  break;
	  case 'n'://*NUMBER_URLBASEINDEX=10;//每次处理多少个urlbase
		  *NUMBER_URLBASEINDEX= atoi(optarg);
		  if (*NUMBER_URLBASEINDEX <=0)
		  {
			  fprintf( stderr, "NUMBER_URLBASEINDEX 参数错误 \n" );
			  exit(1);
		  }
		  break;
	  case 'c'://chip数目
		  *chipNumber= atoi(optarg);
		  if (*chipNumber <=0)
		  {
			  fprintf( stderr, "chipNumber 参数错误 \n" );
			    exit(1);
		  }
		  break;
	  case 'C':// 
		  *chipNumber= atoi(optarg);
		  if (*chipNumber <=0)
		  {
			  fprintf( stderr, "chipNumber 参数错误 \n" );
			    exit(1);
		  }
		  break;
		  // catch unsupplied required arguments and exit
		case 'Y'://优化等级
			 *YOUHUA_TYPE= atoi(optarg);
			 if (*YOUHUA_TYPE==0)
			 {
				 fprintf( stderr, "YOUHUA_TYPE 参数错误 只能是 1 、2或 3\n" );
				   exit(1);
			 }
			 break;
		case 'y'://优化等级
			*YOUHUA_TYPE= atoi(optarg);
			if (*YOUHUA_TYPE==0)
			{
				fprintf( stderr, "YOUHUA_TYPE 参数错误 只能是 1 、2或 3\n" );
				  exit(1);
			}
			break;

		case 'l'://localhost配置
	 		strcpy(host,optarg);
		  break;
		case 'L':
			strcpy(host,optarg);
			break;

		case 'u'://配置数据库user
			strcpy(user,optarg);
			break;
		case 'U':
			strcpy(user,optarg);
			break;

		case 'w'://配置数据库password
			strcpy(passwd,optarg);
			break;
		case 'W':
			strcpy(passwd,optarg);
			break;

		case 'd'://配置数据库db
			strcpy(db,optarg);
			break;
		case 'D':
			strcpy(db,optarg);
			break;
 
		case 'p'://配置数据库db
			*port=atoi(optarg);
			if (*port==0)
			{
				fprintf( stderr, "port 参数错误 \n" );
				  exit(1);
			}
			break;
		case 'P':
			*port=atoi(optarg);
			if (*port==0)
			{
				fprintf( stderr, "port 参数错误 \n" );
				  exit(1);
			}
			break;

		case 'i'://配置数据库db
			*idProgect=CharToLongLong(optarg);
			tagformust=1;//标记为已经赋值
			if (*idProgect==0)
			{
				fprintf( stderr, "idProgect 参数错误 \n" );
				  exit(1);
			}
			break;
		case 'I':
			*idProgect=CharToLongLong(optarg);
			tagformust=1;
			if (*idProgect==0)
			{
				fprintf( stderr, "idProgect 参数错误 \n" );
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
			 fprintf( stderr, "错误的参数%c\n",arg );
			 exit(1);
		 }
		}
    }//按流程处理参数


	//判断是否中间跳出
	if( argc - optind != 0 )
	{
		fprintf( stderr, "参数错误\n" );
		exit(1);
	}

	if( tagformust!= 1 )
	{
		fprintf( stderr, "参数错误,必须输入projectid\n" );
		exit(1);
	}
	


}
