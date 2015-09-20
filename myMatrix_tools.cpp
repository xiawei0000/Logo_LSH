#include "myMatrix_tools.h"



void myCaculateAbsOfVectorInMatrix(CvMat *src)
{//将矩阵 每行都取模 用于算cos

	//printMyMatIndouble(src);
	for (int r=0; r< src->rows; r++)
	{//取绝对值
		double tempRowValue=0.0;
		for ( int c =0;c< src->cols ;c++)
		{
			double tempvaule= CV_MAT_ELEM(*src,double,r,c );
			tempRowValue+= tempvaule*tempvaule;
		}
		tempRowValue=sqrt( tempRowValue);
		for ( int cc =0;cc< src->cols ;cc++)
		{
			double *temptt=(double *)CV_MAT_ELEM_PTR(*src,r,cc );
		//	double *temp=  (double *)(src->data.ptr +src->step * r +cc);//乱码
			//看懂了吗？？？？？？  默认的空间是char*  索引 用+ccc不过是地址+1，  而转化为double后+1对应地址+2啊
			//应该是：double *temp=  (double *)(src->data.ptr +src->step * r) +cc;//
			//*temp = *temp /tempRowValue;	
		//	double *temp=  (double *)(src->data.ptr +src->step * r) +cc;
			*temptt = *temptt /tempRowValue;	

			//CV_MAT_ELEM(*src,double,r,cc+1 )=5.0;

		}
	}

	//	printMyMatIndouble(src);
}

void myCaculateSelfCosValue(CvMat *src ,CvMat *(&dst)   )
{//计算src矩阵的自cos值， 返回到dst中 
	CvMat *temp=cvCloneMat(src);
 //printMyMatIndouble(temp);

	myCaculateAbsOfVectorInMatrix(temp);//计算模

	//	printMyMatIndouble(temp);
	//dst =  cvCreateMat(temp->rows,temp->rows,temp->type);  外面申请
	cvMulTransposed(temp,dst, 0);//计算转置后的乘积
	//这里 计算 cos的  要 a*b  /|a|。|b|
	//printMyMatIndouble(dst);

	cvReleaseMat(&temp);
}


void 	mysaveMatrix(CvMat *result_result,char *filepath,int pices)
{//保存在logo路径里面， logosrc + matrix_i.xml

	char filename[300];
	char *filetype=".xml";
	char *filetemp="\\matrix_";
	char number[20];
	strcpy(filename,filepath);
	strcat(filename,filetemp);
	intToChar(pices,number);
	strcat(filename,number);
	strcat(filename,filetype);//拼接路径

	cvSave(filename,result_result,NULL,NULL,cvAttrList(0,0));

}

void myreadMatrix(CvMat **result_result,char *filepath,int pices)
{//读取文件matrix
	char filename[300];
	char *filetype=".xml";
	char *filetemp="\\matrix_";
	char number[20];
	strcpy(filename,filepath);
	strcat(filename,filetemp);
	intToChar(pices,number);
	strcat(filename,number);
	strcat(filename,filetype);//拼接路径

	* result_result=(CvMat*)cvLoad(filename,0,0,0);

}



void myreleaseLogoMatrix(CvMat  ***LogoMatrix,int numberOfLogo,int NUMBEROF_CENG)
{
	int i,j;
	for (i=0; i<numberOfLogo ;i++ )
	{
		for (j=0;j<NUMBEROF_CENG;j++)
		{
			if (LogoMatrix[i][j]!=NULL)
			{			
				cvReleaseMat(&LogoMatrix[i][j]);
			}

		}
		free(LogoMatrix[i]);
	}
	free(LogoMatrix);


}


void caculateMO(CvMat * result)
{//算矩阵每一行的模
	int i,j;
	double tempindex;

	for (i=0;i< result->rows;i ++)
	{
		tempindex=0.0;
		for(j=0;j< result->cols; j++)
		{
			tempindex+=(double )CV_MAT_ELEM(*result,double,i ,j) *
				(double )CV_MAT_ELEM(*result,double,i ,j);
		}
		tempindex= sqrt(tempindex);

		for(j=0;j< result->cols; j++)
		{
			CV_MAT_ELEM(*result,double,i ,j)=
				(double )	CV_MAT_ELEM(*result,double,i ,j)
				/ tempindex	;


		}

	}
}



void MatrixTranslateAndMO(CvMat *result,CvMat *resultT)
{//result 转置+ 求模
	cvTranspose(result,resultT);	
	caculateMO(resultT);//求模
}


void featToMatrixWithoutTranslate(CvMat *LogoMatrix,
struct feature *feat ,int numberOfFeat)
{//1.feat整合到矩阵， 不转置，不求mo
	int k,m;
	struct feature *feattemp;

	for (k=0 ;k< numberOfFeat ;k++)
	{
		for (m=0;m< NUMBERFORDIVISIOM;m++)
		{
			CV_MAT_ELEM(*LogoMatrix,double,k ,m) =
				(double ) ( ( feat[k]).descr[m]);
		}
	}

}



void featToMatrix(CvMat *LogoMatrix,struct feature *feat ,int numberOfFeat)
{//1.feat整合到矩阵，2，转置 3.除以模
	int k,m;
	struct feature *feattemp;
	double *tempindex;
	tempindex=(double *)malloc(sizeof(double)*numberOfFeat);

	for (k=0 ;k< numberOfFeat ;k++)
	{
		tempindex[k]=0.0;
		for (m=0;m< NUMBERFORDIVISIOM;m++)
		{
			CV_MAT_ELEM(*LogoMatrix,double,m ,k) =
				(double ) ( ( feat[k]).descr[m]);

			tempindex[k]+= (double ) ( ( feat[k]).descr[m]) * (double ) ( ( feat[k]).descr[m]);
		}
	}

	//除以模
	for (k=0 ;k< numberOfFeat ;k++)
	{
		tempindex[k]= sqrt(tempindex[k]);

		for (m=0;m< NUMBERFORDIVISIOM;m++)
		{
			CV_MAT_ELEM(*LogoMatrix,double,m ,k) =
				(double )	CV_MAT_ELEM(*LogoMatrix,double,m ,k) / tempindex[k]	;

		}
	}

	free(tempindex);

}

void LogoFeatStructToMatrix(CvMat *LogoMatrix,LogoFeatStruct logofeat)
{
	int k,m;
	double *tempindex;
	tempindex=(double *)malloc(sizeof(double)*  (logofeat).numberOffeat);

	for (k=0 ;k< (logofeat).numberOffeat ; k++)
	{
		tempindex[k]=0.0;

		for (m=0;m< NUMBERFORDIVISIOM;m++)
		{
			CV_MAT_ELEM(*LogoMatrix,double,k ,m) =
				(double ) (logofeat) .feat1[k].descr[m];

			tempindex[k]+= (double ) (logofeat) .feat1[k].descr[m] * 
				(double ) (logofeat) .feat1[k].descr[m];
		}
	}

	//除以模?????????????????????
	for (k=0 ;k< (logofeat).numberOffeat  ;k++)
	{
		tempindex[k]= sqrt(tempindex[k]);
		for (m=0;m< NUMBERFORDIVISIOM;m++)
		{
			CV_MAT_ELEM(*LogoMatrix,double,k ,m) =
				(double )	CV_MAT_ELEM(*LogoMatrix,double,k,m) / tempindex[k]	;
		}
	}

	free(tempindex);

}


void printMyMatIndouble(CvMat* tagMat)
{
	int i,j;

	FILE *file=fopen("output_matrix.txt","w");

	for(i=0;i<tagMat->rows;i++)
	{
		for (j=0;j<tagMat->cols;j++)
		{ //为何最后3行总是负数？？？？？
				// fprintf( stderr, "%d",(int) CV_MAT_ELEM(*tagMat,int,i,j) );
			double * ptr1=  (double *)(tagMat->data.ptr+tagMat->step *i);
			//printf(  "%lf ",  *(ptr1+j)    );
			fprintf(file,  "%lf ",*(ptr1+j) );

		/*	//三种输出
			printf(  "%lf ",(double) (*(tagMat->data.ptr+tagMat->step *i+j)) );

			double * ptr1=  (double *)(tagMat->data.ptr+tagMat->step *i);
			printf(  "%lf ",  *(ptr1+j)    );

			double * ptr2=  (double *)(tagMat->data.ptr);
			printf(  "%lf ",*(  ptr2+tagMat->step *i   +j  ) );
			//	printf(  "%lf ",(double) (*(tagMat->data.ptr+tagMat->step *i+j)) );

			*/
		}
		fprintf(file,"\n");
		//printf("\n");
	}
	//fprintf(file,"\n\n=================================================================================\n\n\n");
	//???最后一只出错，只是没有关文件
	fclose(file);
}

void printMyMat(CvMat* tagMat)
{
	int i,j;
	for(i=0;i<tagMat->rows;i++)
	{
		for (j=0;j<tagMat->cols;j++)
		{ //为何最后3行总是负数？？？？？
			if ((int) *(tagMat->data.ptr+tagMat->step *i+j) )
			{
				// fprintf( stderr, "%d",(int) CV_MAT_ELEM(*tagMat,int,i,j) );
				fprintf( stderr, " %d",(int) *(tagMat->data.ptr+tagMat->step *i+j));
			}
		}
		fprintf(stderr,"\n");
	}

}
