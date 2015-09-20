#include "mach_index.h"

void printIndex(PRNearNeighborStructT nnStruct)
{
	FILE *file=fopen("nnstruct.txt","w");


	fprintf(file,"����� :%d  \n ",nnStruct->nPoints);

	for (int i=0 ;i< nnStruct->nHFTuples ; i++)
	{
	fprintf(file," ��%d ����  %d��Ͱ \n ",i,nnStruct->hashedBuckets[i]->nHashedBuckets );
	   HybridChainEntryT *hybridChainsStorage= nnStruct->hashedBuckets[i]->hybridChainsStorage;
	 
		//  for ( int j=0; j<nnStruct->hashedBuckets[i]->nHashedBuckets ;j++)
	    for ( int j=0; j<nnStruct->hashedBuckets[i]->nHashedBuckets ;j++)
		  {
			
			  if ( hybridChainsStorage!= NULL)
			  {
				 fprintf(file,"   %d ��Ͱ : %d �� \n ",j,hybridChainsStorage->point.bucketLength) ;
			  }
			  hybridChainsStorage++;
		  }

	  }
/*	
	for (int i=0; i< n; i++)
	{
		for (int j=0; j< NUMBERFORDIVISIOM ; j++)
		{
			fprintf(file,"%lf ",feat[i].descr[j]);
		}

	}

	*/
	fprintf(file,"\n");

	fclose(file);

}
void ouputPictureData( struct feature *feat,  int n)
{
	FILE *file=fopen("output_feat.txt","w");

	for (int i=0; i< n; i++)
	{
		for (int j=0; j< NUMBERFORDIVISIOM ; j++)
		{
			fprintf(file,"%lf ",feat[i].descr[j]);
		}
	
	}
	fprintf(file,"\n");

fclose(file);
}




void  outputOrignalData( struct collectionPointsOfIndex* collectionOfIndexP,
	double thresholdR,	//1�������е��ƽ������ /3=thresholdR
	IntT nSampleQueries,
	PPointT *(sampleQueries))
{
	FILE *file= fopen("index.txt", "w") ;

	for (int i=0; i< collectionOfIndexP->numberOfindex;  i++)
	{
		for (int j=0; j< NUMBEROF_CENG ; j++)
		{
			for (int k=0; k<NUMBERFORDIVISIOM ;k++)
			{
				fprintf(file,"%lf ", collectionOfIndexP->arrayGrayIndex[i].point[k]);
			}			
		}
		fprintf(file,"\n");
	}
	fclose(file);

	FILE *param= fopen("param.txt", "w") ;
	fprintf(param,"\n\n%d ", nSampleQueries);
	for (int i=0; i< nSampleQueries;  i++)
	{

			for (int k=0; k<NUMBERFORDIVISIOM ;k++)
			{
				fprintf(param,"%lf ", sampleQueries[i]->coordinates[k]);
			}			

		fprintf(param,"\n");
	}


	fprintf(param,"\n\n%lf ", thresholdR);//sampleQueries->coordinates[k]);

	fclose(param);
}



void myCaculateIndex(int numberOfLogo,int NUMBEROF_CENG,
struct logoFeatStruct*** logofeat,
	PRNearNeighborStructT nnStruct[GRAY_INDEX_NUMBER],
	 struct collectionPointsOfIndex collectionOfIndex[GRAY_INDEX_NUMBER]
)
{//Ҫ������Ҷ�����+ lsh�����ṹ

	//�Ҷ�����
	CvMat*  grayMatrix[GRAY_INDEX_NUMBER];//���ڱ���Ҷȵ� 
	struct indexAndClassOfGray *tempgrayIndex[GRAY_INDEX_NUMBER];//ÿ���ҶȾ����Ӧ��index

	 featToGrayIndex( numberOfLogo, NUMBEROF_CENG, logofeat, grayMatrix,tempgrayIndex);

	// struct collectionPointsOfIndex collectionOfIndex[GRAY_INDEX_NUMBER];//���վ���Ľ��
 
	//struct arrayOfGrayIndex *arrayGrayIndex [GRAY_INDEX_NUMBER];//���վ����Ľ������
	//struct collectionPointsOfIndex collectionOfIndex[GRAY_INDEX_NUMBER];//���վ����Ľ������
	 //���� 
	myClasserOfNN(  grayMatrix,tempgrayIndex,collectionOfIndex);
	
	for (int i=0;i <GRAY_INDEX_NUMBER  ;i++)
	{
		cvReleaseMat(&grayMatrix[i]);
		free(tempgrayIndex[i]);
	}

	printf("�������\n");
	 //lsh��������

//	PRNearNeighborStructT nnStruct[GRAY_INDEX_NUMBER];//���������Ľ��
	//���뵼�����ݲ��֣� ���ڲ���
	//outputOrignalData(&collectionOfIndex[0]);

	for (int i=0;i <GRAY_INDEX_NUMBER  ;i++)
	{
		myCaculateNNstruct(&collectionOfIndex[i], nnStruct[i]);
	}
	//�ͷ� colllectionOfindex


	 
}

void releaseCollectionPointsOfIndex(struct collectionPointsOfIndex *collectionOfIndex)
{//�ͷ�? �����ͷ�
	for (int i=0;i<GRAY_INDEX_NUMBER ;i++)
	{
		for ( int j=0;  j<collectionOfIndex[i].numberOfindex; j++)
		{
			;
		}
	}
}
void  copytoPPoint(double *data,double *point,int division)
{//�㸴��
	for (int i=0;i< division  ;i++ )
	{
		data[i]= point[i];
	}
}

double  caculateDistance(double *data,double *point,int division)
{
	double tempR=0.0;

	for (int i=0 ; i<division ; i++)
	{
		tempR+=(data[i] - point[i])*(data[i] - point[i]) ;
	}
	tempR= sqrt(tempR);
	return tempR;
}
void initDataForLsh(struct collectionPointsOfIndex *collectionOfIndex,
double &thresholdR,	//1�������е��ƽ������ /3=thresholdR
PPointT *(&dataSet),//2�������ݵ�dataset
IntT nSampleQueries,
PPointT *(&sampleQueries)//3���������ѯ��
)
{//�� colllection�ṹ��תΪlsh��ʼ����Ҫ������
	//1�������е��ƽ������ /3=thresholdR
	double tempR=0.0;

	for (int i=0; i < collectionOfIndex->numberOfindex -1 ; i++ )
	{
		for (int j=i+1; j<  collectionOfIndex->numberOfindex ; j++)
		{
			tempR+=caculateDistance(collectionOfIndex->arrayGrayIndex[i].point,
				collectionOfIndex->arrayGrayIndex[j].point,
				NUMBERFORDIVISIOM);
		}
	}
	//���޸ĵ���ֵ
	int temp_numberOfindex=collectionOfIndex->numberOfindex;
	thresholdR=tempR / ((temp_numberOfindex  -1)*(temp_numberOfindex ) /2);
	//��ʱȡֵ ƽ�������һ��
	//�����logo��ƽ��� ʵ�ʵ��أ���


	//2�������ݵ�dataset
	dataSet=(PPointT *) malloc(sizeof(PPointT) * collectionOfIndex->numberOfindex );
	//�������˰ɣ������typdef  {} * PPPoint. ����������Ƕ���ָ����
	for (int i=0;i< collectionOfIndex->numberOfindex; i++)
	{
		dataSet[i]=(PPointT )malloc(sizeof(PointT));
		dataSet[i]->coordinates=(double *) malloc(sizeof(double) *NUMBERFORDIVISIOM);
		copytoPPoint( dataSet[i]->coordinates,collectionOfIndex->arrayGrayIndex[i].point 
			,NUMBERFORDIVISIOM);
		dataSet[i]->index=i;
	}
	
	sampleQueries=(PPointT *) malloc(sizeof(PPointT)*nSampleQueries );
	//3���������ѯ��
	for (int j=0;j< nSampleQueries; j++)
	{
		sampleQueries[j]=(PPointT )malloc(sizeof(PointT));

		sampleQueries[j]->coordinates=(double *) malloc(sizeof(double) *NUMBERFORDIVISIOM);

		int temp=genRandomInt(0,  collectionOfIndex->numberOfindex-1);
		//int temp= rand()% ( collectionOfIndex->numberOfindex);
		copytoPPoint(  sampleQueries[j]->coordinates, collectionOfIndex->arrayGrayIndex[temp].point  
			,NUMBERFORDIVISIOM);
	}

}

void  myCaculateNNstruct(struct collectionPointsOfIndex *collectionOfIndex,
	PRNearNeighborStructT &nnStruct)
{// ����� lsh�������ṹ
		double thresholdR=1.0;//�����ֵ��
		RealT successProbability=0.9;
		Int32T nPoints = collectionOfIndex->numberOfindex;
		IntT dimension =NUMBERFORDIVISIOM;
		PPointT *dataSet;
		IntT nSampleQueries=50  ; 
		PPointT *sampleQueries;
		MemVarT memoryUpperBound=1024*1024*1000L;//�ڴ��С���Ż���

		if ( nSampleQueries> nPoints)
		{
			 nSampleQueries  = nPoints;
		}

		initDataForLsh(collectionOfIndex,
			thresholdR,	//1�������е��ƽ������ /3=thresholdR
			dataSet,//2�������ݵ�dataset
			nSampleQueries,
			sampleQueries//3���������ѯ��
			);
	
		//����
		outputOrignalData ( collectionOfIndex,thresholdR,
			 nSampleQueries,
			(sampleQueries));

		nnStruct=initSelfTunedRNearNeighborWithDataSet(
			thresholdR, 
			successProbability, 
			nPoints, 
			dimension, 
			dataSet, 
			nSampleQueries, 
			sampleQueries, 
			memoryUpperBound);

		free(sampleQueries);//��ѯ�� û����

}


void  myClasserOfNN(CvMat*  grayMatrix[GRAY_INDEX_NUMBER],//�������ص�ֵ
struct indexAndClassOfGray *tempgrayIndex[GRAY_INDEX_NUMBER],
//struct arrayOfGrayIndex *arrayGrayIndex [GRAY_INDEX_NUMBER]
struct collectionPointsOfIndex  collectionOfIndex[GRAY_INDEX_NUMBER]
)
{ //����  ������浽�� arrayGrayIndexָ�����飺�������ĵ�� ÿ�����Ĵ���ĵ�����
	//ʹ�������+һ����෨
	int i;
	for (i=0;i< GRAY_INDEX_NUMBER;i++)
	{
		myClasserOfNN_one(grayMatrix[i],tempgrayIndex[i],collectionOfIndex[i]);
	}
}


void myGetTwoBigInMatrix(CvMat* resultMatrix,int rows,
	double &max1,double &max2,int &bigcols)//������ֵ+���ֵ������
{
	//��ѯ�����һ�У����� ��һ�ڶ�������飬
		double tempmax=0.0;
		int tempmi=0;
		
		/*double a= CV_MAT_ELEM(*resultMatrix,resultMatrix->type,rows,0);
		//Ϊ�����ַ�����ʦ������????

		*max1=  CV_MAT_ELEM(*resultMatrix,resultMatrix->type,rows,0);
		*max2=  	CV_MAT_ELEM(*resultMatrix,resultMatrix->type,rows,1);
		*/

		double * ptr= (double *)(resultMatrix->data.ptr+resultMatrix->step *rows);
	
		max1= *ptr++;
		max2=*ptr++;

		if (max1 < max2)
		{
			tempmax=max1;
			max1=max2;
			max2=tempmax;
			tempmi=1;//������ֵ��index
		}	
	for (int cols=2;cols <resultMatrix->cols ;cols++)
	{
		//tempmax= 	CV_MAT_ELEM(*resultMatrix,resultMatrix->type,rows,cols);
		tempmax=*ptr++;
			if ( tempmax >= max1)
			{
				max2=max1;
				max1=tempmax;
				tempmi=cols;
			}
			else if ( tempmax > max2)
			{
				max2=tempmax;
			}
	}
	bigcols=tempmi;
}

int  myGetNearNeibor(CvMat* resultMatrix,int rows,
int &bigcols)//����ڵ�����
{//ͨ�����о�����������ֵ��
	//Ȼ���ж��Ƿ�������ڵ�
	//1��ʾ �ǵģ� 0��ʾ����
	double max1,max2;

	myGetTwoBigInMatrix( resultMatrix, rows,max1,max2,bigcols);

	//�жϱ�ֵ ����ֵ��
	if (  (max1 > YUZHI_COS_MAX )  || 
		(  ( max1 > YUZHI_COS_MIN ) &&  ( ((1-max1) < (1- max2)*  0.7 )  ))//YUZHI_COS
		 ) 
	{ 
		return 1;
	}
	return 0;

}

void guibing(struct indexAndClassOfGray* tempgrayIndex,int number,
	int c1,int c2)
{//����c1 ��c2  ȫ���鲢����С�����
		
	if( tempgrayIndex[c1].classvalue  == tempgrayIndex[c2].classvalue  )
	{//���
			return;
	}

	if( tempgrayIndex[c1].classvalue > tempgrayIndex[c2].classvalue  )
	{
		for (int i=0; i< number; i++ )
		{
			if(tempgrayIndex[i].classvalue== tempgrayIndex[c1].classvalue )
			{
				tempgrayIndex[i].classvalue= tempgrayIndex[c2].classvalue;

			}
		}
	}
	else if( tempgrayIndex[c1].classvalue< tempgrayIndex[c2].classvalue  )
	{
		for (int i=0; i< number; i++ )
		{
			if(tempgrayIndex[i].classvalue== tempgrayIndex[c2].classvalue )
			{
				tempgrayIndex[i].classvalue= tempgrayIndex[c1].classvalue;

			}
		}
	}

}

void  myClasserOfNN_one(CvMat*  grayMatrix,//�������ص�ֵ
struct indexAndClassOfGray* tempgrayIndex,
struct collectionPointsOfIndex &collectionOfIndex //	struct arrayOfGrayIndex  *  (&arrayGrayIndex) )//ָ���Ӧ��
)
//struct seqIndexAndClassOfGray *grayIndex)
{ //���������ľ���
//��һ���� ԭ����� cosֵ����

	CvMat *TransposedMutilValue=  cvCreateMat(grayMatrix->rows,grayMatrix->rows,grayMatrix->type);
//	cvMulTransposed(grayMatrix,TransposedMutilValue, 0);//����ת�ú�ĳ˻�
	//����û�м��� cos��  Ҫ a*b  /|a|��|b|
	
	myCaculateSelfCosValue(grayMatrix ,TransposedMutilValue  );

	 

	//��ʼ���� �Խ���ֵΪ0�� ����ֵΪ�Լ�
	for(int r=0;r< TransposedMutilValue->rows; r++)
	{
		double * ptr= (double *)(TransposedMutilValue->data.ptr+TransposedMutilValue->step *r);
		 ptr+=r;
		*ptr=0.0;
		tempgrayIndex[r].classvalue = r;
	}

	for(int r=0;r< TransposedMutilValue->rows; r++)
	{
		double * ptr= (double *)(TransposedMutilValue->data.ptr+TransposedMutilValue->step *r);
	
		for(int c=r+1; c<TransposedMutilValue->cols; c++)
		{//�� 0��ʼ����ɨ�� ���ظ�
			if ( c==r)
			{
				continue;
			}
			if (  ptr[c] >YUZHI_COS_MAX)
			{
				if (tempgrayIndex[c].classvalue ==c )
				{	//δ�����			
					tempgrayIndex[c].classvalue =tempgrayIndex[r].classvalue;
				}
				else
				{//�Ѿ�����ģ�Ҫ�ϲ�������
					guibing(tempgrayIndex,grayMatrix->rows, c,r  );
				}
			}
		}
	}

//�ڶ��������е��Ѿ�����ĵ��� ��С��Ŵ���

	for(int c=0; c<TransposedMutilValue->cols; c++)
	{
		if (tempgrayIndex[c].classvalue  != c )
		{//�Ѿ�����ĵ� ���� ����0���� 
			for(int r=0;r< TransposedMutilValue->rows; r++)
			{
				*((double * )CV_MAT_ELEM_PTR(*TransposedMutilValue,r,c))
					=0.0;
			}
		}		
	}

//������������ڵ� ����
	int bigcols;
	int isnearneibor=0;
	for(int r=0;r< TransposedMutilValue->rows; r++)
	{
		 isnearneibor= myGetNearNeibor(TransposedMutilValue, r,bigcols);
			 //���� ÿһ�е� ��ֵ+���ֵ������
		if ( isnearneibor)
		 {

			// if (tempgrayIndex[bigcols].classvalue ==bigcols )
		//	 {	//δ�����	������		
				//tempgrayIndex[bigcols].classvalue=r;//��ʱ����������
			// }
		//	 else
		//	 {//�Ѿ�����ģ�Ҫ�ϲ�������
				 guibing(tempgrayIndex,grayMatrix->rows, bigcols,r  );
			 //}
					
		 }
	}

//���Ĳ��������������ͳһ�����ĵ㣬
	//1��ͳ���ж��ٸ���
	int classnumber=0;
	for (int i=0; i< grayMatrix->rows; i++)
	{
		if ( 	tempgrayIndex[i].classvalue ==i)
		{
			classnumber++;
		}
	}


	 struct arrayOfGrayIndex  *   arrayGrayIndex;// =&collectionPointsOfIndex.arrayGrayIndex;
	//2������grayIndex �ṹ�壺�ռ�
	arrayGrayIndex = (struct arrayOfGrayIndex* )
		malloc (sizeof(struct arrayOfGrayIndex )*classnumber);
//	arrayGrayIndex->pointsInClass=classnumber;//�ܸ���

	int numberOfClass=0;
	for(int r=0;r< TransposedMutilValue->rows; r++)
	{
		if (tempgrayIndex[r].classvalue  == r)
		{//һ����ĵ�һ��Ԫ��

			//��ʼ��������ͷ
			arrayGrayIndex[numberOfClass].index=
				(struct seqIndexAndClassOfGray  *)malloc(sizeof(struct seqIndexAndClassOfGray  ));
			arrayGrayIndex[numberOfClass].index->	grayindex=tempgrayIndex[r];
			arrayGrayIndex[numberOfClass].index->	next=NULL;
			for (int j=0;j< NUMBERFORDIVISIOM; j++)
			{//���ĳ�ʼ��
				//������ r�и��Ƶ�point
				arrayGrayIndex[numberOfClass].point[j] = 
					CV_MAT_ELEM(*grayMatrix,double,numberOfClass,j ); 
			}
			int numberOfthisClass=1;//�������ĸ���

			struct seqIndexAndClassOfGray *tempstructOfclassGray=
				arrayGrayIndex[numberOfClass].index;

			for(int kk=r+1;kk< TransposedMutilValue->rows; kk++)
			{//��������� ����
				if (tempgrayIndex[kk].classvalue ==r)
				{//֮������е� �ļ�������

					//������ ������ͬһ����  ֱ�ӽṹ����Ч������ͬһ����
					//��Ϊ��̬����ͺ��˶�
					/*
				struct seqIndexAndClassOfGray	tempstructIndex;
				tempstructIndex.grayindex= tempgrayIndex[kk];
				tempstructIndex.next=NULL;
					tempstructOfclassGray->next=&tempstructIndex;
				tempstructOfclassGray =&tempstructIndex;
				*/
					struct seqIndexAndClassOfGray	*tempstructIndex=
						(struct seqIndexAndClassOfGray	*)malloc(sizeof(struct seqIndexAndClassOfGray));
					tempstructIndex->grayindex= tempgrayIndex[kk];
					tempstructIndex->next=NULL;
					tempstructOfclassGray->next=tempstructIndex;
					tempstructOfclassGray =tempstructIndex;

				////������ kkk�мӵ�point

				for (int j=0;j< NUMBERFORDIVISIOM; j++)
				{
					arrayGrayIndex[numberOfClass].point[j] += 
						CV_MAT_ELEM(*grayMatrix,double,kk,j );//grayMatrix->data+grayMatrix->step* ;
				}

				numberOfthisClass++;//������ĵ���+1

				}
			}
			arrayGrayIndex[numberOfClass].pointsInClass=numberOfthisClass;

			//3���������ĵ㣻
			for (int j=0;j< NUMBERFORDIVISIOM; j++)
			{
				arrayGrayIndex[numberOfClass].point[j]/=numberOfthisClass;
			}
			numberOfClass++;//������+1
		}		
	}

	cvReleaseMat(&TransposedMutilValue);
	collectionOfIndex.arrayGrayIndex=arrayGrayIndex;
	collectionOfIndex.numberOfindex=numberOfClass;
}


void  featToGrayIndex(int numberOfLogo,int NUMBEROF_CENG,
struct logoFeatStruct*** logofeat,
CvMat*  grayMatrix[GRAY_INDEX_NUMBER],//�������ص�ֵ
struct indexAndClassOfGray *grayIndex[GRAY_INDEX_NUMBER]
)
{//��� feat�� index��ӳ��
	//��󷵻ػҶ�����
//	CvMat*  grayMatrix[GRAY_INDEX_NUMBER];//���ڱ���Ҷȵ� 
	//struct indexAndClassOfGray *grayIndex[GRAY_INDEX_NUMBER];//ÿ���ҶȾ����Ӧ��index

	struct feature* feat1 ;
	int n1, k, i, m = 0;
	int logoNumber;//�ڼ���logo

	struct FeatIdCeng indexpoint;

	CvSeq * tempIndexOfGray[GRAY_INDEX_NUMBER];
	CvMemStorage* storage[GRAY_INDEX_NUMBER] ;


 
	int tempnum=0;

	for (i=0;i<GRAY_INDEX_NUMBER ; i++)
	{
		storage[i] = cvCreateMemStorage( 0 );
		tempIndexOfGray[i]=cvCreateSeq
			(0,sizeof(CvSeq),sizeof(struct FeatIdCeng),storage[i]);
	}	


	for(logoNumber = 0; logoNumber< numberOfLogo ;logoNumber++)
	{//�̵߳�ÿһ��ͼ

		for (i=0;i<NUMBEROF_CENG   ;i++)
		{//����	
			feat1=(*logofeat)[logoNumber][i].feat1;
			n1=(*logofeat)[logoNumber][i].numberOffeat;
			for (k=0; k <n1; k++)
			{//ÿ���㴦��
				//�Ҷ���������Ӧseq������	
				tempnum=feat1[k].meanOfgray/GRAY_INDEX_PICE;
				indexpoint.logonumber=logoNumber;
				indexpoint.ceng=i;
				indexpoint.featorder=k;
				indexpoint.feat=feat1[k];

				if (tempnum >0 && 
					feat1[k].meanOfgray %GRAY_INDEX_PICE <2)//��߽�
				{
					cvSeqPush( tempIndexOfGray[tempnum-1 ],&indexpoint);
				}
				else if (tempnum < GRAY_INDEX_NUMBER-1 && 
					feat1[k].meanOfgray %GRAY_INDEX_PICE >GRAY_INDEX_PICE-3 )
				{
					cvSeqPush(tempIndexOfGray[tempnum+1] ,&indexpoint );
				}

				cvSeqPush(tempIndexOfGray[tempnum], &indexpoint);
			}
		}//ÿһ��
	}//ÿһ��ͼ

	//ÿ��seqת matrxi
	for (i=0;i<GRAY_INDEX_NUMBER ; i++)
	{
	//����Ҫ����gray���� + ��������
		seqToMatrix(tempIndexOfGray[i],&grayMatrix[i],&grayIndex[i],tempIndexOfGray[i]->total);
	}

	//�ͷ�
	for (i=0;i<GRAY_INDEX_NUMBER ; i++)
	{
		cvReleaseMemStorage(&storage[i]);	
	}		
}

/*
void searchInGrayIndex(struct indexAndClassOfGray *grayIndex[GRAY_INDEX_NUMBER],struct feature   feat)
{
}*/



void seqToMatrix(CvSeq * tempOfGray,
	CvMat **grayOffeatMatrix ,struct indexAndClassOfGray **grayIndex,int numberforfeats)
{//��struct FeatIdCeng �ṹת����matrix �ϣ���������ÿ�еĵ��Ӧ������ֵ����grayindex

	struct FeatIdCeng* indexpoint;
	int i,j,k,m;		

	//��һ����feat���飬ת��Ϊ����
	struct feature *feat1;
 
	*grayIndex=(struct indexAndClassOfGray *) malloc(sizeof(struct indexAndClassOfGray )*numberforfeats );
	*grayOffeatMatrix=cvCreateMat(numberforfeats,NUMBERFORDIVISIOM,CV_64FC1);


	//���������㵽����vector��
		for (k=0 ;k< numberforfeats ;k++)
			{
				struct FeatIdCeng * temp =( (struct FeatIdCeng *)cvGetSeqElem(tempOfGray,k));

			feat1=&	temp->feat;
			/*	feat1= &( (struct FeatIdCeng *)
					cvGetSeqElem(tempOfGray,k)) ->feat;//������������е�ָ��
					*/

				for (m=0;m< NUMBERFORDIVISIOM;m++)
				{
					CV_MAT_ELEM(**grayOffeatMatrix,double,k ,m) =
						(double ) (*feat1).descr[m];		
				}		
				(*grayIndex)[k].ceng=temp->ceng;//��Ŀ�����ҽ��   ������ ���� �����ǡ���. -> ()�ȵ�Ŀ��Ҫ�߼�
				(*grayIndex)[k].classvalue=k;
				(*grayIndex)[k].logoid=temp->logonumber;
				(*grayIndex)[k].featorder=temp->featorder;
		}
}


void buidLogoSeq(int numberOfLogo,int NUMBEROF_CENG,
CvSeq *** (&featseq),
CvMemStorage*** (&storage),
int **(&StatisticsOfPoins)
)
{//����һ��ͳ��logoÿ�� ÿ�� ƥ��ĵ�Ե� �����
	//���� ͳ�Ƶ�int2ά����

	StatisticsOfPoins=(int **) malloc(sizeof(int *) *numberOfLogo);
	for (int i=0; i< numberOfLogo ;i++ )
	{
		StatisticsOfPoins[i]= (int *) malloc(sizeof(int ) *NUMBEROF_CENG);
		for (int j=0; j< NUMBEROF_CENG ;j++)
		{
			StatisticsOfPoins[i][j]= 0;
		}
	}


	storage =(CvMemStorage***  ) malloc( sizeof (CvMemStorage** ) * numberOfLogo);
	featseq = ( CvSeq *** ) malloc(sizeof (CvSeq **) *numberOfLogo );
	for (int i=0; i< numberOfLogo ; i++)
	{
		storage[i]=(CvMemStorage**  ) malloc( sizeof (CvMemStorage* ) * NUMBEROF_CENG);
		featseq[i] = ( CvSeq ** ) malloc(sizeof (CvSeq *) *NUMBEROF_CENG );

		for ( int j=0; j<NUMBEROF_CENG ; j++)
		{

			storage[i][j]=cvCreateMemStorage( 0 );
			featseq[i][j] = cvCreateSeq(0,sizeof(CvSeq),sizeof(MachedFeatPoint),storage[i][j]);
		}
	}
}



void releaseLogoSeq(int numberOfLogo,int NUMBEROF_CENG,
	CvSeq *** (&featseq),
	CvMemStorage*** (&storage),
	int **(&StatisticsOfPoins)
	)
{//����һ��ͳ��logoÿ�� ÿ�� ƥ��ĵ�Ե� �����
	//���� ͳ�Ƶ�int2ά����

	for (int i=0; i< numberOfLogo ;i++ )
	{
		free(StatisticsOfPoins[i] ); 
	}
	free(StatisticsOfPoins);

	for (int i=0; i< numberOfLogo ; i++)
	{
		for ( int j=0; j<NUMBEROF_CENG ; j++)
		{
			cvReleaseMemStorage(&storage[i][j]);
		}
		free(featseq[i]);
		free(storage[i]);
	}
	free(featseq);
	free(storage);

}



void getMaxRatioForMach(	int numberOfLogo,int NUMBEROF_CENG,
struct logoFeatStruct** logofeat,
int **StatisticsOfPoins,
CvSeq ***featseq,
int *maxarry,int &maxSize 	)
{//����ƥ����� ����ֵ����ǰmaxsize����//��ÿ��� ƥ����� �����ܵ���  
	//���ص�������

	for (int k=0 ; k< maxSize ;k++)
	{
		maxarry[k]=-1;
	}
	//��ð�ݷ�+ ͬʱ����ֵ�����
	double **indexAndValue;//2ά���飬���� ��+���
	indexAndValue=(double **)malloc( sizeof(double* ) *NUMBEROF_CENG* numberOfLogo );
	for (int k=0; k< NUMBEROF_CENG* numberOfLogo ; k++)
	{
		indexAndValue[k]=(double *)malloc(sizeof(double ) * 2) ;
		//indexAndValue[k][0]=k;
		//indexAndValue[k][1]=0.0;//��ʼ��Ϊ0.0
	}

	int numberOfratio=0;
	for (int i=0; i< numberOfLogo ; i++)
	{
		for ( int j=0; j<NUMBEROF_CENG ; j++)
		{
			if ( logofeat[i][j].numberOffeat >0 )
			{		
				double tempratio=((double)(  StatisticsOfPoins[i][j]) )/ logofeat[i][j].numberOffeat ;
				if (tempratio  >0.05)
				{
					int m=featseq[i][j]->total;
				//���Բ�Ҫ����	reduceMachedPoints(featseq[i][j],&m);//Լ��
					tempratio=((double) m )/ logofeat[i][j].numberOffeat ;
					indexAndValue[numberOfratio][1]=tempratio;//��ֵ
					indexAndValue[numberOfratio][0]=numberOfratio;//���
					numberOfratio++;
				}	
			}		
		}
	}

	if ( maxSize >numberOfratio)
	{//������С��
		maxSize=numberOfratio;
	}
	for (int i=0; i< maxSize; i++)
	{
		for (int j=0; j< numberOfratio-1-i ;j++)
		{//��ķź���  ֻ��maxSize ��
			if (indexAndValue[j][1] > indexAndValue[j+1][1] )
			{
				//swap
				double temp1= indexAndValue[j][0];
				double temp2= indexAndValue[j][1];
				 indexAndValue[j][0]= indexAndValue[j+1][0];
				 indexAndValue[j][1]= indexAndValue[j+1][1];
				 indexAndValue[j][0]=temp1;
				 indexAndValue[j][1] =temp2;
			}
		}
		//�ŵ����
		maxarry[i]=indexAndValue[ numberOfratio-1-i][0];//��� ���Ҫ����NUMBEROF_CENG =logoid 
	}

	for (int k=0; k< NUMBEROF_CENG* numberOfLogo ; k++)
	{
		free(indexAndValue[k]);
	}
	free(indexAndValue);

}



struct arrayOfGrayIndex*   mySearchInIndex(PRNearNeighborStructT *nnStruct,
struct feature   feat,struct collectionPointsOfIndex *collectionOfIndex
)
{//��feat�㣬 ��ѯ�����е�ֵ  ����struct arrayOfGrayIndex 

	//�Ȳ�ѯ�Ҷ�����
	int grayi=feat.meanOfgray/ GRAY_INDEX_PICE;
	
	
	Int32T resultSize=3;
	PPointT *result = (PPointT*)malloc(resultSize * sizeof(*result));

	PPointT queryPoint;
	FAILIF(NULL == (queryPoint = (PPointT)malloc(sizeof(PointT))));
	FAILIF(NULL == (queryPoint->coordinates = (RealT*)malloc(NUMBERFORDIVISIOM * sizeof(RealT))));
	// read in the query point.
    for(IntT d = 0; d < NUMBERFORDIVISIOM ; d++)
	{
      queryPoint->coordinates[d] =feat.descr[d];
    }
    queryPoint->sqrLength = 0;

	
    IntT nNNs = 0;
	//����lsh����
	nNNs=getRNearNeighbors(nnStruct[grayi], queryPoint, result, resultSize);

	if(nNNs <=0)
	{
		return NULL;
	}
	//���������   ������������ĵ� coolecion���
	int id=zuiJingling( queryPoint, result, nNNs);
	//һ���㴦��
	if (id< 0)
	{
		return NULL;
	}

	//����colection�е�����
	struct arrayOfGrayIndex* tempresult=&
		(collectionOfIndex[grayi].arrayGrayIndex[ result[id]->index ]);


 return tempresult;
}

int zuiJingling(PPointT queryPoint,PPointT *result ,int resultSize)
{//��pPoint�������ҳ� ����������  -1��ʾû�� ��������ʾ���
 #define NN_SQ_DIST_RATIO_THR 0.49
	if (resultSize <=1)
	{
		return 0;
	}
	double  tempdistance;
	int m1=0,m2=1,mt;
	double mindistance1=0.0;
	double mindistance2=0.0;
	mindistance1=caculateDistance( queryPoint->coordinates,result[0]->coordinates,NUMBERFORDIVISIOM );
	mindistance2=caculateDistance( queryPoint->coordinates,result[1]->coordinates,NUMBERFORDIVISIOM );

	if (mindistance1  >mindistance2)
	{
		tempdistance=mindistance2;
		mindistance2=mindistance1;
		mindistance1=tempdistance;
		 
		mt=m1;
		m1=m2;
		m2=mt;
	}

	for (int i=2  ;i<  resultSize; i++)
	{
		tempdistance=caculateDistance( queryPoint->coordinates,result[i]->coordinates,NUMBERFORDIVISIOM );
	
		if (tempdistance  < mindistance1)
		{
			mindistance2=mindistance1;
			m2=m1;
			mindistance1=tempdistance;
			m1=i;
		}
		else if (tempdistance  < mindistance2)
		{
			mindistance2=tempdistance;
			m2=i;
		}
	}

	if ( mindistance1 < mindistance2)//* 0.8
	{
		return m1;
	}
	return -1;

}