#ifndef MACH_BASE_H
#define  MACH_BASE_H
//����mach�����нṹ��


#include "mysql_main.h"
#include <windows.h>  

#include "myBase_tools.h"
#include "tools.h"
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

struct indexAndClassOfGray{
	int logoid;
	int ceng;
	int featorder;
	int classvalue;
};

struct seqIndexAndClassOfGray{
	struct seqIndexAndClassOfGray *next ;
	struct indexAndClassOfGray  grayindex;
};

struct arrayOfGrayIndex{
	double point[NUMBERFORDIVISIOM];//���ĵ�
	int  pointsInClass;//���ĵ� ������ԭ�����
	struct seqIndexAndClassOfGray *index;
};


struct collectionPointsOfIndex{
	struct arrayOfGrayIndex*  arrayGrayIndex;// ���ĵ������
	int numberOfindex;//�ж��ٸ�����

};


struct tempStructForThread_LogoCaculate
{//���ڴ�����
	int numberOfLogo;
	srcIndexStruct *logo_srcIndex;
	LogoFeatStruct *** logofeat;
	int NUMBEROF_CENG;
	int threadNumber;
	IplImage*  *** allImgOfLogo ;

};
struct tempStructForThread_EachPicture{
	srcIndexStruct *Picture_srcIndex;
	srcIndexStruct *logo_srcIndex;
	int numberOfPicture;
	int numberOfLogo;
	LogoFeatStruct ** logofeat;
	CvMat  ***LogoMatrix;
	int threadNumber;
	PRNearNeighborStructT * nnStruct;
	struct collectionPointsOfIndex *collectionOfIndex;
	IplImage*** allImgOfLogo;
};





#endif