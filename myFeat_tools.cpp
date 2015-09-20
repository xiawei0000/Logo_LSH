#include "myFeat_tools.h"



 

int compareIsInWindow(MachedFeatPoint * machedstruct,CvPoint pointWindow1, CvPoint pointWindow2)
{

	if (machedstruct->machedPointFeat2.x >=pointWindow1.x &&
		machedstruct->machedPointFeat2.y> pointWindow1.y && 
		machedstruct->machedPointFeat2.x <=pointWindow2.x  && 
		machedstruct->machedPointFeat2.y <  pointWindow2.y  )
		return 1;
	return 0;
}




void reduceMachedPoints(CvSeq *featseq,int * numberOfMached )
{
//设置一个图片最大 logo数目为5个，如果多余这个的，都是错误的
	/**
	1.循环，相同超过5个，就都认为是错误点，全部删除，
	2.少于5个，保留
	*/
	int k,j,n;
	MachedFeatPoint *machedstruct,*machedstructtemp;
	int tagForRemove=0;
	int *tagIndex;
	int tag=0;
	int numberOftag;
	tagIndex=(int *)malloc(sizeof(int) *MAXNUMBER_LOGO );


	/* 用列扫描两个最近邻， 那么只可能产生 一个logo点 匹配多个p点
	//第一轮： 一个p 匹配多个logo点 ，直接删除

	for (k=0; k< *numberOfMached; k++)
	{
		machedstruct=(MachedFeatPoint *)cvGetSeqElem(featseq,k);
		for( j=k+1;j<*numberOfMached; j++)
		{
			machedstructtemp=(MachedFeatPoint *)cvGetSeqElem(featseq,j);

			if (compareIsEqualFeat2( machedstruct , machedstructtemp ) )
			{
				cvSeqRemove(featseq,j);
				(*numberOfMached)--;
			}

		}

	}

	*/
	//第二轮，5个以上 p点 匹配一个logo点，全部删除
	for (k=0; k< *numberOfMached; k++)
	{
		tagForRemove=0;
		tag=0;
		tagIndex[0]=k;
		tag++;

		machedstruct=(MachedFeatPoint *)cvGetSeqElem(featseq,k);

		for( j=k+1;j< *numberOfMached; j++)
		{//一轮扫描
			machedstructtemp=(MachedFeatPoint *)cvGetSeqElem(featseq,j);

			if (compareIsEqual( machedstruct , machedstructtemp ) )
			{
				if (tagForRemove !=0)
				{
					cvSeqRemove(featseq,j);
					(*numberOfMached)--;
				}
				else
				{
					tagIndex[tag]=j;
					tag++;
					if (tag ==MAXNUMBER_LOGO)//满了
					{//全部删除
						tagForRemove=1;
						for (n=0;n<MAXNUMBER_LOGO;n++)
						{
							cvSeqRemove(featseq,tagIndex[n]-n);
							(*numberOfMached)--;
						}
					}
				}
			}


		}

	}
	free(tagIndex);
 
}

int compareIsEqualFeat2(MachedFeatPoint * machedstruct,MachedFeatPoint * machedstructTemp)
{//结构体中， 一个p 匹配多个logo点 ，直接删除

	if (machedstruct->machedPointFeat2 .x == machedstructTemp->machedPointFeat2.x
		&& machedstruct->machedPointFeat2.y == machedstructTemp->machedPointFeat2.y)
		return 1;
	return 0;
}


int compareIsEqual(MachedFeatPoint * machedstruct,MachedFeatPoint * machedstructTemp)
{//结构体中，匹配的是不是logo中同一个点

	if (machedstruct->machedPointFeat1.x == machedstructTemp->machedPointFeat1.x
		&& machedstruct->machedPointFeat1.y == machedstructTemp->machedPointFeat1.y)
		return 1;
	return 0;
}



void GetMachedWindowPoint(MachedFeatPoint* featTemp,CvPoint * p1,CvPoint * p2,CvSize img1size,CvSize img1size2 )
{//根据两个匹配到的点 ，计算 匹配的窗口位置
	p1->x=( featTemp->machedPointFeat2.x -featTemp->machedPointFeat1.x );
	p1->y=( featTemp->machedPointFeat2.y -featTemp->machedPointFeat1.y);

	p2->x= img1size.width -featTemp->machedPointFeat1.x + featTemp->machedPointFeat2.x;
	p2->y= img1size.height -featTemp->machedPointFeat1.y + featTemp->machedPointFeat2.y;

	//防止越界
	if (p1->x < 0)
	{
		p1->x =0;
	}
	if (p1->y< 0)
	{
		p1->y = 0;
	}

	if (p2->x > img1size2.width)
	{
		p2->x = img1size2.width;
	}
	if (p2->y > img1size2.height)
	{
		p2->y = img1size2.height;
	}
	/*
	 p1->x =p1->x < 0 ? 0:p1->x;
	 p2->x =p2->x > img1size2.width? img1size2.width: p2->x;

	 p1->y =p1->y< 0 ? 0: p1->y;
	 p2->y  =p2->y > img1size2.height ? img1size2.height :p2->y;
	 */
	 
}