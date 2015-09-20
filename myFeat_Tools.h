#ifndef MYFEAT_TOOLS
#define MYFEAT_TOOLS

#include "myBase_tools.h"




int compareIsInWindow(MachedFeatPoint * machedstruct,
	CvPoint pointWindow1, CvPoint pointWindow2);



void reduceMachedPoints(CvSeq *featseq,int * numberOfMached );


int compareIsEqualFeat2(MachedFeatPoint * machedstruct,
	MachedFeatPoint * machedstructTemp);


int compareIsEqual(MachedFeatPoint * machedstruct,
	MachedFeatPoint * machedstructTemp);


void GetMachedWindowPoint(MachedFeatPoint* featTemp,
	CvPoint * p1,CvPoint * p2,CvSize img1size,CvSize img1size2 );




#endif
