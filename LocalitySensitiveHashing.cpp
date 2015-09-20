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
{//���ļ�������
  ASSERT(input != NULL);
  RNNParametersT parameters;
  char s[1000];// TODO: possible buffer overflow

  fscanf(input, "\n");fscanf(input, "%[^\n]\n", s);
  FSCANF_REAL(input, &parameters.parameterR);//��̼��ɣ������duimput��������

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
{/*���� nHFTuples�� �飬hfTuplesLength�����յ�ά��
    ������˹�ֲ������ֵ��  a������ b������  nnStruct->lshFunctions��������
	���ڵ�һ���ĵ����ά
 */
	
	//����gi  hi ���㺯����ĸ�ʽ����ʼ��nnStruct->lshFunctions��ָ��Ķ�άָ��
 
	//nHFTuples��hash����g��ÿ������hfTuplesLength��hi��ÿ��hi����һ������a��һ��b

	//����a��v+b �ĸ�ʽ����ʼ���ṹ���е���������Ϳռ�
	//nnStruct�ṹ���У���ʼ��nnStruct->lshFunctions ��ָ��Ķ�ά�������
  ASSERT(nnStruct != NULL);//��̼��ɣ� asssert���ж�����ִ�У�  __line__  --file__��ʾλ��
   LSHFunctionT **lshFunctions;//LSHFunctionT�ṹ�������Ԫ�أ�һ��ָ��+һ��double
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
  //���￪ʼ�����㷨�е�һ�顰λ�����С���Hash������
  for(IntT i = 0; i < nnStruct->nHFTuples; i++)
  {
    for(IntT j = 0; j < nnStruct->hfTuplesLength; j++)
	{
      // vector a
      for(IntT d = 0; d < nnStruct->dimension; d++)
	  {
#ifdef USE_L1_DISTANCE
	lshFunctions[i][j].a[d] = genCauchyRandom();//L1�����п����ֲ�
#else
	lshFunctions[i][j].a[d] = genGaussianRandom(); //L2ʹ�ø�˹�ֲ�   ��ʽ��a����һ��dά����̬�ֲ������
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
{//��������ṹ�壬��ʼ����gi  hi���������ֵ
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
    nnStruct->nHFTuples = algParameters.parameterM;//����gi�������
    nnStruct->hfTuplesLength = algParameters.parameterK / 2;//����ÿ��gi�� ά�ȳ�
  }
  nnStruct->parameterT = algParameters.parameterT;
  nnStruct->dimension = algParameters.dimension;
  nnStruct->parameterW = algParameters.parameterW;

  nnStruct->nPoints = 0;
  nnStruct->pointsArraySize = nPointsEstimate;

  FAILIF(NULL == (nnStruct->points = (PPointT*)MALLOC(nnStruct->pointsArraySize * sizeof(PPointT))));

  // create the hash functions
  initHashFunctions(nnStruct);//����gi  hi ���㺯����ĸ�ʽ����ʼ��nnStruct->lshFunctions��ָ��Ķ�άָ��
  /*���� nHFTuples�� �飬hfTuplesLength�����յ�ά��
    ������˹�ֲ������ֵ��  a������ b������  nnStruct->lshFunctions��������
	���ڵ�һ���ĵ����ά
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
{//��ʼ���������ݽṹ ��������+l��hash�� +��ӳ�䵽Ͱ
/*���壺�ȳ�ʼ������ṹ�壺 PRNearNeighborStructT nnStruct
 ��ʼ���������gi hi�������
	
	Ȼ���ʼ��PUHashStructureT modelHT�����hansh��

	��󣬽���� modelHT ���ӵ�nnStruct
	*/
  ASSERT(algParameters.typeHT == HT_HYBRID_CHAINS);
  ASSERT(dataSet != NULL);
  ASSERT(USE_SAME_UHASH_FUNCTIONS);

  PRNearNeighborStructT nnStruct = initializePRNearNeighborFields(algParameters, nPoints);
  //��������ṹ�壬��ʼ����gi  hi���������ֵ
 //����gi  hi ���㺯����ĸ�ʽ����ʼ��nnStruct->lshFunctions��ָ��Ķ�άָ��
 //   ������˹�ֲ������ֵ��  a������ b������  nnStruct->lshFunctions��������
 
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
  //��ʼ��hash��Ͱ�ṹ
  //����hash�͸���hash��ĳ�ʼ������һ�����ֵ
  /* ���� uhash ��
 ��typeHT= HT_LINKED_LISTʱ��ֱ�ӳ�ʼ�����飬
 Ȼ�����������hash�� ������ ����hash���������
 ����  (u ��x)%(2^32 -5)/tablesize �е�u1���� 
 �� (u ��x)%(2^32 -5) �е�u2���� 
  ���շ��أ�PUHashStructureT uhash
 */
   
   Uns32T ***(precomputedHashesOfULSHs);
   precomputedHashesOfULSHs= (Uns32T***)malloc(sizeof(Uns32T**)*(nnStruct->nHFTuples));
   //û���ͷ�
 // Uns32T **(precomputedHashesOfULSHs[ (nnStruct->nHFTuples) ]); windows�²�����ֵ̬��������

  for(IntT l = 0; l < nnStruct->nHFTuples; l++)
  {//ÿ��hash���� gi
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
	  //���ݴ���Ķ�άpoint�������Ӧÿ��hash��Ľ�ά=��hashֵ��������nnStruct->precomputedHashesOfULSHs
    preparePointAdding(nnStruct, modelHT, dataSet[i]);

/*	if ( i ==0)
	{
		for (int tempd=150; tempd< 160;tempd++)
		{
			printf(" %lf ",dataSet[i]->coordinates[tempd]);
		}
 	 
	//	printf("��ʼ������ 10�� \n\n");

//		 printf("  :  %lf  \n",dataSet[i][151]);
		//   printf( "��hash��ֵ��  %u \n",modelHT->mainHashA[5] );
	//	   printf( "����hash��ֵ��  %u \n",modelHT->controlHash1[5] );

		//   printf( "a     %u \n",nnStruct->lshFunctions[0][0].a[5]);
	//	   printf( "b     %u \n",nnStruct->lshFunctions[0][0].b  );
 
	}
	*/
    for(IntT l = 0; l < nnStruct->nHFTuples; l++)
	{
      for(IntT h = 0; h < N_PRECOMPUTED_HASHES_NEEDED; h++)
	  {//precomputedHashesOfULSHs�ṹ������ǰ����õ�����hashֵ
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
  {//l����ÿ����	
	  // build the model HT.
    for(IntT p = 0; p < nPoints; p++)
	{  //����ÿ���㣬��hash����Ӧ��Ͱ�Ȼ����hash�ṹ��
      // Add point <dataSet[p]> to modelHT.
      if (!nnStruct->useUfunctions) 
	  {
		// Use usual <g> functions (truly independent; <g>s are precisly
		// <u>s).
		addBucketEntry(modelHT, 1, precomputedHashesOfULSHs[i][p], NULL, p);
		//����, precomputedHashesOfULSHs[i][p]������hashֵ��Ȼ�����Ͱ��������
      }
	  else 
	  {//nnStruct->useUfunctions ��ʾ��άֻȡһ�㣺8��������8ά������ֵ ��16ά������� �����μ�����   
		  //������ firstUComp  secondUComp��ʾ��������
		// Use <u> functions (<g>s are pairs of <u> functions).
		addBucketEntry(modelHT, 2, precomputedHashesOfULSHs[firstUComp][p], precomputedHashesOfULSHs[secondUComp][p], p);
      }
    }

    //ASSERT(nAllocatedGBuckets <= nPoints);
    //ASSERT(nAllocatedBEntries <= nPoints);

    // compute what is the next pair of <u> functions.
    secondUComp++;
    if (secondUComp == nnStruct->nHFTuples)
	{//�������㣺���� 263 ��һ��+263����������  ���� 263����Ҫ��һ����
      firstUComp++;
      secondUComp = firstUComp + 1;
    }

    // copy the model HT into the actual (packed) HT. copy the uhash function too.
    nnStruct->hashedBuckets[i] = newUHashStructure(algParameters.typeHT, nPoints, nnStruct->parameterK, TRUE, mainHashA, controlHash1, modelHT);
	//��ʼ��hash��Ͱ�ṹ
	//�������е�modelHT ת��Ϊ�ռ���յ�HT_HYBRID_CHAINS ��

    // clear the model HT for the next iteration.
    clearUHashStructure(modelHT);//������Ҫ�ͷ��ڴ�
  }

  //�ͷ�
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


 // printf("\n\n����ǰ :\n");

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
{//���point�����Ͷ��hanshӳ����ֵ�� ����ÿ��hash��  a��v+b ���� r
	//������ص�vectorValue ������
  CR_ASSERT(nnStruct != NULL);
  CR_ASSERT(point != NULL);
  CR_ASSERT(vectorValue != NULL);


 //  FILE *file=fopen("vector.txt","a+");
 //   fprintf(file,"\n\n");

  for(IntT i = 0; i < nnStruct->hfTuplesLength; i++){
    RealT value = 0;
    for(IntT d = 0; d < nnStruct->dimension; d++){
      value += point[d] * nnStruct->lshFunctions[gNumber][i].a[d];
	  //��������point[]����gnumber��hash���� ��� �� ����a��v
    }
  
	value=value*97;//�Ŵ�10������
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
  //a��v+b ���� r
  }

  //	 fclose(file);
}

inline void preparePointAdding(PRNearNeighborStructT nnStruct, PUHashStructureT uhash, PPointT point)
{
//���룺 nnntstuct�ṹ�壨��ά����������  uhash��hash����������������  ������
//�������ȼ����Ľ�ά�����Ȼ���������hash����ֵ ���浽nnStruct->precomputedHashesOfULSHs
//���ܣ���ǰ�����ÿ�����hash������ֵ

	//���ݴ���Ķ�άpoint��
	//�����Ӧÿ��hash��Ľ�ά=��hashֵ��
	//������nnStruct->precomputedHashesOfULSHs
  ASSERT(nnStruct != NULL);
  ASSERT(uhash != NULL);
  ASSERT(point != NULL);

  TIMEV_START(timeComputeULSH);
  for(IntT d = 0; d < nnStruct->dimension; d++){
    nnStruct->reducedPoint[d] = point->coordinates[d] / nnStruct->parameterR;
  }


  //��ά
  // Compute all ULSH functions.
  for(IntT i = 0; i < nnStruct->nHFTuples; i++){//nHFTuples��Ԫ�������
	  //���nnStruct->reducedPoint�����Ͷ��hanshӳ����ֵ�� ����ÿ��hash��  a��v+b ���� r
	  //������ص�pointULSHVectors���� ������
	  //pointULSHVectors��i������ ��һ����ά�������ֵ
    computeULSH(nnStruct, i, nnStruct->reducedPoint, nnStruct->pointULSHVectors[i]);
  }

  //ģhash
  // Compute data for <precomputedHashesOfULSHs>.
  if (USE_SAME_UHASH_FUNCTIONS) {
    for(IntT i = 0; i < nnStruct->nHFTuples; i++){
      precomputeUHFsForULSH(uhash, nnStruct->pointULSHVectors[i], nnStruct->hfTuplesLength, nnStruct->precomputedHashesOfULSHs[i]);
	 //���ݽ�ά��Ľ��������pointULSHVectors��i��  ��������hashֵ������precomputedHashesOfULSHs��i��
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
{//ֱ�Ӽ��㣺|p1-p2|_2^2 <= threshold����������Ƿ�С����ֵ
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
{//ͨ������������Ȼ����Ͱ����ȡn������ڵ�
	//ͨ�������Ľ�άֵ��Ȼ���������������������������ұ�
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

  printf("��ѯ�ľ������� 10��  \n\n");
   printf("��ѯ���� :  %lf  \n",query->coordinates[151]);
  // printf( "��hash��ֵ��  %u  \n",nnStruct->hehasdBuckets[0]->mainHashA[5]);
  // printf( "����hash��ֵ��  %u  \n",nnStruct->hashedBuckets[0]->controlHash1[5]);

 //  printf( "a     %u \n",nnStruct->lshFunctions[0][0].a[5]);
 //  printf( "b     %u \n",nnStruct->lshFunctions[0][0].b  );
 */

  preparePointAdding(nnStruct, nnStruct->hashedBuckets[0], point);
  //���ݴ���Ķ�άpoint�������Ӧÿ��hash��Ľ�ά=��hashֵ��������nnStruct->precomputedHashesOfULSHs
  

  Uns32T **(precomputedHashesOfULSHs);//û�ͷ�
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

  //��ʼ������

    /*
  printf("\n�����\n");

  FILE *in = fopen("preconpute.txt", "a+") ;
  fprintf(in,"\n�����\n");
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
  { //L����
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
	  //ͨ��������������������������Ȼ�����������������ȡ��Ӧ��Ͱ

      // compute what is the next pair of <u> functions.
	  //����ÿ���� ��first��second ����first��second ����first��second ����������
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
	{//�Բ�ͬ���͵�hashͰ�ṹ��ʹ�ò�ͬ������ȡ����Ͱ��ʵ��
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

    case HT_HYBRID_CHAINS://Ĭ�ϵ�����
      if (gbucket.hybridGBucket != NULL)
	  {//���������������ҿռ䣬ͬʱҪ�ж�û���ظ���
		PHybridChainEntryT hybridPoint = gbucket.hybridGBucket;//��ȡ ����Ͱ������ָ�룬��ʵ��Ͱ����һ�����飩
		Uns32T offset = 0;
		if (hybridPoint->point.bucketLength == 0)
		{//����Ϊ0����������˵�Ͱ��

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
			  //hybridPoint �Ǹ�����Ͱ+ʵ����ɵ�������׵�ַ����ʵ���Ǹ������̶ȣ�
			  Int32T candidatePIndex = (hybridPoint + index)->point.pointIndex;

			  //����ֻ�Ǽ�¼ÿ�������ţ� ���е㶼��nnStruct->points[candidatePIndex] �ϱ������ֵ

			  CR_ASSERT(candidatePIndex >= 0 && candidatePIndex < nnStruct->nPoints);
			  done = (hybridPoint + index)->point.isLastPoint == 1 ? TRUE : FALSE;
			  //����ı���������������������������
			  index++;
		 
			  if (nnStruct->markedPoints[candidatePIndex] == FALSE)
			  {//�Ѿ�������ĵ㶼���Ϊtrue��
				  //nnStruct->markedPoints ����������Ƿ������

				// mark the point first.
				nnStruct->markedPointsIndeces[nMarkedPoints] = candidatePIndex;
				nnStruct->markedPoints[candidatePIndex] = TRUE; // do not include more points with the same index
				nMarkedPoints++;

				PPointT candidatePoint = nnStruct->points[candidatePIndex];
				if (isDistanceSqrLeq(nnStruct->dimension, point, candidatePoint, nnStruct->parameterR2)
					&& nnStruct->reportingResult)
				{//��������Ƿ�С����ֵ
				  //if (nnStruct->markedPoints[candidatePIndex] == FALSE) {
				  // a new R-NN point was found (not yet in <result>).
				  //TIMEV_START(timeResultStoring);
				  if (nNeighbors >= resultSize)
				  {//���ڵ�̫�࣬����ռ�
					// run out of space => resize the <result> array.
					  resultSize = 2 * resultSize;
					  result = (PPointT*)REALLOC(result, resultSize * sizeof(PPointT));
				  }
				  result[nNeighbors] = candidatePoint;//���뷵�ؽ����
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
