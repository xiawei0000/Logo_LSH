/*
原版： logo多放小， 计算每个的特征点
图片计算一个大小的特征点，
然后一一匹配
窗口由每次logo的大小决定
缩放取1.3效果远远好于2

添加时间的记录

调整sigma

同一个图，不同颜色也不行，（灰度值不同）

*/


#include "mach.h"





void myreleaseFeatStruct(LogoFeatStruct *** logofeat,int numberOfLogo,int NUMBEROF_CENG )
{//释放feat
	int i,j;
	for (j=0;j<numberOfLogo;j++)
	{

		for (i=0;i<NUMBEROF_CENG   ;i++)
		{
			if ((*logofeat)[j][i].feat1 != NULL )
			{			
				free( (*logofeat)[j][i].feat1) ;
			}
		}
		free((*logofeat)[j]);
	}
	free(*logofeat);
	logofeat=NULL;

}
void init_tempForThread_LogoCaculate(
struct tempStructForThread_LogoCaculate *tempstruct,
	srcIndexStruct *logo_srcIndex,LogoFeatStruct *** logofeat
	,int threadNumber,int NUMBEROF_CENG,int numberOfLogo,
	IplImage*  *** allImgOfLogo 
	)
{//复制线程参数
	tempstruct->logofeat=logofeat;
	tempstruct->threadNumber=threadNumber;
	tempstruct->NUMBEROF_CENG=NUMBEROF_CENG;
	tempstruct->logo_srcIndex=logo_srcIndex;
	tempstruct->numberOfLogo=numberOfLogo;
	tempstruct->allImgOfLogo=allImgOfLogo;
}

void  CaculateLogoFeatFirst(
	srcIndexStruct *logo_srcIndex,
	LogoFeatStruct *** logofeat,
	int numberOfLogo,int NUMBEROF_CENG,
	CvMat * ***LogoMatrix ,
	IplImage*  *** allImgOfLogo//保存所有logo图片的数组
	)
{//直接计算所有的logo 的feat

	/* 建立线程，申请feat的空间
	结构体传参数
	*/
	
	int i;
	int k,m;

	clock_t start, finish;
	double duration;

	int j,numberforsrc;
	HANDLE * handle;  
	struct tempStructForThread_LogoCaculate *tempForThread_men;

	struct feature *feat1;
	int realchipNumber;
		
	*LogoMatrix=(CvMat * **)malloc( sizeof(CvMat **) * numberOfLogo );

	start = clock();//开始计时
	
	realchipNumber =chipNumber< numberOfLogo? chipNumber: numberOfLogo;

	tempForThread_men=(	struct tempStructForThread_LogoCaculate *)malloc(
		sizeof(	struct tempStructForThread_LogoCaculate )*realchipNumber);
	handle=(HANDLE *) malloc(sizeof(HANDLE)*realchipNumber);

	*logofeat=(LogoFeatStruct **)malloc(sizeof(LogoFeatStruct *) *numberOfLogo );

	//4 16 修改，加入图片logo数组
	*allImgOfLogo =(IplImage ***)malloc(sizeof(IplImage **) *numberOfLogo );
	for (int i=0; i<numberOfLogo ;i++ )
	{
		(*allImgOfLogo)[i]=(IplImage **)malloc(sizeof(IplImage *) *NUMBEROF_CENG );
	}


	for(j=0;j<realchipNumber;j++)
	{

		init_tempForThread_LogoCaculate(&tempForThread_men[j],logo_srcIndex, logofeat, j, NUMBEROF_CENG,numberOfLogo,allImgOfLogo);
		handle[j] = (HANDLE)_beginthreadex(NULL, 0, MyThreadFun_LogoCaculate,& tempForThread_men[j], 0, NULL);  

	}
	WaitForMultipleObjects(realchipNumber, handle, TRUE, INFINITE);  


	for(j=0;j<realchipNumber;j++)
	{
		CloseHandle(handle[j]);
	}

	//开始会对分类’ 按颜色8个一个等级，32个等级；


	//将feat转为矩阵
	for (i=0; i<numberOfLogo ;i++ )
	{
		(*LogoMatrix)[i]=(CvMat **)malloc( sizeof(CvMat *) * NUMBEROF_CENG );
		for (j=0;j<NUMBEROF_CENG;j++)
		{
			if ((*logofeat)[i][j].numberOffeat !=0)
			{			
				(*LogoMatrix)[i][j]=cvCreateMat( (*logofeat)[i][j].numberOffeat ,128,CV_64FC1);//CV_32FC1);
				LogoFeatStructToMatrix(  (*LogoMatrix)[i][j],(*logofeat)[i][j]);
			}
			else{
				(*LogoMatrix) [i][j]=NULL;
			}
		}
		
	}


	free(handle);
	free(tempForThread_men);

	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("logo处理耗时：%.2lf 秒\n",duration);
//	system("pause");
	return;
}

unsigned int __stdcall MyThreadFun_LogoCaculate(PVOID pM)  
{  //多线程函数
	char srcLogoPicturePath[_MAX_PATH];

	IplImage* img1 ;
	IplImage** tempForLogoImg;

	struct feature* feat1 ;

	int n1, k, i, m = 0;
	int endCeng=0;

	int endi=0;
	int iForPicture;
	int logoNumber;//第几个logo
	int starti;
	int tagForHasFeats;
		



	//传参数
	int threadNumber;
	int numberOfLogo;
	srcIndexStruct *logo_srcIndex;
	LogoFeatStruct *** logofeat;
	LogoFeatStruct temptest;
	int NUMBEROF_CENG;
	IplImage*  *** allImgOfLogo;

	numberOfLogo=((struct tempStructForThread_LogoCaculate*)pM )->numberOfLogo;//logo总数
	threadNumber=((struct tempStructForThread_LogoCaculate*)pM )->threadNumber;//第几个线程
	logo_srcIndex=((struct tempStructForThread_LogoCaculate*)pM )->logo_srcIndex;//logo地址数组
	logofeat=((struct tempStructForThread_LogoCaculate*)pM )->logofeat;//最后的特征点 和个数的数组
	NUMBEROF_CENG=((struct tempStructForThread_LogoCaculate*)pM )->NUMBEROF_CENG;
	allImgOfLogo=((struct tempStructForThread_LogoCaculate*)pM )->allImgOfLogo;//存图片

	if (numberOfLogo /chipNumber==0)
	{//chip 》 logo数目
		starti=threadNumber;//直接以线程序号为图序号
		endi=threadNumber+1;
	}
	else
	{
		starti=numberOfLogo /chipNumber *threadNumber;
		//表示第几个线程
		if (threadNumber==chipNumber-1)//最后一个线程
		{
			endi=numberOfLogo;
		}
		else
		{
			endi=numberOfLogo /chipNumber *(threadNumber+1);
		}
	}


	for(iForPicture = starti; iForPicture< endi ;iForPicture++)
	{//线程的每一张图

	
		logoNumber=iForPicture;//logo编号 从0开始吧
		strcpy(srcLogoPicturePath,logo_srcIndex[logoNumber].srcIndex);	

		
		(*logofeat)[logoNumber]=(LogoFeatStruct *)malloc(sizeof(LogoFeatStruct ) *NUMBEROF_CENG );

		/*数据库读点的 ，不要了
		for (i=0;i<NUMBEROF_CENG   ;i++)
		{//对于已经存入数据库的操作，直接读取就好了
			tagForHasFeats= mySqlGetLogoFeats(logo_srcIndex[logoNumber].id  ,i, &(*logofeat)[logoNumber][i]) ;
 
			if (tagForHasFeats == 0)
			{//没有存好feats
				break;
			}

		}	
		
		if ( i== NUMBEROF_CENG)
			{//最后一层了 都满足
				//测试时间用

				continue; //继续下一层
			}
		*/
		img1 = myLoadImage( srcLogoPicturePath, 1 );	
		if (img1 ==NULL)
		{
			printf(" %s 的logo丢失 ，请补上\n",srcLogoPicturePath);
				fclose(stdout);//关闭文件 
			exit(1);
		//	continue ;//可能没有图片
		}

		//加入logo自动调节大小； 并保存
		img1= myReduceLogoAndSave(img1,srcLogoPicturePath);

		endCeng=0;
		//(*allImgOfLogo)[logoNumber][0] = img1;

		tempForLogoImg=(IplImage**)malloc(sizeof(IplImage*)*NUMBEROF_CENG);
		//对原图  先多次缩小，再算feat
		for (i=0;i<NUMBEROF_CENG   ;i++)
		{//缩放
			if (i ==0)
			{   
				tempForLogoImg[i]=cvCloneImage(img1);
				(*allImgOfLogo)[logoNumber][i]=cvCloneImage( tempForLogoImg[i] );//保存logo
			}  
			else
				{
					tempForLogoImg[i]= smallSizeImg( tempForLogoImg[i-1],NUMBERFOR_RESIZE);
					(*allImgOfLogo)[logoNumber][i]=cvCloneImage( tempForLogoImg[i] );//保存logo
			}
			endCeng=i;
			if (isTooSmall( tempForLogoImg[i]))
			{//泄露了

				for (;i< NUMBEROF_CENG ;i++)
				{
					(*logofeat)[logoNumber][i].feat1=NULL;
					(*logofeat)[logoNumber][i].numberOffeat=0;
					
					/*//插入数据库：
					mySqlInsertLogoFeats(logo_srcIndex[logoNumber].id  ,i, 
						(*logofeat)[logoNumber][i]);
					*/
					/*//测试
					mySqlGetLogoFeats(logo_srcIndex[logoNumber].id  ,i, 
						&temptest) ;
						*/

				}
				break;//logo 太小了
			}


			//特征点检测
		//	printf( "Finding features in %s ----%d ...\n", srcLogoPicturePath,i );
			//fprintf( stderr, "Finding features in %s ----%d ...\n", srcLogoPicturePath,i );
			n1 = sift_features(  tempForLogoImg[i], &feat1 );
		//	fprintf( stderr, " features %d\n", n1 );
		//	printf(  " features %d\n", n1 );
			//(*logofeat)[j][i]= (LogoFeatStruct *)malloc(sizeof(LogoFeatStruct));
			(*logofeat)[logoNumber][i].feat1=feat1;
			(*logofeat)[logoNumber][i].numberOffeat=n1;

			//测试插入数据库：
		//	 mySqlInsertLogoFeats(logo_srcIndex[logoNumber].id  ,i, (*logofeat)[logoNumber][i]);

			// mySqlGetLogoFeats(logo_srcIndex[logoNumber].id  ,i, &temptest);
		}//每一层

		for (i=0;i<=endCeng ;i++)
		{
			cvReleaseImage(&tempForLogoImg[i]);
		}


		free(tempForLogoImg);
		cvReleaseImage(&img1);
	}//每一个图
		
	return 0;
}

void  LogoFeatCaculate(	
int numberOfLogo,int NUMBEROF_CENG,
srcIndexStruct *logo_srcIndex,
struct logoFeatStruct*** logofeat
 )  //传参数
{ 

//直接计算logo特征，
	char srcLogoPicturePath[_MAX_PATH];

	IplImage* img1 ;
	IplImage** tempForLogoImg;

	struct feature* feat1 ;

	int n1, k, i, m = 0;
	int endCeng=0;

	int endi=0;
	int iForPicture;
	int logoNumber;//第几个logo
	int starti;
	int tagForHasFeats;
		
		
	starti=0;endi=numberOfLogo;
	for(iForPicture = starti; iForPicture< endi ;iForPicture++)
	{//线程的每一张图

	
		logoNumber=iForPicture;//logo编号 从0开始吧
		strcpy(srcLogoPicturePath,logo_srcIndex[logoNumber].srcIndex);	

		
		(*logofeat)[logoNumber] =(struct logoFeatStruct *)malloc(sizeof(struct logoFeatStruct) *NUMBEROF_CENG );

		img1 = myLoadImage( srcLogoPicturePath, 1 );	
		if (img1 ==NULL)
		{
			printf(" %s 的logo丢失 ，请补上\n",srcLogoPicturePath);
				fclose(stdout);//关闭文件 
			exit(1);
		//	continue ;//可能没有图片
		}

		//加入logo自动调节大小； 并保存
		img1= myReduceLogoAndSave(img1,srcLogoPicturePath);

		endCeng=0;

		tempForLogoImg=(IplImage**)malloc(sizeof(IplImage*)*NUMBEROF_CENG);
		//对原图  先多次缩小，再算feat
		for (i=0;i<NUMBEROF_CENG   ;i++)
		{//缩放
			if (i ==0)
			{   
				tempForLogoImg[i]=cvCloneImage(img1);
			}  
			if(i!=0)
				tempForLogoImg[i]= smallSizeImg( tempForLogoImg[i-1],NUMBERFOR_RESIZE);

			endCeng=i;
			if (isTooSmall( tempForLogoImg[i]))
			{//泄露了

				for (;i< NUMBEROF_CENG ;i++)
				{
					(*logofeat)[logoNumber][i].feat1=NULL;
					(*logofeat)[logoNumber][i].numberOffeat=0;
				}
				break;//logo 太小了
			}
		//特征点检测
		//	printf( "Finding features in %s ----%d ...\n", srcLogoPicturePath,i );
			//fprintf( stderr, "Finding features in %s ----%d ...\n", srcLogoPicturePath,i );
			n1 = sift_features(  tempForLogoImg[i], &feat1 );
		//	fprintf( stderr, " features %d\n", n1 );
		//	printf(  " features %d\n", n1 );
			//(*logofeat)[j][i]= (LogoFeatStruct *)malloc(sizeof(LogoFeatStruct));
			(*logofeat)[logoNumber][i].feat1=feat1;
			(*logofeat)[logoNumber][i].numberOffeat=n1;

		}//每一层

		for (i=0;i<=endCeng ;i++)
		{
			cvReleaseImage(&tempForLogoImg[i]);
		}


		free(tempForLogoImg);
		cvReleaseImage(&img1);
	}//每一个图
		
	return ;
}

void init_tempStructForThread_EachPicture(
struct tempStructForThread_EachPicture *pM,
	srcIndexStruct *Picture_srcIndex,
	srcIndexStruct *logo_srcIndex,
	int numberOfPicture,
	int numberOfLogo,
	LogoFeatStruct ** logofeat,
	CvMat  ***LogoMatrix,
	int threadNumber,
PRNearNeighborStructT *nnStruct,
struct collectionPointsOfIndex *collectionOfIndex,
	IplImage*** allImgOfLogo)
{//线程参数传递结构体
	( ( struct tempStructForThread_EachPicture*)pM )-> 	Picture_srcIndex=Picture_srcIndex;
	( ( struct tempStructForThread_EachPicture*)pM )->  logo_srcIndex=logo_srcIndex;
	( ( struct tempStructForThread_EachPicture*)pM )->  numberOfPicture=numberOfPicture;
	( ( struct tempStructForThread_EachPicture*)pM )->   numberOfLogo=numberOfLogo;
	( ( struct tempStructForThread_EachPicture*)pM )->  logofeat=logofeat;
	( ( struct tempStructForThread_EachPicture*)pM )->  LogoMatrix=LogoMatrix;
	( ( struct tempStructForThread_EachPicture*)pM )-> threadNumber=threadNumber;
	( ( struct tempStructForThread_EachPicture*)pM )-> nnStruct=nnStruct;
	( ( struct tempStructForThread_EachPicture*)pM )-> collectionOfIndex=collectionOfIndex;
	( ( struct tempStructForThread_EachPicture*)pM )-> allImgOfLogo=allImgOfLogo;

}

 

unsigned int __stdcall MyThreadFun_EachPicture(PVOID pM)  
{	
	//线程参数
	srcIndexStruct *Picture_srcIndex;
	srcIndexStruct *logo_srcIndex;
	int numberOfPicture;
	int numberOfLogo;
	LogoFeatStruct ** logofeat;
	CvMat  ***LogoMatrix;
	int threadNumber;
	PRNearNeighborStructT *nnStruct;
	struct collectionPointsOfIndex *collectionOfIndex;
	IplImage*** allImgOfLogo;


		CvMat *PictureMatrix;
	CvMat **tempMatrix;


	char srcTestPicturePath[_MAX_PATH];//最后读取的地址
	char srcLogoPicturePath[_MAX_PATH];
	IplImage* img2;
	struct feature  * feat2;
	int n2;

	char  tempNameForPictureChange[_MAX_PATH];
	char *nameForPictureChange="_Mach_";
	char *nameForPictureSub="子图_";
	char tempnumber[10];
	char dstTestPicturePath[_MAX_PATH];

	double maxEachPicturesim;//最大相似度
	double tempLogosim;//每个图的临时值

	int tagForMakeIt=0;	
	int tagForMatched=0;
	int tagForMatchedLogo;
	IplImage **imgForPicture;
	int numberOfpices;
	int k;
	int i;

	int tempPictureNumber;
	int endi=0;
	int iForPicture;
	int stari;
	//线程参数传递
	Picture_srcIndex=( ( struct tempStructForThread_EachPicture*)pM )-> 	Picture_srcIndex;
	logo_srcIndex=( ( struct tempStructForThread_EachPicture*)pM )->  logo_srcIndex;
	numberOfPicture=( ( struct tempStructForThread_EachPicture*)pM )->  numberOfPicture;
	numberOfLogo=( ( struct tempStructForThread_EachPicture*)pM )->   numberOfLogo;
	logofeat=( ( struct tempStructForThread_EachPicture*)pM )->  logofeat;
	LogoMatrix=( ( struct tempStructForThread_EachPicture*)pM )->  LogoMatrix;
	threadNumber= ( ( struct tempStructForThread_EachPicture*)pM )->threadNumber;
	nnStruct=   ( ( struct tempStructForThread_EachPicture*)pM )-> nnStruct;
	collectionOfIndex=   ( ( struct tempStructForThread_EachPicture*)pM )-> collectionOfIndex;
	allImgOfLogo=   ( ( struct tempStructForThread_EachPicture*)pM )-> allImgOfLogo;
/*
	//如果图片数小于线程数 那么线程开始就min（图数目，chipernumber）
	if (numberOfPicture /chipNumber==0)
	{
		stari=threadNumber;//直接以线程序号为图序号
		endi=threadNumber+1;
	}
	else
	{
		stari=numberOfPicture /chipNumber *threadNumber;

		 //表示第几个线程
		if (threadNumber==chipNumber-1)//最后一个线程
		{
			endi=numberOfPicture;
		}
		else
		{
			endi=numberOfPicture /chipNumber *(threadNumber+1);
		}
	}
*/
	
	while(1)
	{//循环处理，直到找不到未处理图片
		
		//互斥处理：查找未处理图片序号
		if (srcIndexInterlocked >= numberOfPicture-1)
		{//从-1开始，先加1，然后取地址。
			break;//最后一张都处理完毕了
		}
		 InterlockedIncrement((LPLONG)&srcIndexInterlocked); //原子操作，递增
		 
			iForPicture=srcIndexInterlocked;

			//printf("\n\n====当前处理 %d   \n\n",iForPicture);

	//第一步：得到测试图，分块并计算特征点
		//	logoNumber+=i;
	
		tempPictureNumber=iForPicture;
		tagForMatched=0;//标记有logo


		//保存时间
		//	saveTime(j);

		strcpy(srcTestPicturePath,Picture_srcIndex[tempPictureNumber].srcIndex);

		/* 图片载入 */
		img2 = myLoadImage( srcTestPicturePath, 1 );
		//必须判断读入的东西
		if (img2 ==NULL)
		{
			printf("没有对应图片 %s",srcTestPicturePath);
			continue ;//可能没有图片
		}


		img2=myReduceImage(img2);//对高分辨的下采样


		//从全局变量 ，判断优化层次： 是1 取一个角 2取3个角 3 不变
		//小于 maxlogosize的 局不要分了。
		if (YOUHUA_TYPE ==1)
		{
			numberOfpices=myGetImgQuarter(&imgForPicture, img2);
		}
		else if (YOUHUA_TYPE ==2)
		{
			numberOfpices=myGetImgFourQarters(&imgForPicture, img2);
		}
		else
		{//不优化 
			numberOfpices= myDivedeImg(&imgForPicture, img2);
		}



		//numberOfpices= myDivedeImg(&imgForPicture, img2);

		cvReleaseImage( &img2 );
			
		tagForMakeIt=0;

		maxEachPicturesim=0.0;//每个测试图像的相似度都为哦，o
		tempLogosim=0.0;//初始为0.0；

		for( k=0;k< numberOfpices   && tagForMakeIt==0 ; k++)
		{	
			if (isTooSmall(imgForPicture[k]) )
			{
				continue;//分图后可能很小
			}

			printf(  "Finding features in %s...\n", srcTestPicturePath );
		//	fprintf( stderr, "Finding features in %s...\n", srcTestPicturePath );
			n2 = sift_features( imgForPicture[k], &feat2 );//取得特征点
			//	fprintf( stderr, " features %d\n", n2 );
			printf(  " features %d\n", n2 );

			//测试
		//	PictureMatrix=cvCreateMat(NUMBERFORDIVISIOM,n2,CV_64FC1);//CV_32FC1);
			//featToMatrix(PictureMatrix,feat2,n2);
		//	printMyMatIndouble( PictureMatrix);
			ouputPictureData(feat2,n2);
			if ( n2 >MINNUMBER_FORFEAT)
			{//特征点太少就不要匹配了

//draw_features( imgForPicture[k],feat2,n2);
				//collectionOfIndex 还没加入
				 tagForMakeIt=myJudgeIsMatch(	 feat2, n2	,
					nnStruct,	  collectionOfIndex, numberOfLogo, NUMBEROF_CENG,
				 logofeat,allImgOfLogo,imgForPicture[k]);


				

				/*
			//这里使用矩阵转换
				PictureMatrix=cvCreateMat(NUMBERFORDIVISIOM,n2,CV_64FC1);//CV_32FC1);
				featToMatrix(PictureMatrix,feat2,n2);

//第二大步：图片和每个logo匹配

			//这里修改为新的匹配方法：下面全不要

				for (i=0;i<numberOfLogo  && tagForMakeIt==0;i++)
				{
					strcpy(srcLogoPicturePath,logo_srcIndex[i].srcIndex);//得到logo地址	
					//修改保存路径void chageSavePath()


					//精简版本：
					/*
					//检测结果图片
				//子图_K 
					strcpy(tempNameForPictureChange,nameForPictureSub);//加入分图像的处理
					intToChar(k,tempnumber);
					strcat(tempNameForPictureChange,tempnumber);
			
					strcat(tempNameForPictureChange,nameForPictureChange);
				//_macth i
					intToChar(i,tempnumber);
					strcat(tempNameForPictureChange,tempnumber);

					//拼接保存路径 //默认 格式是4个char   。jpg
					changepath( srcTestPicturePath,tempNameForPictureChange,dstTestPicturePath);
				//dstTestPicturePath=NULL;
*//*
					tempMatrix=LogoMatrix[i];//logo当前类的matrix  从0开始计算
		
			

					
					tagForMakeIt=machEachPicture(i,logofeat,tempMatrix,
						srcLogoPicturePath, imgForPicture[k],
						feat2,PictureMatrix,
						n2,dstTestPicturePath,
						&tempLogosim);//logo和test比较存入dst路径中
/**/



/*
					if ( tempLogosim>maxEachPicturesim)
					{//更新每个图 匹配各种logo 对应相似度
						maxEachPicturesim=tempLogosim;
					}

					if (tagForMakeIt)
					{		//每次匹配后 tagForMakeIt都会被更新，所以用tagForMatched 来记忆是否匹配
						tagForMatched=1;//标记匹配到了，
						tagForMatchedLogo=logo_srcIndex[i].id;
						
						/*
						WaitForSingleObject(mutex, INFINITE);  
					
						 SaveResult(tempPictureNumber ,k ,tagForMatchedLogo);
							//需要同步处理
							numberForMachedPicture++;
						  ReleaseMutex(mutex);  
						  */
	/*				}
				}
			cvReleaseMat(&PictureMatrix);
	*/			
			}
			//tongbu
			/*
			 WaitForSingleObject(mutex, INFINITE);  
			 SaveAReturn();
			  ReleaseMutex(mutex);  
			*/
			
			free( feat2 );feat2=NULL;
		}	//每一份图结束

		for( k=0;k< numberOfpices; k++)
		{	
			cvReleaseImage(  &imgForPicture[k]);
		}
		free(imgForPicture); imgForPicture=NULL;

	//这里改为 maxEachPicturesim 》0.1  10% 就写入数据库
		//if (maxEachPicturesim >0.1)
		/*	{//修改为插入sim值
			WaitForSingleObject(mutex, INFINITE);  
			mySqlInsertResult(Picture_srcIndex[tempPictureNumber].id,maxEachPicturesim);//logoid无用了
			ReleaseMutex(mutex);  					
		}*/


		if(tagForMatched)//改回来
		{// 
			WaitForSingleObject(mutex, INFINITE);  
			//需要同步处理
			numberForMachedPicture++;
			mySqlInsertResultWithLogoId(Picture_srcIndex[tempPictureNumber].id,tagForMatchedLogo);//logoid无用了
			ReleaseMutex(mutex);  					
		}
			/**/
	}

	return 0;
}

int myJudgeIsMatch(	struct feature  * feat2,int n2	,
	PRNearNeighborStructT *nnStruct,
struct collectionPointsOfIndex collectionOfIndex[GRAY_INDEX_NUMBER],
	int numberOfLogo,int NUMBEROF_CENG,
struct logoFeatStruct** logofeat,
IplImage*** allImgOfLogo,
IplImage* img2
)
{//传入 feat2数组，判断是否是有logo的，
	//返回 0，不匹配， 1 匹配
	//struct logoFeatStruct*** logofeat=&logofeatNP;


	//建立 logo的单词的结构体
	CvSeq ***featseq;
	CvMemStorage*** storage ;
	int **StatisticsOfPoins;
	//构建CvSeq *featseq
	buidLogoSeq( numberOfLogo, NUMBEROF_CENG,	featseq,storage,StatisticsOfPoins);
	int tempnumbermach=0;
	//1每个点查询灰度+lsh=》返回一组//2计算最近邻
	for (int i=0;i< n2; i++ )
	{
		struct arrayOfGrayIndex*  arrryIndex= mySearchInIndex(nnStruct,feat2[i],collectionOfIndex);
		if (arrryIndex ==NULL )
		{
			continue;
		}
		//3提取序号组， 逆向到 每个logo的层里

		struct seqIndexAndClassOfGray *listIndex;
		listIndex=arrryIndex->index;
		struct indexAndClassOfGray *tempindxclass;

		do 
		{//接着讲 匹配对 压入，		统计

			tempnumbermach++;

			tempindxclass=&listIndex->grayindex;
			MachedFeatPoint tempFeatPoint;//临时保存匹配对

			struct feature  * feat1=//提取feat1点
				&(  (logofeat[tempindxclass->logoid][tempindxclass->ceng	]).feat1[tempindxclass->featorder]);


			tempFeatPoint.machedPointFeat1=cvPoint( cvRound( feat1->x ), cvRound(feat1->y ) );
			tempFeatPoint.machedPointFeat2=cvPoint( cvRound( feat2->x ), cvRound(feat2->y ) );
			cvSeqPush(featseq[tempindxclass->logoid][tempindxclass->ceng],&tempFeatPoint);//压入 匹配到的 图片上的点
			StatisticsOfPoins[tempindxclass->logoid][tempindxclass->ceng] +=1;//当前层+1

			listIndex=listIndex->next;//下一个

		} while (listIndex != NULL);
	}
	
	if (tempnumbermach==0 )
	{	//一个点也没有
		releaseLogoSeq( numberOfLogo, NUMBEROF_CENG,featseq,storage,StatisticsOfPoins);
		return 0;
	}
	//用每层的 匹配点数 除以总点数 ，取最大的前三个
	int *maxarry;
	int maxSize=5;

	maxarry =(int *) malloc(sizeof(int )* maxSize);
	// featseq  StatisticsOfPoins   logofeat//4取N/总数最大 前三个判断。
	getMaxRatioForMach(	numberOfLogo,NUMBEROF_CENG,logofeat,StatisticsOfPoins,featseq,
		maxarry,maxSize 	);

	//5调用sliperwindows（）判断
	int tagForMakeIt=0;
	for (int i=0 ;i< maxSize ; i++)
	{
		if ( tagForMakeIt !=0)
		{
			break;
		}
		if (maxarry[i] <0  )
		{//最后了，或者没有合适的点
			tagForMakeIt = 0;
			break;
		}
		int logoid= maxarry[i]/NUMBEROF_CENG;
		int ceng= maxarry[i] %NUMBEROF_CENG;

		IplImage*img1 =allImgOfLogo[logoid][ceng];

		IplImage*stacked;
 
		int 	n1 = logofeat[logoid][ceng].numberOffeat;
		struct feature  *feat1=logofeat[logoid][ceng].feat1;

			 stacked = stack_imgs( img1, img2 );
			//	stacked=NULL;//修改为精简版
			//一起显示的面板（两个的高度和）将特征点也画上

		double tempCengsim=0.0;//记录匹配值
		tagForMakeIt= compareInTwoImgTwoFeats( img1, img2, stacked,
			feat1, feat2, featseq[logoid][ceng],  n1,n2, 
			featseq[logoid][ceng]->total,&tempCengsim);
	}

	releaseLogoSeq( numberOfLogo, NUMBEROF_CENG,featseq,storage,StatisticsOfPoins);
	return tagForMakeIt;//完了也没找到

}


int compareInTwoImgTwoFeats(IplImage*img1,IplImage*img2,IplImage*stacked,
struct feature* feat1,struct feature * feat2,CvSeq *featseq,
	int n1,int n2,int n3,//匹配的对个数
	double * maxFeatsim)
{//通过两个图片 和各自的feat， 已经匹配好的 seq ，计算匹配率，返回是否匹配
	//2设置窗口大小，开始找出窗口中的点
	int i,j,k;

	CvPoint pointWindow1,pointWindow2;

	int ImgWidth=img2->width;
	int ImgHight=img2->height;
	CvSize sizeImg1=cvSize(img1->width,img1->height);
	CvSize sizeImg2=cvSize(img2->width,img2->height);
 
	MachedFeatPoint* featTemp;

	double liangduImg1=0.0;

	double  tempFeatsim;
	int tagForCompare=0;
	//先画一次
	myDrawPoint(featseq,n3,stacked,ImgHight);

	cvWaitKey(0);
 
	
	if (Img1BigImg2(img1,img2) )
	{
		return  0;//图片小于logo
	}
	/*可以优化的地方：
	 得到匹配点的最值，然后得到maxmin  的窗口点*/
	//用logo窗口 滑动来匹配点
	liangduImg1=ComputeLiangdu(img1,cvPoint(0,0),sizeImg1);
		
	tagForCompare=0;

	/*用每一个匹配到的点  ，确定 图片logo的区域， 再比较 sim*/
	tempFeatsim=0.0;//点的相似度

	for(i=0;i<n3  && tagForCompare ==0 ;i++)
	{
		featTemp =(MachedFeatPoint*) cvGetSeqElem(featseq,i);
		
		GetMachedWindowPoint( featTemp, &pointWindow1, &pointWindow2, sizeImg1 ,sizeImg2);
	
		tagForCompare=judgeFormPiont( feat1,  feat2, featseq,
			n1,  n2,n3,
			img1,liangduImg1,img2, stacked , 
			pointWindow1,  pointWindow2 ,
			&tempFeatsim) ;

		if (*maxFeatsim < tempFeatsim)
		{	//点的相似度更新
			*maxFeatsim=tempFeatsim;
		}

		if(tagForCompare )
		{//修改为精简版
			;//drawTagForWindow(tagMat,pointWindow1.x ,pointWindow1.y,sizeImg1);//wondow内多有点化为1

		}
	}
 

	return  tagForCompare ;
}



int machEachPicture(int numberForLogo,LogoFeatStruct ** logofeat,CvMat** tempMatrix,
	char* srcLogoPicturePath,IplImage * img2_orig, 
	struct feature  * feat2,CvMat *PictureMatrix,
	int n2,char *dstTestPicturePath,
	double * maxCengsim)
{//一个logo的不同层 对测试图的一个pice  匹配

	double tempCengsim;//每个层的最值

  IplImage* img1 , * stacked;
  IplImage** tempForLogoImg;
  CvMat multiMat;
  struct feature* feat1 ;
  struct feature** nbrs;
 
  CvPoint pt1, pt2;
  double d0, d1;
  int n1, k, i,j, m = 0;
  CvMat *tagMat ;
  IplImage* tagImag,* tagImag2;
 // char * nameofWindow[20]={"0","1","2","3","4","5","6","7","8","9","10"};

  int startCeng=0,endCeng=0;
  int  preRetuurn=0;
  int tagForMakeIt=0;
  
  IplImage *img2=cvCloneImage(img2_orig);

 // struct feature  *feat2 = (struct feature  *)calloc( n2, sizeof(struct feature) );//将feat3 拷贝一份，然后可以修改
  struct feature  * feat3;
  int  machedPointsnumber;

  int numberForFeat;
  //用于匹配到的点的储存
  CvSeq *featseq;
  CvMemStorage* storage ;

  CvMat* tempMatrixForLogo;

  storage = cvCreateMemStorage( 0 );
  featseq=cvCreateSeq(0,sizeof(CvSeq),sizeof(MachedFeatPoint),storage);


  //可以很好地优化 内存空间  只用一个来做
  img1 = myLoadImage( srcLogoPicturePath, 1 );

 /*/ 精简版本tagMat=cvCreateMat(img2->height,img2->width,CV_8SC1);//标记矩阵

  //cvZero(tagMat);
  */
  tagMat=NULL;

  tempForLogoImg=(IplImage**)malloc(sizeof(IplImage*)*NUMBEROF_CENG);


   tempCengsim=0.0;////每个层的最值 初始sim值为o

  //对原图  先多次缩小，再算feat
  for (i=0;i<NUMBEROF_CENG   && tagForMakeIt ==0 ;i++)
  {//缩放
	  if (i ==0)
	  {   
		  tempForLogoImg[i]=cvCloneImage(img1);
	  }  
	  if(i!=0)
	  tempForLogoImg[i]= smallSizeImg( tempForLogoImg[i-1],NUMBERFOR_RESIZE);
 
	  if (Img1BigImg2( tempForLogoImg[i],img2))
	  {
		  startCeng=i+1;//可能泄露？？？？？
		   endCeng=i;
		  continue;//logo>图
	  }
	  endCeng=i;
	  if (isTooSmall( tempForLogoImg[i]))
	  {
		  endCeng=i;//就是这里内存泄露了啊
		  break;//logo 太小了
	  }


		  {//特征点检测
			 numberForFeat=numberForLogo ;
			 n1 = logofeat[numberForFeat][i].numberOffeat;
			 feat1=logofeat[numberForFeat][i].feat1;
			 tempMatrixForLogo=tempMatrix[i];//得到单个层的矩阵

		//	 stacked = stack_imgs(  tempForLogoImg[i], img2 );
			 stacked=NULL;//修改为精简版

			 //一起显示的面板（两个的高度和）将特征点也画上

			  if ( n1 > MINNUMBER_FORFEAT)
			  {			  				  
				  cvClearSeq(featseq);
				  //预先匹配
				  machedPointsnumber=compareFeartureFirstTime(feat1,tempMatrixForLogo,
					  feat2,PictureMatrix,n1,n2, 
					  tempForLogoImg[i],stacked,featseq);

				  //提前判断 feat 》 阀值
				  if (machedPointsnumber > ( n1 *SIGOLIKELY )/3 )
				  {
					//滑动窗口比较
					  tagForMakeIt= sliperWindows( tempForLogoImg[i], img2, stacked,
						  feat1, feat2, featseq,  n1,n2, 
						  machedPointsnumber,tagMat,
						  &tempCengsim);
					 
					  if (tempCengsim > *maxCengsim)
					  {//更新层的最大相似度
						  *maxCengsim =tempCengsim;
					  }
							//  cvWaitKey(0);
				  }

			   }
			  
		/*//修改为精简版
			  if (stacked  !=NULL )
			  {
				  cvReleaseImage( &stacked );
				  stacked=NULL;
			  }
		 */

		  }
  }

 cvReleaseMemStorage(&storage);
  printf("-%d", tagForMakeIt);
 //修改为精简版
 /*
  //显示tagmat  CvMat 转img
  tagImag=cvCreateImageHeader(cvGetSize(tagMat),IPL_DEPTH_8U ,1);
  cvGetImage(tagMat,tagImag);
  tagImag->depth=8;
//  fprintf(stderr," %d \n", tagImag->depth);

  cvThreshold(tagImag,tagImag,0.5,254,CV_THRESH_BINARY); 
  find_Drawcontours(tagImag,img2);

  */
  /*
  if (tagForMakeIt)
  {//直接分析 dstTestPicturePath的 上层目录

	  myCreateFatherFile(dstTestPicturePath);
	//要新建文件夹： 保存
	cvSaveImage(dstTestPicturePath,img2,0);//保存图片
  }
  */


 /*
  cvReleaseImageHeader(&tagImag);//cvGetImage() 中 只是将mat 的数据地址给了 img，img只要一个头局够了，所以不能释放img，只能释放头
  tagImag=NULL;
  cvReleaseMat(&tagMat); 
  tagMat=NULL;

  */
  cvReleaseImage( &img1 );
  cvReleaseImage( &img2 );
  img1=NULL;
   
  for (i=0;i<=endCeng;i++){	 
	  cvReleaseImage(&tempForLogoImg[i]);
	  tempForLogoImg[i]=NULL;
  }

  free(tempForLogoImg);tempForLogoImg=NULL;
  return tagForMakeIt;
}




int judgeFormPiont(struct feature* feat1,struct feature * feat2,CvSeq * featseq,
	int n1,int n2,int n3,
	IplImage*img1,double liangduImg1,IplImage * img2,IplImage*stacked ,
	CvPoint pointWindow1,CvPoint  pointWindow2,
	double * tempsim)
{//在pointWindow1 pointWindow2之间的矩形找到所有特征点，并一一匹配
	int k,j;	
	int muberOfpoint=0;	
	CvSeq* features2,* features3;
	CvMemStorage* storage2,* storage3;
	int simoffearture=0;
	
	struct feature* feat  ,*tempfeat;
	int cengForImag1,cengForImag2;



	MachedFeatPoint *machedstruct,*machedstructtemp;
	double lingduImg2;
	int numberOfMached;//匹配到的点
	muberOfpoint=0;//测试图片在窗口内的点数目

	storage3 = cvCreateMemStorage( 0 );//初始化seq
	features3 = cvCreateSeq( 0, sizeof(CvSeq), sizeof(MachedFeatPoint), storage3 );
	IplImage*stackedTemp;
 	stackedTemp=cvCloneImage(stacked);

	/*
	storage2 = cvCreateMemStorage( 0 );//初始化seq
	features2 = cvCreateSeq( 0, sizeof(CvSeq), sizeof(struct feature), storage2 );
	cvSeqPush( features2, feat )	;
*/

	for (k=0,feat=feat2;k<n2;k++ )
	{//所有窗口内点匹配
		feat=feat2+k;
		if (feat->x >=pointWindow1.x && feat->y> pointWindow1.y && 
			feat->x <=pointWindow2.x  && feat->y <  pointWindow2.y  )
		{		
			muberOfpoint++;
		}					
	}


	numberOfMached=0;
	for (k=0;k<n3;k++ )
	{//所有窗口内点匹配
		machedstruct=(MachedFeatPoint *)cvGetSeqElem(featseq,k);
		if (compareIsInWindow(machedstruct,pointWindow1,pointWindow2) )
		{
			cvSeqPush( features3, machedstruct );
			numberOfMached++;
		}					
	}


	//图片一个区域内 只能是单个点匹配单个logo点， 多个图片点匹配一个logo点就是错误的，
	for (k=0; k<numberOfMached; k++)
	{
		machedstruct=(MachedFeatPoint *)cvGetSeqElem(features3,k);
		for( j=k+1;j<numberOfMached; j++)
		{
			machedstructtemp=(MachedFeatPoint *)cvGetSeqElem(features3,j);

			if (compareIsEqual( machedstruct , machedstructtemp ) )
			{
				cvSeqRemove(features3,j);
				numberOfMached--;
			}


		}

	}


	if (muberOfpoint  > 4)//窗口内点太少，就直接跳过
	{

	//这里修改： 先计算区域图像亮度， 然后计算 sim值
		lingduImg2=ComputeLiangdu(img2,pointWindow1,cvGetSize(img1));

		simoffearture= simOfFearture(n1,muberOfpoint,numberOfMached,
			liangduImg1,lingduImg2 ,
			tempsim);

		if( simoffearture  )
		{
			cvReleaseMemStorage(&storage3);
			return simoffearture;
		}
	}


//	cvReleaseMemStorage(&storage2);
	cvReleaseMemStorage(&storage3);
	//千万千万要记住， 提前退出一定要 释放


	return 0;
}



int sliperWindows(IplImage*img1,IplImage*img2,IplImage*stacked,
struct feature* feat1,struct feature * feat2,CvSeq *featseq,
	int n1,int n2,int n3,
	CvMat * tagMat,
	double * maxFeatsim)
{
	//2设置窗口大小，开始找出窗口中的点
	int i,j,k;

	CvPoint pointWindow1,pointWindow2;

	int ImgWidth=img2->width;
	int ImgHight=img2->height;
	CvSize sizeImg1=cvSize(img1->width,img1->height);
	CvSize sizeImg2=cvSize(img2->width,img2->height);
//修改为精简版	CvSize  SLIPSIZE=cvSize(sizeImg1.width/SLIPER,sizeImg1.height/SLIPER);
	MachedFeatPoint* featTemp;

	double liangduImg1=0.0;

	double  tempFeatsim;
	int tagForCompare=0;

	/*
	//修改为精简版
	if (SLIPSIZE.width<MIN_SLIPSIZE || SLIPSIZE.height< MIN_SLIPSIZE)
	{
		SLIPSIZE.width=MIN_SLIPSIZE;
			 SLIPSIZE.height =MIN_SLIPSIZE;
	}
	*/
	
	if (Img1BigImg2(img1,img2) )
	{
		return  0;//图片小于logo
	}
	/*可以优化的地方：
	 得到匹配点的最值，然后得到maxmin  的窗口点*/
	//用logo窗口 滑动来匹配点
	liangduImg1=ComputeLiangdu(img1,cvPoint(0,0),sizeImg1);
		
	tagForCompare=0;

	/*用每一个匹配到的点  ，确定 图片logo的区域， 再比较 sim*/
	tempFeatsim=0.0;//点的相似度

	for(i=0;i<n3  && tagForCompare ==0 ;i++)
	{
		featTemp =(MachedFeatPoint*) cvGetSeqElem(featseq,i);
		
		GetMachedWindowPoint( featTemp, &pointWindow1, &pointWindow2, sizeImg1 ,sizeImg2);
	
		tagForCompare=judgeFormPiont( feat1,  feat2, featseq,
			n1,  n2,n3,
			img1,liangduImg1,img2, stacked , 
			pointWindow1,  pointWindow2 ,
			&tempFeatsim) ;

		if (*maxFeatsim < tempFeatsim)
		{	//点的相似度更新
			*maxFeatsim=tempFeatsim;
		}

		if(tagForCompare )
		{//修改为精简版
			;//drawTagForWindow(tagMat,pointWindow1.x ,pointWindow1.y,sizeImg1);//wondow内多有点化为1

		}
	}
	/*	for (j=0;j<ImgHight-sizeImg1.height && tagForCompare!=1 ;j+=SLIPSIZE.height)
			{//滑动窗口


				for (i=0;i<ImgWidth-sizeImg1.width && tagForCompare!=1 ;i+=SLIPSIZE.width)
				{

					pointWindow1.x=i;pointWindow1.y=j;//窗口矩形的两个点
					pointWindow2.x=i+sizeImg1.width; pointWindow2.y=j+sizeImg1.height;
					

					if( judgeFormPiont( feat1,  feat2, featseq, n1,  n2,n3, img1,liangduImg1,img2, stacked , pointWindow1,  pointWindow2 ) )
					 					{

						tagForCompare=1;
			
						drawTagForWindow(tagMat,i,j,sizeImg1);//wondow内多有点化为1
						i+=sizeImg1.width-SLIPSIZE.width;

					}
					
				}				
				
				if (tagForCompare)
				{
					j+=sizeImg1.height-SLIPSIZE.height;
					tagForCompare=0;
				}
			
						
		}	*/		/**/

	return  tagForCompare ;
}



int compareFeartureFirstTime( struct feature* feat1,CvMat * tempMatrixForLogo,
struct feature* feat2,CvMat *PictureMatrix,
int n1,int n2,
IplImage* img1,IplImage* stacked,	CvSeq *featseq) 
{   //计算两点矩阵的特征点 的匹配个数。绘制图
	//不再返回特征点，而是返回匹配点的坐标 +logo 对应的坐标

	/*在这里，使用矩阵 相乘 得到两个最小点， 阈值，  加入seq中
		1。矩阵相乘，
		2。求每一列最小两个值，
		3.判别，》阈值，就是了，
	*/
	struct feature** nbrs;
	struct kd_node* kd_root;
		struct feature* feat;

	struct feature* tempFeat1;
	struct feature* tempFeat2;
	int  k, i, m = 0;
	int m2=0;
	int simoffearture=0;
	CvPoint pt1, pt2;
	double d0, d1;
	IplImage*stackedTemp;

	int mi,nj;
	int tempmi;
	double max1,max2,tempmax;
	MachedFeatPoint tempFeatPoint;
	CvMat * resultMatrix;

	/*
	//测试用
	CvSeq *testfeatseq;
	CvMemStorage* storage ;
	IplImage* tempstackedTemp;
	*/


		clock_t start, finish;
		double duration;
	start = clock();//开始计时


	////修改为精简版 stackedTemp=cvCloneImage(stacked);

	resultMatrix=cvCreateMat(tempMatrixForLogo->rows,PictureMatrix->cols,tempMatrixForLogo->type);
 
	cvMatMul(tempMatrixForLogo,PictureMatrix,resultMatrix);


	//printMyMat(resultMatrix);
	//查询矩阵，返回 第一第二大的数组，
	for (nj=0;nj<PictureMatrix->cols ;nj++)
	{
		//特征点产生的时候就过滤了少于5个点的。
		max1= (double )	CV_MAT_ELEM(*resultMatrix,double,0,nj);
		max2= (double )	CV_MAT_ELEM(*resultMatrix,double,1,nj);
		tempmi=0;
		if (max1 < max2)
		{
			tempmax=max1;
			max1=max2;
			max2=tempmax;
			tempmi=1;//标记最大值的index
		}	
		
		for(mi=2;mi< tempMatrixForLogo->rows ;mi++)
		{//求得一列最小两个值  cos应该是最大值啊
			tempmax=(double )	CV_MAT_ELEM(*resultMatrix,double,mi,nj) ;
			if ( tempmax >= max1)
			{
				max2=max1;
				max1=tempmax;
				tempmi=mi;
			}
			else if ( tempmax > max2)
			{
				max2=tempmax;
			}
		}
		tempmax=	 (1- max2)  * YUZHI_COS-   (1-max1)    ;
		//判断比值 》阈值？
		if (  (max1 > YUZHI_COS_MAX )  || 
			(  ( max1 > YUZHI_COS_MIN ) &&  ( ((1-max1) < (1- max2)* YUZHI_COS  )  ))
			 ) 
		{ //

			tempFeat1=feat1+tempmi;
			tempFeat2=feat2+nj;
			/*//修改为精简版
			pt1	 = cvPoint( cvRound( tempFeat1->x ), cvRound( tempFeat1->y ) );
			pt2 = cvPoint( cvRound(tempFeat2->x ), cvRound(tempFeat2->y ) );
			pt2.y += img1->height;
			*/
		////修改为精简版	cvLine( stackedTemp, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );//匹配到的就划线
			m++;
		
			tempFeatPoint.machedPointFeat1=cvPoint( cvRound( tempFeat1->x ), cvRound(tempFeat1->y ) );
			tempFeatPoint.machedPointFeat2=cvPoint( cvRound( tempFeat2->x ), cvRound(tempFeat2->y ) );
			cvSeqPush(featseq,&tempFeatPoint);//压入 匹配到的 图片上的点
		}

	}
    reduceMachedPoints(featseq,&m);
	/*
	if ( m>5)
	{	
	 	myDrawPoint(featseq, m, stacked, img1->height);
	 	printf(" 矩阵匹配： %d  ====",m);

		}*/

	// display_big_img( stackedTemp, "first_Matches" ); // 测试用
//	 cvWaitKey(0);  特变主意； opencv的waitkek会阻止多线程的结束。
	// myDrawPoint(featseq, m, stacked, img1->height);
	/*
	printf(" 矩阵匹配： %d  ====",m);
	
	 display_big_img( stackedTemp, "first_Matches" ); // 测试用
	 //	cvWaitKey(0);
	
	// 	cvWaitKey(0);
	//多个点 匹配logo一个点的去除：




 矩阵耗时 0.013000kdtree耗时  0.207000
 
		display_big_img( stackedTemp, "first_Matches" ); // 测试用
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("矩阵耗时 %lf",duration);*/


	/*

	start = clock();//开始计时
	m2=0;
	kd_root = kdtree_build( feat1, n1 );//	kd_root = kdtree_build( feat2, n2 );
	storage = cvCreateMemStorage( 0 );
	testfeatseq=cvCreateSeq(0,sizeof(CvSeq),sizeof(MachedFeatPoint),storage);
	cvClearSeq(testfeatseq);
		tempstackedTemp=cvCloneImage(stacked);
	for( i = 0; i < n2; i++ )//使用tree匹配？ 具体过程算法
	{
		feat = feat2 + i;
		k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
		if( k == 2 )
		{
			d0 = descr_dist_sq( feat, nbrs[0] );
			d1 = descr_dist_sq( feat, nbrs[1] );
			if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
			{
				pt2 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				pt1 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				pt2.y += img1->height;
				cvLine( tempstackedTemp, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );//匹配到的就划线
				m2++;
				feat2[i].fwd_match = nbrs[0];

				tempFeatPoint.machedPointFeat1=cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				tempFeatPoint.machedPointFeat2=cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				cvSeqPush(testfeatseq,&tempFeatPoint);//压入 匹配到的 图片上的点
			}
		}
		free( nbrs );
		nbrs=NULL;
	}	
	kdtree_release( kd_root );
	/*
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("kdtree耗时 % lf",duration);
	*/

	/*
	printf(" kdtree 匹配： %d \n ",m2 );
	display_big_img( tempstackedTemp, "sencond_Matches" ); // 测试用
	cvWaitKey(0);
		/*		/*MachedFeatPoint ** feat3Points
			*feat3Points=calloc(m,sizeof(MachedFeatPoint));
			*feat3Points=cvCvtSeqToArray(featseq,*feat3Points,CV_WHOLE_SEQ);
		这里 不转 数组，直接seq 出去，让之后也seq处理
			cvReleaseMemStorage( &storage );	
		*/


	cvReleaseMat(&resultMatrix);
//修改为精简版	cvReleaseImage(&stackedTemp);
//修改为精简版	stackedTemp=NULL;
	return m;
}


int compareFearture( struct feature* feat1, struct feature* feat2, 
	int n1,int n2,IplImage* img1,IplImage* stacked,CvPoint pointWindow1,CvPoint pointWindow2)
{//计算两点矩阵的特征点 的匹配个数。绘制图
		//修改， 用 feat1 建立树 
		 
	struct feature** nbrs;
	struct kd_node* kd_root;
	struct feature* feat;
	int  k, i, m = 0;
 	 int simoffearture=0;
	CvPoint pt1, pt2;
	double d0, d1;
	IplImage*stackedTemp;

	//stackedTemp=cvCreateImage(cvGetSize(stacked),stacked->depth,stacked->nChannels);
	stackedTemp=cvCloneImage(stacked);

	kd_root = kdtree_build( feat1, n1 );//	kd_root = kdtree_build( feat2, n2 );
	 
	cvRectangle(stackedTemp,cvPoint(pointWindow1.x,pointWindow1.y+img1->height),
		cvPoint(pointWindow2.x,pointWindow2.y+img1->height),CV_RGB(255,0,0),1,8,0);
	for( i = 0; i < n2; i++ )//使用tree匹配？ 具体过程算法
	  {
		  feat = feat2 + i;
		  k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
		  if( k == 2 )
		  {
			  d0 = descr_dist_sq( feat, nbrs[0] );
			  d1 = descr_dist_sq( feat, nbrs[1] );
			  if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
			  {
				  pt2 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				  pt1 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				  pt2.y += img1->height;
				  cvLine( stackedTemp, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );//匹配到的就划线
				  m++;
				  feat2[i].fwd_match = nbrs[0];
			  }
		  }
		  free( nbrs );
		  nbrs=NULL;
	  }
	
	  kdtree_release( kd_root );

  	display_big_img( stackedTemp, "Matches" );
	//cvWaitKey( 0 );
	//Sleep(500);这个会使得showimge无效
	//cvWaitKey(50);
	cvReleaseImage(&stackedTemp);
	stackedTemp=NULL;

	return m;


}

int simOfFearture(int n1,int n2,int m,
	double huiduImag1,double  huiduImag2,
	double * tempsim)
{	//判断匹配的相似度： @=(AB)/max( A ,B )
	/*
		如果A是很大的图，A就很大， B是缩小后的图标，所以，即使B中点全部匹配到了@也很小。
	*/
	int max;
	double temp;
	double temp_SIGOLIKELY;
	max=n1>n2 ? n1:n2;
	temp=(double)m/max;

	if (temp>0.05 && m> MINNUMBER_MACHING)
	{
		temp_SIGOLIKELY=SIGOLIKELY * ( 1.0 / ( fabs(huiduImag1- huiduImag2 ) + 1 ));	
		
		//fprintf( stderr, " %3d:", m );	
		//fprintf( stderr, "%.3lf ", temp );
		//fprintf(stderr,"%.3lf",temp_SIGOLIKELY );

		printf(  " %3d:", m );	
		printf(  "%.3lf ", temp );
		printf("%.3lf",temp_SIGOLIKELY );

		//logSigma( m, temp , temp_SIGOLIKELY );

		*tempsim=temp/temp_SIGOLIKELY;//返回的相似度

		if ( temp >  temp_SIGOLIKELY *BIG_SIGOLIKELY  )
		{
		//	logSigmaCompare( m, temp/temp_SIGOLIKELY  );
			return 100;//很相似了 ，直接跳出 点的匹配，跳出 层的匹配 ，跳出 其他logo匹配
		}
		else if ( temp > temp_SIGOLIKELY ){
		//	logSigmaCompare( m, temp/temp_SIGOLIKELY  );
			return 90;//一般 相似了 ，直接跳出 点的匹配，跳出 层的匹配 。
		}
		//只要》temp_SIGOLIKELY  都要停算

	}
	return 0;
}