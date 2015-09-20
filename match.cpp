/*
ԭ�棺 logo���С�� ����ÿ����������
ͼƬ����һ����С�������㣬
Ȼ��һһƥ��
������ÿ��logo�Ĵ�С����
����ȡ1.3Ч��ԶԶ����2

���ʱ��ļ�¼

����sigma

ͬһ��ͼ����ͬ��ɫҲ���У����Ҷ�ֵ��ͬ��

*/


#include "mach.h"





void myreleaseFeatStruct(LogoFeatStruct *** logofeat,int numberOfLogo,int NUMBEROF_CENG )
{//�ͷ�feat
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
{//�����̲߳���
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
	IplImage*  *** allImgOfLogo//��������logoͼƬ������
	)
{//ֱ�Ӽ������е�logo ��feat

	/* �����̣߳�����feat�Ŀռ�
	�ṹ�崫����
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

	start = clock();//��ʼ��ʱ
	
	realchipNumber =chipNumber< numberOfLogo? chipNumber: numberOfLogo;

	tempForThread_men=(	struct tempStructForThread_LogoCaculate *)malloc(
		sizeof(	struct tempStructForThread_LogoCaculate )*realchipNumber);
	handle=(HANDLE *) malloc(sizeof(HANDLE)*realchipNumber);

	*logofeat=(LogoFeatStruct **)malloc(sizeof(LogoFeatStruct *) *numberOfLogo );

	//4 16 �޸ģ�����ͼƬlogo����
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

	//��ʼ��Է��࡯ ����ɫ8��һ���ȼ���32���ȼ���


	//��featתΪ����
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
	printf("logo�����ʱ��%.2lf ��\n",duration);
//	system("pause");
	return;
}

unsigned int __stdcall MyThreadFun_LogoCaculate(PVOID pM)  
{  //���̺߳���
	char srcLogoPicturePath[_MAX_PATH];

	IplImage* img1 ;
	IplImage** tempForLogoImg;

	struct feature* feat1 ;

	int n1, k, i, m = 0;
	int endCeng=0;

	int endi=0;
	int iForPicture;
	int logoNumber;//�ڼ���logo
	int starti;
	int tagForHasFeats;
		



	//������
	int threadNumber;
	int numberOfLogo;
	srcIndexStruct *logo_srcIndex;
	LogoFeatStruct *** logofeat;
	LogoFeatStruct temptest;
	int NUMBEROF_CENG;
	IplImage*  *** allImgOfLogo;

	numberOfLogo=((struct tempStructForThread_LogoCaculate*)pM )->numberOfLogo;//logo����
	threadNumber=((struct tempStructForThread_LogoCaculate*)pM )->threadNumber;//�ڼ����߳�
	logo_srcIndex=((struct tempStructForThread_LogoCaculate*)pM )->logo_srcIndex;//logo��ַ����
	logofeat=((struct tempStructForThread_LogoCaculate*)pM )->logofeat;//���������� �͸���������
	NUMBEROF_CENG=((struct tempStructForThread_LogoCaculate*)pM )->NUMBEROF_CENG;
	allImgOfLogo=((struct tempStructForThread_LogoCaculate*)pM )->allImgOfLogo;//��ͼƬ

	if (numberOfLogo /chipNumber==0)
	{//chip �� logo��Ŀ
		starti=threadNumber;//ֱ�����߳����Ϊͼ���
		endi=threadNumber+1;
	}
	else
	{
		starti=numberOfLogo /chipNumber *threadNumber;
		//��ʾ�ڼ����߳�
		if (threadNumber==chipNumber-1)//���һ���߳�
		{
			endi=numberOfLogo;
		}
		else
		{
			endi=numberOfLogo /chipNumber *(threadNumber+1);
		}
	}


	for(iForPicture = starti; iForPicture< endi ;iForPicture++)
	{//�̵߳�ÿһ��ͼ

	
		logoNumber=iForPicture;//logo��� ��0��ʼ��
		strcpy(srcLogoPicturePath,logo_srcIndex[logoNumber].srcIndex);	

		
		(*logofeat)[logoNumber]=(LogoFeatStruct *)malloc(sizeof(LogoFeatStruct ) *NUMBEROF_CENG );

		/*���ݿ����� ����Ҫ��
		for (i=0;i<NUMBEROF_CENG   ;i++)
		{//�����Ѿ��������ݿ�Ĳ�����ֱ�Ӷ�ȡ�ͺ���
			tagForHasFeats= mySqlGetLogoFeats(logo_srcIndex[logoNumber].id  ,i, &(*logofeat)[logoNumber][i]) ;
 
			if (tagForHasFeats == 0)
			{//û�д��feats
				break;
			}

		}	
		
		if ( i== NUMBEROF_CENG)
			{//���һ���� ������
				//����ʱ����

				continue; //������һ��
			}
		*/
		img1 = myLoadImage( srcLogoPicturePath, 1 );	
		if (img1 ==NULL)
		{
			printf(" %s ��logo��ʧ ���벹��\n",srcLogoPicturePath);
				fclose(stdout);//�ر��ļ� 
			exit(1);
		//	continue ;//����û��ͼƬ
		}

		//����logo�Զ����ڴ�С�� ������
		img1= myReduceLogoAndSave(img1,srcLogoPicturePath);

		endCeng=0;
		//(*allImgOfLogo)[logoNumber][0] = img1;

		tempForLogoImg=(IplImage**)malloc(sizeof(IplImage*)*NUMBEROF_CENG);
		//��ԭͼ  �ȶ����С������feat
		for (i=0;i<NUMBEROF_CENG   ;i++)
		{//����
			if (i ==0)
			{   
				tempForLogoImg[i]=cvCloneImage(img1);
				(*allImgOfLogo)[logoNumber][i]=cvCloneImage( tempForLogoImg[i] );//����logo
			}  
			else
				{
					tempForLogoImg[i]= smallSizeImg( tempForLogoImg[i-1],NUMBERFOR_RESIZE);
					(*allImgOfLogo)[logoNumber][i]=cvCloneImage( tempForLogoImg[i] );//����logo
			}
			endCeng=i;
			if (isTooSmall( tempForLogoImg[i]))
			{//й¶��

				for (;i< NUMBEROF_CENG ;i++)
				{
					(*logofeat)[logoNumber][i].feat1=NULL;
					(*logofeat)[logoNumber][i].numberOffeat=0;
					
					/*//�������ݿ⣺
					mySqlInsertLogoFeats(logo_srcIndex[logoNumber].id  ,i, 
						(*logofeat)[logoNumber][i]);
					*/
					/*//����
					mySqlGetLogoFeats(logo_srcIndex[logoNumber].id  ,i, 
						&temptest) ;
						*/

				}
				break;//logo ̫С��
			}


			//��������
		//	printf( "Finding features in %s ----%d ...\n", srcLogoPicturePath,i );
			//fprintf( stderr, "Finding features in %s ----%d ...\n", srcLogoPicturePath,i );
			n1 = sift_features(  tempForLogoImg[i], &feat1 );
		//	fprintf( stderr, " features %d\n", n1 );
		//	printf(  " features %d\n", n1 );
			//(*logofeat)[j][i]= (LogoFeatStruct *)malloc(sizeof(LogoFeatStruct));
			(*logofeat)[logoNumber][i].feat1=feat1;
			(*logofeat)[logoNumber][i].numberOffeat=n1;

			//���Բ������ݿ⣺
		//	 mySqlInsertLogoFeats(logo_srcIndex[logoNumber].id  ,i, (*logofeat)[logoNumber][i]);

			// mySqlGetLogoFeats(logo_srcIndex[logoNumber].id  ,i, &temptest);
		}//ÿһ��

		for (i=0;i<=endCeng ;i++)
		{
			cvReleaseImage(&tempForLogoImg[i]);
		}


		free(tempForLogoImg);
		cvReleaseImage(&img1);
	}//ÿһ��ͼ
		
	return 0;
}

void  LogoFeatCaculate(	
int numberOfLogo,int NUMBEROF_CENG,
srcIndexStruct *logo_srcIndex,
struct logoFeatStruct*** logofeat
 )  //������
{ 

//ֱ�Ӽ���logo������
	char srcLogoPicturePath[_MAX_PATH];

	IplImage* img1 ;
	IplImage** tempForLogoImg;

	struct feature* feat1 ;

	int n1, k, i, m = 0;
	int endCeng=0;

	int endi=0;
	int iForPicture;
	int logoNumber;//�ڼ���logo
	int starti;
	int tagForHasFeats;
		
		
	starti=0;endi=numberOfLogo;
	for(iForPicture = starti; iForPicture< endi ;iForPicture++)
	{//�̵߳�ÿһ��ͼ

	
		logoNumber=iForPicture;//logo��� ��0��ʼ��
		strcpy(srcLogoPicturePath,logo_srcIndex[logoNumber].srcIndex);	

		
		(*logofeat)[logoNumber] =(struct logoFeatStruct *)malloc(sizeof(struct logoFeatStruct) *NUMBEROF_CENG );

		img1 = myLoadImage( srcLogoPicturePath, 1 );	
		if (img1 ==NULL)
		{
			printf(" %s ��logo��ʧ ���벹��\n",srcLogoPicturePath);
				fclose(stdout);//�ر��ļ� 
			exit(1);
		//	continue ;//����û��ͼƬ
		}

		//����logo�Զ����ڴ�С�� ������
		img1= myReduceLogoAndSave(img1,srcLogoPicturePath);

		endCeng=0;

		tempForLogoImg=(IplImage**)malloc(sizeof(IplImage*)*NUMBEROF_CENG);
		//��ԭͼ  �ȶ����С������feat
		for (i=0;i<NUMBEROF_CENG   ;i++)
		{//����
			if (i ==0)
			{   
				tempForLogoImg[i]=cvCloneImage(img1);
			}  
			if(i!=0)
				tempForLogoImg[i]= smallSizeImg( tempForLogoImg[i-1],NUMBERFOR_RESIZE);

			endCeng=i;
			if (isTooSmall( tempForLogoImg[i]))
			{//й¶��

				for (;i< NUMBEROF_CENG ;i++)
				{
					(*logofeat)[logoNumber][i].feat1=NULL;
					(*logofeat)[logoNumber][i].numberOffeat=0;
				}
				break;//logo ̫С��
			}
		//��������
		//	printf( "Finding features in %s ----%d ...\n", srcLogoPicturePath,i );
			//fprintf( stderr, "Finding features in %s ----%d ...\n", srcLogoPicturePath,i );
			n1 = sift_features(  tempForLogoImg[i], &feat1 );
		//	fprintf( stderr, " features %d\n", n1 );
		//	printf(  " features %d\n", n1 );
			//(*logofeat)[j][i]= (LogoFeatStruct *)malloc(sizeof(LogoFeatStruct));
			(*logofeat)[logoNumber][i].feat1=feat1;
			(*logofeat)[logoNumber][i].numberOffeat=n1;

		}//ÿһ��

		for (i=0;i<=endCeng ;i++)
		{
			cvReleaseImage(&tempForLogoImg[i]);
		}


		free(tempForLogoImg);
		cvReleaseImage(&img1);
	}//ÿһ��ͼ
		
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
{//�̲߳������ݽṹ��
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
	//�̲߳���
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


	char srcTestPicturePath[_MAX_PATH];//����ȡ�ĵ�ַ
	char srcLogoPicturePath[_MAX_PATH];
	IplImage* img2;
	struct feature  * feat2;
	int n2;

	char  tempNameForPictureChange[_MAX_PATH];
	char *nameForPictureChange="_Mach_";
	char *nameForPictureSub="��ͼ_";
	char tempnumber[10];
	char dstTestPicturePath[_MAX_PATH];

	double maxEachPicturesim;//������ƶ�
	double tempLogosim;//ÿ��ͼ����ʱֵ

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
	//�̲߳�������
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
	//���ͼƬ��С���߳��� ��ô�߳̿�ʼ��min��ͼ��Ŀ��chipernumber��
	if (numberOfPicture /chipNumber==0)
	{
		stari=threadNumber;//ֱ�����߳����Ϊͼ���
		endi=threadNumber+1;
	}
	else
	{
		stari=numberOfPicture /chipNumber *threadNumber;

		 //��ʾ�ڼ����߳�
		if (threadNumber==chipNumber-1)//���һ���߳�
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
	{//ѭ������ֱ���Ҳ���δ����ͼƬ
		
		//���⴦������δ����ͼƬ���
		if (srcIndexInterlocked >= numberOfPicture-1)
		{//��-1��ʼ���ȼ�1��Ȼ��ȡ��ַ��
			break;//���һ�Ŷ����������
		}
		 InterlockedIncrement((LPLONG)&srcIndexInterlocked); //ԭ�Ӳ���������
		 
			iForPicture=srcIndexInterlocked;

			//printf("\n\n====��ǰ���� %d   \n\n",iForPicture);

	//��һ�����õ�����ͼ���ֿ鲢����������
		//	logoNumber+=i;
	
		tempPictureNumber=iForPicture;
		tagForMatched=0;//�����logo


		//����ʱ��
		//	saveTime(j);

		strcpy(srcTestPicturePath,Picture_srcIndex[tempPictureNumber].srcIndex);

		/* ͼƬ���� */
		img2 = myLoadImage( srcTestPicturePath, 1 );
		//�����ж϶���Ķ���
		if (img2 ==NULL)
		{
			printf("û�ж�ӦͼƬ %s",srcTestPicturePath);
			continue ;//����û��ͼƬ
		}


		img2=myReduceImage(img2);//�Ը߷ֱ���²���


		//��ȫ�ֱ��� ���ж��Ż���Σ� ��1 ȡһ���� 2ȡ3���� 3 ����
		//С�� maxlogosize�� �ֲ�Ҫ���ˡ�
		if (YOUHUA_TYPE ==1)
		{
			numberOfpices=myGetImgQuarter(&imgForPicture, img2);
		}
		else if (YOUHUA_TYPE ==2)
		{
			numberOfpices=myGetImgFourQarters(&imgForPicture, img2);
		}
		else
		{//���Ż� 
			numberOfpices= myDivedeImg(&imgForPicture, img2);
		}



		//numberOfpices= myDivedeImg(&imgForPicture, img2);

		cvReleaseImage( &img2 );
			
		tagForMakeIt=0;

		maxEachPicturesim=0.0;//ÿ������ͼ������ƶȶ�ΪŶ��o
		tempLogosim=0.0;//��ʼΪ0.0��

		for( k=0;k< numberOfpices   && tagForMakeIt==0 ; k++)
		{	
			if (isTooSmall(imgForPicture[k]) )
			{
				continue;//��ͼ����ܺ�С
			}

			printf(  "Finding features in %s...\n", srcTestPicturePath );
		//	fprintf( stderr, "Finding features in %s...\n", srcTestPicturePath );
			n2 = sift_features( imgForPicture[k], &feat2 );//ȡ��������
			//	fprintf( stderr, " features %d\n", n2 );
			printf(  " features %d\n", n2 );

			//����
		//	PictureMatrix=cvCreateMat(NUMBERFORDIVISIOM,n2,CV_64FC1);//CV_32FC1);
			//featToMatrix(PictureMatrix,feat2,n2);
		//	printMyMatIndouble( PictureMatrix);
			ouputPictureData(feat2,n2);
			if ( n2 >MINNUMBER_FORFEAT)
			{//������̫�پͲ�Ҫƥ����

//draw_features( imgForPicture[k],feat2,n2);
				//collectionOfIndex ��û����
				 tagForMakeIt=myJudgeIsMatch(	 feat2, n2	,
					nnStruct,	  collectionOfIndex, numberOfLogo, NUMBEROF_CENG,
				 logofeat,allImgOfLogo,imgForPicture[k]);


				

				/*
			//����ʹ�þ���ת��
				PictureMatrix=cvCreateMat(NUMBERFORDIVISIOM,n2,CV_64FC1);//CV_32FC1);
				featToMatrix(PictureMatrix,feat2,n2);

//�ڶ��󲽣�ͼƬ��ÿ��logoƥ��

			//�����޸�Ϊ�µ�ƥ�䷽��������ȫ��Ҫ

				for (i=0;i<numberOfLogo  && tagForMakeIt==0;i++)
				{
					strcpy(srcLogoPicturePath,logo_srcIndex[i].srcIndex);//�õ�logo��ַ	
					//�޸ı���·��void chageSavePath()


					//����汾��
					/*
					//�����ͼƬ
				//��ͼ_K 
					strcpy(tempNameForPictureChange,nameForPictureSub);//�����ͼ��Ĵ���
					intToChar(k,tempnumber);
					strcat(tempNameForPictureChange,tempnumber);
			
					strcat(tempNameForPictureChange,nameForPictureChange);
				//_macth i
					intToChar(i,tempnumber);
					strcat(tempNameForPictureChange,tempnumber);

					//ƴ�ӱ���·�� //Ĭ�� ��ʽ��4��char   ��jpg
					changepath( srcTestPicturePath,tempNameForPictureChange,dstTestPicturePath);
				//dstTestPicturePath=NULL;
*//*
					tempMatrix=LogoMatrix[i];//logo��ǰ���matrix  ��0��ʼ����
		
			

					
					tagForMakeIt=machEachPicture(i,logofeat,tempMatrix,
						srcLogoPicturePath, imgForPicture[k],
						feat2,PictureMatrix,
						n2,dstTestPicturePath,
						&tempLogosim);//logo��test�Ƚϴ���dst·����
/**/



/*
					if ( tempLogosim>maxEachPicturesim)
					{//����ÿ��ͼ ƥ�����logo ��Ӧ���ƶ�
						maxEachPicturesim=tempLogosim;
					}

					if (tagForMakeIt)
					{		//ÿ��ƥ��� tagForMakeIt���ᱻ���£�������tagForMatched �������Ƿ�ƥ��
						tagForMatched=1;//���ƥ�䵽�ˣ�
						tagForMatchedLogo=logo_srcIndex[i].id;
						
						/*
						WaitForSingleObject(mutex, INFINITE);  
					
						 SaveResult(tempPictureNumber ,k ,tagForMatchedLogo);
							//��Ҫͬ������
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
		}	//ÿһ��ͼ����

		for( k=0;k< numberOfpices; k++)
		{	
			cvReleaseImage(  &imgForPicture[k]);
		}
		free(imgForPicture); imgForPicture=NULL;

	//�����Ϊ maxEachPicturesim ��0.1  10% ��д�����ݿ�
		//if (maxEachPicturesim >0.1)
		/*	{//�޸�Ϊ����simֵ
			WaitForSingleObject(mutex, INFINITE);  
			mySqlInsertResult(Picture_srcIndex[tempPictureNumber].id,maxEachPicturesim);//logoid������
			ReleaseMutex(mutex);  					
		}*/


		if(tagForMatched)//�Ļ���
		{// 
			WaitForSingleObject(mutex, INFINITE);  
			//��Ҫͬ������
			numberForMachedPicture++;
			mySqlInsertResultWithLogoId(Picture_srcIndex[tempPictureNumber].id,tagForMatchedLogo);//logoid������
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
{//���� feat2���飬�ж��Ƿ�����logo�ģ�
	//���� 0����ƥ�䣬 1 ƥ��
	//struct logoFeatStruct*** logofeat=&logofeatNP;


	//���� logo�ĵ��ʵĽṹ��
	CvSeq ***featseq;
	CvMemStorage*** storage ;
	int **StatisticsOfPoins;
	//����CvSeq *featseq
	buidLogoSeq( numberOfLogo, NUMBEROF_CENG,	featseq,storage,StatisticsOfPoins);
	int tempnumbermach=0;
	//1ÿ�����ѯ�Ҷ�+lsh=������һ��//2���������
	for (int i=0;i< n2; i++ )
	{
		struct arrayOfGrayIndex*  arrryIndex= mySearchInIndex(nnStruct,feat2[i],collectionOfIndex);
		if (arrryIndex ==NULL )
		{
			continue;
		}
		//3��ȡ����飬 ���� ÿ��logo�Ĳ���

		struct seqIndexAndClassOfGray *listIndex;
		listIndex=arrryIndex->index;
		struct indexAndClassOfGray *tempindxclass;

		do 
		{//���Ž� ƥ��� ѹ�룬		ͳ��

			tempnumbermach++;

			tempindxclass=&listIndex->grayindex;
			MachedFeatPoint tempFeatPoint;//��ʱ����ƥ���

			struct feature  * feat1=//��ȡfeat1��
				&(  (logofeat[tempindxclass->logoid][tempindxclass->ceng	]).feat1[tempindxclass->featorder]);


			tempFeatPoint.machedPointFeat1=cvPoint( cvRound( feat1->x ), cvRound(feat1->y ) );
			tempFeatPoint.machedPointFeat2=cvPoint( cvRound( feat2->x ), cvRound(feat2->y ) );
			cvSeqPush(featseq[tempindxclass->logoid][tempindxclass->ceng],&tempFeatPoint);//ѹ�� ƥ�䵽�� ͼƬ�ϵĵ�
			StatisticsOfPoins[tempindxclass->logoid][tempindxclass->ceng] +=1;//��ǰ��+1

			listIndex=listIndex->next;//��һ��

		} while (listIndex != NULL);
	}
	
	if (tempnumbermach==0 )
	{	//һ����Ҳû��
		releaseLogoSeq( numberOfLogo, NUMBEROF_CENG,featseq,storage,StatisticsOfPoins);
		return 0;
	}
	//��ÿ��� ƥ����� �����ܵ��� ��ȡ����ǰ����
	int *maxarry;
	int maxSize=5;

	maxarry =(int *) malloc(sizeof(int )* maxSize);
	// featseq  StatisticsOfPoins   logofeat//4ȡN/������� ǰ�����жϡ�
	getMaxRatioForMach(	numberOfLogo,NUMBEROF_CENG,logofeat,StatisticsOfPoins,featseq,
		maxarry,maxSize 	);

	//5����sliperwindows�����ж�
	int tagForMakeIt=0;
	for (int i=0 ;i< maxSize ; i++)
	{
		if ( tagForMakeIt !=0)
		{
			break;
		}
		if (maxarry[i] <0  )
		{//����ˣ�����û�к��ʵĵ�
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
			//	stacked=NULL;//�޸�Ϊ�����
			//һ����ʾ����壨�����ĸ߶Ⱥͣ���������Ҳ����

		double tempCengsim=0.0;//��¼ƥ��ֵ
		tagForMakeIt= compareInTwoImgTwoFeats( img1, img2, stacked,
			feat1, feat2, featseq[logoid][ceng],  n1,n2, 
			featseq[logoid][ceng]->total,&tempCengsim);
	}

	releaseLogoSeq( numberOfLogo, NUMBEROF_CENG,featseq,storage,StatisticsOfPoins);
	return tagForMakeIt;//����Ҳû�ҵ�

}


int compareInTwoImgTwoFeats(IplImage*img1,IplImage*img2,IplImage*stacked,
struct feature* feat1,struct feature * feat2,CvSeq *featseq,
	int n1,int n2,int n3,//ƥ��ĶԸ���
	double * maxFeatsim)
{//ͨ������ͼƬ �͸��Ե�feat�� �Ѿ�ƥ��õ� seq ������ƥ���ʣ������Ƿ�ƥ��
	//2���ô��ڴ�С����ʼ�ҳ������еĵ�
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
	//�Ȼ�һ��
	myDrawPoint(featseq,n3,stacked,ImgHight);

	cvWaitKey(0);
 
	
	if (Img1BigImg2(img1,img2) )
	{
		return  0;//ͼƬС��logo
	}
	/*�����Ż��ĵط���
	 �õ�ƥ������ֵ��Ȼ��õ�maxmin  �Ĵ��ڵ�*/
	//��logo���� ������ƥ���
	liangduImg1=ComputeLiangdu(img1,cvPoint(0,0),sizeImg1);
		
	tagForCompare=0;

	/*��ÿһ��ƥ�䵽�ĵ�  ��ȷ�� ͼƬlogo������ �ٱȽ� sim*/
	tempFeatsim=0.0;//������ƶ�

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
		{	//������ƶȸ���
			*maxFeatsim=tempFeatsim;
		}

		if(tagForCompare )
		{//�޸�Ϊ�����
			;//drawTagForWindow(tagMat,pointWindow1.x ,pointWindow1.y,sizeImg1);//wondow�ڶ��е㻯Ϊ1

		}
	}
 

	return  tagForCompare ;
}



int machEachPicture(int numberForLogo,LogoFeatStruct ** logofeat,CvMat** tempMatrix,
	char* srcLogoPicturePath,IplImage * img2_orig, 
	struct feature  * feat2,CvMat *PictureMatrix,
	int n2,char *dstTestPicturePath,
	double * maxCengsim)
{//һ��logo�Ĳ�ͬ�� �Բ���ͼ��һ��pice  ƥ��

	double tempCengsim;//ÿ�������ֵ

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

 // struct feature  *feat2 = (struct feature  *)calloc( n2, sizeof(struct feature) );//��feat3 ����һ�ݣ�Ȼ������޸�
  struct feature  * feat3;
  int  machedPointsnumber;

  int numberForFeat;
  //����ƥ�䵽�ĵ�Ĵ���
  CvSeq *featseq;
  CvMemStorage* storage ;

  CvMat* tempMatrixForLogo;

  storage = cvCreateMemStorage( 0 );
  featseq=cvCreateSeq(0,sizeof(CvSeq),sizeof(MachedFeatPoint),storage);


  //���Ժܺõ��Ż� �ڴ�ռ�  ֻ��һ������
  img1 = myLoadImage( srcLogoPicturePath, 1 );

 /*/ ����汾tagMat=cvCreateMat(img2->height,img2->width,CV_8SC1);//��Ǿ���

  //cvZero(tagMat);
  */
  tagMat=NULL;

  tempForLogoImg=(IplImage**)malloc(sizeof(IplImage*)*NUMBEROF_CENG);


   tempCengsim=0.0;////ÿ�������ֵ ��ʼsimֵΪo

  //��ԭͼ  �ȶ����С������feat
  for (i=0;i<NUMBEROF_CENG   && tagForMakeIt ==0 ;i++)
  {//����
	  if (i ==0)
	  {   
		  tempForLogoImg[i]=cvCloneImage(img1);
	  }  
	  if(i!=0)
	  tempForLogoImg[i]= smallSizeImg( tempForLogoImg[i-1],NUMBERFOR_RESIZE);
 
	  if (Img1BigImg2( tempForLogoImg[i],img2))
	  {
		  startCeng=i+1;//����й¶����������
		   endCeng=i;
		  continue;//logo>ͼ
	  }
	  endCeng=i;
	  if (isTooSmall( tempForLogoImg[i]))
	  {
		  endCeng=i;//���������ڴ�й¶�˰�
		  break;//logo ̫С��
	  }


		  {//��������
			 numberForFeat=numberForLogo ;
			 n1 = logofeat[numberForFeat][i].numberOffeat;
			 feat1=logofeat[numberForFeat][i].feat1;
			 tempMatrixForLogo=tempMatrix[i];//�õ�������ľ���

		//	 stacked = stack_imgs(  tempForLogoImg[i], img2 );
			 stacked=NULL;//�޸�Ϊ�����

			 //һ����ʾ����壨�����ĸ߶Ⱥͣ���������Ҳ����

			  if ( n1 > MINNUMBER_FORFEAT)
			  {			  				  
				  cvClearSeq(featseq);
				  //Ԥ��ƥ��
				  machedPointsnumber=compareFeartureFirstTime(feat1,tempMatrixForLogo,
					  feat2,PictureMatrix,n1,n2, 
					  tempForLogoImg[i],stacked,featseq);

				  //��ǰ�ж� feat �� ��ֵ
				  if (machedPointsnumber > ( n1 *SIGOLIKELY )/3 )
				  {
					//�������ڱȽ�
					  tagForMakeIt= sliperWindows( tempForLogoImg[i], img2, stacked,
						  feat1, feat2, featseq,  n1,n2, 
						  machedPointsnumber,tagMat,
						  &tempCengsim);
					 
					  if (tempCengsim > *maxCengsim)
					  {//���²��������ƶ�
						  *maxCengsim =tempCengsim;
					  }
							//  cvWaitKey(0);
				  }

			   }
			  
		/*//�޸�Ϊ�����
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
 //�޸�Ϊ�����
 /*
  //��ʾtagmat  CvMat תimg
  tagImag=cvCreateImageHeader(cvGetSize(tagMat),IPL_DEPTH_8U ,1);
  cvGetImage(tagMat,tagImag);
  tagImag->depth=8;
//  fprintf(stderr," %d \n", tagImag->depth);

  cvThreshold(tagImag,tagImag,0.5,254,CV_THRESH_BINARY); 
  find_Drawcontours(tagImag,img2);

  */
  /*
  if (tagForMakeIt)
  {//ֱ�ӷ��� dstTestPicturePath�� �ϲ�Ŀ¼

	  myCreateFatherFile(dstTestPicturePath);
	//Ҫ�½��ļ��У� ����
	cvSaveImage(dstTestPicturePath,img2,0);//����ͼƬ
  }
  */


 /*
  cvReleaseImageHeader(&tagImag);//cvGetImage() �� ֻ�ǽ�mat �����ݵ�ַ���� img��imgֻҪһ��ͷ�ֹ��ˣ����Բ����ͷ�img��ֻ���ͷ�ͷ
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
{//��pointWindow1 pointWindow2֮��ľ����ҵ����������㣬��һһƥ��
	int k,j;	
	int muberOfpoint=0;	
	CvSeq* features2,* features3;
	CvMemStorage* storage2,* storage3;
	int simoffearture=0;
	
	struct feature* feat  ,*tempfeat;
	int cengForImag1,cengForImag2;



	MachedFeatPoint *machedstruct,*machedstructtemp;
	double lingduImg2;
	int numberOfMached;//ƥ�䵽�ĵ�
	muberOfpoint=0;//����ͼƬ�ڴ����ڵĵ���Ŀ

	storage3 = cvCreateMemStorage( 0 );//��ʼ��seq
	features3 = cvCreateSeq( 0, sizeof(CvSeq), sizeof(MachedFeatPoint), storage3 );
	IplImage*stackedTemp;
 	stackedTemp=cvCloneImage(stacked);

	/*
	storage2 = cvCreateMemStorage( 0 );//��ʼ��seq
	features2 = cvCreateSeq( 0, sizeof(CvSeq), sizeof(struct feature), storage2 );
	cvSeqPush( features2, feat )	;
*/

	for (k=0,feat=feat2;k<n2;k++ )
	{//���д����ڵ�ƥ��
		feat=feat2+k;
		if (feat->x >=pointWindow1.x && feat->y> pointWindow1.y && 
			feat->x <=pointWindow2.x  && feat->y <  pointWindow2.y  )
		{		
			muberOfpoint++;
		}					
	}


	numberOfMached=0;
	for (k=0;k<n3;k++ )
	{//���д����ڵ�ƥ��
		machedstruct=(MachedFeatPoint *)cvGetSeqElem(featseq,k);
		if (compareIsInWindow(machedstruct,pointWindow1,pointWindow2) )
		{
			cvSeqPush( features3, machedstruct );
			numberOfMached++;
		}					
	}


	//ͼƬһ�������� ֻ���ǵ�����ƥ�䵥��logo�㣬 ���ͼƬ��ƥ��һ��logo����Ǵ���ģ�
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


	if (muberOfpoint  > 4)//�����ڵ�̫�٣���ֱ������
	{

	//�����޸ģ� �ȼ�������ͼ�����ȣ� Ȼ����� simֵ
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
	//ǧ��ǧ��Ҫ��ס�� ��ǰ�˳�һ��Ҫ �ͷ�


	return 0;
}



int sliperWindows(IplImage*img1,IplImage*img2,IplImage*stacked,
struct feature* feat1,struct feature * feat2,CvSeq *featseq,
	int n1,int n2,int n3,
	CvMat * tagMat,
	double * maxFeatsim)
{
	//2���ô��ڴ�С����ʼ�ҳ������еĵ�
	int i,j,k;

	CvPoint pointWindow1,pointWindow2;

	int ImgWidth=img2->width;
	int ImgHight=img2->height;
	CvSize sizeImg1=cvSize(img1->width,img1->height);
	CvSize sizeImg2=cvSize(img2->width,img2->height);
//�޸�Ϊ�����	CvSize  SLIPSIZE=cvSize(sizeImg1.width/SLIPER,sizeImg1.height/SLIPER);
	MachedFeatPoint* featTemp;

	double liangduImg1=0.0;

	double  tempFeatsim;
	int tagForCompare=0;

	/*
	//�޸�Ϊ�����
	if (SLIPSIZE.width<MIN_SLIPSIZE || SLIPSIZE.height< MIN_SLIPSIZE)
	{
		SLIPSIZE.width=MIN_SLIPSIZE;
			 SLIPSIZE.height =MIN_SLIPSIZE;
	}
	*/
	
	if (Img1BigImg2(img1,img2) )
	{
		return  0;//ͼƬС��logo
	}
	/*�����Ż��ĵط���
	 �õ�ƥ������ֵ��Ȼ��õ�maxmin  �Ĵ��ڵ�*/
	//��logo���� ������ƥ���
	liangduImg1=ComputeLiangdu(img1,cvPoint(0,0),sizeImg1);
		
	tagForCompare=0;

	/*��ÿһ��ƥ�䵽�ĵ�  ��ȷ�� ͼƬlogo������ �ٱȽ� sim*/
	tempFeatsim=0.0;//������ƶ�

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
		{	//������ƶȸ���
			*maxFeatsim=tempFeatsim;
		}

		if(tagForCompare )
		{//�޸�Ϊ�����
			;//drawTagForWindow(tagMat,pointWindow1.x ,pointWindow1.y,sizeImg1);//wondow�ڶ��е㻯Ϊ1

		}
	}
	/*	for (j=0;j<ImgHight-sizeImg1.height && tagForCompare!=1 ;j+=SLIPSIZE.height)
			{//��������


				for (i=0;i<ImgWidth-sizeImg1.width && tagForCompare!=1 ;i+=SLIPSIZE.width)
				{

					pointWindow1.x=i;pointWindow1.y=j;//���ھ��ε�������
					pointWindow2.x=i+sizeImg1.width; pointWindow2.y=j+sizeImg1.height;
					

					if( judgeFormPiont( feat1,  feat2, featseq, n1,  n2,n3, img1,liangduImg1,img2, stacked , pointWindow1,  pointWindow2 ) )
					 					{

						tagForCompare=1;
			
						drawTagForWindow(tagMat,i,j,sizeImg1);//wondow�ڶ��е㻯Ϊ1
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
{   //������������������ ��ƥ�����������ͼ
	//���ٷ��������㣬���Ƿ���ƥ�������� +logo ��Ӧ������

	/*�����ʹ�þ��� ��� �õ�������С�㣬 ��ֵ��  ����seq��
		1��������ˣ�
		2����ÿһ����С����ֵ��
		3.�б𣬡���ֵ�������ˣ�
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
	//������
	CvSeq *testfeatseq;
	CvMemStorage* storage ;
	IplImage* tempstackedTemp;
	*/


		clock_t start, finish;
		double duration;
	start = clock();//��ʼ��ʱ


	////�޸�Ϊ����� stackedTemp=cvCloneImage(stacked);

	resultMatrix=cvCreateMat(tempMatrixForLogo->rows,PictureMatrix->cols,tempMatrixForLogo->type);
 
	cvMatMul(tempMatrixForLogo,PictureMatrix,resultMatrix);


	//printMyMat(resultMatrix);
	//��ѯ���󣬷��� ��һ�ڶ�������飬
	for (nj=0;nj<PictureMatrix->cols ;nj++)
	{
		//�����������ʱ��͹���������5����ġ�
		max1= (double )	CV_MAT_ELEM(*resultMatrix,double,0,nj);
		max2= (double )	CV_MAT_ELEM(*resultMatrix,double,1,nj);
		tempmi=0;
		if (max1 < max2)
		{
			tempmax=max1;
			max1=max2;
			max2=tempmax;
			tempmi=1;//������ֵ��index
		}	
		
		for(mi=2;mi< tempMatrixForLogo->rows ;mi++)
		{//���һ����С����ֵ  cosӦ�������ֵ��
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
		//�жϱ�ֵ ����ֵ��
		if (  (max1 > YUZHI_COS_MAX )  || 
			(  ( max1 > YUZHI_COS_MIN ) &&  ( ((1-max1) < (1- max2)* YUZHI_COS  )  ))
			 ) 
		{ //

			tempFeat1=feat1+tempmi;
			tempFeat2=feat2+nj;
			/*//�޸�Ϊ�����
			pt1	 = cvPoint( cvRound( tempFeat1->x ), cvRound( tempFeat1->y ) );
			pt2 = cvPoint( cvRound(tempFeat2->x ), cvRound(tempFeat2->y ) );
			pt2.y += img1->height;
			*/
		////�޸�Ϊ�����	cvLine( stackedTemp, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );//ƥ�䵽�ľͻ���
			m++;
		
			tempFeatPoint.machedPointFeat1=cvPoint( cvRound( tempFeat1->x ), cvRound(tempFeat1->y ) );
			tempFeatPoint.machedPointFeat2=cvPoint( cvRound( tempFeat2->x ), cvRound(tempFeat2->y ) );
			cvSeqPush(featseq,&tempFeatPoint);//ѹ�� ƥ�䵽�� ͼƬ�ϵĵ�
		}

	}
    reduceMachedPoints(featseq,&m);
	/*
	if ( m>5)
	{	
	 	myDrawPoint(featseq, m, stacked, img1->height);
	 	printf(" ����ƥ�䣺 %d  ====",m);

		}*/

	// display_big_img( stackedTemp, "first_Matches" ); // ������
//	 cvWaitKey(0);  �ر����⣻ opencv��waitkek����ֹ���̵߳Ľ�����
	// myDrawPoint(featseq, m, stacked, img1->height);
	/*
	printf(" ����ƥ�䣺 %d  ====",m);
	
	 display_big_img( stackedTemp, "first_Matches" ); // ������
	 //	cvWaitKey(0);
	
	// 	cvWaitKey(0);
	//����� ƥ��logoһ�����ȥ����




 �����ʱ 0.013000kdtree��ʱ  0.207000
 
		display_big_img( stackedTemp, "first_Matches" ); // ������
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("�����ʱ %lf",duration);*/


	/*

	start = clock();//��ʼ��ʱ
	m2=0;
	kd_root = kdtree_build( feat1, n1 );//	kd_root = kdtree_build( feat2, n2 );
	storage = cvCreateMemStorage( 0 );
	testfeatseq=cvCreateSeq(0,sizeof(CvSeq),sizeof(MachedFeatPoint),storage);
	cvClearSeq(testfeatseq);
		tempstackedTemp=cvCloneImage(stacked);
	for( i = 0; i < n2; i++ )//ʹ��treeƥ�䣿 ��������㷨
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
				cvLine( tempstackedTemp, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );//ƥ�䵽�ľͻ���
				m2++;
				feat2[i].fwd_match = nbrs[0];

				tempFeatPoint.machedPointFeat1=cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				tempFeatPoint.machedPointFeat2=cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				cvSeqPush(testfeatseq,&tempFeatPoint);//ѹ�� ƥ�䵽�� ͼƬ�ϵĵ�
			}
		}
		free( nbrs );
		nbrs=NULL;
	}	
	kdtree_release( kd_root );
	/*
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("kdtree��ʱ % lf",duration);
	*/

	/*
	printf(" kdtree ƥ�䣺 %d \n ",m2 );
	display_big_img( tempstackedTemp, "sencond_Matches" ); // ������
	cvWaitKey(0);
		/*		/*MachedFeatPoint ** feat3Points
			*feat3Points=calloc(m,sizeof(MachedFeatPoint));
			*feat3Points=cvCvtSeqToArray(featseq,*feat3Points,CV_WHOLE_SEQ);
		���� ��ת ���飬ֱ��seq ��ȥ����֮��Ҳseq����
			cvReleaseMemStorage( &storage );	
		*/


	cvReleaseMat(&resultMatrix);
//�޸�Ϊ�����	cvReleaseImage(&stackedTemp);
//�޸�Ϊ�����	stackedTemp=NULL;
	return m;
}


int compareFearture( struct feature* feat1, struct feature* feat2, 
	int n1,int n2,IplImage* img1,IplImage* stacked,CvPoint pointWindow1,CvPoint pointWindow2)
{//������������������ ��ƥ�����������ͼ
		//�޸ģ� �� feat1 ������ 
		 
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
	for( i = 0; i < n2; i++ )//ʹ��treeƥ�䣿 ��������㷨
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
				  cvLine( stackedTemp, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );//ƥ�䵽�ľͻ���
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
	//Sleep(500);�����ʹ��showimge��Ч
	//cvWaitKey(50);
	cvReleaseImage(&stackedTemp);
	stackedTemp=NULL;

	return m;


}

int simOfFearture(int n1,int n2,int m,
	double huiduImag1,double  huiduImag2,
	double * tempsim)
{	//�ж�ƥ������ƶȣ� @=(AB)/max( A ,B )
	/*
		���A�Ǻܴ��ͼ��A�ͺܴ� B����С���ͼ�꣬���ԣ���ʹB�е�ȫ��ƥ�䵽��@Ҳ��С��
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

		*tempsim=temp/temp_SIGOLIKELY;//���ص����ƶ�

		if ( temp >  temp_SIGOLIKELY *BIG_SIGOLIKELY  )
		{
		//	logSigmaCompare( m, temp/temp_SIGOLIKELY  );
			return 100;//�������� ��ֱ������ ���ƥ�䣬���� ���ƥ�� ������ ����logoƥ��
		}
		else if ( temp > temp_SIGOLIKELY ){
		//	logSigmaCompare( m, temp/temp_SIGOLIKELY  );
			return 90;//һ�� ������ ��ֱ������ ���ƥ�䣬���� ���ƥ�� ��
		}
		//ֻҪ��temp_SIGOLIKELY  ��Ҫͣ��

	}
	return 0;
}