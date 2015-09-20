/*
  Detects SIFT features in two images and finds matches between them.

  Copyright (C) 2006-2012  Rob Hess <rob@iqengines.com>

  @version 1.1.2-20100521
*/

#ifndef  _MYMACH_H
#define  _MYMACH_H


#include "mysql_main.h"
#include <windows.h>  

#include "myBase_tools.h"
#include "tools.h"
#include "mach_index.h"
#include "sift.h"
#include "imgfeatures.h"
#include "kdtree.h"
#include "utils.h"
#include "xform.h"
#include "tools.h"


#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <math.h>

#include <stdio.h>
#include<string.h>

#include<string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>  
#include <process.h>  
#include <windows.h>  
#include "FreeImage.h"  

#include "pca.h"

/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.35//0.49
extern 	volatile long srcIndexInterlocked;
#ifndef Mymin(x,y)
	#define Mymin(x,y) ( x<y? x:y)
#endif




void myreleaseFeatStruct(LogoFeatStruct *** logofeat,int numberOfLogo,int NUMBEROF_CENG );

void init_tempForThread_LogoCaculate(
struct tempStructForThread_LogoCaculate *tempstruct,
	srcIndexStruct *logo_srcIndex,LogoFeatStruct *** logofeat
	,int threadNumber,int NUMBEROF_CENG,int numberOfLogo,
	IplImage*  *** allImgOfLogo 
	);

void  CaculateLogoFeatFirst(
	srcIndexStruct *logo_srcIndex,
	LogoFeatStruct *** logofeat,
	int numberOfLogo,int NUMBEROF_CENG,
	CvMat * ***LogoMatrix ,
	IplImage*  *** allImgOfLogo//保存所有logo图片的数组
	);

unsigned int __stdcall MyThreadFun_LogoCaculate(PVOID pM)  ;


void  LogoFeatCaculate(	
	int numberOfLogo,int NUMBEROF_CENG,
	srcIndexStruct *logo_srcIndex,
struct logoFeatStruct*** logofeat
	) ;




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
	IplImage*** allImgOfLogo);



unsigned int __stdcall MyThreadFun_EachPicture(PVOID pM)  ;


int myJudgeIsMatch(	struct feature  * feat2,int n2	,
	PRNearNeighborStructT *nnStruct,
struct collectionPointsOfIndex collectionOfIndex[GRAY_INDEX_NUMBER],
	int numberOfLogo,int NUMBEROF_CENG,
struct logoFeatStruct** logofeat,
	IplImage*** allImgOfLogo,
	IplImage* img2
	);



int compareInTwoImgTwoFeats(IplImage*img1,IplImage*img2,IplImage*stacked,
struct feature* feat1,struct feature * feat2,CvSeq *featseq,
	int n1,int n2,int n3,//匹配的对个数
	double * maxFeatsim);





int machEachPicture(int numberForLogo,LogoFeatStruct ** logofeat,CvMat** tempMatrix,
	char* srcLogoPicturePath,IplImage * img2_orig, 
struct feature  * feat2,CvMat *PictureMatrix,
	int n2,char *dstTestPicturePath,
	double * maxCengsim);


int judgeFormPiont(struct feature* feat1,struct feature * feat2,CvSeq * featseq,
	int n1,int n2,int n3,
	IplImage*img1,double liangduImg1,IplImage * img2,IplImage*stacked ,
	CvPoint pointWindow1,CvPoint  pointWindow2,
	double * tempsim);

int sliperWindows(IplImage*img1,IplImage*img2,IplImage*stacked,
struct feature* feat1,struct feature * feat2,CvSeq *featseq,
	int n1,int n2,int n3,
	CvMat * tagMat,
	double * maxFeatsim);


int compareFeartureFirstTime( struct feature* feat1,CvMat * tempMatrixForLogo,
struct feature* feat2,CvMat *PictureMatrix,
	int n1,int n2,
	IplImage* img1,IplImage* stacked,	CvSeq *featseq) ;

int compareFearture( struct feature* feat1, struct feature* feat2, 
	int n1,int n2,IplImage* img1,IplImage* stacked,CvPoint pointWindow1,CvPoint pointWindow2);

int simOfFearture(int n1,int n2,int m,
	double huiduImag1,double  huiduImag2,
	double * tempsim);


#endif






