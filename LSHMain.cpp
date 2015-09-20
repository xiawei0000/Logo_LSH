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
  The main entry file containing the main() function. The main()
  function parses the command line parameters and depending on them
  calls the correspondin functions.
  */
#pragma warning 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "headers.h"

#define N_SAMPLE_QUERY_POINTS 100

// The data set containing all the points.
PPointT *dataSetPoints = NULL;

// Number of points in the data set.
IntT nPoints = 0;

// The dimension of the points.
IntT pointsDimension = 0;

// The value of parameter R (a near neighbor of a point <q> is any
// point <p> from the data set that is the within distance
// <thresholdR>).
//RealT thresholdR = 1.0;

// The succes probability of each point (each near neighbor is
// reported by the algorithm with probability <successProbability>).
RealT successProbability = 0.9;

// Same as <thresholdR>, only an array of R's (for the case when
// multiple R's are specified).
RealT *listOfRadii = NULL;
IntT nRadii = 0;

RealT *memRatiosForNNStructs = NULL;

char sBuffer[600000];

/*
  Prints the usage of the LSHMain.
 */


/*
void usage(char *programName){
  printf("Usage: %s #pts_in_data_set #queries dimension successProbability radius data_set_file query_points_file max_available_memory [-c|-p params_file]\n", programName);
}
*/
inline PPointT readPoint(FILE *fileHandle)
{//从文件中读入点
  PPointT p;
  RealT sqrLength = 0;
  FAILIF(NULL == (p = (PPointT)MALLOC(sizeof(PointT))));
  FAILIF(NULL == (p->coordinates = (RealT*)MALLOC(pointsDimension * sizeof(RealT))));
  for(IntT d = 0; d < pointsDimension; d++){
    FSCANF_REAL(fileHandle, &(p->coordinates[d]));
    sqrLength += SQR(p->coordinates[d]);
  }
  fscanf(fileHandle, "%[^\n]", sBuffer);
  p->index = -1;
  p->sqrLength = sqrLength;
  return p;
}

// Reads in the data set points from <filename> in the array
// <dataSetPoints>. Each point get a unique number in the field
// <index> to be easily indentifiable.
void readDataSetFromFile(char *filename)
{//申请空间，从文件读入点到结构体dataSetPoints，并加上序号
  FILE *f = fopen(filename, "rt");
  FAILIF(f == NULL);
  
  //fscanf(f, "%d %d ", &nPoints, &pointsDimension);
  //FSCANF_DOUBLE(f, &thresholdR);
  //FSCANF_DOUBLE(f, &successProbability);
  //fscanf(f, "\n");
  FAILIF(NULL == (dataSetPoints = (PPointT*)MALLOC(nPoints * sizeof(PPointT))));
  for(IntT i = 0; i < nPoints; i++){
    dataSetPoints[i] = readPoint(f);
    dataSetPoints[i]->index = i;
  }
}

// Tranforming <memRatiosForNNStructs> from
// <memRatiosForNNStructs[i]=ratio of mem/total mem> to
// <memRatiosForNNStructs[i]=ratio of mem/mem left for structs i,i+1,...>.
void transformMemRatios()
{//化为百分比
  RealT sum = 0;
  for(IntT i = nRadii - 1; i >= 0; i--){
    sum += memRatiosForNNStructs[i];
    memRatiosForNNStructs[i] = memRatiosForNNStructs[i] / sum;
    //DPRINTF("%0.6lf\n", memRatiosForNNStructs[i]);
  }
  ASSERT(sum <= 1.000001);
}


int compareInt32T(const void *a, const void *b){
  Int32T *x = (Int32T*)a;
  Int32T *y = (Int32T*)b;
  return (*x > *y) - (*x < *y);
}


/*
  The main entry to LSH package. Depending on the command line
  parameters, the function computes the R-NN data structure optimal
  parameters and/or construct the R-NN data structure and runs the
  queries on the data structure.
 */
int main_T(int nargs, char **args)
{


	//先分析参数
	/* 官方lsh文件：10个参数
	1000 9 784 0.9  0.6  mnist1k.dts  mnist1k.q
	bin/LSHMain $nDataSet $nQuerySet $dimension $successProbability "$1" "$2" "$3" $m -c*/


	//算参数	bin/LSHMain 1000 9 784 0.9  "0.6" "mnist1k.dts" "mnist1k.q" 1002000000  -c


	//bin/LSHMain $nDataSet $nQuerySet $dimension $successProbability 1.0 "$1" "$2" $m -p "$3"

	//匹配	bin/LSHMain 1000 9 784 0.9 1.0  "mnist1k.dts" "mnist1k.q" 1002000000  -p  "outputparma.txt"
  if(nargs < 9)
  {
    usage(args[0]);
    exit(1);
  }

  //initializeLSHGlobal();

  // Parse part of the command-line parameters.
  nPoints = atoi(args[1]);
  IntT nQueries = atoi(args[2]);
  pointsDimension = atoi(args[3]);
  successProbability = atof(args[4]);
  char* endPtr[1];
  RealT thresholdR = strtod(args[5], endPtr);//点相邻的距离阈值
  //str-to -double  将字符串转换成浮点数的函数
  //endPtr 接收数字结尾后非字符串字母

  //这个r阈值是什么呢？
  if (thresholdR == 0 || endPtr[1] == args[5])
  {//如果阈值为0，或者第一个字符就不是数字， 
	  //表示是用文件保存的
	  //这大概是用于测试哪个阈值好的
    // The value for R is not specified, instead there is a file
    // specifying multiple R's.
    thresholdR = 0;

    // Read in the file
    FILE *radiiFile = fopen(args[5], "rt");
    FAILIF(radiiFile == NULL);
    fscanf(radiiFile, "%d\n", &nRadii);
    ASSERT(nRadii > 0);
    FAILIF(NULL == (listOfRadii = (RealT*)MALLOC(nRadii * sizeof(RealT))));
    FAILIF(NULL == (memRatiosForNNStructs = (RealT*)MALLOC(nRadii * sizeof(RealT))));
    for(IntT i = 0; i < nRadii; i++)
	{
      FSCANF_REAL(radiiFile, &listOfRadii[i]);
      ASSERT(listOfRadii[i] > 0);
      FSCANF_REAL(radiiFile, &memRatiosForNNStructs[i]);
      ASSERT(memRatiosForNNStructs[i] > 0);
    }
  }
  else
  {
    nRadii = 1;
    FAILIF(NULL == (listOfRadii = (RealT*)MALLOC(nRadii * sizeof(RealT))));
    FAILIF(NULL == (memRatiosForNNStructs = (RealT*)MALLOC(nRadii * sizeof(RealT))));
    listOfRadii[0] = thresholdR;
    memRatiosForNNStructs[0] = 1;
  }//对阈值R 和Radiii的处理



  DPRINTF("No. radii: %d\n", nRadii);
  //thresholdR = atof(args[5]);
  availableTotalMemory = atoll(args[8]);//$M表示的是内存空间大小

  if (nPoints > MAX_N_POINTS)
  {
    printf("Error: the structure supports at most %d points (%d were specified).\n", MAX_N_POINTS, nPoints);
    fprintf(ERROR_OUTPUT, "Error: the structure supports at most %d points (%d were specified).\n", MAX_N_POINTS, nPoints);
    exit(1);
  }

  readDataSetFromFile(args[6]);//点读到dataSetPoints

  //这个totalAllocatedMemory初始化为0，但是
  //#define MALLOC(amount) ((amount > 0) ? totalAllocatedMemory += amount, malloc(amount) : NULL)
  //这样，每次申请内存都会统计到了

  DPRINTF("Allocated memory (after reading data set): %lld\n", totalAllocatedMemory);

  Int32T nSampleQueries = N_SAMPLE_QUERY_POINTS;
  PPointT sampleQueries[N_SAMPLE_QUERY_POINTS];
  Int32T sampleQBoundaryIndeces[N_SAMPLE_QUERY_POINTS];
//  PPointT sampleQueries[nSampleQueries];
 // Int32T sampleQBoundaryIndeces[nSampleQueries];
 
  
  if ((nargs <= 9)   ||  (strcmp("-c", args[9]) == 0)    )
  {
    // In this cases, we need to generate a sample query set for
    // computing the optimal parameters.

    // Generate a sample query set.
    FILE *queryFile = fopen(args[7], "rt");
    if (strcmp(args[7], ".") == 0 || queryFile == NULL || nQueries <= 0)
	{//没有查询文件，就用所有点产生随机点
      // Choose several data set points for the sample query points.
		for(IntT i = 0; i < nSampleQueries; i++){
			sampleQueries[i] = dataSetPoints[genRandomInt(0, nPoints - 1)];

	  }
    }
	else
	{
		//从查询文件中选取随机的点，
      // Choose several actual query points for the sample query points.
		  nSampleQueries = MIN(nSampleQueries, nQueries);
		   Int32T sampleIndeces[N_SAMPLE_QUERY_POINTS];
		  //Int32T sampleIndeces[nSampleQueries];
		  for(IntT i = 0; i < nSampleQueries; i++)
		  {
			sampleIndeces[i] = genRandomInt(0, nQueries - 1);
		  }
		  qsort(sampleIndeces, nSampleQueries, sizeof(*sampleIndeces), compareInt32T);
		  //printIntVector("sampleIndeces: ", nSampleQueries, sampleIndeces);
		  Int32T j = 0;
		  for(Int32T i = 0; i < nQueries; i++)
		  {
			  if (i == sampleIndeces[j])
			  {
				  sampleQueries[j] = readPoint(queryFile);
				  j++;
				  while (i == sampleIndeces[j])
				  {
					  sampleQueries[j] = sampleQueries[j - 1];
					  j++;
				  }
			  }else
			  {
				  fscanf(queryFile, "%[^\n]", sBuffer);
				  fscanf(queryFile, "\n");
			  }
		 }
		  nSampleQueries = j;
		  fclose(queryFile);
    }

	//前面那么多，好像就是在申请内存，读文件，读入参数

    // Compute the array sampleQBoundaryIndeces that specifies how to
    // segregate the sample query points according to their distance
    // to NN.
	//采用遍历的方法，计算查询点的最近邻（并且距离小于listOfRadii【nRadii】）
    sortQueryPointsByRadii(pointsDimension,
			   nSampleQueries,
			   sampleQueries,
			   nPoints,
			   dataSetPoints,
			   nRadii,
			   listOfRadii,
			   sampleQBoundaryIndeces);
  }//if ((nargs < 9) || (strcmp("-c", args[9]) == 0))


  RNNParametersT *algParameters = NULL;
  PRNearNeighborStructT *nnStructs = NULL;
  if (nargs > 9) 
  {/* 官方lsh文件：10个参数
bin/LSHMain $nDataSet $nQuerySet $dimension $successProbability "$1" "$2" "$3" $m -c


*/
	     
    // Additional command-line parameter is specified.
    if (strcmp("-c", args[9]) == 0) 	//-c表示参数优化
	{
 // Only compute the R-NN DS parameters and output them to stdout.
      printf("%d\n", nRadii);
      transformMemRatios();
      for(IntT i = 0; i < nRadii; i++)
	  {
		// which sample queries to use
		Int32T segregatedQStart = (i == 0) ? 0 : sampleQBoundaryIndeces[i - 1];
		Int32T segregatedQNumber = nSampleQueries - segregatedQStart;
		if (segregatedQNumber == 0) 
		{
		  // XXX: not the right answer
		  segregatedQNumber = nSampleQueries;
		  segregatedQStart = 0;
		}
		ASSERT(segregatedQStart < nSampleQueries);
		ASSERT(segregatedQStart >= 0);
		ASSERT(segregatedQStart + segregatedQNumber <= nSampleQueries);
		ASSERT(segregatedQNumber >= 0);

		//从文件读取点，然后计算优化后的参数
		RNNParametersT optParameters = computeOptimalParameters(listOfRadii[i],
									successProbability,
									nPoints,
									pointsDimension,
									dataSetPoints,
									segregatedQNumber,
									sampleQueries + segregatedQStart,
/*对内存的约束，就体现在这里，
availableTotalMemory总共的内存（传入） - totalAllocatedMemory（使用mallloc分配的）*1=内存上限

然后(L * nPoints > memoryUpperBound / 12 来约束
*/
									(MemVarT)((availableTotalMemory - totalAllocatedMemory) * memRatiosForNNStructs[i]));
		printRNNParameters(stdout, optParameters);
      }
      exit(0);
    } 
	else if (strcmp("-p", args[9]) == 0) 
	{//-p表示从文件读入参数，然后建立结构体
      // Read the R-NN DS parameters from the given file and run the
      // queries on the constructed data structure.
      if (nargs < 10)
	  {
		  usage(args[0]);
		  exit(1);
      }
      FILE *pFile = fopen(args[10], "rt");
      FAILIFWR(pFile == NULL, "Could not open the params file.");
      fscanf(pFile, "%d\n", &nRadii);
      DPRINTF1("Using the following R-NN DS parameters:\n");
      DPRINTF("N radii = %d\n", nRadii);
      FAILIF(NULL == (nnStructs = (PRNearNeighborStructT*)MALLOC(nRadii * sizeof(PRNearNeighborStructT))));
      FAILIF(NULL == (algParameters = (RNNParametersT*)MALLOC(nRadii * sizeof(RNNParametersT))));
      for(IntT i = 0; i < nRadii; i++)
	  {//默认i=1
		  algParameters[i] = readRNNParameters(pFile);//从文件读参数

		  printRNNParameters(stderr, algParameters[i]);
		  nnStructs[i] = initLSH_WithDataSet(algParameters[i], nPoints, dataSetPoints);
		  //核心
		  //初始化整个数据结构 包括整体+l个hash表 +点映射到桶
	  }

      pointsDimension = algParameters[0].dimension;
      FREE(listOfRadii);
      FAILIF(NULL == (listOfRadii = (RealT*)MALLOC(nRadii * sizeof(RealT))));
      for(IntT i = 0; i < nRadii; i++)
	  {
		  listOfRadii[i] = algParameters[i].parameterR;
	  }
    } 
	else
	{
      // Wrong option.
      usage(args[0]);
      exit(1);
    }
  }//if (nargs > 9) 
  else 
  {
    FAILIF(NULL == (nnStructs = (PRNearNeighborStructT*)MALLOC(nRadii * sizeof(PRNearNeighborStructT))));
    // Determine the R-NN DS parameters, construct the DS and run the queries.
    transformMemRatios();
    for(IntT i = 0; i < nRadii; i++)
	{
      // XXX: segregate the sample queries...
		//建立查询结构，自动优化参数
      nnStructs[i] = initSelfTunedRNearNeighborWithDataSet(listOfRadii[i], 
							   successProbability, 
							   nPoints, 
							   pointsDimension, 
							   dataSetPoints, 
							   nSampleQueries, 
							   sampleQueries, 
							   (MemVarT)((availableTotalMemory - totalAllocatedMemory) * memRatiosForNNStructs[i]));
    }
  } // if (nargs <= 9) 



  //上面都是根据不同配置，对参数的优化，建立查询结构

  DPRINTF1("X\n");

  IntT resultSize = nPoints;
  PPointT *result = (PPointT*)MALLOC(resultSize * sizeof(*result));
  PPointT queryPoint;
  FAILIF(NULL == (queryPoint = (PPointT)MALLOC(sizeof(PointT))));
  FAILIF(NULL == (queryPoint->coordinates = (RealT*)MALLOC(pointsDimension * sizeof(RealT))));

  //读取查询点的文件
  FILE *queryFile = fopen(args[7], "rt");
  FAILIF(queryFile == NULL);
  TimeVarT meanQueryTime = 0;
  PPointAndRealTStructT *distToNN = NULL;
  for(IntT i = 0; i < nQueries; i++)
  {//对于每一个要查询的点

    RealT sqrLength = 0;
    // read in the query point.
    for(IntT d = 0; d < pointsDimension; d++)
	{

      FSCANF_REAL(queryFile, &(queryPoint->coordinates[d]));
      sqrLength += SQR(queryPoint->coordinates[d]);


	  /*//test
	  if (d >150 &&  d<160)
	  {
		  printf(" %lf ",queryPoint->coordinates[d]);
	  }
	  if ( d==160)
	  {
		  printf("原始的文件数据\n");
	  }
	  */
	  
    }
    queryPoint->sqrLength = sqrLength;
    //printRealVector("Query: ", pointsDimension, queryPoint->coordinates);


	
    // get the near neighbors.
    IntT nNNs = 0;
    for(IntT r = 0; r < nRadii; r++)
	{//查询n个近邻点，并计算距离

		//查询核心
      nNNs = getRNearNeighbors(nnStructs[r], queryPoint, result, resultSize);



      printf("Total time for R-NN query at radius %0.6lf (radius no. %d):\t%0.6lf\n", (double)(listOfRadii[r]), r, timeRNNQuery);
      meanQueryTime += timeRNNQuery;

      if (nNNs > 0)
	  {
		printf("Query point %d: found %d NNs at distance %0.6lf (%dth radius). First %d NNs are:\n", 
			i, nNNs, (double)(listOfRadii[r]), r, MIN(nNNs, MAX_REPORTED_POINTS));
	
		// compute the distances to the found NN, and sort according to the distance
		//计算近邻点和查询点的距离
		FAILIF(NULL == (distToNN = (PPointAndRealTStructT*)REALLOC(distToNN, nNNs * sizeof(*distToNN))));
		for(IntT p = 0; p < nNNs; p++)
		{
		  distToNN[p].ppoint = result[p];
		  distToNN[p].real = distance(pointsDimension, queryPoint, result[p]);
		}
		qsort(distToNN, nNNs, sizeof(*distToNN), comparePPointAndRealTStructT);

		// Print the points
		for(IntT j = 0; j < MIN(nNNs, MAX_REPORTED_POINTS); j++)
		{
		  ASSERT(distToNN[j].ppoint != NULL);
		  printf("%09d\tDistance:%0.6lf\n", distToNN[j].ppoint->index, distToNN[j].real);
		  CR_ASSERT(distToNN[j].real <= listOfRadii[r]);
		  //DPRINTF("Distance: %lf\n", distance(pointsDimension, queryPoint, result[j]));
		  //printRealVector("NN: ", pointsDimension, result[j]->coordinates);
		}
		break;
      }
    }
    if (nNNs == 0)
	{
      printf("Query point %d: no NNs found.\n", i);
    }
  }//  for(IntT i = 0; i < nQueries; i++)每个点查询

  //
  if (nQueries > 0)
  {
    meanQueryTime = meanQueryTime / nQueries;
    printf("Mean query time: %0.6lf\n", (double)meanQueryTime);
  }


  for(IntT i = 0; i < nRadii; i++)
  {
    freePRNearNeighborStruct(nnStructs[i]);
  }
  // XXX: should ideally free the other stuff as well.


  return 0;
}
