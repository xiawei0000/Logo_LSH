#include "pca.h"
 

 


int MAXROWSOF_MATRIX=1000;//��������




void caculatePCAParameter(
	CvMat *ALLLogoFeatMatrix ,CvMat **ALLLogoPCAMatrix,int numberforfeats,
	CvMat **AvgVector,CvMat **EigenVector)
{//pca�������� +logo��pcaת��
	CvMat *EigenValue_Row;

	*AvgVector=cvCreateMat(1,NUMBERFORDIVISIOM,CV_64FC1);//��ֵ
	EigenValue_Row=cvCreateMat(Mymin(NUMBERFORDIVISIOM ,numberforfeats) ,1,CV_64FC1);//�б�ʾ�� ����ֵ
	*EigenVector=cvCreateMat(Mymin(NUMBERFORDIVISIOM ,numberforfeats) ,NUMBERFORDIVISIOM,CV_64FC1);//��������

	cvCalcPCA(ALLLogoFeatMatrix,*AvgVector,EigenValue_Row,*EigenVector,CV_PCA_DATA_AS_ROW);

	//
	PrintMatrix(*AvgVector);
	PrintMatrix(EigenValue_Row);

 //	NUMBERFORDIVISIOM_PCA=caculateDivision( EigenValue_Row);

	//������ logo������תpca
//	*ALLLogoPCAMatrix=cvCreateMat(ALLLogoFeatMatrix->rows,NUMBERFORDIVISIOM_PCA,ALLLogoFeatMatrix->type);
	*ALLLogoPCAMatrix=cvCreateMat(ALLLogoFeatMatrix->rows,NUMBERFORDIVISIOM,ALLLogoFeatMatrix->type);
	cvProjectPCA(ALLLogoFeatMatrix,*AvgVector,*EigenVector,*ALLLogoPCAMatrix);
}



int  caculateDivision(CvMat * EigenValue_Row)
{//���㽵ά���ü���ά�ȣ�
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
		i=1;//������һά
	}

	return i;

}


//����������Ĳ�����
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
{	/*Ҫ���صĲ�����
	BigIndex ��һ�� ����  pca��ά��Ľ���� ��index ����������һ�� 128ά����С��
	AvgVector, **EigenVector  ������pca��ά�ľ���
	*/
	//��һ�������pca����
	//�ڶ�������logo pcaת��  
	//�����������Ե� pcaת��   
	//���ģ� ����˫���������
	int n1, k, i,j, m = 0,numberforfeats=0;
	CvMat *ALLLogoFeatMatrix;
	//	CvMat *AvgVector;
	CvMat *EigenValue_Row;
	//	CvMat *EigenVector;
	CvMat *ALLLogoPCAMatrix;//pca��ά��Ľ��.

	struct feature *feat1;


	struct LogoTagAndFeatTag *tag;//��¼����ÿһ�ж�Ӧ�ĸ�logo��feat
	//int *tagForFeatNumber;//��¼ÿ�������У���Ӧfeat�ĵڼ�����
	int tagtemp;

	//��һ����feat���飬ת��Ϊ����
	numberforfeats=0;//����ȷ��feat����
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
	//���������㵽����vector��
	for (j=0;j<numberOfLogo;j++)
	{
		for (i=0;i<NUMBEROF_CENG   ;i++)
		{	

			if ((logofeat)[j][i].numberOffeat !=0)
			{
	

				feat1=(logofeat)[j][i].feat1;//������������е�ָ��
				for (k=0 ;k< (logofeat)[j][i].numberOffeat ;k++)
				{
					for (m=0;m< NUMBERFORDIVISIOM;m++)
					{
						
				/*		*(double *)CV_MAT_ELEM_PTR(*ALLLogoFeatMatrix,numberforfeats ,m)
					=(double ) (logofeat[j][i]) .feat1[k].descr[m];
*/
						//CV_MAT_ELEM չ���� �Ѿ�����*��double*���ˣ��Լ��ټ�һ����Ȼ������

						CV_MAT_ELEM(*ALLLogoFeatMatrix,double,numberforfeats ,m)=
						(double ) (logofeat[j][i]) .feat1[k].descr[m];	

						
						


					}
					tag[tagtemp].tagForCeng=i;
						tag[tagtemp].tagForClass=j;
							tag[tagtemp].tagForFeatNumber=k;
					//tagForFeatNumber[tagtemp]=k;
				//	tag[tagtemp]=( j*100 +i);//��*100 +���� �����
					tagtemp++;
					numberforfeats++;	
				}

			}
		}

	}

	//�ڶ�������ƫ�����

	*AvgVector=cvCreateMat(1,NUMBERFORDIVISIOM,CV_64FC1);//��ֵ
	EigenValue_Row=cvCreateMat(Mymin(NUMBERFORDIVISIOM ,numberforfeats) ,1,CV_64FC1);//�б�ʾ�� ����ֵ
	*EigenVector=cvCreateMat(Mymin(NUMBERFORDIVISIOM ,numberforfeats) ,NUMBERFORDIVISIOM,CV_64FC1);//��������

	cvCalcPCA(ALLLogoFeatMatrix,*AvgVector,EigenValue_Row,*EigenVector,CV_PCA_DATA_AS_ROW);

	//
 	PrintMatrix(*AvgVector);
	PrintMatrix(EigenValue_Row);

	NUMBERFORDIVISIOM_PCA=caculateDivision( EigenValue_Row);


	//������ logo������תpca
	ALLLogoPCAMatrix=cvCreateMat(ALLLogoFeatMatrix->rows,NUMBERFORDIVISIOM_PCA,ALLLogoFeatMatrix->type);
	cvProjectPCA(ALLLogoFeatMatrix,*AvgVector,*EigenVector,ALLLogoPCAMatrix);
	

//	PrintMatrix(ALLLogoPCAMatrix);
	
	//��ʼ���ࣺ tag��¼ÿ�����Ӧ�����
//	CvSeq *featseq_ForBigClass;//����࣬ ����
//	CvMemStorage* storage_ForBigClass ;
	
	*featseq_ForBigClass=cvCreateSeq(0,sizeof(CvSeq),
		sizeof(struct IndexStructOfSmallclass),storage_ForBigClass);
//���Ĳ�������˫����������

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
	//biginex + ��Ӧ�� seq ��Ҫ���أ� ���������ͷš�
	//��ʼ���ࣺ 
	//һ����R ÿһ�е�ģ ���� R * RT
	CvSeq *featseq_ForSmallClass;//��С���
	CvMemStorage* storage_ForSmallClass ;

	CvMat *result_result;

	int numberOfseq;
	int muberOfBigClass;
	struct IndexStructOfSmallclass tempIndexStruct;
	int *tagForAlreadClassified;//��ǹ������
	int i,j,k;

	//CvMat *result_result;
	int numberOfMatrix=0;
	//һ��pca���� ����ģ�� �� ��ˡ�
	//�޸�Ϊ�����ļ���
	classifiedFeat( ALLLogoPCAMatrix, & numberOfMatrix,LogoFilepath);


	//����ɨ��ÿһ�� ����Сֵ������seq�У� 
	tagForAlreadClassified=(int *)malloc(sizeof(int ) *numberforfeats);
	memset(tagForAlreadClassified,0,sizeof(int ) *numberforfeats);

	storage_ForSmallClass = cvCreateMemStorage( 0 );
	featseq_ForSmallClass=cvCreateSeq(0,sizeof(CvSeq),sizeof(int),storage_ForSmallClass);


	//�޸�Ϊ���ļ���һ������ȡ
	muberOfBigClass=0;
	for (k=0; k<numberOfMatrix; k++)
	{	
		//һ���ļ� matrix �� 
		myreadMatrix(&result_result,LogoFilepath, k);
		//Ȼ��ÿ����matrix ������
		for(i=0;i<result_result->rows ;i++)
		{
			if (tagForAlreadClassified[i] ==0)
			{ //����С����ɺ� ������mat
				tempIndexStruct.numberi=i;

				numberOfseq=0;
				cvClearSeq(featseq_ForSmallClass);
				for(j=0;j< result_result->cols; j++)
				{
					if ((double )	CV_MAT_ELEM(*result_result,double,i ,j) > MINCOS_PCA )//0.6)// )//��ֵ0.95����
					{//��Ϊһ��С���
						tagForAlreadClassified[j] =1;
						cvSeqPush(featseq_ForSmallClass,&j);
						numberOfseq++;
					}
				}
				//��seq ����������mat
				mycaculateSmallClass(featseq_ForSmallClass,numberOfseq,ALLLogoFeatMatrix,&tempIndexStruct,
					tagForClass);

				//�޸�������վ
				cvSeqPush(featseq_ForBigClass,&tempIndexStruct);//���������
				muberOfBigClass++;
			}

		}
	
		cvReleaseMat(&result_result);
	}
	
		
	//�ģ������൯��������mat
	*BigIndex=cvCreateMat(muberOfBigClass,NUMBERFORDIVISIOM_PCA,ALLLogoFeatMatrix->type);
	mycaculateBigClass(featseq_ForBigClass,muberOfBigClass,*BigIndex,ALLLogoPCAMatrix);


	mycaculateBigIndexAndSmallClassMO(featseq_ForBigClass,*BigIndex);//��mo



	cvReleaseMemStorage(&storage_ForSmallClass);
	free(tagForAlreadClassified);
	//���䣺 ����bigindex ��smalllclasss��mo


}*/

void mycaculateBigIndexAndSmallClassMO(CvSeq * featseq_ForBigClass,CvMat *BigIndex)
{//���� ���� ��С��ľ����ģ ��ô������˾��ǽǶ�cosֵ��
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
{//�ͷ����е����� ����
	//seq�� strorageҪ�������룬�����ͷ�
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
{//����  logopca�����Ƶ� bigindex�����С�
	int i,j;
	struct IndexStructOfSmallclass* tempstruct;
	for (i=0; i< muberOfBigClass ;i++)
	{
		tempstruct=(struct IndexStructOfSmallclass *)cvGetSeqElem(featseq_ForBigClass,i);
		for (j=0;j<BigIndex->cols; j++)
		{
			//��logo����PCA  ���Ƶ�index ��
			CV_MAT_ELEM(*( BigIndex),double,i,j)=
				CV_MAT_ELEM(*( ALLLogoPCAMatrix),double,tempstruct->numberi,j);
		}
	}
}

void mycaculateSmallClass(CvSeq *featseq_ForSmallClass,int numberOfseq,
	CvMat * ALLLogoFeatMatrix,struct IndexStructOfSmallclass  *tempIndexStruct,
	struct LogoTagAndFeatTag *tagForClass)
{//��seq��ȡ�� ͬһ��С��� temp ��Ȼ�� �ҵ�logo��ԭ128ά ���Ƶ�classsmatrix�У������ ÿ�ж�Ӧԭlogo �ǵڼ�����tag��
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
		//С���У�ÿ���У���Ӧ ԭfeat�����

		for (j=0;j <NUMBERFORDIVISIOM ;j++)
		{//��logo���� ���Ƶ�clas��
			CV_MAT_ELEM(*( tempIndexStruct->MatrixForClass),double,i,j)=
					CV_MAT_ELEM(*( ALLLogoFeatMatrix),double,*temp,j);
		}
	}
}




void myprojectOnePoint( struct feature* feat2,CvMat *AvgVector,CvMat *EigenVector,CvMat * result)
{//���ڵ������pcaת��  resultû���ͷ�
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
{//�����ṹ��
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
{//ֻ��ǰ����������������Ȼ��simֵ�ɣ�ǰ������Ӧ��sim��󰡡�
	int i,j,k;
	int temp;
	*index=(struct LogoTagAndFeatTag *)malloc(sizeof(struct LogoTagAndFeatTag) *numberOfLogo*NUMBEROF_CENG);//��������� i��j�㣬 k=total


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
	{//���� NUMBEROF_CENG* numberOfLogo �����Ƹ�����ÿ�ֶ���ȡһ������ֵ�ŵ���i����
		for (j=i+1 ;j<  NUMBEROF_CENG* numberOfLogo ;j++)
		{
			if(  (*index)[j].tagForFeatNumber > (*index)[i].tagForFeatNumber )
			{//����
				swapLogoTagAndFeatTag(&(*index)[j],&(*index)[i]);
			}
		}
	}

	//û�ͷ�
}

