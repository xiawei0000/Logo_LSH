#ifndef MYBASE_TOOLS_H
#define MYBASE_TOOLS_H


#include "mysql.h"
#include <windows.h>
#include <windows.h>  
#include <direct.h>
#include "FreeImage.h"  

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include "imgfeatures.h"

#include <math.h>

#include<string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>  
#include <process.h>  

#define  GRAY_INDEX_NUMBER 1
#define  GRAY_INDEX_PICE 256

 extern int YOUHUA_TYPE;//1,2,3 ��Ӧ һ����  3���� ��ȫͼ
extern const int NUMBEROF_CENG;//logoҪ����-1 ��
extern const double NUMBERFOR_RESIZE;//ÿ����С����
extern const  int  SLIPER;//һ�� �������� =logo��10��֮һ
extern const int MIN_SLIPSIZE;  //��С����
extern const int SMALLIMAG_SIZE;//��Сlogo�Ĵ�С
extern const double SIGOLIKELY;//���ƶȵķ�ֵ
extern const double BIG_SIGOLIKELY;//�����Ƶ�


extern const int MINNUMBER_MACHING;
extern int MAXLOG_SIZE;//logo���ֵ
extern const int MAXSIZE_FORPICTURE;//�����ͼƬ



extern const int MINNUMBER_FORFEAT;
//static char filename[300];�ŵ��ļ���
extern int  chipNumber;
extern int numberForMachedPicture;
extern HANDLE  mutex ;  //���ڻ���ı����ļ�
extern HANDLE  FilePathmutex;//�����ļ�Ŀ¼
//HANDLE   SrcIndexmutex;//����ͼƬ������ �������
extern volatile long srcIndexInterlocked; //ԭ�Ӳ����ı���

//const char nameForPictureChange[50]="_Mach_";
//extern const int NUMBERFORDIVISIOM;

#define NUMBERFORDIVISIOM 128


extern const int MAXNUMBER_LOGO;

//ʹ�� 1-cos ��������ֵ
extern double YUZHI_COS_MIN;
extern double YUZHI_COS_MAX;//���������
extern double YUZHI_COS;


extern int NUMBERFORDIVISIOM_PCA;
extern const double PART_PCA;
extern const double MINCOS_PCA;//=35  0.95= 18;  //������ -1 --1�ķ�Χ��



typedef struct logoFeatStruct{//ԭʼ�ṹ
	struct feature * feat1;
	int numberOffeat;
}LogoFeatStruct;

struct FeatIdCeng{//�Ҷ�����
	struct feature feat;
	int logonumber;//��logofeatstuct����ţ�
	int ceng;
	int  featorder;
};


typedef struct machedFeatPoint{
	CvPoint machedPointFeat1;
	CvPoint machedPointFeat2;
}MachedFeatPoint;
typedef struct SrcIndexSturct{
	char * srcIndex;//������ַ
	int id;//��ַ��Ӧ��id
}srcIndexStruct;


 long long   CharToLongLong(const char *numstr);
void intToChar(int n,char *numstr);
 void longToChar(long n,char *numstr);

 #endif