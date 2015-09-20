#include "myPicture_tools.h"


IplImage* myReduceImage(IplImage* maximg)
{//如果图片分辨率 （只考虑宽度）太高，那就对图片缩小
	IplImage* smalllimg;
	int smallImagesize=500;//若是大图，那就多缩小；
	if ( maximg->width <= MAXSIZE_FORPICTURE)
	{
		return  maximg;
	}
	else
	{
		smalllimg=cvCreateImage
			(cvSize(smallImagesize,smallImagesize*maximg->height/maximg->width ),
			maximg->depth,maximg->nChannels);
		cvResize(maximg,smalllimg,1);
		cvReleaseImage(&maximg);
		return smalllimg;
	}
}

IplImage* myReduceLogoAndSave(IplImage* maximg,char *dstsrc)
{//如果logo 图片分辨率 太高，那就对图片缩小 最后保存到路径
	IplImage* smalllimg;
	int TEMP_MAXLOG_SIZE=MAXLOG_SIZE*1.5;
	//一： 找最大的边  二 最大边比较max  
	if ( maximg->width > maximg->height )
	{//很宽的logo
		if ( maximg->width <= TEMP_MAXLOG_SIZE )
		{
			return  maximg;		
		}
		else
		{
			smalllimg=cvCreateImage
				(cvSize(MAXLOG_SIZE,MAXLOG_SIZE*maximg->height/maximg->width ),
				maximg->depth,maximg->nChannels);
			cvResize(maximg,smalllimg,1);
			cvReleaseImage(&maximg);
			//保存
			cvSaveImage(dstsrc,smalllimg,0);
			return smalllimg;
		}
	}

	else
	{//很长的
		if ( maximg->height <= TEMP_MAXLOG_SIZE )
		{
			return  maximg;		
		}
		else
		{
			smalllimg=cvCreateImage
				(cvSize(MAXLOG_SIZE*maximg->width/maximg->height,MAXLOG_SIZE ),
				maximg->depth,maximg->nChannels);
			cvResize(maximg,smalllimg,1);
			cvReleaseImage(&maximg);
			//保存
			cvSaveImage(dstsrc,smalllimg,0);
			return smalllimg;
		}
	}
}


IplImage* myReduceLogo(IplImage* maximg)
{//如果logo 图片分辨率 太高，那就对图片缩小
	IplImage* smalllimg;

	//一： 找最大的边  二 最大边比较max  
	if ( maximg->width > maximg->height )
	{//很宽的logo
		if ( maximg->width <= MAXLOG_SIZE )
		{
			return  maximg;		
		}
		else
		{
			smalllimg=cvCreateImage
				(cvSize(MAXLOG_SIZE,MAXLOG_SIZE*maximg->height/maximg->width ),
				maximg->depth,maximg->nChannels);
			cvResize(maximg,smalllimg,1);
			cvReleaseImage(&maximg);
		
			return smalllimg;
		}
	}

	else
	{//很长的
		if ( maximg->height <= MAXLOG_SIZE )
		{
			return  maximg;		
		}
		else
		{
			smalllimg=cvCreateImage
				(cvSize(MAXLOG_SIZE*maximg->width/maximg->height,MAXLOG_SIZE ),
				maximg->depth,maximg->nChannels);
			cvResize(maximg,smalllimg,1);
			cvReleaseImage(&maximg);
			return smalllimg;
		}

	}
	
}

IplImage* myLoadImage(char*	filename,int tag)
{
	if (myIsGif(filename))
	{
		return gif2ipl(filename);
	}
	else
		return cvLoadImage( filename, tag );
}

int myIsGif(char*	filename)
{
	//定义图片格式为未知
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//初始化FreeImage
	FreeImage_Initialise(0);


	//获取图片格式
	fif = FreeImage_GetFileType(filename,0);
	if(fif!=FIF_GIF )
	{
			FreeImage_DeInitialise();  
		//printf("load error!\n");
		return 0;
	}

		FreeImage_DeInitialise();  
	return 1;
}


IplImage*  gif2ipl(const char* filename)  
{  

	int i,j;
	int width,height;
	FREE_IMAGE_FORMAT fif = FIF_GIF;  
	FIBITMAP* fiBmp ;
	RGBQUAD* ptrPalette;

	BYTE intens;  
	BYTE* pIntensity;  

	char* ptrImgDataPerLine;
	IplImage* iplImg ;

	FreeImage_Initialise(0);         //load the FreeImage function lib  
	//	FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(memory);


	 fiBmp = FreeImage_Load(fif,filename,GIF_DEFAULT);  

	//  FIBITMAPINFO fiBmpInfo = getfiBmpInfo(fiBmp);  

	 width = FreeImage_GetWidth(fiBmp);;  
	 height = FreeImage_GetHeight(fiBmp);  
	//  int depth = fiBmpInfo.depth;  
	//  int widthStep = (width*depth+31)/32*4;  
	//  size_t imgDataSize = widthStep*height*3;  

	ptrPalette = FreeImage_GetPalette(fiBmp);  

	pIntensity = &intens;  

	iplImg = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,3);  

	iplImg->origin = 1;//should set to 1-top-left structure(Windows bitmap style)  

	for ( i=0;i<height;i++)  
	{  
		 ptrImgDataPerLine = iplImg->imageData + i*iplImg->widthStep;  

		for( j=0;j<width;j++)  
		{  
			//get the pixel index   
			FreeImage_GetPixelIndex(fiBmp,j,i,pIntensity);    

			ptrImgDataPerLine[3*j] = ptrPalette[intens].rgbBlue;  
			ptrImgDataPerLine[3*j+1] = ptrPalette[intens].rgbGreen;  
			ptrImgDataPerLine[3*j+2] = ptrPalette[intens].rgbRed;  
		}  
	}  

	FreeImage_Unload(fiBmp);  
	FreeImage_DeInitialise();                 

	return iplImg;  
}  



void myGetMaxLogoSize(
	srcIndexStruct *logo_srcIndex,
	int numberOfLogo)
{/* 读入所有图片 计算最大的logosize
	*/
	int n1, k, i, m = 0;
	int endCeng=0;


	int iForPicture;
	int logoNumber;//第几个logo

	char srcLogoPicturePath[_MAX_PATH];

	IplImage* img1 ;
	MAXLOG_SIZE=150;//初始化为150

	for(iForPicture = 0; iForPicture< numberOfLogo ;iForPicture++)
	{//线程的每一张图

		logoNumber=iForPicture;//logo编号 从0开始吧
		strcpy(srcLogoPicturePath,logo_srcIndex[logoNumber].srcIndex);	

		img1 = cvLoadImage( srcLogoPicturePath, 0 );	
		if (img1 ==NULL)
		{
			printf(" %s 的logo丢失 ，请补上\n",srcLogoPicturePath);
			fclose(stdout);//关闭文件 
			exit(1);
			//	continue ;//可能没有图片
		}
		if (MAXLOG_SIZE < img1->height )
		{
			MAXLOG_SIZE = img1->height;
		}
		if (MAXLOG_SIZE < img1->width )
		{
			MAXLOG_SIZE = img1->width;
		}//获得最大size


		cvReleaseImage(&img1);
	}//每一个图

}


int myGetImgQuarter(IplImage* **imgForPicture,IplImage* img2)
{/*得到原图的1/4 
		
 */
	int i=0,j;
	int Width,Height;
	CvRect roi ;

	Width=img2->width /4;
	Height=img2->height/4;

	if (Width > MAXLOG_SIZE)
	{//对于太大的图， 如果四分之一都 大于maxlog 那就 取logo大小。
		//太小的图 取四分之一 更小了，  如果放大到 maxlogo 那就反而更慢了
		Width= MAXLOG_SIZE;
	}
	if (Height >MAXLOG_SIZE)// && MAXLOG_SIZE<= img2->height
	{
		Height =MAXLOG_SIZE;
	}

	roi=cvRect(0,0, Width,Height);
	cvSetImageROI(img2,roi);
	*imgForPicture=(IplImage**)malloc(sizeof( IplImage *)* 1);
	(*imgForPicture)[0] = cvCreateImage(cvSize(Width ,Height)	, img2->depth,img2->nChannels);
	cvCopy(img2, (*imgForPicture)[0],NULL);
	cvResetImageROI(img2);//必须重新还原 区域
	return 1;
}


int myGetImgFourQarters(IplImage** *imgForPicture,IplImage* img2)
{/*得到原图的1/4 
		
 */
	int i=0,j;
	int Width,Height;
	CvRect roi ;
	int location[4];
	Width=img2->width /4;
	Height=img2->height/4;

	
	if (Width > MAXLOG_SIZE)
	{//对于太大的图， 如果四分之一都 大于maxlog 那就 取logo大小。
		//太小的图 取四分之一 更小了，  如果放大到 maxlogo 那就反而更慢了
		Width= MAXLOG_SIZE;
	}
	if (Height >MAXLOG_SIZE)// && MAXLOG_SIZE<= img2->height
	{
		Height =MAXLOG_SIZE;
	}

	if (Height <  SMALLIMAG_SIZE   ||  Width < SMALLIMAG_SIZE)
	{//如果 区域太小了 ，那就只 要一个角，返回
		*imgForPicture=(IplImage**)malloc(sizeof( IplImage *)* 1);
		// 左上角
		roi=cvRect(0,0, Width,Height);
		cvSetImageROI(img2,roi);

		(*imgForPicture)[0] = cvCreateImage(cvSize(Width ,Height)	, img2->depth,img2->nChannels);
		cvCopy(img2, (*imgForPicture)[0],NULL);
		cvResetImageROI(img2);//必须重新还原 区域
		return 1;
	}
	*imgForPicture=(IplImage**)malloc(sizeof( IplImage *)* 4);//申请空间


	

	// 左上角
	roi=cvRect(0,0, Width,Height);
	cvSetImageROI(img2,roi);

	(*imgForPicture)[0] = cvCreateImage(cvSize(Width ,Height)	, img2->depth,img2->nChannels);
	cvCopy(img2, (*imgForPicture)[0],NULL);
	cvResetImageROI(img2);//必须重新还原 区域



	// 右上角 
	roi=cvRect(img2->width- Width,0, Width,Height);
	cvSetImageROI(img2,roi);

	(*imgForPicture)[1] = cvCreateImage(cvSize(Width ,Height)	, img2->depth,img2->nChannels);
	cvCopy(img2, (*imgForPicture)[1],NULL);
	cvResetImageROI(img2);//必须重新还原 区域

	// 左下角
	roi=cvRect(0,img2->height -Height , Width,Height);
	cvSetImageROI(img2,roi);
	 
	(*imgForPicture)[2] = cvCreateImage(cvSize(Width ,Height)	, img2->depth,img2->nChannels);
	cvCopy(img2, (*imgForPicture)[2],NULL);
	cvResetImageROI(img2);//必须重新还原 区域

	// 右下角
	roi=cvRect(img2->width -Width ,img2->height -Height, Width,Height);
	cvSetImageROI(img2,roi);

	(*imgForPicture)[3] = cvCreateImage(cvSize(Width ,Height)	, img2->depth,img2->nChannels);
	cvCopy(img2, (*imgForPicture)[3],NULL);
	cvResetImageROI(img2);//必须重新还原 区域

	return 4;
}


int myDivedeImg(IplImage** *imgForPicture,IplImage* img2)
{/*分图： 大图按MAXSIZE_FORPICTURE width  height都 。
 都是：location[0] =j*MAXSIZE_FORPICTURE ;
		location[1]= i*MAXSIZE_FORPICTURE ;

						location[3]=(i+1) *MAXSIZE_FORPICTURE +MAXLOG_SIZE;
						location[2]=(j+1) *MAXSIZE_FORPICTURE +MAXLOG_SIZE;
						4个点
		
 */
	int i=0,j;
	int numberWidth,numberHeight;
	int kforpicture;
	int location[4];
	CvRect roi ;
	if (img2->width%MAXSIZE_FORPICTURE==0)
	{//整除
		numberWidth=img2->width/MAXSIZE_FORPICTURE ;
	}
	else
		{
			numberWidth=img2->width/MAXSIZE_FORPICTURE +1;
	}

	if (img2->height% MAXSIZE_FORPICTURE==0)
	{
			numberHeight=img2->height/ MAXSIZE_FORPICTURE;
	}
	else{
	numberHeight=img2->height/ MAXSIZE_FORPICTURE +1;
	}


	*imgForPicture=(IplImage**)malloc(sizeof( IplImage *)* numberWidth*numberHeight);
	
	kforpicture=0;
	for (i=0;i<numberHeight ;i++)
	{
		for (j=0;j< numberWidth ;j++)
		{
				location[0] =j*MAXSIZE_FORPICTURE ;
				location[1]= i*MAXSIZE_FORPICTURE ;

	
				location[2]=(j+1) *MAXSIZE_FORPICTURE +MAXLOG_SIZE/2;//只要一半的logo
				location[3]=(i+1) *MAXSIZE_FORPICTURE +MAXLOG_SIZE/2;
			
			
				if ( location[2] >img2->width )
				{//超过的大小 
					location[2]=img2->width;
				}
				if (location[3] >img2->height)
				{
					location[3]=img2->height;
				}

				roi=cvRect(location[0],location[1], 
					location[2]-location[0] ,location[3]-location[1]);
				cvSetImageROI(img2,roi);

				(*imgForPicture)[kforpicture]=cvCreateImage(cvSize(location[2]-location[0] ,location[3]-location[1])
					, img2->depth,img2->nChannels);
				cvCopy(img2,(*imgForPicture)[kforpicture],NULL);

				cvResetImageROI(img2);//必须重新还原 区域

				//cvclone没有roi的作用 ，cvcopy 有，但是需要先建立imge

			//	cvShowImage("",(*imgForPicture)[kforpicture]);
			//	cvWaitKey(0);
				kforpicture++;
		}
	}

	return numberWidth*numberHeight;
}



void drawTagForWindow(CvMat * tagMat,int x,int y,CvSize size)
{//将window内所有点化为1
	int i,j;

	for(i=x;i<x+size.width  && i< tagMat->cols;i++)
	{//图像中，x表示第y行的第x列（向下的坐标）
		for(j=y;j<y+size.height  && j< tagMat->rows;j++)
		{
			*(uchar*)CV_MAT_ELEM_PTR(*tagMat,j,i)=1;//矩阵中 第一个表示行，第二个表示第几列
		}

	}
}

void myDrawPoint(CvSeq *featseq,int numberOfMached,IplImage* stacked,int height)
{
	MachedFeatPoint *machedstruct;
	int k;
	CvPoint pt1,pt2;
	IplImage* tempStacked;


	tempStacked=cvCloneImage(stacked);


	for (k=0; k<numberOfMached; k++)
	{
		machedstruct=(MachedFeatPoint *)cvGetSeqElem(featseq,k);

		pt1	 = machedstruct->machedPointFeat1;
		pt2 =  machedstruct->machedPointFeat2;
		pt2.y += height;
		cvLine( tempStacked, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );
	}

	cvNamedWindow( "比较", 1 );
	cvShowImage(  "比较", tempStacked );
	cvWaitKey(1);
	cvReleaseImage(&tempStacked);

}





void printMyImag(IplImage* img)
{
	int i,j;
	for(i=0;i<img->width;i++)
	{
		for (j=0;j<img->height;j++)
		{ //为何最后3行总是负数？？？？？

			// fprintf( stderr, "%d",(int) CV_MAT_ELEM(*tagMat,int,i,j) );
			fprintf( stderr, "%d",(int) *(img->imageData+img->widthStep*i+j));

		}
		fprintf(stderr,"\n");
	}

}





IplImage* smallSizeImg( IplImage* img ,double nForSize)
{//将图片缩小nForSize分之一
	CvSize c= cvSize((int)(img->width /nForSize), (int)(img->height /nForSize));
	IplImage* smaller = cvCreateImage(c,
		img->depth, img->nChannels );
	cvResize( img, smaller,CV_INTER_CUBIC);//效果最好
	// CV_INTER_AREA );
	//cvSmooth(smaller,smaller,CV_MEDIAN,3,3,0,0);
	return smaller;
}

int Img1BigImg2(IplImage*img1,IplImage*img2)
{
	if (img1->width > img2->width || img1->height > img2->height)
	{
		return  1;//图片1大于图2
	}
	return 0;

}
int isTooSmall(IplImage*img1)
{

	if (img1->width<SMALLIMAG_SIZE ||  img1->height<SMALLIMAG_SIZE )
	{
		return 1;
	}
	return 0;

}




int  find_Drawcontours (IplImage *src,IplImage *dst)
{

	CvMemStorage *storage = cvCreateMemStorage(0);  
	CvSeq *seq = NULL;  
	int cnt ;	 
	cvThreshold(src, src,10, 255, CV_THRESH_BINARY);      
	//	cvShowImage("src",src);  

	cnt= cvFindContours(src,storage,&seq,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0)); // ,8,1,2,cvPoint(0,0)
	//不能加后面的变量
	printf("%d",cnt);
	for(;seq !=NULL;	seq = seq->h_next )
	{

		CvRect rect = cvBoundingRect(seq,1);  
		cvDrawContours(dst,seq,CV_RGB(255,0,0),CV_RGB(255,0,0),0,1,8,cvPoint(0,0));  
		cvRectangleR(dst,rect,CV_RGB(255,0,0),3,8,0);  
		//	cvShowImage("dst",dst);  
		//cvWaitKey(0);  

	}

	//cvWaitKey(0); 
	//cvReleaseImage(&src);  
	//cvReleaseImage(&dst);  
	cvReleaseMemStorage(&storage);    
	storage=NULL;
	return 0;
}






