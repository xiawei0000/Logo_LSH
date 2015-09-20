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
  The self-tuning module. This file contains all the functions for
  estimating the running times and for computing the optimal (at least
  in estimation) parameters for the R-NN data structure (within the
  memory limits).
 */

#include "headers.h"
/*优化参数的模块
*/
// Computes how much time it takes to run timing functions (functions
// that compute timings) -- we need to substract this value when we
// compute the length of an actual interval of time.
void tuneTimeFunctions(){
	
  timevSpeed = 0;
  // Compute the time needed for a calls to TIMEV_START and TIMEV_END
  IntT nIterations = 100000;
  TimeVarT timeVar = 0;
  for(IntT i = 0; i < nIterations; i++){
    TIMEV_START(timeVar);
    TIMEV_END(timeVar);
  }
  timevSpeed = timeVar / nIterations;
  DPRINTF("Tuning: timevSpeed = %0.9lf\n", timevSpeed);
}

/* 
 * Given a set of queries, the data set, and a set of (sorted) radii,
 * this function will compute the <boundaryIndeces>, i.e., the indeces
 * at which the query points go from one "radius class" to another
 * "radius class".
 * 
 * More formally, the query set is sorted according to the distance to
 * NN. Then, function fills in <boundaryIndeces> such that if
 * <boundaryIndeces[i]=A>, then all query points with index <A have
 * their NN at distance <=radii[i], and other query points (with index
 * >=A) have their NN at distance >radii[i].
 *
 * <boundaryIndeces> must be preallocated for at least <nQueries>
 * elements.
 */
void sortQueryPointsByRadii(IntT dimension,
			    Int32T nQueries, 
			    PPointT *queries, 
			    Int32T nPoints, 
			    PPointT *dataSet,
			    IntT nRadii,
			    RealT *radii,
			    Int32T *boundaryIndeces)
{
  ASSERT(queries != NULL);
  ASSERT(dataSet != NULL);
  ASSERT(radii != NULL);
  ASSERT(boundaryIndeces != NULL);


  PPointAndRealTStructT *distToNN = NULL;
  FAILIF(NULL == (distToNN = (PPointAndRealTStructT*)MALLOC(nQueries * sizeof(*distToNN))));
  for(IntT i = 0; i < nQueries; i++)
  {//采用遍历，求最近邻的点
    distToNN[i].ppoint = queries[i];
    distToNN[i].real = distance(dimension, queries[i], dataSet[0]);
    for(IntT p = 0; p < nPoints; p++)
	{
      RealT dist = distance(dimension, queries[i], dataSet[p]);
      if (dist < distToNN[i].real)
	  {
		distToNN[i].real = dist;
      }
    }
  }

  qsort(distToNN, nQueries, sizeof(*distToNN), comparePPointAndRealTStructT);

  IntT radiusIndex = 0;
  for(IntT i = 0; i < nQueries; i++) 
  {
    //DPRINTF("%0.6lf\n", distToNN[i].real);
    queries[i] = distToNN[i].ppoint; // copy the sorted queries array back to <queries>
    while ( (distToNN[i].real > radii[radiusIndex])
		&& (radiusIndex < nRadii) ) 
	{
      boundaryIndeces[radiusIndex] = i;
      radiusIndex++;
    }
  }

  FREE(distToNN);
}

// Determines the run-time coefficients of the different parts of the
// query algorithm. Values that are computed and returned are
// <lshPrecomp>, <uhashOver>, <distComp>. <lshPrecomp> is the time for
// pre-computing one function from the LSH family. <uhashOver> is the
// time for getting a bucket from a hash table (of buckets).<distComp>
// is the time to compute one distance between two points. These times
// are computed by constructing a R-NN DS on a sample data set and
// running a sample query set on it.
void determineRTCoefficients(RealT thresholdR, 
			     RealT successProbability, 
			     BooleanT useUfunctions, 
			     IntT typeHT, 
			     IntT dimension, 
			     Int32T nPoints, 
			     PPointT *realData, 
			     RealT &lshPrecomp, 
			     RealT &uhashOver, 
			     RealT &distComp)
{// 随机取点，然后计算mk，用mk建立索引结构。多次测试查询时间。

  // use a subset of the original data set.
  // there is not much theory behind the formula below.
  IntT n = nPoints / 50;
  if (n < 100) {
    n = nPoints;
  }
  if (n > 10000) {
    n = 10000;
  }

  // Initialize the data set to use.
  PPointT *dataSet;
  //从数据中随机取了一些点
  FAILIF(NULL == (dataSet = (PPointT*)MALLOC(n * sizeof(PPointT))));
  for(IntT i = 0; i < n; i++){
    dataSet[i] = realData[genRandomInt(0, nPoints - 1)];
  }

  IntT hashTableSize = n;
  RNNParametersT algParameters;
  algParameters.parameterR = thresholdR;
  algParameters.successProbability = successProbability;
  algParameters.dimension = dimension;
#ifdef USE_L1_DISTANCE
  algParameters.parameterR2 = thresholdR;
#else  
  algParameters.parameterR2 = SQR(thresholdR);
#endif
  algParameters.useUfunctions = useUfunctions;
  algParameters.parameterK = 16;
  algParameters.parameterW = PARAMETER_W_DEFAULT;
  algParameters.parameterT = n;
  algParameters.typeHT = typeHT;

  if (algParameters.useUfunctions){

	  //计算参数的函数，计算最佳M和L
    algParameters.parameterM = computeMForULSH(algParameters.parameterK, algParameters.successProbability);
    algParameters.parameterL = algParameters.parameterM * (algParameters.parameterM - 1) / 2;
  }else{
    algParameters.parameterM = computeLfromKP(algParameters.parameterK, algParameters.successProbability);
    algParameters.parameterL = algParameters.parameterM;
  }

//   FAILIF(NULL == (dataSet = (PPointT*)MALLOC(n * sizeof(PPointT))));
//   for(IntT i = 0; i < n; i++){
//     FAILIF(NULL == (dataSet[i] = (PPointT)MALLOC(sizeof(PointT))));
//     FAILIF(NULL == (dataSet[i]->coordinates = (RealT*)MALLOC(dimension * sizeof(RealT))));

//     dataSet[i]->index = i;
//     sqrLength = 0;
//     for(IntT d = 0; d < dimension; d++){
//       if (i == 0) {
// 	dataSet[i]->coordinates[d] = genUniformRandom(-100, 100);
//       }else{
// 	dataSet[i]->coordinates[d] = dataSet[0]->coordinates[d];
//       }
//       sqrLength += SQR(dataSet[i]->coordinates[d]);
//     }
//     dataSet[i]->sqrLength = sqrLength;
//   }

  // switch on timing
  BooleanT tempTimingOn = timingOn;
  timingOn = TRUE;

  // initialize result arrays
  PPointT *result = NULL;
  IntT resultSize = 0;
  IntT nNNs;
  IntT nSucReps;

  do{//循环了五次
    // create the test structure
    PRNearNeighborStructT nnStruct;
    switch(algParameters.typeHT)
	{
    case HT_LINKED_LIST:
      nnStruct = initLSH(algParameters, n);
      // add points to the test structure
      for(IntT i = 0; i < n; i++)
	  {
		addNewPointToPRNearNeighborStruct(nnStruct, realData[i]);
      }
      break;
    case HT_HYBRID_CHAINS://默认的
      nnStruct = initLSH_WithDataSet(algParameters, n, dataSet);
      break;
    default:
      ASSERT(FALSE);
    }

    // query point
    PPointT queryPoint;
//     FAILIF(NULL == (queryPoint = (PPointT)MALLOC(sizeof(PointT))));
//     FAILIF(NULL == (queryPoint->coordinates = (RealT*)MALLOC(dimension * sizeof(RealT))));
//     RealT sqrLength = 0;
//     for(IntT i = 0; i < dimension; i++){
//       queryPoint->coordinates[i] = dataSet[0]->coordinates[i];
//       //queryPoint->coordinates[i] = 0.1;
//       sqrLength += SQR(queryPoint->coordinates[i]);
//     }
    //queryPoint->coordinates[0] = dataPoint->coordinates[0] + 0.0001;
    //queryPoint->sqrLength = sqrLength;

    // reset the R parameter so that there are no NN neighbors.
    setResultReporting(nnStruct, FALSE);//设置为不反回值
    //DPRINTF1("X\n");

    lshPrecomp = 0;
    uhashOver = 0;
    distComp = 0;
    IntT nReps = 20;
    nSucReps = 0;
    for(IntT rep = 0; rep < nReps; rep++)
	{//取20个点测试
      queryPoint = realData[genRandomInt(0, nPoints - 1)];
      timeComputeULSH = 0;
      timeGetBucket = 0;
      timeCycleBucket = 0;
      nOfDistComps = 0;
      nNNs = getNearNeighborsFromPRNearNeighborStruct(nnStruct, queryPoint, result, resultSize);//细看
	  //通过查找索引，然后获得桶，提取n个最近邻点

      //DPRINTF("Time to compute LSH: %0.6lf\n", timeComputeULSH);
      //DPRINTF("Time to get bucket: %0.6lf\n", timeGetBucket);
      //DPRINTF("Time to cycle through buckets: %0.9lf\n", timeCycleBucket);
      //DPRINTF("N of dist comp: %d\n", nOfDistComps);

      ASSERT(nNNs == 0);
      if (nOfDistComps >= MIN(n / 10, 100))
	  {
		  nSucReps++;
		  lshPrecomp += timeComputeULSH / algParameters.parameterK / algParameters.parameterM;
		  uhashOver += timeGetBucket / algParameters.parameterL;
		  distComp += timeCycleBucket / nOfDistComps;
      }
    }//20次，每次随机取点，然后计算最近邻，（只是测试时间）

    if (nSucReps >= 5)
	{
      lshPrecomp /= nSucReps;
      uhashOver /= nSucReps;
      distComp /= nSucReps;
      DPRINTF1("RT coeffs computed.\n");
    }
	else
	{
		//5次循环中，每次r都翻倍
      algParameters.parameterR *= 2; // double the radius and repeat
      DPRINTF1("Could not determine the RT coeffs. Repeating.\n");
    }

    freePRNearNeighborStruct(nnStruct);

  }while(nSucReps < 5);

  FREE(dataSet);
  FREE(result);

  timingOn = tempTimingOn;
}



double erfc(double x) 
{
	double a1 = -1.26551223, a2 = 1.00002368,
		a3 = 0.37409196, a4 = 0.09678418,
		a5 = -0.18628806, a6 = 0.27886807,
		a7 = -1.13520398, a8 = 1.48851587,
		a9 = -0.82215223, a10 = 0.17087277;

	double value=1;
	if(x>0)
	{double b=1/(1+0.5*x);
	value= b*exp((-x*x) +a1+b*(a2+b*(a3+b*(a4+b*(a5+b*(a6+
		b*(a7+b*(a8+b*(a9+b*a10)))))))));}
	if(x<0) value=2-erfc(-x);
	return value;
}
/*
  The function <p> from the paper (probability of collision of 2
  points for 1 LSH function).
 */
RealT computeFunctionP(RealT w, RealT c){
	//GGGfLY的公式
  RealT x = w / c;
  return 1 - ERFC(x / M_SQRT2) - M_2_SQRTPI / M_SQRT2 / x * (1 - EXP(-SQR(x) / 2));
}

// Computes the parameter <L> of the algorithm, given the parameter
// <k> and the desired success probability
// <successProbability>. Functions <g> are considered all independent
// (original scheme).
IntT computeLfromKP(IntT k, RealT successProbability){
  return CEIL(LOG(1 - successProbability) / LOG(1 - POW(computeFunctionP(PARAMETER_W_DEFAULT, 1), k)));
}

// Computes the parameter <m> of the algorithm, given the parameter
// <k> and the desired success probability <successProbability>. Only
// meaningful when functions <g> are interdependent (pairs of
// functions <u>, where the <m> functions <u> are each k/2-tuples of
// independent LSH functions).
IntT computeMForULSH(IntT k, RealT successProbability)
{//给参数k和查询成功概率，计算最佳M
  ASSERT((k & 1) == 0); // k should be even in order to use ULSH.
  RealT mu = 1 - POW(computeFunctionP(PARAMETER_W_DEFAULT, 1), k / 2);

  //computeFunctionP 是个 GGGfLY的公式


  RealT P = successProbability;
  RealT d = (1-mu)/(1-P)*1/LOG(1/mu) * POW(mu, -1/(1-mu));
  RealT y = LOG(d);
  IntT m = CEIL(1 - y/LOG(mu) - 1/(1-mu));
  while (POW(mu, m-1) * (1 + m * (1-mu)) > 1 - P){
    m++;
  }
  return m;
}


RealT estimateNCollisions(IntT nPoints, IntT dim, PPointT *dataSet, PPointT query, IntT k, IntT L, RealT R){
  RealT sumCollisions = 0;
  for(IntT i = 0; i < nPoints; i++){
    if (query != dataSet[i]) {
      RealT dist = distance(dim, query, dataSet[i]);
      sumCollisions += POW(computeFunctionP(PARAMETER_W_DEFAULT, dist / R), k);
    }
  }
  return L * sumCollisions;
}

RealT estimateNCollisionsFromDSPoint(IntT nPoints, IntT dim, PPointT *dataSet, IntT queryIndex, IntT k, IntT L, RealT R){
  RealT sumCollisions = 0;
  for(IntT i = 0; i < nPoints; i++){
    if (queryIndex != i) {
      RealT dist = distance(dim, dataSet[queryIndex], dataSet[i]);
      sumCollisions += POW(computeFunctionP(PARAMETER_W_DEFAULT, dist / R), k);
    }
  }
  return L * sumCollisions;
}

RealT estimateNDistinctCollisions(IntT nPoints, IntT dim, PPointT *dataSet,
	PPointT query, BooleanT useUfunctions, IntT k, IntT LorM, RealT R)
{//计算冲突率
  RealT sumCollisions = 0;
  for(IntT i = 0; i < nPoints; i++){
    if (query != dataSet[i]) {
      RealT dist = distance(dim, query, dataSet[i]);
      if (!useUfunctions){
	sumCollisions += 1-POW(1-POW(computeFunctionP(PARAMETER_W_DEFAULT, dist / R), k), LorM);
      }else{
	RealT mu = 1 - POW(computeFunctionP(PARAMETER_W_DEFAULT, dist / R), k / 2);
	RealT x = POW(mu, LorM - 1);
	sumCollisions += 1 - mu * x - LorM * (1 - mu) * x;
      }
    }
  }
  return sumCollisions;
}

RealT estimateNDistinctCollisionsFromDSPoint(IntT nPoints, IntT dim, PPointT *dataSet, IntT queryIndex, BooleanT useUfunctions, IntT k, IntT LorM, RealT R){
  RealT sumCollisions = 0;
  for(IntT i = 0; i < nPoints; i++){
    if (queryIndex != i) {
      RealT dist = distance(dim, dataSet[queryIndex], dataSet[i]);
      if (!useUfunctions){
	sumCollisions += 1-POW(1-POW(computeFunctionP(PARAMETER_W_DEFAULT, dist / R), k), LorM);
      }else{
	RealT mu = 1 - POW(computeFunctionP(PARAMETER_W_DEFAULT, dist / R), k / 2);
	RealT x = POW(mu, LorM - 1);
	sumCollisions += 1 - mu * x - LorM * (1 - mu) * x;
      }
    }
  }
  return sumCollisions;
}

/*
  Given the actual data set <dataSet>, estimates the values for
  algorithm parameters that would give the optimal running time of a
  query. 

  The set <sampleQueries> is a set with query sample points
  (R-NN DS's parameters are optimized for query points from the set
  <sampleQueries>). <sampleQueries> could be a sample of points from the
  actual query set or from the data set. When computing the estimated
  number of collisions of a sample query point <q> with the data set
  points, if there is a point in the data set with the same pointer
  with <q> (that is when <q> is a data set point), then the
  corresponding point (<q>) is not considered in the data set (for the
  purpose of computing the respective #collisions estimation).

  The return value is the estimate of the optimal parameters.
*/
RNNParametersT computeOptimalParameters(RealT R, 
					RealT successProbability, 
					IntT nPoints, 
					IntT dimension, 
					PPointT *dataSet, 
					IntT nSampleQueries, 
					PPointT *sampleQueries, 
					MemVarT memoryUpperBound)
{//通过多次统计和测试得到最佳的参数 mkl
/*处理流程：
	先多次统计hash时间，距离比较时间，uhashOver时间
	然后调节参数k
	由公式由k计算m 和l
	然后计算所有消耗时间，
	求得k，使时间最小

	再次返回lkm参数	
*/
  ASSERT(nSampleQueries > 0);

  initializeLSHGlobal();//初始化了时间 随机函数，。。。

  RNNParametersT optParameters;
  optParameters.successProbability = successProbability;
  optParameters.dimension = dimension;
  optParameters.parameterR = R;
#ifdef USE_L1_DISTANCE
  optParameters.parameterR2 = R;
#else  
  optParameters.parameterR2 = SQR(R);
#endif
  optParameters.useUfunctions = TRUE;//好像是用哪一种hash表
  // TODO: could optimize here:
				      // maybe sometimes, the old way
				      // was better.
  optParameters.parameterW = PARAMETER_W_DEFAULT;
  optParameters.parameterT = nPoints;
  optParameters.typeHT = HT_HYBRID_CHAINS;
  
  // Compute the run-time parameters (timings of different parts of the algorithm).
  IntT nReps = 10; // # number of repetions
  RealT lshPrecomp = 0, uhashOver = 0, distComp = 0;
 

  //这里只是多次测试，统计索引时间等。
  for(IntT i = 0; i < nReps; i++)
  {//重复十次
    RealT lP, uO, dC;
    determineRTCoefficients(optParameters.parameterR, 
			    optParameters.successProbability, 
			    optParameters.useUfunctions, 
			    optParameters.typeHT, 
			    optParameters.dimension, 
			    nPoints, 
			    dataSet, 
			    lP, 
			    uO, 
			    dC);
    lshPrecomp += lP;//索引比较时间
    uhashOver += uO;//
    distComp += dC;//距离比较时间
    DPRINTF4("Coefs: lP = %0.9lf\tuO = %0.9lf\tdC = %0.9lf\n", lP, uO, dC);
  }
  lshPrecomp /= nReps;
  uhashOver /= nReps;
  distComp /= nReps;
  DPRINTF("Coefs (final): lshPrecomp = %0.9lf\n", lshPrecomp);
  DPRINTF("Coefs (final): uhashOver = %0.9lf\n", uhashOver);
  DPRINTF("Coefs (final): distComp = %0.9lf\n", distComp);
  DPRINTF("Remaining memory: %lld\n", memoryUpperBound);


  //这一步主要是
  //测试所有的k，得到最快检测的k值=bestk
  // Try all possible <k>s and choose the one for which the time
  // estimate of a query is minimal.
  IntT k;
  RealT timeLSH, timeUH, timeCycling;
  //IntT queryIndex = genRandomInt(0, nPoints);
  //PPointT query = dataSet[queryIndex]; // query points = a random points from the data set.
  IntT bestK = 0;
  RealT bestTime = 0;
  for(k = 2; ; k += 2)
  {//好像是在不断调节bestk，


    DPRINTF("ST. k = %d\n", k);
    IntT m = computeMForULSH(k, successProbability);
    IntT L = m * (m-1) / 2;
    //DPRINTF("Available memory: %lld\n", getAvailableMemory());
    if (L * nPoints > memoryUpperBound / 12)
	{
      break;
    }
    timeLSH = m * k * lshPrecomp;
    timeUH = L * uhashOver;
    //RealT nCollisions = estimateNCollisionsFromDSPoint(nPoints, dimension, dataSet, queryIndex, k, L, R);

    // Compute the mean number of collisions for the points from the sample query set.
	//计算最小冲突率
    RealT nCollisions = 0;
    for(IntT i = 0; i < nSampleQueries; i++)
	{
      nCollisions += estimateNDistinctCollisions(nPoints, dimension, dataSet, sampleQueries[i], TRUE, k, m, R);
    }
    nCollisions /= nSampleQueries;

    timeCycling = nCollisions * distComp;
    DPRINTF3("ST.m=%d L=%d \n", m, L);
    DPRINTF("ST.Estimated # distinct collisions = %0.6lf\n", (double)nCollisions);
    DPRINTF("ST.TimeLSH = %0.6lf\n", timeLSH);
    DPRINTF("ST.TimeUH = %0.6lf\n", timeUH);
    DPRINTF("ST.TimeCycling = %0.6lf\n", timeCycling);
    DPRINTF("ST.Sum = %0.6lf\n", timeLSH + timeUH + timeCycling);
    if (bestK == 0 || (timeLSH + timeUH + timeCycling) < bestTime)
	{//通过时间 判断哪个k最好
		//timeLSH + timeUH + timeCycling最小的就是最佳的k，
		//timeLSH + timeUH由mk决定，  timeLSH = m * k * lshPrecomp;	timeUH = L * uhashOver;
		//timeCycling由冲突率决定  timeCycling = nCollisions * distComp;
      bestK = k;
      bestTime = timeLSH + timeUH + timeCycling;
    }
    ASSERT(k < 100); //  otherwise, k reached 100 -- which, from
    //  experience, should never happen for reasonable
    //  data set & available memory amount.
  }//  for(k 


  DPRINTF("STO.Optimal k = %d\n", bestK);
  IntT m = computeMForULSH(bestK, successProbability);
  IntT L = m * (m-1) / 2;
  timeLSH = m * bestK * lshPrecomp;
  timeUH = L * uhashOver;
  
  // Compute the mean number of collisions for the points from the sample query set.
  //计算平均冲突率
  RealT nCollisions = 0;
  for(IntT i = 0; i < nSampleQueries; i++)
  {
    nCollisions += estimateNDistinctCollisions(nPoints, dimension, dataSet, sampleQueries[i], TRUE, k, m, R);
  }
  nCollisions /= nSampleQueries;

  // timeCycling = estimateNCollisionsFromDSPoint(nPoints, dimension, dataSet, queryIndex, bestK, L, R) * distComp;
  timeCycling = nCollisions * distComp;
  DPRINTF("STO.TimeLSH = %0.6lf\n", timeLSH);
  DPRINTF("STO.TimeUH = %0.6lf\n", timeUH);
  DPRINTF("STO.TimeCycling = %0.6lf\n", timeCycling);
  DPRINTF("STO.Sum = %0.6lf\n", timeLSH + timeUH + timeCycling);
  
  optParameters.parameterK = bestK;
  optParameters.parameterM = m;
  optParameters.parameterL = L;

  return optParameters;
}
