#ifndef  MAIN_H
#define  MAIN_H


#include "mysql_main.h"
#include "mach.h"
#include "stdlib.h"
#include"axutil_getopt_windows.h"
#include "mach_index.h"
extern int optind ;
extern int opterr ;
extern int optopt;
extern  char *optarg;

#define OPTIONS "l:L:u:U:w:W:d:D:p:P:i:I:Y:y:c:C:N:01"

/*
//l localhost  u user w password d db  p port i projectid y youhua c chipnumber  n numberofIndex
extern int   NUMBER_URLBASEINDEX;// һ�δ�����ٸ�ͼ
extern int YOUHUA_TYPE;//1,2,3 ��Ӧ һ����  3���� ��ȫͼ

//#include "..\include\vld.h"
//#pragma comment(lib,"vld.lib")

//extern  int find_Drawcontours (IplImage *src,IplImage *dst);
extern const int NUMBEROF_CENG;//logoҪ����-1 ��

extern  const int MINNUMBER_MACHING;
extern const int MINNUMBER_FORFEAT;

extern const char nameForPictureChange[50];
extern  int numberForMachedPicture;
extern  int  chipNumber;//�ɱ�
extern 	HANDLE  mutex;
extern 	HANDLE  FilePathmutex;
extern 	volatile long srcIndexInterlocked;
*/

extern int CMDORLOG;//�����cmd����log�汾
static void usage( char* );
static void arg_parse( int argc, char** argv ,	
	int *YOUHUA_TYPE, char *host,
	char *user,char * passwd,char *db,unsigned int *port,
	long long *idProgect,int *chipNumber,
	int *NUMBER_URLBASEINDEX,int *CMDORLOG);

#endif