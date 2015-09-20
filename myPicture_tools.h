#ifndef MYPICTURE_TOOLS
#define MYPICTURE_TOOLS

#include "myBase_tools.h"
#include <cv.h>  
#include <highgui.h>  
#include "FreeImage.h"  


IplImage* myLoadImage(char*	filename,int tag);

IplImage* myReduceImage(IplImage* maximg);
IplImage* myReduceLogo(IplImage* maximg);
IplImage* myReduceLogoAndSave(IplImage* maximg,char *dstsrc);

int myIsGif(char*	filename);

IplImage*  gif2ipl(const char* filename)  ;

void myGetMaxLogoSize(
	srcIndexStruct *logo_srcIndex,
	int numberOfLogo);

int myGetImgQuarter(IplImage* **imgForPicture,IplImage* img2);

int myGetImgFourQarters(IplImage** *imgForPicture,IplImage* img2);

int myDivedeImg(IplImage** *imgForPicture,IplImage* img2);


void drawTagForWindow(CvMat * tagMat,int x,int y,CvSize size);

void myDrawPoint(CvSeq *featseq,
	int numberOfMached,IplImage* stacked,int height);




void printMyImag(IplImage* img);






IplImage* smallSizeImg( IplImage* img ,double nForSize);



int Img1BigImg2(IplImage*img1,IplImage*img2);


int isTooSmall(IplImage*img1);


int  find_Drawcontours (IplImage *src,IplImage *dst);

int compareIsEqualFeat2(MachedFeatPoint * machedstruct,MachedFeatPoint * machedstructTemp);

int compareIsEqual(MachedFeatPoint * machedstruct,MachedFeatPoint * machedstructTemp);

#endif