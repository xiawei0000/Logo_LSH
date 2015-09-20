/*
  Functions for detecting SIFT image features.
  
  For more information, refer to:
  
  Lowe, D.  Distinctive image features from scale-invariant keypoints.
  <EM>International Journal of Computer Vision, 60</EM>, 2 (2004),
  pp.91--110.
  
  Copyright (C) 2006-2012  Rob Hess <rob@iqengines.com>

  Note: The SIFT algorithm is patented in the United States and cannot be
  used in commercial products without a license from the University of
  British Columbia.  For more information, refer to the file LICENSE.ubc
  that accompanied this distribution.

  @version 1.1.2-20100521
*/

#include "sift.h"
#include "imgfeatures.h"
#include "utils.h"

#include <cxcore.h>
#include <cv.h>
#include <malloc.h>
/************************* Local Function Prototypes *************************/

static IplImage* create_init_img( IplImage*, int, double );
static IplImage* convert_to_gray32( IplImage* );
static IplImage*** build_gauss_pyr( IplImage*, int, int, double );
static IplImage* downsample( IplImage* );
static IplImage*** build_dog_pyr( IplImage***, int, int );
static CvSeq* scale_space_extrema( IplImage***, int, int, double, int,
				   CvMemStorage*);
static int is_extremum( IplImage***, int, int, int, int );
static struct feature* interp_extremum( IplImage***, int, int, int, int, int,
					double);
static void interp_step( IplImage***, int, int, int, int, double*, double*,
			 double* );
static CvMat* deriv_3D( IplImage***, int, int, int, int );
static CvMat* hessian_3D( IplImage***, int, int, int, int );
static double interp_contr( IplImage***, int, int, int, int, double, double,
			    double );
static struct feature* new_feature( void );
static int is_too_edge_like( IplImage*, int, int, int );
static void calc_feature_scales( CvSeq*, double, int );
static void adjust_for_img_dbl( CvSeq* );
static void calc_feature_oris( CvSeq*, IplImage*** );
static double* ori_hist( IplImage*, int, int, int, int, double );
static int calc_grad_mag_ori( IplImage*, int, int, double*, double* );
static void smooth_ori_hist( double*, int );
static double dominant_ori( double*, int );
static void add_good_ori_features( CvSeq*, double*, int, double,
				   struct feature* );
static struct feature* clone_feature( struct feature* );
static void compute_descriptors( CvSeq*, IplImage***, int, int );

static void interp_hist_entry( double***, double, double, double, double, int,
			       int);
static void hist_to_descr( double***, int, int, struct feature* );
static void normalize_descr( struct feature* );
static int feature_cmp( void*, void*, void* );
static void release_descr_hist( double****, int );
static void release_pyr( IplImage****, int, int );


static double*** descr_hist( 
	IplImage* img, int r, int c, double ori,
	double scl, int d, int n,
struct feature* feat);
/*********************** Functions prototyped in sift.h **********************/




/**
   Finds SIFT features in an image using default parameter values.  All
   detected features are stored in the array pointed to by \a feat.

   @param img the image in which to detect features
   @param feat a pointer to an array in which to store detected features

   @return Returns the number of features stored in \a feat or -1 on failure
   @see _sift_features()
*/
int sift_features( IplImage* img, struct feature** feat )
{
	return _sift_features( img, feat, SIFT_INTVLS, SIFT_SIGMA, SIFT_CONTR_THR,
		SIFT_CURV_THR, SIFT_IMG_DBL, SIFT_DESCR_WIDTH,
		SIFT_DESCR_HIST_BINS );
}

int One_sift_features( IplImage* img, struct feature** feat ,int numberOfIntvls)
{
	return _sift_features( img, feat, numberOfIntvls, SIFT_SIGMA, SIFT_CONTR_THR,
		SIFT_CURV_THR, SIFT_IMG_DBL, SIFT_DESCR_WIDTH,
		SIFT_DESCR_HIST_BINS );
}

/**
   Finds SIFT features in an image using user-specified parameter values.  All
   detected features are stored in the array pointed to by \a feat.

   @param img the image in which to detect features
   @param fea a pointer to an array in which to store detected features
   @param intvls the number of intervals sampled per octave of scale space
   @param sigma the amount of Gaussian smoothing applied to each image level
     before building the scale space representation for an octave
   @param cont_thr a threshold on the value of the scale space function
     \f$\left|D(\hat{x})\right|\f$, where \f$\hat{x}\f$ is a vector specifying
     feature location and scale, used to reject unstable features;  assumes
     pixel values in the range [0, 1]
   @param curv_thr threshold on a feature's ratio of principle curvatures
     used to reject features that are too edge-like
   @param img_dbl should be 1 if image doubling prior to scale space
     construction is desired or 0 if not
   @param descr_width the width, \f$n\f$, of the \f$n \times n\f$ array of
     orientation histograms used to compute a feature's descriptor
   @param descr_hist_bins the number of orientations in each of the
     histograms in the array used to compute a feature's descriptor

   @return Returns the number of keypoints stored in \a feat or -1 on failure
   @see sift_keypoints()
*/
int _sift_features( IplImage* img, struct feature** feat, int intvls,
		    double sigma, double contr_thr, int curv_thr,
		    int img_dbl, int descr_width, int descr_hist_bins )
{
	/*img为输入图像；
	feat为所要提取的特征指针；
	intvl指的是高斯金字塔和差分金字塔的层数；
	sigma指的是图像初始化过程中高斯模糊所使用的参数；
	contr_thr是归一化之后的去除不稳定特征的阈值；
	curv_thr指的是去除边缘的特征的主曲率阈值；
	img_dbl是是否将图像放大为之前的两倍；
	descr_with用来计算特征描述子的方向直方图的宽度；
	descr_hist_bins是直方图中的条数
*/
  IplImage* init_img;
  IplImage*** gauss_pyr, *** dog_pyr;
  CvMemStorage* storage;
  CvSeq* features;
  int octvs, i, n = 0;
  
  /* check arguments */
  if( ! img )
    fatal_error( "NULL pointer error, %s, line %d",  __FILE__, __LINE__ );
  if( ! feat )
    fatal_error( "NULL pointer error, %s, line %d",  __FILE__, __LINE__ );

  /* build scale space pyramid; smallest dimension of top level is ~4 pixels */
  init_img = create_init_img( img, img_dbl, sigma );//得到灰度图像

  //octvs = log( MIN( init_img->width, init_img->height ) ) / log(2) - 2;//大层的数目 有矩阵的 min(m n)决定
  octvs=1;
  gauss_pyr = build_gauss_pyr( init_img, octvs, intvls, sigma );//建立了高斯金字塔 每层图片小四分之一

  dog_pyr = build_dog_pyr( gauss_pyr, octvs, intvls );//建立DOG金字塔
  
  storage = cvCreateMemStorage( 0 );
   
  //极值是dog上的
  //尺度空间极值点检测  
  features = scale_space_extrema( dog_pyr, octvs, intvls, contr_thr,
				  curv_thr, storage );

  //特征点是高斯上的
   // 计算每个特征点的尺度  放入feat的scl  和data-> scl_octv
  calc_feature_scales( features, sigma, intvls );

  if( img_dbl )/*原图是放大2倍的 最好缩小特征值*/
    adjust_for_img_dbl( features );

   //计算特征的主  辅助 方向   ：是基于当前层的* 缩放的比例
  calc_feature_oris( features, gauss_pyr );

    //计算描述子，包括计算二维方向直方图和转换其为特征描述子   ：是基于当前层的* 缩放的比例
  compute_descriptors( features, gauss_pyr, descr_width, descr_hist_bins );

  /* sort features by decreasing scale and move from CvSeq to array */
  cvSeqSort( features, (CvCmpFunc)feature_cmp, NULL );//根据特征点的缩放尺度来排序
  //所以本身就是按照不同尺度的特征点排序的

  n = features->total;
  *feat =(feature *) calloc( n, sizeof(struct feature) );
  *feat =(feature *) cvCvtSeqToArray( features, *feat, CV_WHOLE_SEQ );
  for( i = 0; i < n; i++ )
    {
      free( (*feat)[i].feature_data );
      (*feat)[i].feature_data = NULL;
    }
  
  cvReleaseMemStorage( &storage );
  storage=NULL;
  cvReleaseImage( &init_img );
  init_img=NULL;
  release_pyr( &gauss_pyr, octvs, intvls + 3 );
  release_pyr( &dog_pyr, octvs, intvls + 2 );
  return n;
}


/************************ Functions prototyped here **************************/

/*
  Converts an image to 8-bit grayscale and Gaussian-smooths it.  The image is
  optionally doubled in size prior to smoothing.

  @param img input image
  @param img_dbl if true, image is doubled in size prior to smoothing
  @param sigma total std of Gaussian smoothing
*/
static IplImage* create_init_img( IplImage* img, int img_dbl, double sigma )
{/*将原图转换为灰度 ，并用高斯模糊，
   */

  IplImage* gray, * dbl;
  double sig_diff;

  gray = convert_to_gray32( img );//这sb函数是别人 自己写的不是官方函数呢。量化灰度到1.0
  if( img_dbl )//原图是否要放大2倍 不同处理
    {
      sig_diff = sqrt( sigma * sigma - SIFT_INIT_SIGMA * SIFT_INIT_SIGMA * 4 );
      dbl = cvCreateImage( cvSize( img->width*2, img->height*2 ),
			   IPL_DEPTH_32F, 1 );
      cvResize( gray, dbl, CV_INTER_CUBIC );
      cvSmooth( dbl, dbl, CV_GAUSSIAN, 0, 0, sig_diff, sig_diff );
      cvReleaseImage( &gray );
	  gray=NULL;
      return dbl;
    }
  else
    {
      sig_diff = sqrt( sigma * sigma - SIFT_INIT_SIGMA * SIFT_INIT_SIGMA );
	  cvSmooth( gray, gray, CV_GAUSSIAN, 0, 0, sig_diff, sig_diff );
	  return gray;
    }
}



/*
  Converts an image to 32-bit grayscale

  @param img a 3-channel 8-bit color (BGR) or 8-bit gray image

  @return Returns a 32-bit grayscale image
*/
static IplImage* convert_to_gray32( IplImage* img )
{
  IplImage* gray8, * gray32;
  
  gray32 = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 1 );
  if( img->nChannels == 1 )
    gray8 =(IplImage *) cvClone( img );
  else
    {
      gray8 = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );
      cvCvtColor( img, gray8, CV_BGR2GRAY );
    }
  cvConvertScale( gray8, gray32, 1.0 / 255.0, 0 );

  cvReleaseImage( &gray8 );
    gray8=NULL;;
  return gray32;
}



/*
  Builds Gaussian scale space pyramid from an image

  @param base base image of the pyramid
  @param octvs number of octaves of scale space
  @param intvls number of intervals per octave
  @param sigma amount of Gaussian smoothing per octave

  @return Returns a Gaussian scale space pyramid as an octvs x (intvls + 3)
    array
*/
static IplImage*** build_gauss_pyr( IplImage* base, int octvs,
			     int intvls, double sigma )
{/*intvls 是每层金字塔的层数  octvs是金字塔层数（大层）  sigma是高斯模糊的 增量值
  gauss_pyr要主函数释放
 */
  IplImage*** gauss_pyr;
  const int _intvls = intvls;
  double  sig_total, sig_prev, k;
 double* sig=(double *)malloc((_intvls+3)*sizeof(double)) ; //存放每小层的高斯sigma值
  int i, o;

  gauss_pyr =( IplImage*** ) calloc( octvs, sizeof( IplImage** ) );
  for( i = 0; i < octvs; i++ )
    gauss_pyr[i] = ( IplImage** )calloc( intvls + 3, sizeof( IplImage *) );

  /*
    precompute Gaussian sigmas using the following formula:

    \sigma_{total}^2 = \sigma_{i}^2 + \sigma_{i-1}^2
  */

 // printf("%d",_msize(base));
  sig[0] = sigma;
  k = pow( POW_BASE, 1.0 / intvls ); //每一小层的增量
  for( i = 1; i < intvls + 3; i++ )
    {/*   对应下文用高斯的sig、【】参数*/
      sig_prev = pow( k, i - 1 ) * sigma;
      sig_total = sig_prev * k;
      sig[i] = sqrt( sig_total * sig_total - sig_prev * sig_prev );
    }

  for( o = 0; o < octvs; o++ )//每一大层
    for( i = 0; i < intvls + 3; i++ )
      {
	if( o == 0  &&  i == 0 )
	  gauss_pyr[o][i] = cvCloneImage(base);//第一层

	/* base of new octvave is halved image from end of previous octave */
	else if( i == 0 )
	  gauss_pyr[o][i] = downsample( gauss_pyr[o-1][intvls] );//第一层=上一大层的倒数第三层 的四分之一 
	  //因为倒数第三层： k^intvls =2， 所以正好sigma放大两倍就是下一大层的sigma
	/* blur the current octave's last image to create the next one */
	else
	{
		gauss_pyr[o][i] = cvCreateImage( cvGetSize(gauss_pyr[o][i-1]),
			IPL_DEPTH_32F, 1 );
		cvSmooth( gauss_pyr[o][i-1], gauss_pyr[o][i],
			CV_GAUSSIAN, 0, 0, sig[i], sig[i] );

		//这里将上一个图像 再次高斯模糊到 下一个图像；
		//设上一个 的sigma{i-1} 下一个在于原图相比较时 是\sigma_{i} 
		//所以这里要用两层的sigma差来求：保存在simg【i}中
		// \sigma_{total}^2 = \sigma_{i}^2 + \sigma_{i-1}^2
		//sig[i] = sqrt( sig_total * sig_total - sig_prev * sig_prev );=》正好就是两层高斯值得差
	}
	}

	free(sig);
	sig=NULL;
  return gauss_pyr;
}



/*
  Downsamples an image to a quarter of its size (half in each dimension)
  using nearest-neighbor interpolation

  @param img an image

  @return Returns an image whose dimensions are half those of img
*/
static IplImage* downsample( IplImage* img )
{//将图片缩小四分之一
  IplImage* smaller = cvCreateImage( cvSize(img->width / 2, img->height / 2),
				     img->depth, img->nChannels );
  cvResize( img, smaller, CV_INTER_NN );

  return smaller;
}



/*
  Builds a difference of Gaussians scale space pyramid by subtracting adjacent
  intervals of a Gaussian pyramid

  @param gauss_pyr Gaussian scale-space pyramid
  @param octvs number of octaves of scale space
  @param intvls number of intervals per octave

  @return Returns a difference of Gaussians scale space pyramid as an
    octvs x (intvls + 2) array
*/
static IplImage*** build_dog_pyr( IplImage*** gauss_pyr, int octvs, int intvls )
{//只不过是用高斯模糊后的图像相减罢了
  IplImage*** dog_pyr;
  int i, o;

  dog_pyr = (IplImage ***)calloc( octvs, sizeof( IplImage** ) );
  for( i = 0; i < octvs; i++ )
    dog_pyr[i] =(IplImage **) calloc( intvls + 2, sizeof(IplImage*) );

  for( o = 0; o < octvs; o++ )
    for( i = 0; i < intvls + 2; i++ )
      {
	dog_pyr[o][i] = cvCreateImage( cvGetSize(gauss_pyr[o][i]),
				       IPL_DEPTH_32F, 1 );
	cvSub( gauss_pyr[o][i+1], gauss_pyr[o][i], dog_pyr[o][i], NULL );//相减
      }

  return dog_pyr;
}



/*
  Detects features at extrema in DoG scale space.  Bad features are discarded
  based on contrast and ratio of principal curvatures.

  @param dog_pyr DoG scale space pyramid
  @param octvs octaves of scale space represented by dog_pyr
  @param intvls intervals per octave
  @param contr_thr low threshold on feature contrast  低阀值
  @param curv_thr high threshold on feature ratio of principal curvatures 高阀值
  @param storage memory storage in which to store detected features

  @return Returns an array of detected features whose scales, orientations,
    and descriptors are yet to be determined.
*/
static CvSeq* scale_space_extrema( IplImage*** dog_pyr, int octvs, int intvls,
				   double contr_thr, int curv_thr,
				   CvMemStorage* storage )
{/* 对每层的每个点：
	1 判断是不是局部 最大最小值
	2.精确定位  消除边缘效应
	3，加入seq中
 */
  CvSeq* features;
  double prelim_contr_thr = 0.5 * contr_thr / intvls;
  struct feature* feat;
  struct detection_data* ddata;
  int o, i, r, c;

  features = cvCreateSeq( 0, sizeof(CvSeq), sizeof(struct feature), storage );
  for( o = 0; o < octvs; o++ )
    for( i = 1; i <= intvls; i++ )
      for(r = SIFT_IMG_BORDER; r < dog_pyr[o][0]->height-SIFT_IMG_BORDER; r++)
		for(c = SIFT_IMG_BORDER; c < dog_pyr[o][0]->width-SIFT_IMG_BORDER; c++)
			 {   /* perform preliminary check on contrast */
				if( ABS( pixval32f( dog_pyr[o][i], r, c ) ) > prelim_contr_thr )
					//取dog金字塔的o大层 i小层 矩阵的R,C坐标的点值 和阀值比较
				{	
					if( is_extremum( dog_pyr, o, i, r, c ) )
						/*和邻近的层 比较，看是不是最值*/
					{
						feat = interp_extremum(dog_pyr, o, i, r, c, intvls, contr_thr);
						////得到《原图中的》精确位置 ，存入feature结构

						if( feat )
						{
							ddata = feat_detection_data( feat );
							if( ! is_too_edge_like( dog_pyr[ddata->octv][ddata->intvl],
								ddata->r, ddata->c, curv_thr ) )
							{
								cvSeqPush( features, feat );
							}
							else
								{
									free( ddata );
									ddata=NULL;
							}
							free( feat );
							feat=NULL;
						}
				  }
				}
		}
  return features;
}



/*
  Determines whether a pixel is a scale-space extremum by comparing it to it's
  3x3x3 pixel neighborhood.

  @param dog_pyr DoG scale space pyramid
  @param octv pixel's scale space octave
  @param intvl pixel's within-octave interval
  @param r pixel's image row
  @param c pixel's image col

  @return Returns 1 if the specified pixel is an extremum (max or min) among
    it's 3x3x3 pixel neighborhood.
*/
static int is_extremum( IplImage*** dog_pyr, int octv, int intvl, int r, int c )
{/*和邻近的层 比较，看是不是最值*/
  double val = pixval32f( dog_pyr[octv][intvl], r, c );
  int i, j, k;

  /*和上下层   上下左右的点比较，看是不是最大 最小值*/
  /* check for maximum */
  if( val > 0 )
  {
	  for( i = -1; i <= 1; i++ )//和上下层
		  for( j = -1; j <= 1; j++ )//同一层 上下的点
			  for( k = -1; k <= 1; k++ )//左右的点
				  if( val < pixval32f( dog_pyr[octv][intvl+i], r + j, c + k ) )
					  return 0;
    }

  /* check for minimum */
  else
  {
	  for( i = -1; i <= 1; i++ )
		  for( j = -1; j <= 1; j++ )
			  for( k = -1; k <= 1; k++ )
				  if( val > pixval32f( dog_pyr[octv][intvl+i], r + j, c + k ) )
					  return 0;
  }

  return 1;
}



/*
  Interpolates a scale-space extremum's location and scale to subpixel
  accuracy to form an image feature.  Rejects features with low contrast.
  Based on Section 4 of Lowe's paper.  

  @param dog_pyr DoG scale space pyramid
  @param octv feature's octave of scale space
  @param intvl feature's within-octave interval
  @param r feature's image row
  @param c feature's image column
  @param intvls total intervals per octave
  @param contr_thr threshold on feature contrast

  @return Returns the feature resulting from interpolation of the given
    parameters or NULL if the given location could not be interpolated or
    if contrast at the interpolated loation was too low.  If a feature is
    returned, its scale, orientation, and descriptor are yet to be determined.
*/
static struct feature* interp_extremum( IplImage*** dog_pyr, int octv,
					int intvl, int r, int c, int intvls,
					double contr_thr )
{//得到极值点在一个大层中，具体的x，y ，小层坐标 ：精确位置 ，存入feature结构
	/*
	调节包括两个方面：
	1，用函数求极值的方法求得坐标：
		如果极值太小，说明没必要调整
	 更新左边（三维）
	 循环5次调整


	 2，求得到的新点的主曲率，如果太大，说明是不稳定的
	   二阶Hess矩阵 特征值与主曲率正比
	   而特征值又可以用：Hess阵的Tr^2 和 Det 比值来描述 	
 */

  struct feature* feat;
  struct detection_data* ddata;
  double xi, xr, xc, contr;
  int i = 0;
  
  while( i < SIFT_MAX_INTERP_STEPS )
    {//调整5次
      interp_step( dog_pyr, octv, intvl, r, c, &xi, &xr, &xc );//计算精确位置：层数 平面坐标
	  if( ABS( xi ) < 0.5  &&  ABS( xr ) < 0.5  &&  ABS( xc ) < 0.5 )
		  break;//如果极值点离现在的点《0.5 那就不要改了

	  c += cvRound( xc );//得到xc最近的整数
	  r += cvRound( xr );
	  intvl += cvRound( xi );//小层数目

	  if( intvl < 1  ||  intvl > intvls  ||
	  c < SIFT_IMG_BORDER  ||
	  r < SIFT_IMG_BORDER  ||
	  c >= dog_pyr[octv][0]->width - SIFT_IMG_BORDER  ||
	  r >= dog_pyr[octv][0]->height - SIFT_IMG_BORDER )
		{//超出范围了
		  return NULL;
		}      
      i++;
    }
  
  /* ensure convergence of interpolation */
  if( i >= SIFT_MAX_INTERP_STEPS )
    return NULL;
  
  // *就是将极值坐标 带入D（X）的泰勒公式中：求得极值 是否 <0.04 ： 
  contr = interp_contr( dog_pyr, octv, intvl, r, c, xi, xr, xc );
  if( ABS( contr ) < contr_thr / intvls )
    return NULL;



  feat = new_feature();
  ddata = feat_detection_data( feat );//宏 取feat的data

  //????已经求得了精确位置？？为何还要再次加xc？？
  //因为前面不一定5次都得到偏移》0.5 所以就直接break了， 这里在补上
  feat->img_pt.x = feat->x = ( c + xc ) * pow( POW_BASE, octv );
  feat->img_pt.y = feat->y = ( r + xr ) * pow( POW_BASE, octv );
  /* 精确坐标 (c+xc)* 2^octv  octv是大层
	其实就是得到精确坐标在原图的坐标： 每大层X，Y都是缩小2的
  */

  ddata->r = r;//这里将一些点的信息保存在feat的data里
  ddata->c = c;
  ddata->octv = octv;
  ddata->intvl = intvl;
  ddata->subintvl = xi;//小层的偏移量

  return feat;
}



/*
  Performs one step of extremum interpolation.  Based on Eqn. (3) in Lowe's
  paper.

  @param dog_pyr difference of Gaussians scale space pyramid
  @param octv octave of scale space
  @param intvl interval being interpolated
  @param r row being interpolated
  @param c column being interpolated
  @param xi output as interpolated subpixel increment to interval
  @param xr output as interpolated subpixel increment to row
  @param xc output as interpolated subpixel increment to col
*/

static void interp_step( IplImage*** dog_pyr, int octv, int intvl, int r, int c,
			 double* xi, double* xr, double* xc )
{// 就是 求解 H *x=dD  x就是极值点，精确地位置  opencv自带了x= mat。solve（db）函数；
 //将求hess 求der矩阵分出函数后，代码更加好看了
  CvMat* dD, * H, * H_inv, X;
  //CvMat*temp;
  double x[3] = { 0 };
  
  dD = deriv_3D( dog_pyr, octv, intvl, r, c );
  H = hessian_3D( dog_pyr, octv, intvl, r, c );
  H_inv = cvCreateMat( 3, 3, CV_64FC1 );
  cvInvert( H, H_inv, CV_SVD );
  cvInitMatHeader( &X, 3, 1, CV_64FC1, x, CV_AUTOSTEP );
  cvGEMM( H_inv, dD, -1, NULL, 0, &X, 0 );//H^-1 * dD =X GEMM表示矩阵通用乘法
  
 // cvReleaseMatHeader(&temp);
 // free(temp);
//	temp=NULL;
  cvReleaseMat( &dD );
  cvReleaseMat( &H );
  cvReleaseMat( &H_inv );
  H=NULL;dD=NULL;H_inv=NULL;
  *xi = x[2];
  *xr = x[1];
  *xc = x[0];
}



/*
  Computes the partial derivatives in x, y, and scale of a pixel in the DoG
  scale space pyramid.

  @param dog_pyr DoG scale space pyramid
  @param octv pixel's octave in dog_pyr
  @param intvl pixel's interval in octv
  @param r pixel's image row
  @param c pixel's image col

  @return Returns the vector of partial derivatives for pixel I
    { dI/dx, dI/dy, dI/ds }^T as a CvMat*
*/
static CvMat* deriv_3D( IplImage*** dog_pyr, int octv, int intvl, int r, int c )
{//三维xy z（层数）求 一阶导数 
  CvMat* dI;
  double dx, dy, ds;

  dx = ( pixval32f( dog_pyr[octv][intvl], r, c+1 ) -
	 pixval32f( dog_pyr[octv][intvl], r, c-1 ) ) / 2.0;
  dy = ( pixval32f( dog_pyr[octv][intvl], r+1, c ) -
	 pixval32f( dog_pyr[octv][intvl], r-1, c ) ) / 2.0;
  ds = ( pixval32f( dog_pyr[octv][intvl+1], r, c ) -
	 pixval32f( dog_pyr[octv][intvl-1], r, c ) ) / 2.0;
  
  dI = cvCreateMat( 3, 1, CV_64FC1 );
  cvmSet( dI, 0, 0, dx );
  cvmSet( dI, 1, 0, dy );
  cvmSet( dI, 2, 0, ds );

  return dI;
}



/*
  Computes the 3D Hessian matrix for a pixel in the DoG scale space pyramid.

  @param dog_pyr DoG scale space pyramid
  @param octv pixel's octave in dog_pyr
  @param intvl pixel's interval in octv
  @param r pixel's image row
  @param c pixel's image col

  @return Returns the Hessian matrix (below) for pixel I as a CvMat*

  / Ixx  Ixy  Ixs \ <BR>
  | Ixy  Iyy  Iys | <BR>
  \ Ixs  Iys  Iss /
*/
static CvMat* hessian_3D( IplImage*** dog_pyr, int octv, int intvl, int r,
			  int c )
{//求hesse矩阵
  CvMat* H;
  double v, dxx, dyy, dss, dxy, dxs, dys;
  
  v = pixval32f( dog_pyr[octv][intvl], r, c );
  dxx = ( pixval32f( dog_pyr[octv][intvl], r, c+1 ) + 
	  pixval32f( dog_pyr[octv][intvl], r, c-1 ) - 2 * v );
  dyy = ( pixval32f( dog_pyr[octv][intvl], r+1, c ) +
	  pixval32f( dog_pyr[octv][intvl], r-1, c ) - 2 * v );
  dss = ( pixval32f( dog_pyr[octv][intvl+1], r, c ) +
	  pixval32f( dog_pyr[octv][intvl-1], r, c ) - 2 * v );
  dxy = ( pixval32f( dog_pyr[octv][intvl], r+1, c+1 ) -
	  pixval32f( dog_pyr[octv][intvl], r+1, c-1 ) -
	  pixval32f( dog_pyr[octv][intvl], r-1, c+1 ) +
	  pixval32f( dog_pyr[octv][intvl], r-1, c-1 ) ) / 4.0;
  dxs = ( pixval32f( dog_pyr[octv][intvl+1], r, c+1 ) -
	  pixval32f( dog_pyr[octv][intvl+1], r, c-1 ) -
	  pixval32f( dog_pyr[octv][intvl-1], r, c+1 ) +
	  pixval32f( dog_pyr[octv][intvl-1], r, c-1 ) ) / 4.0;
  dys = ( pixval32f( dog_pyr[octv][intvl+1], r+1, c ) -
	  pixval32f( dog_pyr[octv][intvl+1], r-1, c ) -
	  pixval32f( dog_pyr[octv][intvl-1], r+1, c ) +
	  pixval32f( dog_pyr[octv][intvl-1], r-1, c ) ) / 4.0;
  
  H = cvCreateMat( 3, 3, CV_64FC1 );
  cvmSet( H, 0, 0, dxx );
  cvmSet( H, 0, 1, dxy );
  cvmSet( H, 0, 2, dxs );
  cvmSet( H, 1, 0, dxy );
  cvmSet( H, 1, 1, dyy );
  cvmSet( H, 1, 2, dys );
  cvmSet( H, 2, 0, dxs );
  cvmSet( H, 2, 1, dys );
  cvmSet( H, 2, 2, dss );

  return H;
}



/*
  Calculates interpolated pixel contrast.  Based on Eqn. (3) in Lowe's
  paper.

  @param dog_pyr difference of Gaussians scale space pyramid
  @param octv octave of scale space
  @param intvl within-octave interval
  @param r pixel row
  @param c pixel column
  @param xi interpolated subpixel increment to interval
  @param xr interpolated subpixel increment to row
  @param xc interpolated subpixel increment to col

  @param Returns interpolated contrast.
*/
static double interp_contr( IplImage*** dog_pyr, int octv, int intvl, int r,
			    int c, double xi, double xr, double xc )
{//将新的值 X*dD=t[0]  返回原值+0.5X*dD  
	/*就是将极值坐标 带入D（X）的泰勒公式中：求得极值 略去了 二次相*/
  CvMat* dD, X, T;
 // CvMat*temp1,*temp2;
  double t[1], x[3] = { xc, xr, xi };

  cvInitMatHeader( &X, 3, 1, CV_64FC1, x, CV_AUTOSTEP );

   cvInitMatHeader( &T, 1, 1, CV_64FC1, t, CV_AUTOSTEP );

  //修正后的坐标带入泰勒展开式，得到的结果小于contrastThreshold=0.04则抛弃该点
  dD = deriv_3D( dog_pyr, octv, intvl, r, c );
  cvGEMM( dD, &X, 1, NULL, 0, &T,  CV_GEMM_A_T );
  //=dD*X *1 + NULL*0 =>T 
  //新的X带入方程，得到估计的极值 忽略了二阶矩阵

  //cvReleaseMatHeader(&temp1);
 // cvReleaseMatHeader(&temp2);
 // temp1=NULL;temp2=NULL;
//  X=NULL;
//  T=NULL;
  cvReleaseMat( &dD );
  dD=NULL;
  return pixval32f( dog_pyr[octv][intvl], r, c ) + t[0] * 0.5;
}



/*
  Allocates and initializes a new feature

  @return Returns a pointer to the new feature
*/
static struct feature* new_feature( void )
{
  struct feature* feat;
  struct detection_data* ddata;

  feat = (struct feature *)malloc( sizeof( struct feature ) );
  memset( feat, 0, sizeof( struct feature ) );
  ddata = (struct detection_data *)malloc( sizeof( struct detection_data ) );
  memset( ddata, 0, sizeof( struct detection_data ) );
  feat->feature_data = ddata;
  feat->type = FEATURE_LOWE;

  return feat;
}



/*
  Determines whether a feature is too edge like to be stable by computing the
  ratio of principal curvatures at that feature.  Based on Section 4.1 of
  Lowe's paper.

  @param dog_img image from the DoG pyramid in which feature was detected
  @param r feature row
  @param c feature col
  @param curv_thr high threshold on ratio of principal curvatures

  @return Returns 0 if the feature at (r,c) in dog_img is sufficiently
    corner-like or 1 otherwise.
*/
static int is_too_edge_like( IplImage* dog_img, int r, int c, int curv_thr )
{	//edgeThreshold=10,去除边缘点  公式
	/*2，求得到的新点的主曲率，如果太大，说明是不稳定的
	二阶Hess矩阵 特征值与主曲率正比
	而特征值又可以用：Hess阵的Tr^2 和 Det 比值来描述 
	*/
  double d, dxx, dyy, dxy, tr, det;

  /* principal curvatures are computed using the trace and det of Hessian */
  d = pixval32f(dog_img, r, c);
  dxx = pixval32f( dog_img, r, c+1 ) + pixval32f( dog_img, r, c-1 ) - 2 * d;
  dyy = pixval32f( dog_img, r+1, c ) + pixval32f( dog_img, r-1, c ) - 2 * d;
  dxy = ( pixval32f(dog_img, r+1, c+1) - pixval32f(dog_img, r+1, c-1) -
	  pixval32f(dog_img, r-1, c+1) + pixval32f(dog_img, r-1, c-1) ) / 4.0;
  tr = dxx + dyy;
  det = dxx * dyy - dxy * dxy;

  /* negative determinant -> curvatures have different signs; reject feature */
  if( det <= 0 )
    return 1;

  if( tr * tr / det < ( curv_thr + 1.0 )*( curv_thr + 1.0 ) / curv_thr )
    return 0;
  return 1;
}



/*
  Calculates characteristic scale for each feature in an array.

  @param features array of features
  @param sigma amount of Gaussian smoothing per octave of scale space
  @param intvls intervals per octave of scale space
*/
static void calc_feature_scales( CvSeq* features, double sigma, int intvls )
{//计算每个特征点的 尺度  放入feat的scl  scl_octv
  struct feature* feat;
  struct detection_data* ddata;
  double intvl;
  int i, n;

  n = features->total;
  for( i = 0; i < n; i++ )
    {
      feat = CV_GET_SEQ_ELEM( struct feature, features, i );
      ddata = feat_detection_data( feat );
      intvl = ddata->intvl + ddata->subintvl;//特征点的精确小层数 

      feat->scl = sigma * pow( POW_BASE, ddata->octv + intvl / intvls );
	  //计算精确的  sigma*2^（精确大层数）= 特征点精确地大层对应的 缩放值2^大层数目次方

      ddata->scl_octv = sigma * pow( POW_BASE, intvl / intvls );
	  //计算 和原来大层的精确偏差倍数
    }
}



/*
  Halves feature coordinates and scale in case the input image was doubled
  prior to scale space construction.

  @param features array of features
*/
static void adjust_for_img_dbl( CvSeq* features )
{/*输入图像是放大了两倍的（论文推存将图片先放大2倍）
   那么最好将特征值缩小两倍*/
  struct feature* feat;
  int i, n;

  n = features->total;
  for( i = 0; i < n; i++ )
    {
      feat = CV_GET_SEQ_ELEM( struct feature, features, i );
      feat->x /= 2.0;
      feat->y /= 2.0;
      feat->scl /= 2.0;
      feat->img_pt.x /= 2.0;
      feat->img_pt.y /= 2.0;
    }
}



/*
Computes a canonical orientation for each image feature in an array.  Based
on Section 5 of Lowe's paper.  This function adds features to the array when
there is more than one dominant orientation at a given feature location.

  @param features an array of image features
  @param gauss_pyr Gaussian scale space pyramid
*/
static void calc_feature_oris( CvSeq* features, IplImage*** gauss_pyr )
{//计算特征 主方向 辅助方向  
  struct feature* feat;
  struct detection_data* ddata;
  double* hist;
  double omax;
  int i, j, n = features->total;

  for( i = 0; i < n; i++ )
  {
	  feat = (struct feature* )malloc( sizeof( struct feature ) );
	  cvSeqPopFront( features, feat );
	  ddata = feat_detection_data( feat );

	  /* 由周围的点 计算柱状图 ：*/
	  hist = ori_hist( gauss_pyr[ddata->octv][ddata->intvl],
		  ddata->r, ddata->c, SIFT_ORI_HIST_BINS,/*直方图是36分*/
		  cvRound( SIFT_ORI_RADIUS * ddata->scl_octv ),/*区域大小 1.5*3* */
		  SIFT_ORI_SIG_FCTR * ddata->scl_octv );
	  /*由于特征点精确后的层次intvl+调整值  和产生点的层次intvl 不同
	  所以区域大小 也要根据这个缩放
	  偏离越大，sigma越大，图像越模糊， 自然就要更大的范围来找方向柱状图
	  所以区域乘上了scl_octv 
	  */

	  for( j = 0; j < SIFT_ORI_SMOOTH_PASSES; j++ )
		  smooth_ori_hist( hist, SIFT_ORI_HIST_BINS );//高斯平滑柱状图

	  omax = dominant_ori( hist, SIFT_ORI_HIST_BINS );//找主方向

	  add_good_ori_features( features, hist, SIFT_ORI_HIST_BINS,
		  omax * SIFT_ORI_PEAK_RATIO, feat );//得到辅助方向：柱子大于主方向80%

	  free( ddata );
      free( feat );
      free( hist );
	  ddata=NULL;
	  feat=NULL;
	  hist=NULL;
    }
}



/*
  Computes a gradient orientation histogram at a specified pixel.

  @param img image
  @param r pixel row
  @param c pixel col
  @param n number of histogram bins
  @param rad radius of region over which histogram is computed
  @param sigma std for Gaussian weighting of histogram entries

  @return Returns an n-element array containing an orientation histogram
    representing orientations between 0 and 2 PI.
*/
static double* ori_hist( IplImage* img, int r, int c, int n, int rad,
			 double sigma )
{/* 由周围的点 计算柱状图 ：
	横坐标是点的角度，纵坐标是每个点模*高斯权值
 */
  double* hist;
  double mag, ori, w, exp_denom, PI2 = CV_PI * 2.0;
  int bin, i, j;

  hist =(double *) calloc( n, sizeof( double ) );//callloc不同于malloc就在于他会自动清零
  exp_denom = 2.0 * sigma * sigma;
  for( i = -rad; i <= rad; i++ )
    for( j = -rad; j <= rad; j++ )/*在原图的上下左右范围内的点*/
      if( calc_grad_mag_ori( img, r + i, c + j, &mag, &ori ) )
		  //计算 点梯度方向和模
	{
	  w = exp( -( i*i + j*j ) / exp_denom );

	  bin = cvRound( n * ( ori + CV_PI ) / PI2 );//bin就是柱的意思
	  //=36*(pi + 角度) /2pi  因为 角度可能小于0， 所以+pi从而将角度分到0-36的柱中
	  bin = ( bin < n )? bin : 0;

	  hist[bin] += w * mag; //模* 点位置对应的高斯权值
	  //得到的不是方向的个数，是每个方向上 ，其模*高斯权值的 和
	}

  return hist;
}



/*
  Calculates the gradient magnitude and orientation at a given pixel.

  @param img image
  @param r pixel row
  @param c pixel col
  @param mag output as gradient magnitude at pixel (r,c)
  @param ori output as gradient orientation at pixel (r,c)

  @return Returns 1 if the specified pixel is a valid one and sets mag and
    ori accordingly; otherwise returns 0
*/
static int calc_grad_mag_ori( IplImage* img, int r, int c, double* mag,
			      double* ori )
{//求点的梯度模 和梯度方向
  double dx, dy;

  if( r > 0  &&  r < img->height - 1  &&  c > 0  &&  c < img->width - 1 )
    {//求点的梯度模 和梯度方向： 就是用左右点差求dx  上下差求dy
      dx = pixval32f( img, r, c+1 ) - pixval32f( img, r, c-1 );
      dy = pixval32f( img, r-1, c ) - pixval32f( img, r+1, c );
      *mag = sqrt( dx*dx + dy*dy );
      *ori = atan2( dy, dx );
      return 1;
    }

  else
    return 0;
}



/*
  Gaussian smooths an orientation histogram.

  @param hist an orientation histogram
  @param n number of bins
*/
static void smooth_ori_hist( double* hist, int n )
{//高斯平滑柱状图
  double prev, tmp, h0 = hist[0];
  int i;

  prev = hist[n-1];
  for( i = 0; i < n; i++ )
    {//只不过是对每个柱子 *【0.25  0.5  0.25 】的权值
      tmp = hist[i];
      hist[i] = 0.25 * prev + 0.5 * hist[i] + 
	0.25 * ( ( i+1 == n )? h0 : hist[i+1] );
      prev = tmp;
    }
}



/*
  Finds the magnitude of the dominant orientation in a histogram

  @param hist an orientation histogram
  @param n number of bins

  @return Returns the value of the largest bin in hist
*/
static double dominant_ori( double* hist, int n )
{//找主方向，就是柱子的最大值
  double omax;
  int maxbin, i;

  omax = hist[0];
  maxbin = 0;
  for( i = 1; i < n; i++ )
    if( hist[i] > omax )
      {
	omax = hist[i];
	maxbin = i;
      }
  return omax;
}



/*
  Interpolates a histogram peak from left, center, and right values
*/
#define interp_hist_peak( l, c, r ) ( 0.5 * ((l)-(r)) / ((l) - 2.0*(c) + (r)) )



/*
  Adds features to an array for every orientation in a histogram greater than
  a specified threshold.

  @param features new features are added to the end of this array
  @param hist orientation histogram
  @param n number of bins in hist
  @param mag_thr new features are added for entries in hist greater than this
  @param feat new features are clones of this with different orientations
*/
static void add_good_ori_features( CvSeq* features, double* hist, int n,
				   double mag_thr, struct feature* feat )
{//得到辅助方向：柱子大于主方向80%
  struct feature* new_feat;
  double bin, PI2 = CV_PI * 2.0;
  int l, r, i;

  for( i = 0; i < n; i++ )
    {
      l = ( i == 0 )? n - 1 : i-1;//上一个  用0填补第一个
      r = ( i + 1 ) % n;//下一个  这里的编程技巧%n
      
      if( hist[i] > hist[l]  &&  hist[i] > hist[r]  &&  hist[i] >= mag_thr )
	{//柱子是局部极值 且》最大柱子80%
	  bin = i + interp_hist_peak( hist[l], hist[i], hist[r] );
	  bin = ( bin < 0 )? n + bin : ( bin >= n )? bin - n : bin;
	  //??这样插值就更精确了？？

	  new_feat = clone_feature( feat );
	  new_feat->ori = ( ( PI2 * bin ) / n ) - CV_PI;//反求角度
	  cvSeqPush( features, new_feat );//复制特征点 加入seq中

	//  free((new_feat->feature_data));//xiuloule !!!
	  free( new_feat );
	  new_feat=NULL;
	}
    }
}



/*
  Makes a deep copy of a feature

  @param feat feature to be cloned

  @return Returns a deep copy of feat
*/
static struct feature* clone_feature( struct feature* feat )
{
  struct feature* new_feat;
  struct detection_data* ddata;

  new_feat = new_feature();
  ddata = feat_detection_data( new_feat );
  memcpy( new_feat, feat, sizeof( struct feature ) );
  memcpy( ddata, feat_detection_data(feat), sizeof( struct detection_data ) );
  new_feat->feature_data = ddata;

  return new_feat;
}



/*
  Computes feature descriptors for features in an array.  Based on Section 6
  of Lowe's paper.

  @param features array of features
  @param gauss_pyr Gaussian scale space pyramid
  @param d width of 2D array of orientation histograms
  @param n number of bins per orientation histogram
*/
static void compute_descriptors(
	CvSeq* features, IplImage*** gauss_pyr,
	int d, int n 
	)
{//d=4 n=8
  struct feature* feat;
  struct detection_data* ddata;
  double*** hist;
  int i, k = features->total;

  for( i = 0; i < k; i++ )
    {//对每个特征点
      feat = CV_GET_SEQ_ELEM( struct feature, features, i );
      ddata = feat_detection_data( feat );

      hist = descr_hist( gauss_pyr[ddata->octv][ddata->intvl], ddata->r,
			 ddata->c, feat->ori, ddata->scl_octv, d, n ,feat);//建立每个特征点的直方图

      hist_to_descr( hist, d, n, feat );//将三维直方图化为特征向量 并归一化

      release_descr_hist( &hist, d );
    }
}



/*
  Computes the 2D array of orientation histograms that form the feature
  descriptor.  Based on Section 6.1 of Lowe's paper.

  @param img image used in descriptor computation
  @param r row coord of center of orientation histogram array
  @param c column coord of center of orientation histogram array
  @param ori canonical orientation of feature whose descr is being computed
  @param scl scale relative to img of feature whose descr is being computed
  @param d width of 2d array of orientation histograms
  @param n bins per orientation histogram

  @return Returns a d x d array of n-bin orientation histograms.
*/
static double*** descr_hist( 
	IplImage* img, int r, int c, double ori,
	double scl, int d, int n,
struct feature* feat)
{/*计算区域内的点的梯度方向 模直方图， 建立特征点的描述
	在区域内，将点的方向 模映射到d*d的格子内
	然后方向映射到0-7上， 最后每个方向统计模的和

	通过dada->scl最终 都可以等价到同一个大层上

	这是在一个大层的灰度梯度统计，没有回归到原始图
*/
  double*** hist;
  double cos_t, sin_t, hist_width, exp_denom, r_rot, c_rot, grad_mag,
    grad_ori, w, rbin, cbin, obin, bins_per_rad, PI2 = 2.0 * CV_PI;
  int radius, i, j;


  //2014 3 6修改，增加灰度矩阵统计；
  double max1=0.0,max2=0.0;
  int tempmaxindex[2];


  const int RGB =256;
  const int PICES=16;
  int temprgb,tempg;
  int tempr;
  int tempall=0;
  double  tempsum;
  #define HISTPICES 256
  int histOfgray[HISTPICES];
  memset(histOfgray,0,sizeof(histOfgray));

  hist =(double ***) calloc( d, sizeof( double** ) );
  for( i = 0; i < d; i++ )
    {//3维的矩阵
      hist[i] =(double **) calloc( d, sizeof( double* ) );
      for( j = 0; j < d; j++ )
	hist[i][j] =(double *) calloc( n, sizeof( double ) );
    }
  
  cos_t = cos( ori );
  sin_t = sin( ori );
  bins_per_rad = n / PI2;//每一份的角度
  exp_denom = d * d * 0.5;
  hist_width = SIFT_DESCR_SCL_FCTR * scl;

  radius = hist_width * sqrt((double)2) * ( d + 1.0 ) * 0.5 + 0.5;
  //采样区域考虑到了缩放scl  旋转sqrt（2）  线性插值采样 d+1   +0.5是为了整数

  for( i = -radius; i <= radius; i++ )
    for( j = -radius; j <= radius; j++ )
      {//范围内的每个点
		/*
		  Calculate sample's histogram array coords rotated relative to ori.
		  Subtract 0.5 so samples that fall e.g. in the center of row 1 (i.e.
		  r_rot = 1.5) have full weight placed in row 1 after interpolation.
		*/
		//变换到主方向的坐标值
		c_rot = ( j * cos_t - i * sin_t ) / hist_width;//为何除以hist_width?
		r_rot = ( j * sin_t + i * cos_t ) / hist_width;
		//缩放scl大的话，图像模糊，所以采样范围radius就要更大（不是4了）
		//除以hist_width 是的最终点分配到d*d的小方格内

		rbin = r_rot + d / 2 - 0.5;//？？？没看懂  
		cbin = c_rot + d / 2 - 0.5;
		//c_rot以（-radius / hist_width,-radius / hist_width） 开始，现在要以0,0点开始

		if( rbin > -1.0  &&  rbin < d  &&  cbin > -1.0  &&  cbin < d )
			if( calc_grad_mag_ori( img, r + i, c + j, &grad_mag, &grad_ori ))
				//计算了原图 上坐标的梯度模 角度
			{

				//2014  3-6  统计灰度
			// 	printf(" %f ",pixval32f( img, r+i, c+j ));
				tempall++;
				temprgb= (int)( ( pixval32f( img, r+i, c+j ))*255);
				histOfgray[temprgb]++;
			/*	if (temprgb > 128 )
				{
					printf("均值 %d \n",temprgb);
				}
*/
				

				grad_ori -= ori;//原梯度方向-主方向 相当于以主方向位坐标了
				//调整到0-2pi上
				while( grad_ori < 0.0 )
					grad_ori += PI2;
				while( grad_ori >= PI2 )
					grad_ori -= PI2;

				obin = grad_ori * bins_per_rad;//化到每一份的柱子上

				w = exp( -(c_rot * c_rot + r_rot * r_rot) / exp_denom );//高斯权值
				interp_hist_entry( hist, rbin, cbin, obin, grad_mag * w, d, n );
				//生成柱状图
			}
      }


	//2014 3 6 修改  改为求均值
	//tempall=(radius<<1)*(radius<<1);
	//tempall=tempall<<1;
 
	tempsum=0.0;
	for (i=0;i< HISTPICES; i++)
	{
		tempsum+=histOfgray[i]*i; 
	}
	
	tempsum=tempsum / tempall;
	
	
	feat->meanOfgray=(int)tempsum;
	/*printf("均值 %d \n",feat->meanOfgray);

	if (feat->meanOfgray > 125 )
	{
		printf("均值 %d \n",feat->meanOfgray);
	}*/
  return hist;
}



/*
  Interpolates an entry into the array of orientation histograms that form
  the feature descriptor.

  @param hist 2D array of orientation histograms
  @param rbin sub-bin row coordinate of entry
  @param cbin sub-bin column coordinate of entry
  @param obin sub-bin orientation coordinate of entry
  @param mag size of entry
  @param d width of 2D array of orientation histograms
  @param n number of bins per orientation histogram
*/
static void interp_hist_entry( double*** hist, double rbin, double cbin,
			       double obin, double mag, int d, int n )
{/*rbin ， cbin 是旋转后的坐标位置
	obin是角度 mag是模
	通过对（rbin,cbin,obin角度）的3线性插值==》（其实就是位置（r,c）上方向的模）
	将三维直方图化为特征向量的描述
*/
  double d_r, d_c, d_o, v_r, v_c, v_o;
  double** row, * h;
  int r0, c0, o0, rb, cb, ob, r, c, o;

  r0 = cvFloor( rbin );
  c0 = cvFloor( cbin );
  o0 = cvFloor( obin );
  d_r = rbin - r0;//小数部分
  d_c = cbin - c0;
  d_o = obin - o0;

  /*
    The entry is distributed into up to 8 bins.  Each entry into a bin
    is multiplied by a weight of 1 - d for each dimension, where d is the
    distance from the center value of the bin measured in bin units.
  */
  for( r = 0; r <= 1; r++ )
    {//这里是个三线性插值
      rb = r0 + r;//rbin的上一个整数 和下一个整数 都计算一次
	  if( rb >= 0  &&  rb < d )
	  {//
		  v_r = mag * ( ( r == 0 )? 1.0 - d_r : d_r );
		  row = hist[rb];
		  for( c = 0; c <= 1; c++ )
		  {
			  cb = c0 + c;//cbin的上下两个整数
			  if( cb >= 0  &&  cb < d )
			  {
				  v_c = v_r * ( ( c == 0 )? 1.0 - d_c : d_c );
				  h = row[cb];
				  for( o = 0; o <= 1; o++ )
				  {
					  ob = ( o0 + o ) % n;//映射到0-7上ob就是0-8的方向柱啊！
					  v_o = v_c * ( ( o == 0 )? 1.0 - d_o : d_o );
					  h[ob] += v_o;
					  /*考虑到平移后的点坐标r，c 已经不是整数了，
					  所以无法确定点的模 具体落在hist[x][y][o]的那个上了
					  所以这里用3维插值 确定落在哪
						 最终结果是：在x y o附近的hist都得到不同程度的增加
					  */
				  }
			  }
		  }
	  }
  }
}



/*
  Converts the 2D array of orientation histograms into a feature's descriptor
  vector.
  
  @param hist 2D array of orientation histograms
  @param d width of hist
  @param n bins per histogram
  @param feat feature into which to store descriptor
*/
static void hist_to_descr( double*** hist, int d, int n, struct feature* feat )
{//将三维直方图化为特征向量的描述
  int int_val, i, r, c, o, k = 0;

  for( r = 0; r < d; r++ )
    for( c = 0; c < d; c++ )
      for( o = 0; o < n; o++ )
	feat->descr[k++] = hist[r][c][o];

  feat->d = k;
  normalize_descr( feat );

  for( i = 0; i < k; i++ )
    if( feat->descr[i] > SIFT_DESCR_MAG_THR )
      feat->descr[i] = SIFT_DESCR_MAG_THR;//超过阀值就=阀值 0.2

  normalize_descr( feat );//将特征点的描述 单位化：

  /* convert floating-point descriptor to integer valued descriptor */
  for( i = 0; i < k; i++ )
    {//扩大512倍 化为整数
      int_val = SIFT_INT_DESCR_FCTR * feat->descr[i];
      feat->descr[i] = MIN( 255, int_val );
    }
}



/*
  Normalizes a feature's descriptor vector to unitl length

  @param feat feature
*/
static void normalize_descr( struct feature* feat )
{//将特征点的描述 单位化：
  double cur, len_inv, len_sq = 0.0;
  int i, d = feat->d;

  for( i = 0; i < d; i++ )
    {
      cur = feat->descr[i];
      len_sq += cur*cur;
    }
  //不像一般的求平均，这里使用平方来计算的 

  len_inv = 1.0 / sqrt( len_sq );//得到平均值

  for( i = 0; i < d; i++ )
    feat->descr[i] *= len_inv;
  //每个值= x/<x* sqrt(n) >
}



/*
  Compares features for a decreasing-scale ordering.  Intended for use with
  CvSeqSort

  @param feat1 first feature
  @param feat2 second feature
  @param param unused

  @return Returns 1 if feat1's scale is greater than feat2's, -1 if vice versa,
    and 0 if their scales are equal
*/
static int feature_cmp( void* feat1, void* feat2, void* param )
{//比较函数，根据缩放尺度来比较
  struct feature* f1 = (struct feature*) feat1;
  struct feature* f2 = (struct feature*) feat2;

  if( f1->scl < f2->scl )
    return 1;
  if( f1->scl > f2->scl )
    return -1;
  return 0;
}



/*
  De-allocates memory held by a descriptor histogram

  @param hist pointer to a 2D array of orientation histograms
  @param d width of hist
*/
static void release_descr_hist( double**** hist, int d )
{
	int i, j;

	for( i = 0; i < d; i++)
	{
		for( j = 0; j < d; j++ )
			{
				free( (*hist)[i][j] );
				   (*hist)[i][j] =NULL;
			}
		free( (*hist)[i] );
		(*hist)[i] =NULL;
	}
	free( *hist );
	*hist = NULL;
}


/*
  De-allocates memory held by a scale space pyramid

  @param pyr scale space pyramid
  @param octvs number of octaves of scale space
  @param n number of images per octave
*/
static void release_pyr( IplImage**** pyr, int octvs, int n )
{
	int i, j;
	for( i = 0; i < octvs; i++ )
	{
		for( j = 0; j < n; j++ )
			{
				cvReleaseImage( &(*pyr)[i][j] );
				(*pyr)[i][j]=NULL;
		}
		free( (*pyr)[i] );
		(*pyr)[i]=NULL;
	}
	free( *pyr );
	*pyr = NULL;
}
