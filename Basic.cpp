
#include "Basic.h"

void usage(char *programName){
	printf("Usage: %s dimension max_size #_of_points R probability_of_success\n", programName);
}

int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;

	GetLocalTime(&wtm);
	tm.tm_year     = wtm.wYear - 1900;
	tm.tm_mon     = wtm.wMonth - 1;
	tm.tm_mday     = wtm.wDay;
	tm.tm_hour     = wtm.wHour;
	tm.tm_min     = wtm.wMinute;
	tm.tm_sec     = wtm.wSecond;
	tm. tm_isdst    = -1;
	clock = mktime(&tm);
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;
	return (0);
}
long long   atoll(const char *numstr)
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