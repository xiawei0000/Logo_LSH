#ifndef  MAIN_INDEX_H
#define  MAIN_INDEX_H

#include "tools.h"
#include "NearNeighbors.h"
#include "mach_base.h"
//索引的操作

void printIndex(PRNearNeighborStructT nnStruct);

void ouputPictureData( struct feature *str,  int n);


void  outputOrignalData( struct collectionPointsOfIndex* collectionOfIndexP,
	double thresholdR,	//1计算所有点的平均距离 /3=thresholdR
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
	double &thresholdR,	//1计算所有点的平均距离 /3=thresholdR
	PPointT *(&dataSet),//2复制数据到dataset
	IntT nSampleQueries,
	PPointT *(&sampleQueries)//3随机产生查询点
	);

void  myCaculateNNstruct(struct collectionPointsOfIndex *collectionOfIndex,
	PRNearNeighborStructT &nnStruct);


void  myClasserOfNN(CvMat*  grayMatrix[GRAY_INDEX_NUMBER],//两个返回的值
struct indexAndClassOfGray *tempgrayIndex[GRAY_INDEX_NUMBER],
	//struct arrayOfGrayIndex *arrayGrayIndex [GRAY_INDEX_NUMBER]
struct collectionPointsOfIndex  collectionOfIndex[GRAY_INDEX_NUMBER]
);


void myGetTwoBigInMatrix(CvMat* resultMatrix,int rows,
	double &max1,double &max2,int &bigcols);//返回最值+最大值的索引


int  myGetNearNeibor(CvMat* resultMatrix,int rows,
	int &bigcols);

 

void guibing(struct indexAndClassOfGray* tempgrayIndex,int number,
	int c1,int c2);

void  myClasserOfNN_one(CvMat*  grayMatrix,//两个返回的值
struct indexAndClassOfGray* tempgrayIndex,
struct collectionPointsOfIndex &collectionOfIndex //	struct arrayOfGrayIndex  *  (&arrayGrayIndex) )//指针的应用
	);

 

void  featToGrayIndex(int numberOfLogo,int NUMBEROF_CENG,
struct logoFeatStruct*** logofeat,
	CvMat*  grayMatrix[GRAY_INDEX_NUMBER],//两个返回的值
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