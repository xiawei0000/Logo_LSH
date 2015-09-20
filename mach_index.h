#ifndef  MAIN_INDEX_H
#define  MAIN_INDEX_H

#include "tools.h"
#include "NearNeighbors.h"
#include "mach_base.h"
//�����Ĳ���

void printIndex(PRNearNeighborStructT nnStruct);

void ouputPictureData( struct feature *str,  int n);


void  outputOrignalData( struct collectionPointsOfIndex* collectionOfIndexP,
	double thresholdR,	//1�������е��ƽ������ /3=thresholdR
	IntT nSampleQueries,
	PPointT *(sampleQueries));


void myCaculateIndex(int numberOfLogo,int NUMBEROF_CENG,
struct logoFeatStruct*** logofeat,
	PRNearNeighborStructT nnStruct[GRAY_INDEX_NUMBER],
struct collectionPointsOfIndex collectionOfIndex[GRAY_INDEX_NUMBER]
);
void releaseCollectionPointsOfIndex(struct collectionPointsOfIndex *collectionOfIndex);


void  copytoPPoint(double *data,double *point,int division);


double  caculateDistance(double *data,double *point,int division);


void initDataForLsh(struct collectionPointsOfIndex *collectionOfIndex,
	double &thresholdR,	//1�������е��ƽ������ /3=thresholdR
	PPointT *(&dataSet),//2�������ݵ�dataset
	IntT nSampleQueries,
	PPointT *(&sampleQueries)//3���������ѯ��
	);

void  myCaculateNNstruct(struct collectionPointsOfIndex *collectionOfIndex,
	PRNearNeighborStructT &nnStruct);


void  myClasserOfNN(CvMat*  grayMatrix[GRAY_INDEX_NUMBER],//�������ص�ֵ
struct indexAndClassOfGray *tempgrayIndex[GRAY_INDEX_NUMBER],
	//struct arrayOfGrayIndex *arrayGrayIndex [GRAY_INDEX_NUMBER]
struct collectionPointsOfIndex  collectionOfIndex[GRAY_INDEX_NUMBER]
);


void myGetTwoBigInMatrix(CvMat* resultMatrix,int rows,
	double &max1,double &max2,int &bigcols);//������ֵ+���ֵ������


int  myGetNearNeibor(CvMat* resultMatrix,int rows,
	int &bigcols);

 

void guibing(struct indexAndClassOfGray* tempgrayIndex,int number,
	int c1,int c2);

void  myClasserOfNN_one(CvMat*  grayMatrix,//�������ص�ֵ
struct indexAndClassOfGray* tempgrayIndex,
struct collectionPointsOfIndex &collectionOfIndex //	struct arrayOfGrayIndex  *  (&arrayGrayIndex) )//ָ���Ӧ��
	);

 

void  featToGrayIndex(int numberOfLogo,int NUMBEROF_CENG,
struct logoFeatStruct*** logofeat,
	CvMat*  grayMatrix[GRAY_INDEX_NUMBER],//�������ص�ֵ
struct indexAndClassOfGray *grayIndex[GRAY_INDEX_NUMBER]
);

void seqToMatrix(CvSeq * tempOfGray,
	CvMat **grayOffeatMatrix ,struct indexAndClassOfGray **grayIndex,int numberforfeats);


void buidLogoSeq(int numberOfLogo,int NUMBEROF_CENG,
	CvSeq *** (&featseq),
	CvMemStorage*** (&storage),
	int **(&StatisticsOfPoins)
	);
void releaseLogoSeq(int numberOfLogo,int NUMBEROF_CENG,
	CvSeq *** (&featseq),
	CvMemStorage*** (&storage),
	int **(&StatisticsOfPoins)
	);


void getMaxRatioForMach(	int numberOfLogo,int NUMBEROF_CENG,
struct logoFeatStruct** logofeat,
	int **StatisticsOfPoins,
	CvSeq ***featseq,
	int *maxarry,int &maxSize 	);

struct arrayOfGrayIndex*   mySearchInIndex(PRNearNeighborStructT *nnStruct,
struct feature   feat,struct collectionPointsOfIndex *collectionOfIndex
	);


int zuiJingling(PPointT queryPoint,PPointT *result ,int resultSize);

#endif