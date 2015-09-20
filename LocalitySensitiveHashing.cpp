/*
 * Copyright (c) 2004-2005 Massachusetts Institute of Technology.
 * All Rights Reserved.
 *
 * MIT grants permission to use, copy, modify, and distribute this software and
 * its documentation for NON-COMMERCIAL purposes and without fee, provided that
 * this copyright notice appears in all copies.
 *
 * MIT provides this software "as is," without representations or warranties of
 * any kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability, fitness for a particular purpose, and
 * noninfringement.  MIT shall not be liable for any damages arising from any
 * use of this software.
 *
 * Author: Alexandr Andoni (andoni@mit.edu), Piotr Indyk (indyk@mit.edu)
 */

/*
  The main functionality of the LSH scheme is in this file (all except
  the hashing of the buckets). This file includes all the functions
  for processing a PRNearNeighborStructT data structure, which is the
  main R-NN data structure based on LSH scheme. The particular
  functions are: initializing a DS, adding new points to the DS, and
  responding to queries on the DS.
 */

#include "headers.h"

void printRNNParameters(FILE *output, RNNParametersT parameters){
  ASSERT(output != NULL);
  fprintf(output, "R\n");
  fprintf(output, "%0.9lf\n", parameters.parameterR);
  fprintf(output, "Success probability\n");
  fprintf(output, "%0.9lf\n", parameters.successProbability);
  fprintf(output, "Dimension\n");
  fprintf(output, "%d\n", parameters.dimension);
  fprintf(output, "R^2\n");
  fprintf(output, "%0.9lf\n", parameters.parameterR2);
  fprintf(output, "Use <u> functions\n");
  fprintf(output, "%d\n", parameters.useUfunctions);
  fprintf(output, "k\n");
  fprintf(output, "%d\n", parameters.parameterK);
  fprintf(output, "m [# independent tuples of LSH functions]\n");
  fprintf(output, "%d\n", parameters.parameterM);
  fprintf(output, "L\n");
  fprintf(output, "%d\n", parameters.parameterL);
  fprintf(output, "W\n");
  fprintf(output, "%0.9lf\n", parameters.parameterW);
  fprintf(output, "T\n");
  fprintf(output, "%d\n", parameters.parameterT);
  fprintf(output, "typeHT\n");
  fprintf(output, "%d\n", parameters.typeHT);
}

RNNParametersT readRNNParameters(FILE *input)
{//从文件读参数
  ASSERT(input != NULL);
  RNNParametersT parameters;
  char s[1000];// TODO: possible buffer overflow

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  FSCANF_REAL(input, &parameters.parameterR);//编程技巧：宏命令：duimput到参数里

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  FSCANF_REAL(input, &parameters.successProbability);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  fscanf(input, "%d", &parameters.dimension);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  FSCANF_REAL(input, &parameters.parameterR2);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  fscanf(input, "%d", &parameters.useUfunctions);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  fscanf(input, "%d", &parameters.parameterK);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  fscanf(input, "%d", &parameters.parameterM);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  fscanf(input, "%d", &parameters.parameterL);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  FSCANF_REAL(input, &parameters.parameterW);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  fscanf(input, "%d", &parameters.parameterT);

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  fscanf(input, "%d", &parameters.typeHT);

  return parameters;
}

// Creates the LSH hash functions for the R-near neighbor structure
// <nnStruct>. The functions fills in the corresponding field of
// <nnStruct>.
void initHashFunctions(PRNearNeighborStructT nnStruct)
{/*按照 nHFTuples个 组，hfTuplesLength个最终的维度
    产生高斯分布的随机值：  a向量和 b，付给  nnStruct->lshFunctions【】【】
	用于第一步的点积降维
 */
	
	//按照gi  hi 两层函数族的格式，初始化nnStruct->lshFunctions所指向的二维指针
 
	//nHFTuples组hash函数g，每个组有hfTuplesLength个hi，每个hi就是一个向量a和一个b

	//按照a。v+b 的格式，初始化结构体中的随机向量和空间
	//nnStruct结构体中，初始化nnStruct->lshFunctions 所指向的二维随机矩阵
  ASSERT(nnStruct != NULL);//编程技巧： asssert来判断条件执行：  __line__  --file__显示位置
   LSHFunctionT **lshFunctions;//LSHFunctionT结构体就两个元素：一个指针+一个double
  // allocate memory for the functions
  FAILIF(NULL == (lshFunctions = (LSHFunctionT**)MALLOC(nnStruct->nHFTuples * sizeof(LSHFunctionT*))));
  for(IntT i = 0; i < nnStruct->nHFTuples; i++)
  {
    FAILIF(NULL == (lshFunctions[i] = (LSHFunctionT*)MALLOC(nnStruct->hfTuplesLength * sizeof(LSHFunctionT))));
    for(IntT j = 0; j < nnStruct->hfTuplesLength; j++)
	{
      FAILIF(NULL == (lshFunctions[i][j].a = (RealT*)MALLOC(nnStruct->dimension * sizeof(RealT))));
    }
  }

  // initialize the LSH functions
  //这里开始构造算法中的一组“位置敏感”的Hash函数。
  for(IntT i = 0; i < nnStruct->nHFTuples; i++)
  {
    for(IntT j = 0; j < nnStruct->hfTuplesLength; j++)
	{
      // vector a
      for(IntT d = 0; d < nnStruct->dimension; d++)
	  {
#ifdef USE_L1_DISTANCE
	lshFunctions[i][j].a[d] = genCauchyRandom();//L1距离有柯西分布
#else
	lshFunctions[i][j].a[d] = genGaussianRandom(); //L2使用高斯分布   公式中a就是一组d维的正态分布随机数
#endif
      }
      // b
      lshFunctions[i][j].b = genUniformRandom(0, nnStruct->parameterW);
    }
  }

  nnStruct->lshFunctions = lshFunctions;
}

// Initializes the fields of a R-near neighbors data structure except
// the hash tables for storing the buckets.
PRNearNeighborStructT initializePRNearNeighborFields(RNNParametersT algParameters, Int32T nPointsEstimate)
{//就是申请结构体，初始化了gi  hi函数的随机值
  PRNearNeighborStructT nnStruct;
  FAILIF(NULL == (nnStruct = (PRNearNeighborStructT)MALLOC(sizeof(RNearNeighborStructT))));
  nnStruct->parameterR = algParameters.parameterR;
  nnStruct->parameterR2 = algParameters.parameterR2;
  nnStruct->useUfunctions = algParameters.useUfunctions;
  nnStruct->parameterK = algParameters.parameterK;
  if (!algParameters.useUfunctions)
  {
    // Use normal <g> functions.
    nnStruct->parameterL = algParameters.parameterL;
    nnStruct->nHFTuples = algParameters.parameterL;
    nnStruct->hfTuplesLength = algParameters.parameterK;
  }
  else
  {
    // Use <u> hash functions; a <g> function is a pair of 2 <u> functions.
    nnStruct->parameterL = algParameters.parameterL;
    nnStruct->nHFTuples = algParameters.parameterM;//这是gi数组个数
    nnStruct->hfTuplesLength = algParameters.parameterK / 2;//这是每组gi中 维度长
  }
  nnStruct->parameterT = algParameters.parameterT;
  nnStruct->dimension = algParameters.dimension;
  nnStruct->parameterW = algParameters.parameterW;

  nnStruct->nPoints = 0;
  nnStruct->pointsArraySize = nPointsEstimate;

  FAILIF(NULL == (nnStruct->points = (PPointT*)MALLOC(nnStruct->pointsArraySize * sizeof(PPointT))));

  // create the hash functions
  initHashFunctions(nnStruct);//按照gi  hi 两层函数族的格式，初始化nnStruct->lshFunctions所指向的二维指针
  /*按照 nHFTuples个 组，hfTuplesLength个最终的维度
    产生高斯分布的随机值：  a向量和 b，付给  nnStruct->lshFunctions【】【】
	用于第一步的点积降维
 */


  // init fields that are used only in operations ("temporary" variables for operations).

  // init the vector <pointULSHVectors> and the vector
  // <precomputedHashesOfULSHs>
  FAILIF(NULL == (nnStruct->pointULSHVectors = (Uns32T**)MALLOC(nnStruct->nHFTuples * sizeof(Uns32T*))));
  for(IntT i = 0; i < nnStruct->nHFTuples; i++)
  {
    FAILIF(NULL == (nnStruct->pointULSHVectors[i] = (Uns32T*)MALLOC(nnStruct->hfTuplesLength * sizeof(Uns32T))));
  }
  FAILIF(NULL == (nnStruct->precomputedHashesOfULSHs = (Uns32T**)MALLOC(nnStruct->nHFTuples * sizeof(Uns32T*))));
  for(IntT i = 0; i < nnStruct->nHFTuples; i++)
  {
    FAILIF(NULL == (nnStruct->precomputedHashesOfULSHs[i] = (Uns32T*)MALLOC(N_PRECOMPUTED_HASHES_NEEDED * sizeof(Uns32T))));
  }
  // init the vector <reducedPoint>
  FAILIF(NULL == (nnStruct->reducedPoint = (RealT*)MALLOC(nnStruct->dimension * sizeof(RealT))));
  // init the vector <nearPoints>
  nnStruct->sizeMarkedPoints = nPointsEstimate;
  FAILIF(NULL == (nnStruct->markedPoints = (BooleanT*)MALLOC(nnStruct->sizeMarkedPoints * sizeof(BooleanT))));
  for(IntT i = 0; i < nnStruct->sizeMarkedPoints; i++)
  {
    nnStruct->markedPoints[i] = FALSE;
  }
  // init the vector <nearPointsIndeces>
  FAILIF(NULL == (nnStruct->markedPointsIndeces = (Int32T*)MALLOC(nnStruct->sizeMarkedPoints * sizeof(Int32T))));

  nnStruct->reportingResult = TRUE;

  return nnStruct;
}

// Constructs a new empty R-near-neighbor data structure.
PRNearNeighborStructT initLSH(RNNParametersT algParameters, Int32T nPointsEstimate){
  ASSERT(algParameters.typeHT == HT_LINKED_LIST || algParameters.typeHT == HT_STATISTICS);
  PRNearNeighborStructT nnStruct = initializePRNearNeighborFields(algParameters, nPointsEstimate);

  // initialize second level hashing (bucket hashing)
  FAILIF(NULL == (nnStruct->hashedBuckets = (PUHashStructureT*)MALLOC(nnStruct->parameterL * sizeof(PUHashStructureT))));
  Uns32T *mainHashA = NULL, *controlHash1 = NULL;
  BooleanT uhashesComputedAlready = FALSE;
  for(IntT i = 0; i < nnStruct->parameterL; i++)
  {
    nnStruct->hashedBuckets[i] = newUHashStructure(algParameters.typeHT,
		nPointsEstimate, nnStruct->parameterK, uhashesComputedAlready, mainHashA, controlHash1, NULL);
    uhashesComputedAlready = TRUE;
  }

  return nnStruct;
}

void preparePointAdding(PRNearNeighborStructT nnStruct, PUHashStructureT uhash, PPointT point);


// Construct PRNearNeighborStructT given the data set <dataSet> (all
// the points <dataSet> will be contained in the resulting DS).
// Currenly only type HT_HYBRID_CHAINS is supported for this
// operation.
PRNearNeighborStructT initLSH_WithDataSet(RNNParametersT algParameters, Int32T nPoints, PPointT *dataSet)
{//初始化整个数据结构 包括整体+l个hash表 +点映射到桶
/*整体：先初始化整体结构体： PRNearNeighborStructT nnStruct
 初始化随机向量gi hi，点存入
	
	然后初始化PUHashStructureT modelHT：多个hansh表

	最后，将多个 modelHT 链接到nnStruct
	*/
  ASSERT(algParameters.typeHT == HT_HYBRID_CHAINS);
  ASSERT(dataSet != NULL);
  ASSERT(USE_SAME_UHASH_FUNCTIONS);

  PRNearNeighborStructT nnStruct = initializePRNearNeighborFields(algParameters, nPoints);
  //就是申请结构体，初始化了gi  hi函数的随机值
 //按照gi  hi 两层函数族的格式，初始化nnStruct->lshFunctions所指向的二维指针
 //   产生高斯分布的随机值：  a向量和 b，付给  nnStruct->lshFunctions【】【】
 
  // Set the fields <nPoints> and <points>.
  nnStruct->nPoints = nPoints;
  for(Int32T i = 0; i < nPoints; i++)
  {
    nnStruct->points[i] = dataSet[i];
  }
  
  // initialize second level hashing (bucket hashing)
  FAILIF(NULL == (nnStruct->hashedBuckets = (PUHashStructureT*)MALLOC(nnStruct->parameterL * sizeof(PUHashStructureT))));
  Uns32T *mainHashA = NULL, *controlHash1 = NULL;
  PUHashStructureT modelHT = newUHashStructure(HT_LINKED_LIST, nPoints, nnStruct->parameterK, FALSE, mainHashA, controlHash1, NULL);
  //初始化hash的桶结构
  //对主hash和辅助hash表的初始化：给一个随机值
  /* 建立 uhash ，
 当typeHT= HT_LINKED_LIST时，直接初始化数组，
 然后建立随机的主hash表 向量， 辅助hash表的向量：
 就是  (u 。x)%(2^32 -5)/tablesize 中的u1向量 
 和 (u 。x)%(2^32 -5) 中的u2向量 
  最终返回：PUHashStructureT uhash
 */
   
   Uns32T ***(precomputedHashesOfULSHs);
   precomputedHashesOfULSHs= (Uns32T***)malloc(sizeof(Uns32T**)*(nnStruct->nHFTuples));
   //没有释放
 // Uns32T **(precomputedHashesOfULSHs[ (nnStruct->nHFTuples) ]); windows下不允许动态值建立数组

  for(IntT l = 0; l < nnStruct->nHFTuples; l++)
  {//每组hash函数 gi
    FAILIF(NULL == (precomputedHashesOfULSHs[l] = (Uns32T**)MALLOC(nPoints * sizeof(Uns32T*))));
    for(IntT i = 0; i < nPoints; i++)
	{
      FAILIF(NULL == (precomputedHashesOfULSHs[l][i] = (Uns32T*)MALLOC(N_PRECOMPUTED_HASHES_NEEDED * sizeof(Uns32T))));
	  for (int temi=0;temi< N_PRECOMPUTED_HASHES_NEEDED ;temi++)
	  {
		  precomputedHashesOfULSHs[l][i][temi]=0;
	  }
	  
    }
  }

  for(IntT i = 0; i < nPoints; i++)
  {
	  ASSERT(nnStruct != NULL);
	  //根据传入的多维point。计算对应每个hash表的降维=》hash值，存入了nnStruct->precomputedHashesOfULSHs
    preparePointAdding(nnStruct, modelHT, dataSet[i]);

/*	if ( i ==0)
	{
		for (int tempd=150; tempd< 160;tempd++)
		{
			printf(" %lf ",dataSet[i]->coordinates[tempd]);
		}
 	 
	//	printf("初始化数据 10个 \n\n");

//		 printf("  :  %lf  \n",dataSet[i][151]);
		//   printf( "主hash的值：  %u \n",modelHT->mainHashA[5] );
	//	   printf( "辅助hash的值：  %u \n",modelHT->controlHash1[5] );

		//   printf( "a     %u \n",nnStruct->lshFunctions[0][0].a[5]);
	//	   printf( "b     %u \n",nnStruct->lshFunctions[0][0].b  );
 
	}
	*/
    for(IntT l = 0; l < nnStruct->nHFTuples; l++)
	{
      for(IntT h = 0; h < N_PRECOMPUTED_HASHES_NEEDED; h++)
	  {//precomputedHashesOfULSHs结构保存提前计算好的最终hash值
		precomputedHashesOfULSHs[l][i][h] = nnStruct->precomputedHashesOfULSHs[l][h];

		/*
		if ( i==0)
		{
			
			printf(" %u",precomputedHashesOfULSHs[l][i][h]);
			FILE *in;
			 in = fopen("preconpute.txt", "a+") ;
			fprintf(in," %u",precomputedHashesOfULSHs[l][i][h]);
			fclose(in);
		}	/**/
		
		
      }

	 /*  if ( i==0)
	  {

		 
		  FILE *in;
		  in = fopen("preconpute.txt", "a+") ;
		  fprintf(in," \n");
		  fclose(in);
		
		  printf(" \n");
	  }  */
    }
  }

  //DPRINTF("Allocated memory(modelHT and precomputedHashesOfULSHs just a.): %lld\n", totalAllocatedMemory);

  // Initialize the counters for defining the pair of <u> functions used for <g> functions.
  IntT firstUComp = 0;
  IntT secondUComp = 1;
  for(IntT i = 0; i < nnStruct->parameterL; i++)
  {//l个表，每个表，	
	  // build the model HT.
    for(IntT p = 0; p < nPoints; p++)
	{  //对于每个点，都hash到对应的桶里，然后建立hash结构体
      // Add point <dataSet[p]> to modelHT.
      if (!nnStruct->useUfunctions) 
	  {
		// Use usual <g> functions (truly independent; <g>s are precisly
		// <u>s).
		addBucketEntry(modelHT, 1, precomputedHashesOfULSHs[i][p], NULL, p);
		//根据, precomputedHashesOfULSHs[i][p]来计算hash值，然后查找桶，并插入
      }
	  else 
	  {//nnStruct->useUfunctions 表示降维只取一般：8个，所以8维特征点值 和16维随机索引 分两次计算了   
		  //所以用 firstUComp  secondUComp表示两级索引
		// Use <u> functions (<g>s are pairs of <u> functions).
		addBucketEntry(modelHT, 2, precomputedHashesOfULSHs[firstUComp][p], precomputedHashesOfULSHs[secondUComp][p], p);
      }
    }

    //ASSERT(nAllocatedGBuckets <= nPoints);
    //ASSERT(nAllocatedBEntries <= nPoints);

    // compute what is the next pair of <u> functions.
    secondUComp++;
    if (secondUComp == nnStruct->nHFTuples)
	{//分两次算：就有 263 个一级+263个二级索引  到了 263，就要变一下了
      firstUComp++;
      secondUComp = firstUComp + 1;
    }

    // copy the model HT into the actual (packed) HT. copy the uhash function too.
    nnStruct->hashedBuckets[i] = newUHashStructure(algParameters.typeHT, nPoints, nnStruct->parameterK, TRUE, mainHashA, controlHash1, modelHT);
	//初始化hash的桶结构
	//根据已有的modelHT 转化为空间紧凑的HT_HYBRID_CHAINS 表

    // clear the model HT for the next iteration.
    clearUHashStructure(modelHT);//可能需要释放内存
  }

  //释放
  freeUHashStructure(modelHT, FALSE); // do not free the uhash functions since they are used by nnStruct->hashedBuckets[i]

  // freeing precomputedHashesOfULSHs
  for(IntT l = 0; l < nnStruct->nHFTuples; l++)
  {
    for(IntT i = 0; i < nPoints; i++)
	{
      FREE(precomputedHashesOfULSHs[l][i]);
    }
    FREE(precomputedHashesOfULSHs[l]);
  }


 // printf("\n\n返回前 :\n");

//  printf( "a     %u \n",nnStruct->lshFunctions[0][0].a[5]);
 // printf( "b     %u \n\n",nnStruct->lshFunctions[0][0].b  );
  return nnStruct;
}



// // Packed version (static).
// PRNearNeighborStructT buildPackedLSH(RealT R, BooleanT useUfunctions, IntT k, IntT LorM, RealT successProbability, IntT dim, IntT T, Int32T nPoints, PPointT *points){
//   ASSERT(points != NULL);
//   PRNearNeighborStructT nnStruct = initializePRNearNeighborFields(R, useUfunctions, k, LorM, successProbability, dim, T, nPoints);

//   // initialize second level hashing (bucket hashing)
//   FAILIF(NULL == (nnStruct->hashedBuckets = (PUHashStructureT*)MALLOC(nnStruct->parameterL * sizeof(PUHashStructureT))));
//   Uns32T *mainHashA = NULL, *controlHash1 = NULL;
//   PUHashStructureT modelHT = newUHashStructure(HT_STATISTICS, nPoints, nnStruct->parameterK, FALSE, mainHashA, controlHash1, NULL);
//   for(IntT i = 0; i < nnStruct->parameterL; i++){
//     // build the model HT.
//     for(IntT p = 0; p < nPoints; p++){
//       // addBucketEntry(modelHT, );
//     }



//     // copy the model HT into the actual (packed) HT.
//     nnStruct->hashedBuckets[i] = newUHashStructure(HT_PACKED, nPointsEstimate, nnStruct->parameterK, TRUE, mainHashA, controlHash1, modelHT);

//     // clear the model HT for the next iteration.
//     clearUHashStructure(modelHT);
//   }

//   return nnStruct;
// }


// Optimizes the nnStruct (non-agressively, i.e., without changing the
// parameters).
void optimizeLSH(PRNearNeighborStructT nnStruct){
  ASSERT(nnStruct != NULL);

  PointsListEntryT *auxList = NULL;
  for(IntT i = 0; i < nnStruct->parameterL; i++){
    optimizeUHashStructure(nnStruct->hashedBuckets[i], auxList);
  }
  FREE(auxList);
}

// Frees completely all the memory occupied by the <nnStruct>
// structure.
void freePRNearNeighborStruct(PRNearNeighborStructT nnStruct){
  if (nnStruct == NULL){
    return;
  }

  if (nnStruct->points != NULL) {
    free(nnStruct->points);
  }
  
  if (nnStruct->lshFunctions != NULL) {
    for(IntT i = 0; i < nnStruct->nHFTuples; i++){
      for(IntT j = 0; j < nnStruct->hfTuplesLength; j++){
	free(nnStruct->lshFunctions[i][j].a);
      }
      free(nnStruct->lshFunctions[i]);
    }
    free(nnStruct->lshFunctions);
  }
  
  if (nnStruct->precomputedHashesOfULSHs != NULL) {
    for(IntT i = 0; i < nnStruct->nHFTuples; i++){
      free(nnStruct->precomputedHashesOfULSHs[i]);
    }
    free(nnStruct->precomputedHashesOfULSHs);
  }

  freeUHashStructure(nnStruct->hashedBuckets[0], TRUE);
  for(IntT i = 1; i < nnStruct->parameterL; i++){
    freeUHashStructure(nnStruct->hashedBuckets[i], FALSE);
  }
  free(nnStruct->hashedBuckets);

  if (nnStruct->pointULSHVectors != NULL){
    for(IntT i = 0; i < nnStruct->nHFTuples; i++){
      free(nnStruct->pointULSHVectors[i]);
    }
    free(nnStruct->pointULSHVectors);
  }

  if (nnStruct->reducedPoint != NULL){
    free(nnStruct->reducedPoint);
  }

  if (nnStruct->markedPoints != NULL){
    free(nnStruct->markedPoints);
  }

  if (nnStruct->markedPointsIndeces != NULL){
    free(nnStruct->markedPointsIndeces);
  }
}

// If <reportingResult> == FALSe, no points are reported back in a
// <get> function. In particular any point that is found in the bucket
// is considered to be outside the R-ball of the query point.  If
// <reportingResult> == TRUE, then the structure behaves normally.
void setResultReporting(PRNearNeighborStructT nnStruct, BooleanT reportingResult){
  ASSERT(nnStruct != NULL);
  nnStruct->reportingResult = reportingResult;
}

// Compute the value of a hash function u=lshFunctions[gNumber] (a
// vector of <hfTuplesLength> LSH functions) in the point <point>. The
// result is stored in the vector <vectorValue>. <vectorValue> must be
// already allocated (and have space for <hfTuplesLength> Uns32T-words).
inline void computeULSH(PRNearNeighborStructT nnStruct, IntT gNumber, RealT *point, Uns32T *vectorValue)
{//求出point向量和多个hansh映射后的值， 对于每个hash：  a。v+b 除以 r
	//结果返回到vectorValue 向量上
  CR_ASSERT(nnStruct != NULL);
  CR_ASSERT(point != NULL);
  CR_ASSERT(vectorValue != NULL);


 //  FILE *file=fopen("vector.txt","a+");
 //   fprintf(file,"\n\n");

  for(IntT i = 0; i < nnStruct->hfTuplesLength; i++){
    RealT value = 0;
    for(IntT d = 0; d < nnStruct->dimension; d++){
      value += point[d] * nnStruct->lshFunctions[gNumber][i].a[d];
	  //两个向量point[]。第gnumber的hash向量 点乘 ； 就是a。v
    }
  
	value=value*97;//放大10倍看看
	double tempv=( (value + nnStruct->lshFunctions[gNumber][i].b)  );
	double temp_w=tempv/ nnStruct->parameterW ;
	int vi=temp_w;
	if ( vi < 0)
	{
		vi+=1793;
	}
	 vectorValue[i] = (Uns32T)(FLOOR_INT32( (value + nnStruct->lshFunctions[gNumber][i].b)  / nnStruct->parameterW )) ;
	 vectorValue[i] =vi;
	// fprintf(file,"%lf  %lf %d  ||",value,temp_w ,vi );

  //  vectorValue[i] = (Uns32T)(FLOOR_INT32( (value + nnStruct->lshFunctions[gNumber][i].b) / nnStruct->parameterW) /* - MIN_INT32T*/);
  //a。v+b 除以 r
  }

  //	 fclose(file);
}

inline void preparePointAdding(PRNearNeighborStructT nnStruct, PUHashStructureT uhash, PPointT point)
{
//输入： nnntstuct结构体（降维的向量），  uhash（hash的两个主副向量）  特征点
//操作：先计算点的降维结果，然后计算两个hash索引值 保存到nnStruct->precomputedHashesOfULSHs
//功能，提前计算好每个点的hash表索引值

	//根据传入的多维point。
	//计算对应每个hash表的降维=》hash值，
	//存入了nnStruct->precomputedHashesOfULSHs
  ASSERT(nnStruct != NULL);
  ASSERT(uhash != NULL);
  ASSERT(point != NULL);

  TIMEV_START(timeComputeULSH);
  for(IntT d = 0; d < nnStruct->dimension; d++){
    nnStruct->reducedPoint[d] = point->coordinates[d] / nnStruct->parameterR;
  }


  //降维
  // Compute all ULSH functions.
  for(IntT i = 0; i < nnStruct->nHFTuples; i++){//nHFTuples是元组个数，
	  //求出nnStruct->reducedPoint向量和多个hansh映射后的值， 对于每个hash：  a。v+b 除以 r
	  //结果返回到pointULSHVectors【】 向量上
	  //pointULSHVectors【i】就是 第一步降维后的向量值
    computeULSH(nnStruct, i, nnStruct->reducedPoint, nnStruct->pointULSHVectors[i]);
  }

  //模hash
  // Compute data for <precomputedHashesOfULSHs>.
  if (USE_SAME_UHASH_FUNCTIONS) {
    for(IntT i = 0; i < nnStruct->nHFTuples; i++){
      precomputeUHFsForULSH(uhash, nnStruct->pointULSHVectors[i], nnStruct->hfTuplesLength, nnStruct->precomputedHashesOfULSHs[i]);
	 //根据降维后的结果向量：pointULSHVectors【i】  计算两个hash值，存入precomputedHashesOfULSHs【i】
    }
  }

  TIMEV_END(timeComputeULSH);
}

inline void batchAddRequest(PRNearNeighborStructT nnStruct, IntT i, IntT &firstIndex, IntT &secondIndex, PPointT point){
//   Uns32T *(gVector[4]);
//   if (!nnStruct->useUfunctions) {
//     // Use usual <g> functions (truly independent).
//     gVector[0] = nnStruct->pointULSHVectors[i];
//     gVector[1] = nnStruct->precomputedHashesOfULSHs[i];
//     addBucketEntry(nnStruct->hashedBuckets[firstIndex], gVector, 1, point);
//   } else {
//     // Use <u> functions (<g>s are pairs of <u> functions).
//     gVector[0] = nnStruct->pointULSHVectors[firstIndex];
//     gVector[1] = nnStruct->pointULSHVectors[secondIndex];
//     gVector[2] = nnStruct->precomputedHashesOfULSHs[firstIndex];
//     gVector[3] = nnStruct->precomputedHashesOfULSHs[secondIndex];
    
//     // compute what is the next pair of <u> functions.
//     secondIndex++;
//     if (secondIndex == nnStruct->nHFTuples) {
//       firstIndex++;
//       secondIndex = firstIndex + 1;
//     }
    
//     addBucketEntry(nnStruct->hashedBuckets[i], gVector, 2, point);
//   }
  ASSERT(1 == 0);
}

// Adds a new point to the LSH data structure, that is for each
// i=0..parameterL-1, the point is added to the bucket defined by
// function g_i=lshFunctions[i].
void addNewPointToPRNearNeighborStruct(PRNearNeighborStructT nnStruct, PPointT point){
  ASSERT(nnStruct != NULL);
  ASSERT(point != NULL);
  ASSERT(nnStruct->reducedPoint != NULL);
  ASSERT(!nnStruct->useUfunctions || nnStruct->pointULSHVectors != NULL);
  ASSERT(nnStruct->hashedBuckets[0]->typeHT == HT_LINKED_LIST || nnStruct->hashedBuckets[0]->typeHT == HT_STATISTICS);

  nnStruct->points[nnStruct->nPoints] = point;
  nnStruct->nPoints++;

  preparePointAdding(nnStruct, nnStruct->hashedBuckets[0], point);

  // Initialize the counters for defining the pair of <u> functions used for <g> functions.
  IntT firstUComp = 0;
  IntT secondUComp = 1;

  TIMEV_START(timeBucketIntoUH);
  for(IntT i = 0; i < nnStruct->parameterL; i++){
    if (!nnStruct->useUfunctions) {
      // Use usual <g> functions (truly independent; <g>s are precisly
      // <u>s).
      addBucketEntry(nnStruct->hashedBuckets[i], 1, nnStruct->precomputedHashesOfULSHs[i], NULL, nnStruct->nPoints - 1);
    } else {
      // Use <u> functions (<g>s are pairs of <u> functions).
      addBucketEntry(nnStruct->hashedBuckets[i], 2, nnStruct->precomputedHashesOfULSHs[firstUComp], nnStruct->precomputedHashesOfULSHs[secondUComp], nnStruct->nPoints - 1);

      // compute what is the next pair of <u> functions.
      secondUComp++;
      if (secondUComp == nnStruct->nHFTuples) {
	firstUComp++;
	secondUComp = firstUComp + 1;
      }
    }
    //batchAddRequest(nnStruct, i, firstUComp, secondUComp, point);
  }
  TIMEV_END(timeBucketIntoUH);

  // Check whether the vectors <nearPoints> & <nearPointsIndeces> is still big enough.
  if (nnStruct->nPoints > nnStruct->sizeMarkedPoints) {
    nnStruct->sizeMarkedPoints = 2 * nnStruct->nPoints;
    FAILIF(NULL == (nnStruct->markedPoints = (BooleanT*)REALLOC(nnStruct->markedPoints, nnStruct->sizeMarkedPoints * sizeof(BooleanT))));
    for(IntT i = 0; i < nnStruct->sizeMarkedPoints; i++){
      nnStruct->markedPoints[i] = FALSE;
    }
    FAILIF(NULL == (nnStruct->markedPointsIndeces = (Int32T*)REALLOC(nnStruct->markedPointsIndeces, nnStruct->sizeMarkedPoints * sizeof(Int32T))));
  }
}

// Returns TRUE iff |p1-p2|_2^2 <= threshold
inline BooleanT isDistanceSqrLeq(IntT dimension, PPointT p1, PPointT p2, RealT threshold)
{//直接计算：|p1-p2|_2^2 <= threshold；两点距离是否小于阈值
  RealT result = 0;
  nOfDistComps++;

  TIMEV_START(timeDistanceComputation);
  for (IntT i = 0; i < dimension; i++){
    RealT temp = p1->coordinates[i] - p2->coordinates[i];
#ifdef USE_L1_DISTANCE
    result += ABS(temp);
#else
    result += SQR(temp);
#endif
    if (result > threshold){
      // TIMEV_END(timeDistanceComputation);
      return 0;
    }
  }
  TIMEV_END(timeDistanceComputation);

  //return result <= threshold;
  return 1;
}

// // Returns TRUE iff |p1-p2|_2^2 <= threshold
// inline BooleanT isDistanceSqrLeq(IntT dimension, PPointT p1, PPointT p2, RealT threshold){
//   RealT result = 0;
//   nOfDistComps++;

//   //TIMEV_START(timeDistanceComputation);
//   for (IntT i = 0; i < dimension; i++){
//     result += p1->coordinates[i] * p2->coordinates[i];
//   }
//   //TIMEV_END(timeDistanceComputation);

//   return p1->sqrLength + p2->sqrLength - 2 * result <= threshold;
// }

// Returns the list of near neighbors of the point <point> (with a
// certain success probability). Near neighbor is defined as being a
// point within distance <parameterR>. Each near neighbor from the
// data set is returned is returned with a certain probability,
// dependent on <parameterK>, <parameterL>, and <parameterT>. The
// returned points are kept in the array <result>. If result is not
// allocated, it will be allocated to at least some minimum size
// (RESULT_INIT_SIZE). If number of returned points is bigger than the
// size of <result>, then the <result> is resized (to up to twice the
// number of returned points). The return value is the number of
// points found.
Int32T getNearNeighborsFromPRNearNeighborStruct(
	PRNearNeighborStructT nnStruct, PPointT query,
	PPointT *(&result), Int32T &resultSize)
{//通过查找索引，然后获得桶，提取n个最近邻点
	//通过计算点的降维值，然后计算主副索引，最后由索引查找表
	ASSERT(nnStruct != NULL);
  ASSERT(query != NULL);
  ASSERT(nnStruct->reducedPoint != NULL);
  ASSERT(!nnStruct->useUfunctions || nnStruct->pointULSHVectors != NULL);

  PPointT point = query;

  if (result == NULL)
  {
    resultSize = RESULT_INIT_SIZE;
    FAILIF(NULL == (result = (PPointT*)MALLOC(resultSize * sizeof(PPointT))));
  }
  
  /*
  for (int tempd=150; tempd< 160;tempd++)
  {
	  printf(" %lf ",query->coordinates[tempd]);
  }

  printf("查询的具体数据 10个  \n\n");
   printf("查询数据 :  %lf  \n",query->coordinates[151]);
  // printf( "主hash的值：  %u  \n",nnStruct->hehasdBuckets[0]->mainHashA[5]);
  // printf( "辅助hash的值：  %u  \n",nnStruct->hashedBuckets[0]->controlHash1[5]);

 //  printf( "a     %u \n",nnStruct->lshFunctions[0][0].a[5]);
 //  printf( "b     %u \n",nnStruct->lshFunctions[0][0].b  );
 */

  preparePointAdding(nnStruct, nnStruct->hashedBuckets[0], point);
  //根据传入的多维point。计算对应每个hash表的降维=》hash值，存入了nnStruct->precomputedHashesOfULSHs
  

  Uns32T **(precomputedHashesOfULSHs);//没释放
  precomputedHashesOfULSHs= (Uns32T**)malloc(sizeof(Uns32T*)*(nnStruct->nHFTuples));
 // Uns32T precomputedHashesOfULSHs[nnStruct->nHFTuples][N_PRECOMPUTED_HASHES_NEEDED];
  for (IntT i=0;i< nnStruct->nHFTuples ;i++)
  {
	    precomputedHashesOfULSHs[i]= (Uns32T*)malloc(sizeof(Uns32T)*(N_PRECOMPUTED_HASHES_NEEDED));

		for (int temi=0;temi< N_PRECOMPUTED_HASHES_NEEDED ;temi++)
		{
			precomputedHashesOfULSHs[i][temi]=0;
		}
  }

  //初始化？？

    /*
  printf("\n输出：\n");

  FILE *in = fopen("preconpute.txt", "a+") ;
  fprintf(in,"\n输出：\n");
  fclose(in);
  */

  for(IntT i = 0; i < nnStruct->nHFTuples; i++)
  {
    for(IntT j = 0; j < N_PRECOMPUTED_HASHES_NEEDED; j++)
	{
      precomputedHashesOfULSHs[i][j] = nnStruct->precomputedHashesOfULSHs[i][j];
	  		  
	/*   printf(" %u", precomputedHashesOfULSHs[i][j]);
	
		  FILE *in = fopen("preconpute.txt", "a+") ;
		  fprintf(in," %u", precomputedHashesOfULSHs[i][j]);
		    fclose(in);
		   */
	}

	/*printf(" \n");
	
	  FILE *in = fopen("preconpute.txt", "a+") ;
	  fprintf(in," \n");
	  fclose(in);

	  */
 }


  TIMEV_START(timeTotalBuckets);

  BooleanT oldTimingOn = timingOn;
  if (noExpensiveTiming)
  {
    timingOn = FALSE;
  }
  
  // Initialize the counters for defining the pair of <u> functions used for <g> functions.
  IntT firstUComp = 0;
  IntT secondUComp = 1;

  Int32T nNeighbors = 0;// the number of near neighbors found so far.
  Int32T nMarkedPoints = 0;// the number of marked points
  for(IntT i = 0; i < nnStruct->parameterL; i++)
  { //L个表
    TIMEV_START(timeGetBucket);
    GeneralizedPGBucket gbucket;
    if (!nnStruct->useUfunctions)
	{
      // Use usual <g> functions (truly independent; <g>s are precisly
      // <u>s).
      gbucket = getGBucket(nnStruct->hashedBuckets[i], 1, precomputedHashesOfULSHs[i], NULL);
    } 
	else 
	{
      // Use <u> functions (<g>s are pairs of <u> functions).
      gbucket = getGBucket(nnStruct->hashedBuckets[i], 2, precomputedHashesOfULSHs[firstUComp], precomputedHashesOfULSHs[secondUComp]);
	  //通过两个向量，计算主副索引。然后遍历二级索引，提取对应的桶

      // compute what is the next pair of <u> functions.
	  //不是每个都 （first，second ）（first，second ）（first，second ）的数组吗？
      secondUComp++;
      if (secondUComp == nnStruct->nHFTuples)
	  {
		  firstUComp++;
		  secondUComp = firstUComp + 1;
	  }
    }

    TIMEV_END(timeGetBucket);

    PGBucketT bucket;

    TIMEV_START(timeCycleBucket);
    switch (nnStruct->hashedBuckets[i]->typeHT)
	{//对不同类型的hash桶结构，使用不同方法获取二级桶的实体
	case HT_LINKED_LIST:
		bucket = gbucket.llGBucket;
		if (bucket != NULL)
		{
			// circle through the bucket and add to <result> the points that are near.
			PBucketEntryT bucketEntry = &(bucket->firstEntry);
			//TIMEV_START(timeCycleProc);
			while (bucketEntry != NULL)
			{
			  //TIMEV_END(timeCycleProc);
			  //ASSERT(bucketEntry->point != NULL);
			  //TIMEV_START(timeDistanceComputation);
			  Int32T candidatePIndex = bucketEntry->pointIndex;
			  PPointT candidatePoint = nnStruct->points[candidatePIndex];
			  if (isDistanceSqrLeq(nnStruct->dimension, point, candidatePoint, nnStruct->parameterR2)
				  && nnStruct->reportingResult)
			  {

				//TIMEV_END(timeDistanceComputation);
				if (nnStruct->markedPoints[candidatePIndex] == FALSE) 
				{

				  //TIMEV_START(timeResultStoring);
				  // a new R-NN point was found (not yet in <result>).
				  if (nNeighbors >= resultSize)
				  {
					// run out of space => resize the <result> array.
					resultSize = 2 * resultSize;
					result = (PPointT*)REALLOC(result, resultSize * sizeof(PPointT));
				  }
				  result[nNeighbors] = candidatePoint;
				  nNeighbors++;
				  nnStruct->markedPointsIndeces[nMarkedPoints] = candidatePIndex;
				  nnStruct->markedPoints[candidatePIndex] = TRUE; // do not include more points with the same index
				  nMarkedPoints++;
				  //TIMEV_END(timeResultStoring);
				}
			  }
			  else
			  {
				//TIMEV_END(timeDistanceComputation);
			  }
			  //TIMEV_START(timeCycleProc);
			  bucketEntry = bucketEntry->nextEntry;
			}//while

		//TIMEV_END(timeCycleProc);
		}
      break;


    case HT_STATISTICS:
      ASSERT(FALSE); // HT_STATISTICS not supported anymore
		//       if (gbucket.linkGBucket != NULL && gbucket.linkGBucket->indexStart != INDEX_START_EMPTY){
		// 	Int32T position;
		// 	PointsListEntryT *pointsList = nnStruct->hashedBuckets[i]->bucketPoints.pointsList;
		// 	position = gbucket.linkGBucket->indexStart;
		// 	// circle through the bucket and add to <result> the points that are near.
		// 	while (position != INDEX_START_EMPTY){
		// 	  PPointT candidatePoint = pointsList[position].point;
		// 	  if (isDistanceSqrLeq(nnStruct->dimension, point, candidatePoint, nnStruct->parameterR2) && nnStruct->reportingResult){
		// 	    if (nnStruct->nearPoints[candidatePoint->index] == FALSE) {
		// 	      // a new R-NN point was found (not yet in <result>).
		// 	      if (nNeighbors >= resultSize){
		// 		// run out of space => resize the <result> array.
		// 		resultSize = 2 * resultSize;
		// 		result = (PPointT*)REALLOC(result, resultSize * sizeof(PPointT));
		// 	      }
		// 	      result[nNeighbors] = candidatePoint;
		// 	      nNeighbors++;
		// 	      nnStruct->nearPoints[candidatePoint->index] = TRUE; // do not include more points with the same index
		// 	    }
		// 	  }
		// 	  // Int32T oldP = position;
		// 	  position = pointsList[position].nextPoint;
		// 	  // ASSERT(position == INDEX_START_EMPTY || position == oldP + 1);
		// 	}
		//       }
      break;

    case HT_HYBRID_CHAINS://默认的链条
      if (gbucket.hybridGBucket != NULL)
	  {//好像是在链表中找空间，同时要判断没有重复的
		PHybridChainEntryT hybridPoint = gbucket.hybridGBucket;//获取 二级桶的数组指针，（实际桶就是一个数组）
		Uns32T offset = 0;
		if (hybridPoint->point.bucketLength == 0)
		{//长度为0，就是溢出了的桶，

		  // there are overflow points in this bucket.
		  offset = 0;
		  for(IntT j = 0; j < N_FIELDS_PER_INDEX_OF_OVERFLOW; j++)
		  {
			offset += ((Uns32T)((hybridPoint + 1 + j)->point.bucketLength) << (j * N_BITS_FOR_BUCKET_LENGTH));
		  }
		}
		Uns32T index = 0;
		BooleanT done = FALSE;
		while(!done)
		{
			  if (index == MAX_NONOVERFLOW_POINTS_PER_BUCKET)
			  {
				//CR_ASSERT(hybridPoint->point.bucketLength == 0);
				index = index + offset;
			  }
			  //hybridPoint 是个二级桶+实体组成的数组的首地址（其实就是个二级刻度）
			  Int32T candidatePIndex = (hybridPoint + index)->point.pointIndex;

			  //索引只是记录每个点的序号， 所有点都在nnStruct->points[candidatePIndex] 上保存具体值

			  CR_ASSERT(candidatePIndex >= 0 && candidatePIndex < nnStruct->nPoints);
			  done = (hybridPoint + index)->point.isLastPoint == 1 ? TRUE : FALSE;
			  //链表的遍历？好像是用数组来当链表用
			  index++;
		 
			  if (nnStruct->markedPoints[candidatePIndex] == FALSE)
			  {//已经计算过的点都标记为true了
				  //nnStruct->markedPoints 是用来标记是否检测过得

				// mark the point first.
				nnStruct->markedPointsIndeces[nMarkedPoints] = candidatePIndex;
				nnStruct->markedPoints[candidatePIndex] = TRUE; // do not include more points with the same index
				nMarkedPoints++;

				PPointT candidatePoint = nnStruct->points[candidatePIndex];
				if (isDistanceSqrLeq(nnStruct->dimension, point, candidatePoint, nnStruct->parameterR2)
					&& nnStruct->reportingResult)
				{//两点距离是否小于阈值
				  //if (nnStruct->markedPoints[candidatePIndex] == FALSE) {
				  // a new R-NN point was found (not yet in <result>).
				  //TIMEV_START(timeResultStoring);
				  if (nNeighbors >= resultSize)
				  {//近邻点太多，扩大空间
					// run out of space => resize the <result> array.
					  resultSize = 2 * resultSize;
					  result = (PPointT*)REALLOC(result, resultSize * sizeof(PPointT));
				  }
				  result[nNeighbors] = candidatePoint;//存入返回结果中
				  nNeighbors++;
				  //TIMEV_END(timeResultStoring);
				  //nnStruct->markedPointsIndeces[nMarkedPoints] = candidatePIndex;
				  //nnStruct->markedPoints[candidatePIndex] = TRUE; // do not include more points with the same index
				  //nMarkedPoints++;
				  //}
				}
			  }// if (nnStruct->markedPoints[candidatePIndex] == FALSE)
		  else
			  {
				// the point was already marked (& examined)
			  }
		}//	while(!done)


      }// if (gbucket.hybridGBucket != NULL)
      break;
   
	default:
      ASSERT(FALSE);
    }//swichcase


    TIMEV_END(timeCycleBucket);  
  }//for

  timingOn = oldTimingOn;
  TIMEV_END(timeTotalBuckets);

  // we need to clear the array nnStruct->nearPoints for the next query.
  for(Int32T i = 0; i < nMarkedPoints; i++)
  {
    ASSERT(nnStruct->markedPoints[nnStruct->markedPointsIndeces[i]] == TRUE);
    nnStruct->markedPoints[nnStruct->markedPointsIndeces[i]] = FALSE;
  }
  DPRINTF("nMarkedPoints: %d\n", nMarkedPoints);

  return nNeighbors;
}
