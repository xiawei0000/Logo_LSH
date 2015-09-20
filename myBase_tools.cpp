

#include "myBase_tools.h"

int YOUHUA_TYPE;//1,2,3 对应 一个角  3个角 ，全图
const int NUMBEROF_CENG=8;//logo要多少-1 层
const double NUMBERFOR_RESIZE=1.3;//每层缩小几倍
const  int  SLIPER=10;//一行 滑动次数 =logo的10分之一
const int MIN_SLIPSIZE=5;  //最小滑动
const int SMALLIMAG_SIZE=25;//最小logo的大小
const double SIGOLIKELY=  0.15;//相似度的阀值
const double BIG_SIGOLIKELY=  1.5;//很相似的


const int MINNUMBER_MACHING=5;
int MAXLOG_SIZE=300;//logo最大值
const int MAXSIZE_FORPICTURE=800;//最大检测图片



const int MINNUMBER_FORFEAT=5;
//static char filename[300];放到文件内
int  chipNumber=1;
int numberForMachedPicture=0;
HANDLE  mutex ;  //用于互斥的保存文件
HANDLE  FilePathmutex;//用于文件目录
//HANDLE   SrcIndexmutex;//用于图片的索引 互斥访问
volatile long srcIndexInterlocked; //原子操作的变量

//const char nameForPictureChange[50]="_Mach_";
//#define NUMBERFORDIVISIOM 128
const int MAXNUMBER_LOGO=3;

//使用 1-cos 倒数的阈值
double YUZHI_COS_MIN=0.8;
double YUZHI_COS_MAX=0.98;//测试这个点
double YUZHI_COS=0.35;


//pca转换后的参数
int NUMBERFORDIVISIOM_PCA=20;
const double PART_PCA=0.98;//降维的力度
const double MINCOS_PCA=0.995;
// 按cos=？ 分类   =35  0.95= 18;  //换到了 -1 --1的范围里
  /*
 //第一种直接用cos的阈值
 const double YUZHI_COS=1.7;
 const double  YUZHI_COS_MAX=0.95;
 const double YUZHI_COS_MIN=0.80;*/
 
 long long   CharToLongLong(const char *numstr)
 {
	 long long i,j,result;
	 int len;
	 i=0;
	 result=0;
	 len=strlen(numstr);

	 for(i=0; i < len ; i++)
	 {
		 result=(numstr[i]-'0')+result*10 ;
	 }

	 return  result;
 }

 void longToChar(long n,char *numstr)
 {
	 int i,j;
	 i=0;
	 for(i=0;n/10;i++)
	 {
		 numstr[i]=n%10+'0';
		 n=n/10;
	 }
	 numstr[i]=n%10+'0';
	 numstr[i+1]=0;
	 strrev(numstr);
 }



void intToChar(int n,char *numstr)
{
	int i,j;
	i=0;
	for(i=0;n/10;i++)
	{
		numstr[i]=n%10+'0';
		n=n/10;
	}
	numstr[i]=n%10+'0';
	numstr[i+1]=0;
	strrev(numstr);
}

