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

#include "headers.h"
#include "Random.h"
#include <stdlib.h>    // Need random(), srandom()  
#include <time.h> 
 
//各种随机函数
// The state vector for generation of random numbers.
char rngState[256];

// Initialize the random number generator.
void initRandom(){
 // FAILIF(NULL == initstate(2, rngState, 256));
	srand(time(NULL));
}

int mysrand()
{

	/*
	long  Max=99999;
	long i,temp;
	for (i=33;i<Max;i++)
	{
		temp=i*i%5;
		temp=temp*temp/345;
	}

	*/
	srand(time(NULL)/rand());
	 rand();
	return rand();
}



// Generate a random integer in the range [rangeStart,
// rangeEnd]. Inputs must satisfy: rangeStart <= rangeEnd.
IntT genRandomInt(IntT rangeStart, IntT rangeEnd){
  ASSERT(rangeStart <= rangeEnd);
  IntT r;

  
  r = rangeStart + (IntT)((rangeEnd - rangeStart + 1.0) *mysrand()/ (RAND_MAX + 1.0));
  ASSERT(r >= rangeStart && r <= rangeEnd);
  return r;
}

// Generate a random 32-bits unsigned (Uns32T) in the range
// [rangeStart, rangeEnd]. Inputs must satisfy: rangeStart <=
// rangeEnd.
Uns32T genRandomUns32(Uns32T rangeStart, Uns32T rangeEnd){
  ASSERT(rangeStart <= rangeEnd);
  Uns32T r;
  LongUns64T randtemp1;
  


  if (RAND_MAX >= rangeEnd - rangeStart) {
	
    r = rangeStart + (Uns32T)((rangeEnd - rangeStart + 1.0) * mysrand()/ (RAND_MAX + 1.0));
  } else {
	 
    r = rangeStart + (Uns32T)((rangeEnd - rangeStart + 1.0) * 
		((LongUns64T)mysrand() * ((LongUns64T)RAND_MAX + 1) +mysrand()) /
		((LongUns64T)RAND_MAX * ((LongUns64T)RAND_MAX + 1) + (LongUns64T)RAND_MAX + 1.0));
  }
  ASSERT(r >= rangeStart && r <= rangeEnd);
  return r;
}

// Generate a random real distributed uniformly in [rangeStart,
// rangeEnd]. Input must satisfy: rangeStart <= rangeEnd. The
// granularity of generated random reals is given by RAND_MAX.
RealT genUniformRandom(RealT rangeStart, RealT rangeEnd)
{//产生从 rangeStart 到 rangeEnd的随机数
	ASSERT(rangeStart <= rangeEnd);
	RealT r;
	 
	r = rangeStart + ((rangeEnd - rangeStart) * (RealT)mysrand()/ (RealT)RAND_MAX);
	ASSERT(r >= rangeStart && r <= rangeEnd);
	return r;
}

// Generate a random real from normal distribution N(0,1).
RealT genGaussianRandom(){
  // Use Box-Muller transform to generate a point from normal
  // distribution.
  RealT x1, x2;
  do{
    x1 = genUniformRandom(0.0, 1.0);
  } while (x1 == 0); // cannot take log of 0.
  x2 = genUniformRandom(0.0, 1.0);
  RealT z;
#define  M_PI 3.1415926
  z = SQRT(-2.0 * LOG(x1)) * COS(2.0 * M_PI * x2);
  return z;
}

// Generate a random real from Cauchy distribution N(0,1).
RealT genCauchyRandom(){
  RealT x, y;
  x = genGaussianRandom();
  y = genGaussianRandom();
  if (ABS(y) < 0.0000001) {
    y = 0.0000001;
  }
  return x / y;
}
