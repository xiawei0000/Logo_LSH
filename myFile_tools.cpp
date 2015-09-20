#include "myFile_tools.h"
#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>
#include <io.h>
  static char filename[300];



void myInitLogFile(char *dstTestPicturePath)
{/*
	得到当前目录，然后。/ 创建目录
	然后 + 时间 +.log
	创建文件
	*/


	time_t rawtime;
	struct tm * timeinfo;
	char strtime[300];

	char FatherFileName[_MAX_PATH];
	char *FatherFileName_type="Logo_log";
	char *logFileName="\\log";
		char *logFileName_type=".log";
	char FathrePath[_MAX_PATH];
	//	char childFile[_MAX_PATH];
	int len,i,j,k;

	//获取时间 精确到毫秒
	struct __timeb64 timebuffer;
	char millitmTime[100];//毫秒时间
//	char *timeline;
	
	

//得到文件夹

	strcpy(FatherFileName,FatherFileName_type);
	strcpy(FathrePath,FatherFileName);

	if (_access(FathrePath,0)  != -1)
	{//目录存在：
		;
	}
	else
	{//同步访问  
			_mkdir(FathrePath);//建立父目录完成
	
	}



	//增加时间戳
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	strcpy(strtime,asctime (timeinfo));
	timeTostring(strtime);

	_ftime64( &timebuffer );//获得时间函数的结构体，精确到毫秒
	//	timeline = _ctime64( & ( timebuffer.time ) );
	longToChar(timebuffer.millitm,millitmTime);
	strcat(strtime,"_");
	strcat(strtime,millitmTime);


	strcpy(dstTestPicturePath,FathrePath);
	strcat(dstTestPicturePath,logFileName);// Logo_log + /log
	strcat(dstTestPicturePath,strtime);// Logo_log + /log +时间
	strcat(dstTestPicturePath,logFileName_type);// Logo_log + /log +时间 + .log


}

void saveResult_Matched(int matchedNumber)
{
			FILE * savefile;
		
			if( !( savefile=fopen( filename,"a+") ) )
				{
					printf("打开文件错误 %s",filename);
					fclose(savefile);
					return ;
				}
		
			fprintf(savefile,"一共匹配到%d 个logo",matchedNumber);
			fclose(savefile);
			
}

void saveConsumeTime(double time)
{
	FILE * savefile;
	if( !( savefile=fopen( filename,"a+") ) )
	{
		printf("打开文件错误 %s",filename);
		fclose(savefile);
		return ;
	}

	fprintf(savefile,"总耗时 ：%.2lf 分钟",time);
	fclose(savefile);


}
static void timeTostring(char * time)
{//时间最后有个 10=回车
	int len;
	
	int i;
	len=strlen(time);
	for(i=0;i< len; i++)
	{
		if (time[i] ==' ' || time[i]== ':')
		{
			time[i]='_';
		}
		if (i ==len-1)
		{
			time[i]=0;

		}
	}


}
void ClearFile(char * name )
{	
	FILE * savefile;
//	char filename[200];
	char *filetype="save.txt";

	time_t rawtime;
	struct tm * timeinfo;
	char strtime[300];
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	strcpy(strtime,asctime (timeinfo));
	strcpy(filename,name);

	timeTostring(strtime);
	strcat(filename, strtime );
	strcat(filename,filetype);

	if( !( savefile=fopen( filename,"w") ) )
	{
		printf("打开文件错误 %s",filename);
		fclose(savefile);
		return ;
	}

	fputs("记录结果 \n \n",savefile);

	fclose(savefile);
}

void myCreateFatherFile(char *dstTestPicturePath)
{//取出 加入时间标记
	//找到路径的父目录，然后建立保存文件的目录，最后返回保存的路径
	//D:\360\Workspace\C++\LAB_work\babaoli_pictureLab\NEW_08\burberry\ ***_match.jpg
	/*
		第一步：后扫描，\，  复制到 【】中
		第二步：建立新目录： \burberry\Result_File_
	*/


	time_t rawtime;
	struct tm * timeinfo;
	char strtime[300];

	char FatherFileName[_MAX_PATH];
	char *FatherFileName_type="Picture_Result_File_";
	char FathrePath[_MAX_PATH];
	//	char childFile[_MAX_PATH];
	int len,i,j,k;

	//增加时间戳
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	strcpy(strtime,asctime (timeinfo));

	timeTostring(strtime);
	strcpy(FatherFileName,FatherFileName_type);
//	strcat(FatherFileName, strtime );//加入时间




	len=strlen(dstTestPicturePath);
	for (i=len-1;i >=0 ; i-- )
	{//得到父目录
		if (dstTestPicturePath[i] == '\\')
		{
			break;
		}
	}

	//复制父目录到FatherPaht
	for(j=0; j<= i; j++)
	{// / 也复制过去

		FathrePath[j]=dstTestPicturePath[j];
	}
	FathrePath[j]=0;
	strcat(FathrePath,FatherFileName);


	if (_access(FathrePath,0)  != -1)
	{//目录存在：
		;
	}
	else
	{//同步访问
		WaitForSingleObject(FilePathmutex, INFINITE);  
			_mkdir(FathrePath);//建立父目录完成
		ReleaseMutex(FilePathmutex); 
	
	}


	//i++;//跳过\  
	for (k=i,j=strlen(FathrePath);k< len;k++,j++)
	{
		FathrePath[j]=dstTestPicturePath[k];
	}
	FathrePath[j]=0;

	strcpy(dstTestPicturePath,FathrePath);

}


void logSigma(int d,double temp ,double temp_SIGOLIKELY )
{	
	FILE * savefile;
	//	char filename[200];
	char *filetype="save.txt";

	if( !( savefile=fopen( filename,"a+") ) )
	{
		printf("打开文件错误 %s",filename);
		fclose(savefile);
		return ;
	}
	
	fprintf(savefile,"	%3d %.3lf %.3lf ",d,temp,temp_SIGOLIKELY);
	fclose(savefile);
}

void logSigmaCompare(int d,double bili )
{	
	FILE * savefile;
	//	char filename[200];
	char *filetype="save.txt";

	if( !( savefile=fopen( filename,"a+") ) )
	{
		printf("打开文件错误 %s",filename);
		fclose(savefile);
		return ;
	}

	fprintf(savefile,"	%3d   %.3lf ",d,bili);
	fclose(savefile);
}


void saveTime(int i)
{
	double 	duration;
	FILE * savefile;

	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	//printf ( "\007The current date/time is: %s", asctime (timeinfo) );

	if( !( savefile=fopen( filename,"a+") ) )
	{
		printf("打开文件错误 %s",filename);
		fclose(savefile);
		return ;
	}
	if(i == -1)
	{	
		fprintf(savefile,"\n 结束检测 *****************\n 当前时间:");

	}
	else if( i==0){
		fprintf(savefile,"\n 开始检测*****************\n 当前时间:");
	}
	else  
	{
		fprintf(savefile,"\n 开始检测图片T%d    当前时间：",i);
	}

//	fputs("",savefile);
	fputs(asctime (timeinfo),savefile);
	fprintf(savefile,"\n");
	fclose(savefile);
}

void SaveAReturn(  )
{
	FILE * savefile;
	
	/*char filename[200];
	char *filetype="save.txt";

	strcpy(filename,name);
	strcat(filename,filetype);
	*/
	if( !( savefile=fopen( filename,"a+") ) )
	{
		printf("打开文件错误 %s",filename);
		fclose(savefile);
		return ;
	}

	fputs("\n",savefile);

	fclose(savefile);
}


void SaveLastResult( int j, int k, int i)
{//保存最后的匹配结果， 用于统计匹配了多少个， 多线程后 SaveResult被打乱， 就看这个吧
	FILE * savefile;	
	char pictureName[100];	
	char temp[100];


	if( !( savefile=fopen( filename,"a+") ) )
	{
		printf("打开文件错误 %s",filename);
		fclose(savefile);
		return ;
	}
	strcpy(pictureName,"\n测试图片T");

	intToChar(j,temp);
	strcat(pictureName,temp);

	if (k >0)
	{
		strcat(pictureName,"子图");
		intToChar(k,temp);
		strcat(pictureName,temp);
	}

	strcat(pictureName," 匹配LOGO ：");
	intToChar(i,temp);
	strcat(pictureName,temp);

	fputs(pictureName,savefile);
	fputs("\n",savefile);

	fclose(savefile);
}


void getFatherPath(char* FatherPath,char *childPath)
{//添加时间

	int len,i,j,k;
	len=strlen(childPath);
	for (i=len-1;i >=0 ; i-- )
	{//得到父目录
		if (childPath[i] == '\\')
		{
			break;
		}
	}
	//复制父目录到FatherPaht
	for(j=0; j<= i; j++)
	{// / 也复制过去

		FatherPath[j]=childPath[j];
	}
	FatherPath[j]=0;//结尾
}

void SaveResult( int j, int k, int i)
{
	FILE * savefile;	
	char pictureName[100];	
	char temp[100];

/*	char filename[200];
	char *filetype="save.txt";
	strcpy(filename,name);
	strcat(filename,filetype);
	*/

	if( !( savefile=fopen( filename,"a+") ) )
	{
		printf("打开文件错误 %s",filename);
		fclose(savefile);
		return ;
	}
	strcpy(pictureName,"\n测试图片T");

	intToChar(j,temp);
	strcat(pictureName,temp);

	if (k >0)
	{
		strcat(pictureName,"子图");
		intToChar(k,temp);
		strcat(pictureName,temp);
	}

	strcat(pictureName," 匹配LOGO ：");
	intToChar(i,temp);
	strcat(pictureName,temp);

	fputs(pictureName,savefile);
	fputs("\n",savefile);

	fclose(savefile);
}



void pathCatNumber(char * str,int n)
{//str是目录 要加上 n+ .jpg 
	const char * picture=".jpg";
	int i;
	int len=strlen(str);
	char  number[50];
	intToChar(n,number);
	strcat(str,number);
	strcat(str,picture);
/*	 
	for(i=0;i<strlen(picture);i++)
	{
		str[len++]=picture[i];
	}
	str[len]=0;
	*/
}

char * changepath(const char * srcTest,char *catstr ,char *src)
{//将dst插入到src中
	int i=0,j;

	const int LENGOF_END=4;
	int strend=0;
	int leng;

	strcpy(src,srcTest);
	leng=strlen(catstr);
	i=strlen(srcTest); //得到字符串结尾
	i-=LENGOF_END;//.jpg 4个字符
	strend=i;

	
	for (j=0;j<leng;j++)
	{
		src[strend+j]=catstr[j];
	}
	for (j=0;j<LENGOF_END;j++)
	{
		src[strend+leng+j]=srcTest[strend+j];
	}

	src[strend+leng+LENGOF_END]=0;
	return src;
}


