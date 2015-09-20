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

 extern int YOUHUA_TYPE;//1,2,3 对应 一个角  3个角 ，全图
extern const int NUMBEROF_CENG;//logo要多少-1 层
extern const double NUMBERFOR_RESIZE;//每层缩小几倍
extern const  int  SLIPER;//一行 滑动次数 =logo的10分之一
extern const int MIN_SLIPSIZE;  //最小滑动
extern const int SMALLIMAG_SIZE;//最小logo的大小
extern const double SIGOLIKELY;//相似度的阀值
extern const double BIG_SIGOLIKELY;//很相似的


extern const int MINNUMBER_MACHING;
extern int MAXLOG_SIZE;//logo最大值
extern const int MAXSIZE_FORPICTURE;//最大检测图片



extern const int MINNUMBER_FORFEAT;
//static char filename[300];放到文件内
extern int  chipNumber;
extern int numberForMachedPicture;
extern HANDLE  mutex ;  //用于互斥的保存文件
extern HANDLE  FilePathmutex;//用于文件目录
//HANDLE   SrcIndexmutex;//用于图片的索引 互斥访问
extern volatile long srcIndexInterlocked; //原子操作的变量

//const char nameForPictureChange[50]="_Mach_";
//extern const int NUMBERFORDIVISIOM;

#define NUMBERFORDIVISIOM 128


extern const int MAXNUMBER_LOGO;

//使用 1-cos 倒数的阈值
extern double YUZHI_COS_MIN;
extern double YUZHI_COS_MAX;//测试这个点
extern double YUZHI_COS;


extern int NUMBERFORDIVISIOM_PCA;
extern const double PART_PCA;
extern const double MINCOS_PCA;//=35  0.95= 18;  //换到了 -1 --1的范围里



typedef struct logoFeatStruct{//原始结构
	struct feature * feat1;
	int numberOffeat;
}LogoFeatStruct;

struct FeatIdCeng{//灰度索引
	struct feature feat;
	int logonumber;//在logofeatstuct的序号；
	int ceng;
	int  featorder;
};


typedef struct machedFeatPoint{
	CvPoint machedPointFeat1;
	CvPoint machedPointFeat2;
}MachedFeatPoint;
typedef struct SrcIndexSturct{
	char * srcIndex;//单个地址
	int id;//地址对应的id
}srcIndexStruct;


 long long   CharToLongLong(const char *numstr);
void intToChar(int n,char *numstr);
 void longToChar(long n,char *numstr);

 #endif