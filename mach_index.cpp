#include "mach_index.h"

void printIndex(PRNearNeighborStructT nnStruct)
{
	FILE *file=fopen("nnstruct.txt","w");


	fprintf(file,"点个数 :%d  \n ",nnStruct->nPoints);

	for (int i=0 ;i< nnStruct->nHFTuples ; i++)
	{
	fprintf(file," 第%d 个表  %d个桶 \n ",i,nnStruct->hashedBuckets[i]->nHashedBuckets );
	   HybridChainEntryT *hybridChainsStorage= nnStruct->hashedBuckets[i]->hybridChainsStorage;
	 
		//  for ( int j=0; j<nnStruct->hashedBuckets[i]->nHashedBuckets ;j++)
	    for ( int j=0; j<nnStruct->hashedBuckets[i]->nHashedBuckets ;j++)
		  {
			
			  if ( hybridChainsStorage!= NULL)
			  {
				 fprintf(file,"   %d 个桶 : %d 点 \n ",j,hybridChainsStorage->point.bucketLength) ;
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
	double thresholdR,	//1计算所有点的平均距离 /3=thresholdR
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
{//要计算出灰度索引+ lsh索引结构

	//灰度索引
	CvMat*  grayMatrix[GRAY_INDEX_NUMBER];//用于保存灰度的 
	struct indexAndClassOfGray *tempgrayIndex[GRAY_INDEX_NUMBER];//每个灰度矩阵对应的index

	 featToGrayIndex( numberOfLogo, NUMBEROF_CENG, logofeat, grayMatrix,tempgrayIndex);

	// struct collectionPointsOfIndex collectionOfIndex[GRAY_INDEX_NUMBER];//最终聚类的结果
 
	//struct arrayOfGrayIndex *arrayGrayIndex [GRAY_INDEX_NUMBER];//最终聚类后的结果保存
	//struct collectionPointsOfIndex collectionOfIndex[GRAY_INDEX_NUMBER];//最终聚类后的结果保存
	 //聚类 
	myClasserOfNN(  grayMatrix,tempgrayIndex,collectionOfIndex);
	
	for (int i=0;i <GRAY_INDEX_NUMBER  ;i++)
	{
		cvReleaseMat(&grayMatrix[i]);
		free(tempgrayIndex[i]);
	}

	printf("聚类完成\n");
	 //lsh索引建立

//	PRNearNeighborStructT nnStruct[GRAY_INDEX_NUMBER];//最终索引的结果
	//加入导出数据部分， 用于测试
	//outputOrignalData(&collectionOfIndex[0]);

	for (int i=0;i <GRAY_INDEX_NUMBER  ;i++)
	{
		myCaculateNNstruct(&collectionOfIndex[i], nnStruct[i]);
	}
	//释放 colllectionOfindex


	 
}

void releaseCollectionPointsOfIndex(struct collectionPointsOfIndex *collectionOfIndex)
{//释放? 不能释放
	for (int i=0;i<GRAY_INDEX_NUMBER ;i++)
	{
		for ( int j=0;  j<collectionOfIndex[i].numberOfindex; j++)
		{
			;
		}
	}
}
void  copytoPPoint(double *data,double *point,int division)
{//点复制
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
double &thresholdR,	//1计算所有点的平均距离 /3=thresholdR
PPointT *(&dataSet),//2复制数据到dataset
IntT nSampleQueries,
PPointT *(&sampleQueries)//3随机产生查询点
)
{//将 colllection结构体转为lsh初始化需要的数据
	//1计算所有点的平均距离 /3=thresholdR
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
	//新修改的阈值
	int temp_numberOfindex=collectionOfIndex->numberOfindex;
	thresholdR=tempR / ((temp_numberOfindex  -1)*(temp_numberOfindex ) /2);
	//暂时取值 平均距离的一半
	//这个是logo的平均差， 实际的呢？？


	//2复制数据到dataset
	dataSet=(PPointT *) malloc(sizeof(PPointT) * collectionOfIndex->numberOfindex );
	//看明白了吧，这里的typdef  {} * PPPoint. 所以这里就是二级指针了
	for (int i=0;i< collectionOfIndex->numberOfindex; i++)
	{
		dataSet[i]=(PPointT )malloc(sizeof(PointT));
		dataSet[i]->coordinates=(double *) malloc(sizeof(double) *NUMBERFORDIVISIOM);
		copytoPPoint( dataSet[i]->coordinates,collectionOfIndex->arrayGrayIndex[i].point 
			,NUMBERFORDIVISIOM);
		dataSet[i]->index=i;
	}
	
	sampleQueries=(PPointT *) malloc(sizeof(PPointT)*nSampleQueries );
	//3随机产生查询点
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
{// 计算出 lsh的索引结构
		double thresholdR=1.0;//计算均值？
		RealT successProbability=0.9;
		Int32T nPoints = collectionOfIndex->numberOfindex;
		IntT dimension =NUMBERFORDIVISIOM;
		PPointT *dataSet;
		IntT nSampleQueries=50  ; 
		PPointT *sampleQueries;
		MemVarT memoryUpperBound=1024*1024*1000L;//内存大小可优化？

		if ( nSampleQueries> nPoints)
		{
			 nSampleQueries  = nPoints;
		}

		initDataForLsh(collectionOfIndex,
			thresholdR,	//1计算所有点的平均距离 /3=thresholdR
			dataSet,//2复制数据到dataset
			nSampleQueries,
			sampleQueries//3随机产生查询点
			);
	
		//测试
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

		free(sampleQueries);//查询的 没用了

}


void  myClasserOfNN(CvMat*  grayMatrix[GRAY_INDEX_NUMBER],//两个返回的值
struct indexAndClassOfGray *tempgrayIndex[GRAY_INDEX_NUMBER],
//struct arrayOfGrayIndex *arrayGrayIndex [GRAY_INDEX_NUMBER]
struct collectionPointsOfIndex  collectionOfIndex[GRAY_INDEX_NUMBER]
)
{ //聚类  结果保存到了 arrayGrayIndex指针数组：包含中心点和 每个中心代表的点索引
	//使用最近邻+一般聚类法
	int i;
	for (i=0;i< GRAY_INDEX_NUMBER;i++)
	{
		myClasserOfNN_one(grayMatrix[i],tempgrayIndex[i],collectionOfIndex[i]);
	}
}


void myGetTwoBigInMatrix(CvMat* resultMatrix,int rows,
	double &max1,double &max2,int &bigcols)//返回最值+最大值的索引
{
	//查询矩阵的一行，返回 第一第二大的数组，
		double tempmax=0.0;
		int tempmi=0;
		
		/*double a= CV_MAT_ELEM(*resultMatrix,resultMatrix->type,rows,0);
		//为何这种方法老师出问题????

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
			tempmi=1;//标记最大值的index
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
int &bigcols)//最近邻的索引
{//通过单行矩阵，求两个最值，
	//然后判断是否是最近邻点
	//1表示 是的， 0表示不是
	double max1,max2;

	myGetTwoBigInMatrix( resultMatrix, rows,max1,max2,bigcols);

	//判断比值 》阈值？
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
{//将类c1 和c2  全部归并到最小序号上
		
	if( tempgrayIndex[c1].classvalue  == tempgrayIndex[c2].classvalue  )
	{//相等
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

void  myClasserOfNN_one(CvMat*  grayMatrix,//两个返回的值
struct indexAndClassOfGray* tempgrayIndex,
struct collectionPointsOfIndex &collectionOfIndex //	struct arrayOfGrayIndex  *  (&arrayGrayIndex) )//指针的应用
)
//struct seqIndexAndClassOfGray *grayIndex)
{ //单个索引的聚类
//第一步： 原矩阵的 cos值聚类

	CvMat *TransposedMutilValue=  cvCreateMat(grayMatrix->rows,grayMatrix->rows,grayMatrix->type);
//	cvMulTransposed(grayMatrix,TransposedMutilValue, 0);//计算转置后的乘积
	//这里没有计算 cos的  要 a*b  /|a|。|b|
	
	myCaculateSelfCosValue(grayMatrix ,TransposedMutilValue  );

	 

	//初始化： 对角线值为0， 索引值为自己
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
		{//从 0开始左到右扫描 会重复
			if ( c==r)
			{
				continue;
			}
			if (  ptr[c] >YUZHI_COS_MAX)
			{
				if (tempgrayIndex[c].classvalue ==c )
				{	//未分类的			
					tempgrayIndex[c].classvalue =tempgrayIndex[r].classvalue;
				}
				else
				{//已经分类的，要合并两个类
					guibing(tempgrayIndex,grayMatrix->rows, c,r  );
				}
			}
		}
	}

//第二步：所有的已经分类的点用 最小序号代替

	for(int c=0; c<TransposedMutilValue->cols; c++)
	{
		if (tempgrayIndex[c].classvalue  != c )
		{//已经分类的点 整列 都用0代替 
			for(int r=0;r< TransposedMutilValue->rows; r++)
			{
				*((double * )CV_MAT_ELEM_PTR(*TransposedMutilValue,r,c))
					=0.0;
			}
		}		
	}

//第三步：最近邻的 归类
	int bigcols;
	int isnearneibor=0;
	for(int r=0;r< TransposedMutilValue->rows; r++)
	{
		 isnearneibor= myGetNearNeibor(TransposedMutilValue, r,bigcols);
			 //返回 每一行的 最值+最大值的索引
		if ( isnearneibor)
		 {

			// if (tempgrayIndex[bigcols].classvalue ==bigcols )
		//	 {	//未分类的	或类首		
				//tempgrayIndex[bigcols].classvalue=r;//临时的数组索引
			// }
		//	 else
		//	 {//已经分类的，要合并两个类
				 guibing(tempgrayIndex,grayMatrix->rows, bigcols,r  );
			 //}
					
		 }
	}

//第四步：计算类个数，统一算中心点，
	//1、统计有多少个类
	int classnumber=0;
	for (int i=0; i< grayMatrix->rows; i++)
	{
		if ( 	tempgrayIndex[i].classvalue ==i)
		{
			classnumber++;
		}
	}


	 struct arrayOfGrayIndex  *   arrayGrayIndex;// =&collectionPointsOfIndex.arrayGrayIndex;
	//2、建立grayIndex 结构体：空间
	arrayGrayIndex = (struct arrayOfGrayIndex* )
		malloc (sizeof(struct arrayOfGrayIndex )*classnumber);
//	arrayGrayIndex->pointsInClass=classnumber;//总个数

	int numberOfClass=0;
	for(int r=0;r< TransposedMutilValue->rows; r++)
	{
		if (tempgrayIndex[r].classvalue  == r)
		{//一个类的第一个元素

			//初始化类数组头
			arrayGrayIndex[numberOfClass].index=
				(struct seqIndexAndClassOfGray  *)malloc(sizeof(struct seqIndexAndClassOfGray  ));
			arrayGrayIndex[numberOfClass].index->	grayindex=tempgrayIndex[r];
			arrayGrayIndex[numberOfClass].index->	next=NULL;
			for (int j=0;j< NUMBERFORDIVISIOM; j++)
			{//中心初始化
				//将矩阵 r行复制到point
				arrayGrayIndex[numberOfClass].point[j] = 
					CV_MAT_ELEM(*grayMatrix,double,numberOfClass,j ); 
			}
			int numberOfthisClass=1;//这组类点的个数

			struct seqIndexAndClassOfGray *tempstructOfclassGray=
				arrayGrayIndex[numberOfClass].index;

			for(int kk=r+1;kk< TransposedMutilValue->rows; kk++)
			{//类的其他点 加入
				if (tempgrayIndex[kk].classvalue ==r)
				{//之后的所有点 的加入链表

					//有问题 ，都是同一个点  直接结构体无效，都是同一个点
					//改为动态申请就好了额
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

				////将矩阵 kkk行加到point

				for (int j=0;j< NUMBERFORDIVISIOM; j++)
				{
					arrayGrayIndex[numberOfClass].point[j] += 
						CV_MAT_ELEM(*grayMatrix,double,kk,j );//grayMatrix->data+grayMatrix->step* ;
				}

				numberOfthisClass++;//单个类的点数+1

				}
			}
			arrayGrayIndex[numberOfClass].pointsInClass=numberOfthisClass;

			//3、计算中心点；
			for (int j=0;j< NUMBERFORDIVISIOM; j++)
			{
				arrayGrayIndex[numberOfClass].point[j]/=numberOfthisClass;
			}
			numberOfClass++;//类总数+1
		}		
	}

	cvReleaseMat(&TransposedMutilValue);
	collectionOfIndex.arrayGrayIndex=arrayGrayIndex;
	collectionOfIndex.numberOfindex=numberOfClass;
}


void  featToGrayIndex(int numberOfLogo,int NUMBEROF_CENG,
struct logoFeatStruct*** logofeat,
CvMat*  grayMatrix[GRAY_INDEX_NUMBER],//两个返回的值
struct indexAndClassOfGray *grayIndex[GRAY_INDEX_NUMBER]
)
{//完成 feat到 index的映射
	//最后返回灰度索引
//	CvMat*  grayMatrix[GRAY_INDEX_NUMBER];//用于保存灰度的 
	//struct indexAndClassOfGray *grayIndex[GRAY_INDEX_NUMBER];//每个灰度矩阵对应的index

	struct feature* feat1 ;
	int n1, k, i, m = 0;
	int logoNumber;//第几个logo

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
	{//线程的每一张图

		for (i=0;i<NUMBEROF_CENG   ;i++)
		{//缩放	
			feat1=(*logofeat)[logoNumber][i].feat1;
			n1=(*logofeat)[logoNumber][i].numberOffeat;
			for (k=0; k <n1; k++)
			{//每个点处理
				//灰度索引到对应seq链表中	
				tempnum=feat1[k].meanOfgray/GRAY_INDEX_PICE;
				indexpoint.logonumber=logoNumber;
				indexpoint.ceng=i;
				indexpoint.featorder=k;
				indexpoint.feat=feat1[k];

				if (tempnum >0 && 
					feat1[k].meanOfgray %GRAY_INDEX_PICE <2)//软边界
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
		}//每一层
	}//每一个图

	//每个seq转 matrxi
	for (i=0;i<GRAY_INDEX_NUMBER ; i++)
	{
	//这里要建立gray矩阵 + 矩阵的序号
		seqToMatrix(tempIndexOfGray[i],&grayMatrix[i],&grayIndex[i],tempIndexOfGray[i]->total);
	}

	//释放
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
{//将struct FeatIdCeng 结构转化到matrix 上，并将矩阵每行的点对应的索引值存入grayindex

	struct FeatIdCeng* indexpoint;
	int i,j,k,m;		

	//第一步：feat数组，转换为矩阵
	struct feature *feat1;
 
	*grayIndex=(struct indexAndClassOfGray *) malloc(sizeof(struct indexAndClassOfGray )*numberforfeats );
	*grayOffeatMatrix=cvCreateMat(numberforfeats,NUMBERFORDIVISIOM,CV_64FC1);


	//拷贝特征点到矩阵vector中
		for (k=0 ;k< numberforfeats ;k++)
			{
				struct FeatIdCeng * temp =( (struct FeatIdCeng *)cvGetSeqElem(tempOfGray,k));

			feat1=&	temp->feat;
			/*	feat1= &( (struct FeatIdCeng *)
					cvGetSeqElem(tempOfGray,k)) ->feat;//获得特征点序列的指针
					*/

				for (m=0;m< NUMBERFORDIVISIOM;m++)
				{
					CV_MAT_ELEM(**grayOffeatMatrix,double,k ,m) =
						(double ) (*feat1).descr[m];		
				}		
				(*grayIndex)[k].ceng=temp->ceng;//单目运算右结合   【】是 左结合 ，但是【】. -> ()比单目还要高级
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
{//建立一个统计logo每个 每层 匹配的点对的 坐标对
	//建立 统计的int2维数组

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
{//建立一个统计logo每个 每层 匹配的点对的 坐标对
	//建立 统计的int2维数组

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
{//计算匹配对中 ，比值最大的前maxsize个，//用每层的 匹配点数 除以总点数  
	//返回到数组中

	for (int k=0 ; k< maxSize ;k++)
	{
		maxarry[k]=-1;
	}
	//用冒泡法+ 同时交换值和序号
	double **indexAndValue;//2维数组，包含 比+序号
	indexAndValue=(double **)malloc( sizeof(double* ) *NUMBEROF_CENG* numberOfLogo );
	for (int k=0; k< NUMBEROF_CENG* numberOfLogo ; k++)
	{
		indexAndValue[k]=(double *)malloc(sizeof(double ) * 2) ;
		//indexAndValue[k][0]=k;
		//indexAndValue[k][1]=0.0;//初始化为0.0
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
				//测试不要看看	reduceMachedPoints(featseq[i][j],&m);//约减
					tempratio=((double) m )/ logofeat[i][j].numberOffeat ;
					indexAndValue[numberOfratio][1]=tempratio;//比值
					indexAndValue[numberOfratio][0]=numberOfratio;//序号
					numberOfratio++;
				}	
			}		
		}
	}

	if ( maxSize >numberOfratio)
	{//返回最小的
		maxSize=numberOfratio;
	}
	for (int i=0; i< maxSize; i++)
	{
		for (int j=0; j< numberOfratio-1-i ;j++)
		{//大的放后面  只求maxSize 个
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
		//放到最后
		maxarry[i]=indexAndValue[ numberOfratio-1-i][0];//序号 最后要除以NUMBEROF_CENG =logoid 
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
{//给feat点， 查询索引中的值  返回struct arrayOfGrayIndex 

	//先查询灰度索引
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
	//再求lsh索引
	nNNs=getRNearNeighbors(nnStruct[grayi], queryPoint, result, resultSize);

	if(nNNs <=0)
	{
		return NULL;
	}
	//计算最近邻   返回最近点中心的 coolecion序号
	int id=zuiJingling( queryPoint, result, nNNs);
	//一个点处理
	if (id< 0)
	{
		return NULL;
	}

	//返回colection中的链表
	struct arrayOfGrayIndex* tempresult=&
		(collectionOfIndex[grayi].arrayGrayIndex[ result[id]->index ]);


 return tempresult;
}

int zuiJingling(PPointT queryPoint,PPointT *result ,int resultSize)
{//从pPoint数组中找出 距离的最近邻  -1表示没有 ，其他表示序号
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