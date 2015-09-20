#ifndef MYFILE_TOOLS
#define MYFILE_TOOLS

#include "myBase_tools.h"
   
void myInitLogFile(char *dstTestPicturePath);

void saveResult_Matched(int matchedNumber);

void saveConsumeTime(double time);


static void timeTostring(char * time);


void ClearFile(char * name );

void myCreateFatherFile(char *dstTestPicturePath);

void logSigma(int d,double temp ,
	double temp_SIGOLIKELY );

void logSigmaCompare(int d,double bili );


void saveTime(int i);

void SaveAReturn(  );


void SaveLastResult( int j, int k, int i);

void getFatherPath(char* FatherPath,char *childPath);

void SaveResult( int j, int k, int i);



void pathCatNumber(char * str,int n);

char * changepath(const char * srcTest,
	char *catstr ,char *src);

#endif