#ifndef  _PCA_H
#define  _PCA_H

#include "tools.h"
#include "utils.h"
#include "imgfeatures.h"
#include <string.h>
#define Mymin(x,y) ( x<y? x:y)

extern int MAXROWSOF_MATRIX ;

struct LogoTagAndFeatTag{
	int tagForClass;//��¼����ÿһ�ж�Ӧ�����ĸ�logo��ĵ�
	int tagForCeng;//��¼����ÿһ�ж�Ӧ�����ĸ�ceng
	int tagForFeatNumber;//��¼��Ӧ������ logo�ĵڼ���Ԫ��
};


struct IndexStructOfSmallclass{
	int numberi;//��������ĸ�����
	CvMat * MatrixForClass;//��¼С�����
	struct LogoTagAndFeatTag* logoTagAndFeatTag;
	int numberOfFeat;//һ�����ٸ���
};
;
void caculatePCAParameter(
	CvMat *ALLLogoFeatMatrix ,CvMat **ALLLogoPCAMatrix,int numberforfeats,
	CvMat **AvgVector,CvMat **EigenVector);




void  myCacultatePCA_new(
	LogoFeatStruct ** logofeat,int numberOfLogo,int NUMBEROF_CENG ,
	CvMat **AvgVector,CvMat **EigenVector,CvMat **BigIndex,
	CvSeq **featseq_ForBigClass,CvMemStorage* storage_ForBigClass,
	char* LogoFilepath)	;

void mycaculateALL(CvMat *ALLLogoFeatMatrix ,CvMat * AvgVector ,CvMat *EigenVector 
	,CvMat *ALLLogoPCAMatrix,struct LogoTagAndFeatTag *tagForClass,
	int numberforfeats ,CvMat ** BigIndex ,int NUMBERFORDIVISIOM_PCA,CvSeq *featseq_ForBigClass
	,char *LogoFilepath);

void mycaculateBigIndexAndSmallClassMO(CvSeq * featseq_ForBigClass,CvMat *BigIndex);

void myreleaseBigIndexAndSmallClass(CvSeq * featseq_ForBigClass,CvMat *BigIndex);

void  mycaculateBigClass(CvSeq * featseq_ForBigClass,int muberOfBigClass,CvMat *BigIndex,CvMat *ALLLogoPCAMatrix);

void mycaculateSmallClass(CvSeq *featseq_ForSmallClass,int numberOfseq,
	CvMat * ALLLogoFeatMatrix,struct IndexStructOfSmallclass  *tempIndexStruct,
struct LogoTagAndFeatTag *tagForClass);




void classifiedFeat(CvMat * result_orignal,int* numberOfMatrix,char *LogoFilepath);//, CvMat* result_result�����ļ�����
int  caculateDivision(CvMat * EigenValue_Row);
void myprojectOnePoint( struct feature* feat2,CvMat *AvgVector,CvMat *EigenVector,CvMat * result);

void PrintMatrix(CvMat *Matrix);

void swapLogoTagAndFeatTag(struct LogoTagAndFeatTag *index,struct LogoTagAndFeatTag * index2);
void mysortTotalSeq(	CvSeq ***SeqForeachLogoCeng,int numberOfLogo ,
	int NUMBEROF_CENG	,struct LogoTagAndFeatTag **index);


#endif