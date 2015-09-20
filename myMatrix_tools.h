#ifndef MYMATRIXTOOLS_H
#define MYMATRIXTOOLS_H


#include "myBase_tools.h"
#include "myPicture_tools.h"
#include <stdio.h>  
#include <process.h>  



void myCaculateAbsOfVectorInMatrix(CvMat *src);


void myCaculateSelfCosValue(CvMat *src ,CvMat *(&dst)   ) ;


void caculateMO(CvMat * result);

void featToMatrixWithoutTranslate(CvMat *LogoMatrix,
struct feature *feat ,int numberOfFeat);

void mysaveMatrix(CvMat *result_result,char *filepath,int pices);

void myreadMatrix(CvMat **result_result,char *filepath,int pices);

void myreleaseLogoMatrix(CvMat  ***LogoMatrix,int numberOfLogo,int NUMBEROF_CENG);

void featToMatrix(CvMat *LogoMatrix,struct feature *feat,int n);

void LogoFeatStructToMatrix(CvMat *LogoMatrix,LogoFeatStruct logofeat);

void MatrixTranslateAndMO(CvMat *result,CvMat *resultT);


void printMyMatIndouble(CvMat* tagMat);

void printMyMat(CvMat* tagmat);


#endif