

#include "myBase_tools.h"

int YOUHUA_TYPE;//1,2,3 ��Ӧ һ����  3���� ��ȫͼ
const int NUMBEROF_CENG=8;//logoҪ����-1 ��
const double NUMBERFOR_RESIZE=1.3;//ÿ����С����
const  int  SLIPER=10;//һ�� �������� =logo��10��֮һ
const int MIN_SLIPSIZE=5;  //��С����
const int SMALLIMAG_SIZE=25;//��Сlogo�Ĵ�С
const double SIGOLIKELY=  0.15;//���ƶȵķ�ֵ
const double BIG_SIGOLIKELY=  1.5;//�����Ƶ�


const int MINNUMBER_MACHING=5;
int MAXLOG_SIZE=300;//logo���ֵ
const int MAXSIZE_FORPICTURE=800;//�����ͼƬ



const int MINNUMBER_FORFEAT=5;
//static char filename[300];�ŵ��ļ���
int  chipNumber=1;
int numberForMachedPicture=0;
HANDLE  mutex ;  //���ڻ���ı����ļ�
HANDLE  FilePathmutex;//�����ļ�Ŀ¼
//HANDLE   SrcIndexmutex;//����ͼƬ������ �������
volatile long srcIndexInterlocked; //ԭ�Ӳ����ı���

//const char nameForPictureChange[50]="_Mach_";
//#define NUMBERFORDIVISIOM 128
const int MAXNUMBER_LOGO=3;

//ʹ�� 1-cos ��������ֵ
double YUZHI_COS_MIN=0.8;
double YUZHI_COS_MAX=0.98;//���������
double YUZHI_COS=0.35;


//pcaת����Ĳ���
int NUMBERFORDIVISIOM_PCA=20;
const double PART_PCA=0.98;//��ά������
const double MINCOS_PCA=0.995;
// ��cos=�� ����   =35  0.95= 18;  //������ -1 --1�ķ�Χ��
  /*
 //��һ��ֱ����cos����ֵ
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

