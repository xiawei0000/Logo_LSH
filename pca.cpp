#include "pca.h"
 

 


int MAXROWSOF_MATRIX=1000;//最大的行数




void caculatePCAParameter(
	CvMat *ALLLogoFeatMatrix ,CvMat **ALLLogoPCAMatrix,int numberforfeats,
	CvMat **AvgVector,CvMat **EigenVector)
{//pca参数计算 +logo的pca转换
	CvMat *EigenValue_Row;

	*AvgVector=cvCreateMat(1,NUMBERFORDIVISIOM,CV_64FC1);//均值
	EigenValue_Row=cvCreateMat(Mymin(NUMBERFORDIVISIOM ,numberforfeats) ,1,CV_64FC1);//列表示的 特征值
	*EigenVector=cvCreateMat(Mymin(NUMBERFORDIVISIOM ,numberforfeats) ,NUMBERFORDIVISIOM,CV_64FC1);//特征向量

	cvCalcPCA(ALLLogoFeatMatrix,*AvgVector,EigenValue_Row,*EigenVector,CV_PCA_DATA_AS_ROW);

	//
	PrintMatrix(*AvgVector);
	PrintMatrix(EigenValue_Row);

 //	NUMBERFORDIVISIOM_PCA=caculateDivision( EigenValue_Row);

	//第三步 logo特征点转pca
//	*ALLLogoPCAMatrix=cvCreateMat(ALLLogoFeatMatrix->rows,NUMBERFORDIVISIOM_PCA,ALLLogoFeatMatrix->type);
	*ALLLogoPCAMatrix=cvCreateMat(ALLLogoFeatMatrix->rows,NUMBERFORDIVISIOM,ALLLogoFeatMatrix->type);
	cvProjectPCA(ALLLogoFeatMatrix,*AvgVector,*EigenVector,*ALLLogoPCAMatrix);
}



int  caculateDivision(CvMat * EigenValue_Row)
{//计算降维后，用几个维度？
	int i;
	double sum=0.0,sumtemp=0.0;
	for (i=0;i<EigenValue_Row->rows;i++)
	{
		sum+=(double )CV_MAT_ELEM(*EigenValue_Row,double,i,0) ;
	}
	for (i=0;i<EigenValue_Row->rows;i++)
	{
		sumtemp+=(double )CV_MAT_ELEM(*EigenValue_Row,double,i,0) ;
		if (sumtemp >=sum*PART_PCA)
		{
			break;
		}
	}
	if (i==0)
	{
		i=1;//不能用一维
	}

	return i;

}


//传入特征点的参数，
/*
void  myCacultatePCA(
	LogoFeatStruct ** logofeat,int numberOfLogo,int NUMBEROF_CENG ,
	CvMat **AvgVector,CvMat **EigenVector,CvMat **BigIndex,
	CvSeq **featseq_ForBigClass,CvMemStorage* storage_ForBigClass,
	char* LogoFilepath)	*/
void  myCacultatePCA_new(
	LogoFeatStruct ** logofeat,int numberOfLogo,int NUMBEROF_CENG ,
	CvMat **AvgVector,CvMat **EigenVector,CvMat **BigIndex,
	CvSeq **featseq_ForBigClass,CvMemStorage* storage_ForBigClass,
	char* LogoFilepath)	
{	/*要返回的参数，
	BigIndex 是一层 索引  pca降维后的结果。 由index 来索引到下一层 128维矩阵小类
	AvgVector, **EigenVector  是用于pca降维的矩阵。
	*/
	//第一步：获得pca参数
	//第二步：将logo pca转换  
	//第三步：测试点 pca转换   
	//第四： 建立双层矩阵索引
	int n1, k, i,j, m = 0,numberforfeats=0;
	CvMat *ALLLogoFeatMatrix;
	//	CvMat *AvgVector;
	CvMat *EigenValue_Row;
	//	CvMat *EigenVector;
	CvMat *ALLLogoPCAMatrix;//pca降维后的结果.

	struct feature *feat1;


	struct LogoTagAndFeatTag *tag;//记录矩阵每一行对应哪个logo的feat
	//int *tagForFeatNumber;//记录每个矩阵行，对应feat的第几个，
	int tagtemp;

	//第一步：feat数组，转换为矩阵
	numberforfeats=0;//遍历确定feat个数
	for (j=0;j<numberOfLogo;j++)
	{
		for (i=0;i<NUMBEROF_CENG   ;i++)
		{	

			if ((logofeat)[j][i].numberOffeat !=0)
			{
				numberforfeats+= (logofeat)[j][i].numberOffeat;

			}
		}

	}

	ALLLogoFeatMatrix=cvCreateMat(numberforfeats,NUMBERFORDIVISIOM,CV_64FC1);

	tag=(struct LogoTagAndFeatTag*)malloc(sizeof(struct LogoTagAndFeatTag) *numberforfeats );
//	tagForFeatNumber=(int*)malloc(sizeof(int) *numberforfeats );
	tagtemp=0;
	numberforfeats=0;
	//拷贝特征点到矩阵vector中
	for (j=0;j<numberOfLogo;j++)
	{
		for (i=0;i<NUMBEROF_CENG   ;i++)
		{	

			if ((logofeat)[j][i].numberOffeat !=0)
			{
	

				feat1=(logofeat)[j][i].feat1;//获得特征点序列的指针
				for (k=0 ;k< (logofeat)[j][i].numberOffeat ;k++)
				{
					for (m=0;m< NUMBERFORDIVISIOM;m++)
					{
						
				/*		*(double *)CV_MAT_ELEM_PTR(*ALLLogoFeatMatrix,numberforfeats ,m)
					=(double ) (logofeat[j][i]) .feat1[k].descr[m];
*/
						//CV_MAT_ELEM 展开后 已经是有*（double*）了，自己再加一个当然出错了

						CV_MAT_ELEM(*ALLLogoFeatMatrix,double,numberforfeats ,m)=
						(double ) (logofeat[j][i]) .feat1[k].descr[m];	

						
						


					}
					tag[tagtemp].tagForCeng=i;
						tag[tagtemp].tagForClass=j;
							tag[tagtemp].tagForFeatNumber=k;
					//tagForFeatNumber[tagtemp]=k;
				//	tag[tagtemp]=( j*100 +i);//类*100 +层数 来标记
					tagtemp++;
					numberforfeats++;	
				}

			}
		}

	}

	//第二步计算偏差参数

	*AvgVector=cvCreateMat(1,NUMBERFORDIVISIOM,CV_64FC1);//均值
	EigenValue_Row=cvCreateMat(Mymin(NUMBERFORDIVISIOM ,numberforfeats) ,1,CV_64FC1);//列表示的 特征值
	*EigenVector=cvCreateMat(Mymin(NUMBERFORDIVISIOM ,numberforfeats) ,NUMBERFORDIVISIOM,CV_64FC1);//特征向量

	cvCalcPCA(ALLLogoFeatMatrix,*AvgVector,EigenValue_Row,*EigenVector,CV_PCA_DATA_AS_ROW);

	//
 	PrintMatrix(*AvgVector);
	PrintMatrix(EigenValue_Row);

	NUMBERFORDIVISIOM_PCA=caculateDivision( EigenValue_Row);


	//第三步 logo特征点转pca
	ALLLogoPCAMatrix=cvCreateMat(ALLLogoFeatMatrix->rows,NUMBERFORDIVISIOM_PCA,ALLLogoFeatMatrix->type);
	cvProjectPCA(ALLLogoFeatMatrix,*AvgVector,*EigenVector,ALLLogoPCAMatrix);
	

//	PrintMatrix(ALLLogoPCAMatrix);
	
	//开始分类： tag记录每个点对应的类别
//	CvSeq *featseq_ForBigClass;//存大类， 索引
//	CvMemStorage* storage_ForBigClass ;
	
	*featseq_ForBigClass=cvCreateSeq(0,sizeof(CvSeq),
		sizeof(struct IndexStructOfSmallclass),storage_ForBigClass);
//第四部：建立双层索引矩阵

	/*
	mycaculateALL(ALLLogoFeatMatrix , *AvgVector ,*EigenVector ,
		ALLLogoPCAMatrix,tag, 
		numberforfeats ,BigIndex,NUMBERFORDIVISIOM_PCA,*featseq_ForBigClass,LogoFilepath);
*/

	//	cvReleaseMat(&EigenValue_Row);
	//cvReleaseMat(&ALLLogoFeatMatrix);
	return  ;
}

/*

void mycaculateALL(CvMat *ALLLogoFeatMatrix ,CvMat * AvgVector ,CvMat *EigenVector 
	,CvMat *ALLLogoPCAMatrix,struct LogoTagAndFeatTag *tagForClass,
int numberforfeats ,CvMat ** BigIndex ,int NUMBERFORDIVISIOM_PCA,CvSeq *featseq_ForBigClass
,char *LogoFilepath)
{	
	//biginex + 对应的 seq 都要返回， 并在外面释放。
	//开始分类： 
	//一：求R 每一行的模 ，再 R * RT
	CvSeq *featseq_ForSmallClass;//存小类别
	CvMemStorage* storage_ForSmallClass ;

	CvMat *result_result;

	int numberOfseq;
	int muberOfBigClass;
	struct IndexStructOfSmallclass tempIndexStruct;
	int *tagForAlreadClassified;//标记归类与否
	int i,j,k;

	//CvMat *result_result;
	int numberOfMatrix=0;
	//一：pca矩阵 先求模， 再 相乘。
	//修改为存入文件夹
	classifiedFeat( ALLLogoPCAMatrix, & numberOfMatrix,LogoFilepath);


	//二：扫描每一行 求最小值，加入seq中， 
	tagForAlreadClassified=(int *)malloc(sizeof(int ) *numberforfeats);
	memset(tagForAlreadClassified,0,sizeof(int ) *numberforfeats);

	storage_ForSmallClass = cvCreateMemStorage( 0 );
	featseq_ForSmallClass=cvCreateSeq(0,sizeof(CvSeq),sizeof(int),storage_ForSmallClass);


	//修改为从文件夹一个个读取
	muberOfBigClass=0;
	for (k=0; k<numberOfMatrix; k++)
	{	
		//一读文件 matrix ， 
		myreadMatrix(&result_result,LogoFilepath, k);
		//然后，每个分matrix 建立类
		for(i=0;i<result_result->rows ;i++)
		{
			if (tagForAlreadClassified[i] ==0)
			{ //建立小类完成后 ，建立mat
				tempIndexStruct.numberi=i;

				numberOfseq=0;
				cvClearSeq(featseq_ForSmallClass);
				for(j=0;j< result_result->cols; j++)
				{
					if ((double )	CV_MAT_ELEM(*result_result,double,i ,j) > MINCOS_PCA )//0.6)// )//阈值0.95测试
					{//归为一个小类别
						tagForAlreadClassified[j] =1;
						cvSeqPush(featseq_ForSmallClass,&j);
						numberOfseq++;
					}
				}
				//对seq 弹出，建立mat
				mycaculateSmallClass(featseq_ForSmallClass,numberOfseq,ALLLogoFeatMatrix,&tempIndexStruct,
					tagForClass);

				//修改完后才入站
				cvSeqPush(featseq_ForBigClass,&tempIndexStruct);//大类入队列
				muberOfBigClass++;
			}

		}
	
		cvReleaseMat(&result_result);
	}
	
		
	//四，将大类弹出，建立mat
	*BigIndex=cvCreateMat(muberOfBigClass,NUMBERFORDIVISIOM_PCA,ALLLogoFeatMatrix->type);
	mycaculateBigClass(featseq_ForBigClass,muberOfBigClass,*BigIndex,ALLLogoPCAMatrix);


	mycaculateBigIndexAndSmallClassMO(featseq_ForBigClass,*BigIndex);//求mo



	cvReleaseMemStorage(&storage_ForSmallClass);
	free(tagForAlreadClassified);
	//补充： 计算bigindex 和smalllclasss的mo


}*/

void mycaculateBigIndexAndSmallClassMO(CvSeq * featseq_ForBigClass,CvMat *BigIndex)
{//计算 大类 和小类的矩阵的模 那么矩阵相乘就是角度cos值了
	int i;
	struct IndexStructOfSmallclass* tempstruct;
	CvMat *tempMat;

	for (i=0;i <featseq_ForBigClass->total; i++ )
	{
		tempstruct=(struct IndexStructOfSmallclass *)cvGetSeqElem(featseq_ForBigClass,i);

		caculateMO( (tempstruct->MatrixForClass));


	}
	caculateMO(BigIndex);

}

void myreleaseBigIndexAndSmallClass(CvSeq * featseq_ForBigClass,CvMat *BigIndex)
{//释放所有的索引 矩阵。
	//seq的 strorage要在外申请，在外释放
	int i;

	struct IndexStructOfSmallclass* tempstruct;



	for (i=0;i <featseq_ForBigClass->total; i++ )
	{
		tempstruct=(struct IndexStructOfSmallclass *)cvGetSeqElem(featseq_ForBigClass,i);

		cvReleaseMat( &(tempstruct->MatrixForClass));
		free((tempstruct->logoTagAndFeatTag));
	//	free(tempstruct->tagForFeatNumber);
	}
	cvReleaseMat(&BigIndex);

	
}

void  mycaculateBigClass(CvSeq * featseq_ForBigClass,int muberOfBigClass,CvMat *BigIndex,CvMat *ALLLogoPCAMatrix)
{//大类  logopca矩阵复制到 bigindex矩阵中。
	int i,j;
	struct IndexStructOfSmallclass* tempstruct;
	for (i=0; i< muberOfBigClass ;i++)
	{
		tempstruct=(struct IndexStructOfSmallclass *)cvGetSeqElem(featseq_ForBigClass,i);
		for (j=0;j<BigIndex->cols; j++)
		{
			//从logo矩阵PCA  复制到index 中
			CV_MAT_ELEM(*( BigIndex),double,i,j)=
				CV_MAT_ELEM(*( ALLLogoPCAMatrix),double,tempstruct->numberi,j);
		}
	}
}

void mycaculateSmallClass(CvSeq *featseq_ForSmallClass,int numberOfseq,
	CvMat * ALLLogoFeatMatrix,struct IndexStructOfSmallclass  *tempIndexStruct,
	struct LogoTagAndFeatTag *tagForClass)
{//从seq中取出 同一个小类的 temp ，然后 找到logo的原128维 复制到classsmatrix中，并标记 每行对应原logo 是第几个（tag）
	int i,j,k;
	int *temp;
	tempIndexStruct->MatrixForClass=cvCreateMat(numberOfseq,NUMBERFORDIVISIOM,ALLLogoFeatMatrix->type);
	tempIndexStruct->numberOfFeat=numberOfseq;

	tempIndexStruct->logoTagAndFeatTag=(struct LogoTagAndFeatTag *)malloc(sizeof(struct LogoTagAndFeatTag ) *numberOfseq );

	//tempIndexStruct->tagForFeatNumber=(int *)malloc(sizeof(int ) *numberOfseq );

	for(i=0;i< numberOfseq;i++)
	{
		temp=(int*)cvGetSeqElem(featseq_ForSmallClass,i); 
		tempIndexStruct->logoTagAndFeatTag[i].tagForClass= tagForClass[*temp].tagForClass;
			tempIndexStruct->logoTagAndFeatTag[i].tagForCeng= tagForClass[*temp].tagForCeng;
				tempIndexStruct->logoTagAndFeatTag[i].tagForFeatNumber= tagForClass[*temp].tagForFeatNumber;
		//tempIndexStruct->tagForFeatNumber[i]=tagForFeatNumber[*temp];
		//tagForFeatNumber
		//小类中，每个行，对应 原feat的类别，

		for (j=0;j <NUMBERFORDIVISIOM ;j++)
		{//从logo矩阵 复制到clas中
			CV_MAT_ELEM(*( tempIndexStruct->MatrixForClass),double,i,j)=
					CV_MAT_ELEM(*( ALLLogoFeatMatrix),double,*temp,j);
		}
	}
}




void myprojectOnePoint( struct feature* feat2,CvMat *AvgVector,CvMat *EigenVector,CvMat * result)
{//用于单个点的pca转化  result没有释放
	result=cvCreateMat(1,NUMBERFORDIVISIOM_PCA, AvgVector->type);
//	CvMat mat = cvMat( 1, NUMBERFORDIVISIOM, AvgVector->type,feat2->descr); 
//	cvProjectPCA(&mat,AvgVector,EigenVector,result);

}



void PrintMatrix(CvMat *Matrix)
{
	int i,j;
	int Rows= Matrix->rows;
	int Cols=Matrix->cols;
	for( i=0;i<Rows;i++)
	{
		for( j=0;j<Cols;j++)
		{
			printf("%.2lf ", CV_MAT_ELEM(*Matrix,double,i,j) );//cvGet2D(Matrix,i,j).val[0]);
		}
		printf("\n");
	}
}




void swapLogoTagAndFeatTag(struct LogoTagAndFeatTag *index,struct LogoTagAndFeatTag * index2)
{//交换结构体
	struct LogoTagAndFeatTag tem;
	struct LogoTagAndFeatTag *temp=&tem;

	temp->tagForCeng=index->tagForCeng;
	temp->tagForClass=index->tagForClass;
	temp->tagForFeatNumber=index->tagForFeatNumber;

	index->tagForCeng=index2->tagForCeng;
	index->tagForClass=index2->tagForClass;
	index->tagForFeatNumber=index2->tagForFeatNumber;

	index2->tagForCeng=temp->tagForCeng;
	index2->tagForClass=temp->tagForClass;
	index2->tagForFeatNumber=temp->tagForFeatNumber;

}
void mysortTotalSeq(	CvSeq ***SeqForeachLogoCeng,int numberOfLogo ,
	int NUMBEROF_CENG	,struct LogoTagAndFeatTag **index)
{//只找前三个：：还是排序，然后看sim值吧，前三个不应定sim最大啊。
	int i,j,k;
	int temp;
	*index=(struct LogoTagAndFeatTag *)malloc(sizeof(struct LogoTagAndFeatTag) *numberOfLogo*NUMBEROF_CENG);//索引结果： i个j层， k=total


	for (i=0; i<numberOfLogo ;i++ )
	{
		for (j=0;j<NUMBEROF_CENG;j++)
		{	
			(*index)[i*NUMBEROF_CENG +j].tagForClass=i;
			(*index)[i*NUMBEROF_CENG +j].tagForCeng=j;
			(*index)[i*NUMBEROF_CENG +j].tagForFeatNumber= SeqForeachLogoCeng[i][j]->total;
		}	
	}

	for (i=0 ;i< NUMBEROF_CENG* numberOfLogo ;i++)
	{//控制 NUMBEROF_CENG* numberOfLogo 来控制个数，每轮都是取一个最大的值放到【i】处
		for (j=i+1 ;j<  NUMBEROF_CENG* numberOfLogo ;j++)
		{
			if(  (*index)[j].tagForFeatNumber > (*index)[i].tagForFeatNumber )
			{//交换
				swapLogoTagAndFeatTag(&(*index)[j],&(*index)[i]);
			}
		}
	}

	//没释放
}

