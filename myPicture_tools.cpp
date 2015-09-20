#include "myPicture_tools.h"


IplImage* myReduceImage(IplImage* maximg)
{//���ͼƬ�ֱ��� ��ֻ���ǿ�ȣ�̫�ߣ��ǾͶ�ͼƬ��С
	IplImage* smalllimg;
	int smallImagesize=500;//���Ǵ�ͼ���ǾͶ���С��
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
{//���logo ͼƬ�ֱ��� ̫�ߣ��ǾͶ�ͼƬ��С ��󱣴浽·��
	IplImage* smalllimg;
	int TEMP_MAXLOG_SIZE=MAXLOG_SIZE*1.5;
	//һ�� �����ı�  �� ���߱Ƚ�max  
	if ( maximg->width > maximg->height )
	{//�ܿ��logo
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
			//����
			cvSaveImage(dstsrc,smalllimg,0);
			return smalllimg;
		}
	}

	else
	{//�ܳ���
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
			//����
			cvSaveImage(dstsrc,smalllimg,0);
			return smalllimg;
		}
	}
}


IplImage* myReduceLogo(IplImage* maximg)
{//���logo ͼƬ�ֱ��� ̫�ߣ��ǾͶ�ͼƬ��С
	IplImage* smalllimg;

	//һ�� �����ı�  �� ���߱Ƚ�max  
	if ( maximg->width > maximg->height )
	{//�ܿ��logo
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
	{//�ܳ���
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
	//����ͼƬ��ʽΪδ֪
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//��ʼ��FreeImage
	FreeImage_Initialise(0);


	//��ȡͼƬ��ʽ
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
{/* ��������ͼƬ ��������logosize
	*/
	int n1, k, i, m = 0;
	int endCeng=0;


	int iForPicture;
	int logoNumber;//�ڼ���logo

	char srcLogoPicturePath[_MAX_PATH];

	IplImage* img1 ;
	MAXLOG_SIZE=150;//��ʼ��Ϊ150

	for(iForPicture = 0; iForPicture< numberOfLogo ;iForPicture++)
	{//�̵߳�ÿһ��ͼ

		logoNumber=iForPicture;//logo��� ��0��ʼ��
		strcpy(srcLogoPicturePath,logo_srcIndex[logoNumber].srcIndex);	

		img1 = cvLoadImage( srcLogoPicturePath, 0 );	
		if (img1 ==NULL)
		{
			printf(" %s ��logo��ʧ ���벹��\n",srcLogoPicturePath);
			fclose(stdout);//�ر��ļ� 
			exit(1);
			//	continue ;//����û��ͼƬ
		}
		if (MAXLOG_SIZE < img1->height )
		{
			MAXLOG_SIZE = img1->height;
		}
		if (MAXLOG_SIZE < img1->width )
		{
			MAXLOG_SIZE = img1->width;
		}//������size


		cvReleaseImage(&img1);
	}//ÿһ��ͼ

}


int myGetImgQuarter(IplImage* **imgForPicture,IplImage* img2)
{/*�õ�ԭͼ��1/4 
		
 */
	int i=0,j;
	int Width,Height;
	CvRect roi ;

	Width=img2->width /4;
	Height=img2->height/4;

	if (Width > MAXLOG_SIZE)
	{//����̫���ͼ�� ����ķ�֮һ�� ����maxlog �Ǿ� ȡlogo��С��
		//̫С��ͼ ȡ�ķ�֮һ ��С�ˣ�  ����Ŵ� maxlogo �Ǿͷ���������
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
	cvResetImageROI(img2);//�������»�ԭ ����
	return 1;
}


int myGetImgFourQarters(IplImage** *imgForPicture,IplImage* img2)
{/*�õ�ԭͼ��1/4 
		
 */
	int i=0,j;
	int Width,Height;
	CvRect roi ;
	int location[4];
	Width=img2->width /4;
	Height=img2->height/4;

	
	if (Width > MAXLOG_SIZE)
	{//����̫���ͼ�� ����ķ�֮һ�� ����maxlog �Ǿ� ȡlogo��С��
		//̫С��ͼ ȡ�ķ�֮һ ��С�ˣ�  ����Ŵ� maxlogo �Ǿͷ���������
		Width= MAXLOG_SIZE;
	}
	if (Height >MAXLOG_SIZE)// && MAXLOG_SIZE<= img2->height
	{
		Height =MAXLOG_SIZE;
	}

	if (Height <  SMALLIMAG_SIZE   ||  Width < SMALLIMAG_SIZE)
	{//��� ����̫С�� ���Ǿ�ֻ Ҫһ���ǣ�����
		*imgForPicture=(IplImage**)malloc(sizeof( IplImage *)* 1);
		// ���Ͻ�
		roi=cvRect(0,0, Width,Height);
		cvSetImageROI(img2,roi);

		(*imgForPicture)[0] = cvCreateImage(cvSize(Width ,Height)	, img2->depth,img2->nChannels);
		cvCopy(img2, (*imgForPicture)[0],NULL);
		cvResetImageROI(img2);//�������»�ԭ ����
		return 1;
	}
	*imgForPicture=(IplImage**)malloc(sizeof( IplImage *)* 4);//����ռ�


	

	// ���Ͻ�
	roi=cvRect(0,0, Width,Height);
	cvSetImageROI(img2,roi);

	(*imgForPicture)[0] = cvCreateImage(cvSize(Width ,Height)	, img2->depth,img2->nChannels);
	cvCopy(img2, (*imgForPicture)[0],NULL);
	cvResetImageROI(img2);//�������»�ԭ ����



	// ���Ͻ� 
	roi=cvRect(img2->width- Width,0, Width,Height);
	cvSetImageROI(img2,roi);

	(*imgForPicture)[1] = cvCreateImage(cvSize(Width ,Height)	, img2->depth,img2->nChannels);
	cvCopy(img2, (*imgForPicture)[1],NULL);
	cvResetImageROI(img2);//�������»�ԭ ����

	// ���½�
	roi=cvRect(0,img2->height -Height , Width,Height);
	cvSetImageROI(img2,roi);
	 
	(*imgForPicture)[2] = cvCreateImage(cvSize(Width ,Height)	, img2->depth,img2->nChannels);
	cvCopy(img2, (*imgForPicture)[2],NULL);
	cvResetImageROI(img2);//�������»�ԭ ����

	// ���½�
	roi=cvRect(img2->width -Width ,img2->height -Height, Width,Height);
	cvSetImageROI(img2,roi);

	(*imgForPicture)[3] = cvCreateImage(cvSize(Width ,Height)	, img2->depth,img2->nChannels);
	cvCopy(img2, (*imgForPicture)[3],NULL);
	cvResetImageROI(img2);//�������»�ԭ ����

	return 4;
}


int myDivedeImg(IplImage** *imgForPicture,IplImage* img2)
{/*��ͼ�� ��ͼ��MAXSIZE_FORPICTURE width  height�� ��
 ���ǣ�location[0] =j*MAXSIZE_FORPICTURE ;
		location[1]= i*MAXSIZE_FORPICTURE ;

						location[3]=(i+1) *MAXSIZE_FORPICTURE +MAXLOG_SIZE;
						location[2]=(j+1) *MAXSIZE_FORPICTURE +MAXLOG_SIZE;
						4����
		
 */
	int i=0,j;
	int numberWidth,numberHeight;
	int kforpicture;
	int location[4];
	CvRect roi ;
	if (img2->width%MAXSIZE_FORPICTURE==0)
	{//����
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

	
				location[2]=(j+1) *MAXSIZE_FORPICTURE +MAXLOG_SIZE/2;//ֻҪһ���logo
				location[3]=(i+1) *MAXSIZE_FORPICTURE +MAXLOG_SIZE/2;
			
			
				if ( location[2] >img2->width )
				{//�����Ĵ�С 
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

				cvResetImageROI(img2);//�������»�ԭ ����

				//cvcloneû��roi������ ��cvcopy �У�������Ҫ�Ƚ���imge

			//	cvShowImage("",(*imgForPicture)[kforpicture]);
			//	cvWaitKey(0);
				kforpicture++;
		}
	}

	return numberWidth*numberHeight;
}



void drawTagForWindow(CvMat * tagMat,int x,int y,CvSize size)
{//��window�����е㻯Ϊ1
	int i,j;

	for(i=x;i<x+size.width  && i< tagMat->cols;i++)
	{//ͼ���У�x��ʾ��y�еĵ�x�У����µ����꣩
		for(j=y;j<y+size.height  && j< tagMat->rows;j++)
		{
			*(uchar*)CV_MAT_ELEM_PTR(*tagMat,j,i)=1;//������ ��һ����ʾ�У��ڶ�����ʾ�ڼ���
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

	cvNamedWindow( "�Ƚ�", 1 );
	cvShowImage(  "�Ƚ�", tempStacked );
	cvWaitKey(1);
	cvReleaseImage(&tempStacked);

}





void printMyImag(IplImage* img)
{
	int i,j;
	for(i=0;i<img->width;i++)
	{
		for (j=0;j<img->height;j++)
		{ //Ϊ�����3�����Ǹ�������������

			// fprintf( stderr, "%d",(int) CV_MAT_ELEM(*tagMat,int,i,j) );
			fprintf( stderr, "%d",(int) *(img->imageData+img->widthStep*i+j));

		}
		fprintf(stderr,"\n");
	}

}





IplImage* smallSizeImg( IplImage* img ,double nForSize)
{//��ͼƬ��СnForSize��֮һ
	CvSize c= cvSize((int)(img->width /nForSize), (int)(img->height /nForSize));
	IplImage* smaller = cvCreateImage(c,
		img->depth, img->nChannels );
	cvResize( img, smaller,CV_INTER_CUBIC);//Ч�����
	// CV_INTER_AREA );
	//cvSmooth(smaller,smaller,CV_MEDIAN,3,3,0,0);
	return smaller;
}

int Img1BigImg2(IplImage*img1,IplImage*img2)
{
	if (img1->width > img2->width || img1->height > img2->height)
	{
		return  1;//ͼƬ1����ͼ2
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
	//���ܼӺ���ı���
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






