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
//实现hash表映射到桶里

// Creates a new bucket with specified fields. The new bucket contains
// only a single entry -- bucketEntry. bucketEntry->nextEntry is
// expected to be NULL.
inline PGBucketT newGBucket(PUHashStructureT uhash, Uns32T control1, /*PPointT point, */ Int32T pointIndex, PGBucketT nextGBucket){
  PGBucketT bucket;
  if (uhash != NULL && uhash->unusedPGBuckets != NULL){
    bucket = uhash->unusedPGBuckets;
    uhash->unusedPGBuckets = uhash->unusedPGBuckets->nextGBucketInChain;
  } else {
    FAILIF(NULL == (bucket = (PGBucketT)MALLOC(sizeof(GBucketT))));
    nAllocatedGBuckets++;
  }
  ASSERT(bucket != NULL);
  bucket->controlValue1 = control1;
  bucket->firstEntry.pointIndex = pointIndex;
  bucket->firstEntry.nextEntry = NULL;
  bucket->nextGBucketInChain = nextGBucket;

  nGBuckets++;
  return bucket;
}

// Adds the entry <bucketEntry> to the bucket <bucket>.
inline void addPointToGBucket(PUHashStructureT uhash, PGBucketT bucket/*, PPointT point*/ , Int32T pointIndex){
  ASSERT(bucket != NULL);
  ASSERT(uhash != NULL);

  // create a new bucket entry for the point
  TIMEV_START(timeBucketCreation);
  PBucketEntryT bucketEntry;
  if (uhash->unusedPBucketEntrys != NULL){
    bucketEntry = uhash->unusedPBucketEntrys;
    uhash->unusedPBucketEntrys = uhash->unusedPBucketEntrys->nextEntry;
  }else{
    FAILIF(NULL == (bucketEntry = (PBucketEntryT)MALLOC(sizeof(BucketEntryT))));
    nAllocatedBEntries++;
  }
  ASSERT(bucketEntry != NULL);
  bucketEntry->pointIndex = pointIndex;
  TIMEV_END(timeBucketCreation);
  
  bucketEntry->nextEntry = bucket->firstEntry.nextEntry;
  bucket->firstEntry.nextEntry = bucketEntry;
}

// Creates a new UH structure (initializes the hash table and the hash
// functions used). If <typeHT>==HT_PACKED or HT_HYBRID_CHAINS, then
// <modelHT> gives the sizes of all the static arrays that are
// used. Otherwise parameter <modelHT> is not used.
PUHashStructureT newUHashStructure(IntT typeHT, Int32T hashTableSize,
	IntT bucketVectorLength, BooleanT useExternalUHFs,
	Uns32T *(&mainHashA), Uns32T *(&controlHash1), PUHashStructureT modelHT)
{/* 建立 uhash ，   
 分两种用途：1，计算新的linked list表，
 2.根据已有的modelHT 转化为空间紧凑的HT_HYBRID_CHAINS 表

 1、
 当typeHT= HT_LINKED_LIST时，直接初始化数组，
 然后建立随机的主hash表 向量， 辅助hash表的向量：
 就是  (u 。x)%(2^32 -5)/tablesize 中的u1向量 
 和 (u 。x)%(2^32 -5) 中的u2向量 
  最终返回：PUHashStructureT uhash

2、 当typeHT= HT_HYBRID_CHAINS时，
先初始化 一级指针索引 ：uhash->hybridHashTable 【1000】 
     存放具体二级桶和实体的uhash->hybridChainsStorage【1000+额外的263个桶所占的一格】

	 然后从modelHT取出每个一级索引：
	 没有则hybridHashTable【i】为空
	 有则：hybridHashTable【i】 指向 storage的第一个序号值
	 然后取出每个modeLHT的二级桶： 每个桶，取出每个桶的实体，copy到storage中。
	 只是复制了主副hash的向量
	 
	 最后返回结构体
 */
	
	//初始化hash的桶结构
	 //对主hash和辅助hash表的初始化：给一个随机值
  PUHashStructureT uhash;
  FAILIF(NULL == (uhash = (PUHashStructureT)MALLOC(sizeof(UHashStructureT))));
  uhash->typeHT = typeHT;
  uhash->hashTableSize = hashTableSize;
  uhash->nHashedBuckets = 0;
  uhash->nHashedPoints = 0;
  uhash->unusedPGBuckets = NULL;
  uhash->unusedPBucketEntrys = NULL;

  uhash->prime = UH_PRIME_DEFAULT;
  uhash->hashedDataLength = bucketVectorLength;

  uhash->chainSizes = NULL;
  uhash->bucketPoints.pointsArray = NULL;
  uhash->hybridChainsStorage = NULL;

  Int32T totalN = 0;
  Int32T indexInStorage = 0;
  Int32T lastIndexInSt = 0;


  //对hash的桶结构初始化
  switch (typeHT) 
  {
  case HT_LINKED_LIST:
    FAILIF(NULL == (uhash->hashTable.llHashTable = (PGBucketT*)MALLOC(hashTableSize * sizeof(PGBucketT))));
    uhash->chainSizes = NULL;
    for(Int32T i = 0; i < hashTableSize; i++)
	{
      uhash->hashTable.llHashTable[i] = NULL;
    }
    break;
  case HT_STATISTICS:
    ASSERT(FALSE); // Not supported
    FAILIF(NULL == (uhash->hashTable.linkHashTable = (LinkPackedGBucketT**)MALLOC(hashTableSize * sizeof(LinkPackedGBucketT*))));
    FAILIF(NULL == (uhash->chainSizes = (IntT*)(MALLOC(hashTableSize * sizeof(IntT)))));
    FAILIF(NULL == (uhash->bucketPoints.pointsList = (PointsListEntryT*)MALLOC(hashTableSize * sizeof(PointsListEntryT))));
    for(Int32T i = 0; i < hashTableSize; i++)
	{
      uhash->chainSizes[i] = CHAIN_INIT_SIZE;
      FAILIF(NULL == (uhash->hashTable.linkHashTable[i] = (LinkPackedGBucketT*)MALLOC(uhash->chainSizes[i] * sizeof(LinkPackedGBucketT))) && (uhash->chainSizes[i] > 0));
      if (uhash->chainSizes[i] > 0)
	  {
		// the first bucket is empty
		uhash->hashTable.linkHashTable[i][0].indexStart = INDEX_START_EMPTY; 
      }
    }
    break;
  case HT_PACKED:
    ASSERT(FALSE); // Not supported
//     ASSERT(modelHT != NULL);
//     ASSERT(modelHT->typeHT == HT_STATISTICS);
//     ASSERT(modelHT->nHashedPoints == hashTableSize); // TODO
//     FAILIF(NULL == (uhash->hashTable.packedHashTable = (PackedGBucketT**)MALLOC(hashTableSize * sizeof(PackedGBucketT*))));
//     FAILIF(NULL == (uhash->chainSizes = (IntT*)(MALLOC(hashTableSize * sizeof(IntT)))));
//     FAILIF(NULL == (uhash->bucketPoints.pointsArray = (PPointT*)MALLOC(hashTableSize * sizeof(PPointT))));
//     totalN = 0; // total number of points hashed so far.
//     for(Int32T i = 0; i < hashTableSize; i++){

//       // TODO: NOT TESTED AT ALL

//       // count first size of the chain:
//       IntT j;
//       for(j = 0; j < modelHT->chainSizes[i] && modelHT->hashTable.linkHashTable[i][j].indexStart != INDEX_START_EMPTY; j++)
// 	;
//       uhash->chainSizes[i] = j;

//       if (j == 0){
// 	uhash->hashTable.packedHashTable[i] = NULL;
// 	continue;
//       }

//       // allocate memory for the chain
//       FAILIF(NULL == (uhash->hashTable.packedHashTable[i] = (PackedGBucketT*)MALLOC(uhash->chainSizes[i] * sizeof(PackedGBucketT))));

//       // copy each bucket in the chain:
//       for(j = 0; j < uhash->chainSizes[i]; j++){
// 	// "general" info for the bucket
// 	uhash->hashTable.packedHashTable[i][j].controlValue1 = modelHT->hashTable.linkHashTable[i][j].controlValue1;
// 	uhash->hashTable.packedHashTable[i][j].indexStart = totalN;
	
// 	Int32T p = modelHT->hashTable.linkHashTable[i][j].indexStart;
// 	ASSERT(p != INDEX_START_EMPTY);
// 	IntT count = 0;
// 	while(p != INDEX_START_EMPTY){
// 	  uhash->bucketPoints.pointsArray[totalN] = modelHT->bucketPoints.pointsList[p].point;
// 	  totalN++;
// 	  count++;
// 	  p = modelHT->bucketPoints.pointsList[p].nextPoint;
// 	}
// 	uhash->hashTable.packedHashTable[i][j].nPointsInBucket = count;
//       }
//     }
    break;
  case HT_HYBRID_CHAINS://默认这个
    ASSERT(modelHT != NULL);
    ASSERT(modelHT->typeHT == HT_LINKED_LIST);
	//建立了 chain类型的hash指针数组，每个指针，指向一块HybridChainEntryT，。就是hash中 二级的链表
    FAILIF(NULL == (uhash->hashTable.hybridHashTable = (PHybridChainEntryT*)MALLOC(hashTableSize * sizeof(PHybridChainEntryT))));
    FAILIF(NULL == (uhash->hybridChainsStorage = (HybridChainEntryT*)MALLOC((modelHT->nHashedPoints + modelHT->nHashedBuckets) * sizeof(HybridChainEntryT))));
    
    // the index of the first unoccupied entry in <uhash->hybridChainsStorage>.
    indexInStorage = 0; 
    
    // the index of the last unoccupied entry in <uhash->hybridChainsStorage>
    // (the entries of <uhash->hybridChainsStorage> are filled from start and from end:
    // at the beginning we fill the normal buckets with their points;
    // at the end we fill the "overflow" points of buckets (additional points of buckets that have
    // more than MAX_NONOVERFLOW_POINTS_PER_BUCKET points).
    lastIndexInSt = modelHT->nHashedPoints + modelHT->nHashedBuckets - 1; 

    for(Int32T i = 0; i < hashTableSize; i++)
	{
		/*原来的modelHT有1000个 一级索引：  从已经映射好的modelHT结构体 转化到 uhash表的hybridHashTable中去
		*/
		
		//对每一个hash表；根据传入的modellHT结构来初始化hash表：让指针链接好结构体
      PGBucketT bucket = modelHT->hashTable.llHashTable[i];
      if (bucket != NULL)
	  {//原来有值，就映射到storage存储空间
		uhash->hashTable.hybridHashTable[i] = uhash->hybridChainsStorage + indexInStorage; // the position where the bucket starts
      }
	  else
	  {//原来的一级索引是空，那么新的桶不给存储空间（查询肯定没有值，）
		uhash->hashTable.hybridHashTable[i] = NULL;
      }


      while(bucket != NULL)
	  {/*最后有bucket = bucket->nextGBucketInChain;
	   这个循环是：不断遍历二级的桶，copy到uhash表的hybridHashTable中去
	   */
		  
		  //对桶结构体赋值
			// Compute number of points in the current bucket.
			Int32T nPointsInBucket = 1;
			PBucketEntryT bucketEntry = bucket->firstEntry.nextEntry;//得到第一个二级桶的值
			while(bucketEntry != NULL)
			{//遍历二级桶的每一个实体， 统计个数
			  nPointsInBucket++;
			  bucketEntry = bucketEntry->nextEntry;
			}


			// Copy the points from the bucket to the new HT.
			ASSERT(nPointsInBucket > 0);
	
			//采用的 大概是：用数组来模拟链表：  空出的第一个值作为循环链表的结束判断
			uhash->hybridChainsStorage[indexInStorage].controlValue1 = bucket->controlValue1;//一个二级桶 有一个二级索引值，  这里将他拷贝给uhash
			indexInStorage++;
			uhash->hybridChainsStorage[indexInStorage].point.isLastBucket = (bucket->nextGBucketInChain == NULL ? 1 : 0);//二级桶后面 还有没有其他二级桶
			uhash->hybridChainsStorage[indexInStorage].point.bucketLength = (nPointsInBucket <= MAX_NONOVERFLOW_POINTS_PER_BUCKET ?
											 nPointsInBucket : 
											 0); // 0 means there are "overflow" points
			uhash->hybridChainsStorage[indexInStorage].point.isLastPoint = (nPointsInBucket == 1 ? 1 : 0);//巡回到了第一个，当然是当前的二级桶最后一个点
			uhash->hybridChainsStorage[indexInStorage].point.pointIndex = bucket->firstEntry.pointIndex;//二级桶的第一个实体 copy
			indexInStorage++;

			// Store all other points in the storage
			Uns32T currentIndex = indexInStorage; // index where the "current" point will be stored.
			Uns32T nOverflow = 0;
			Uns32T overflowStart = lastIndexInSt;
			if (nPointsInBucket <= MAX_NONOVERFLOW_POINTS_PER_BUCKET){
			  indexInStorage = indexInStorage + nPointsInBucket - 1;
			}
			else
			{
			  // bucket too large.
			  // store the overflow points at the end of the array <uhash->hybridChainsStorage>.
			  nOverflow = nPointsInBucket - MAX_NONOVERFLOW_POINTS_PER_BUCKET;
			  overflowStart = lastIndexInSt - nOverflow + 1;
			  lastIndexInSt = overflowStart - 1;

			  // specify the offset of the start of overflow points in the
			  // fields <bucketLength> of points 2, 3, ... of the space
			  // immediately after the bucket.
			  Uns32T value = overflowStart - (currentIndex - 1 + MAX_NONOVERFLOW_POINTS_PER_BUCKET);
			  for(IntT j = 0; j < N_FIELDS_PER_INDEX_OF_OVERFLOW; j++)
			  {
				uhash->hybridChainsStorage[currentIndex + j].point.bucketLength = value & ((1U << N_BITS_FOR_BUCKET_LENGTH) - 1);
				value = value >> N_BITS_FOR_BUCKET_LENGTH;
			  }

			  // update <indexInStorage>
			  indexInStorage = indexInStorage + MAX_NONOVERFLOW_POINTS_PER_BUCKET - 1;
			  ASSERT(indexInStorage <= lastIndexInSt + 1);

			  //FAILIFWR(nPointsInBucket > MAX_NONOVERFLOW_POINTS_PER_BUCKET, "Too many points in a bucket -- feature not implemented yet. Try to lower N_BITS_PER_POINT_INDEX as much as possible.");// TODO: not implemented yet
			}

			bucketEntry = bucket->firstEntry.nextEntry;//重新获得当前二级桶的第一个实体
			while(bucketEntry != NULL)
			{//每个实体从 model结构体 复制到 uhash->hybridChainsStorage
			  uhash->hybridChainsStorage[currentIndex].point.pointIndex = bucketEntry->pointIndex;
			  uhash->hybridChainsStorage[currentIndex].point.isLastPoint = 0;
			  bucketEntry = bucketEntry->nextEntry;

			  currentIndex++;
			  if (currentIndex == indexInStorage && nPointsInBucket > MAX_NONOVERFLOW_POINTS_PER_BUCKET)
			  {
				// finished the normal alloted space -> going to the space reserved at end of the table.
				currentIndex = overflowStart;
			  }
			}

			// set the <isLastBucket> field of the last point = 1.
			uhash->hybridChainsStorage[currentIndex - 1].point.isLastPoint = 1;
	
			bucket = bucket->nextGBucketInChain;
			//ASSERT((uhash->hashTable.hybridHashTable[i] + 1)->point.bucketLength > 0);
      }//   while(bucket != NULL)
    }// for(Int32T i = 0; i < hashTableSize; i++)


    ASSERT(indexInStorage == lastIndexInSt + 1);
    uhash->nHashedPoints = modelHT->nHashedPoints;
    uhash->nHashedBuckets = modelHT->nHashedBuckets;
    break;
  default:
    ASSERT(FALSE);
  }//  switch (typeHT) 



  //对主hash和辅助hash表的初始化：给一个随机值
  // Initializing the main hash function.
  if (!useExternalUHFs)
  {
    FAILIF(NULL == (uhash->mainHashA = (Uns32T*)MALLOC(uhash->hashedDataLength * sizeof(Uns32T))));
    for(IntT i = 0; i < uhash->hashedDataLength; i++)
	{
      uhash->mainHashA[i] = genRandomUns32(1, MAX_HASH_RND);
    }
    mainHashA = uhash->mainHashA;
  }
  else 
  {
    uhash->mainHashA = mainHashA;
  }

  // Initializing the control hash functions.
  if (!useExternalUHFs)
  {
    FAILIF(NULL == (uhash->controlHash1 = (Uns32T*)MALLOC(uhash->hashedDataLength * sizeof(Uns32T))));
    for(IntT i = 0; i < uhash->hashedDataLength; i++)
	{
      uhash->controlHash1[i] = genRandomUns32(1, MAX_HASH_RND);
    }
    controlHash1 = uhash->controlHash1;
  } 
  else 
  {
    uhash->controlHash1 = controlHash1;
  }

  return uhash;
}

// Removes all the buckets/points from the hash table. Used only for
// HT_LINKED_LIST.
void clearUHashStructure(PUHashStructureT uhash){
  ASSERT(uhash != NULL);
  switch (uhash->typeHT) {
  case HT_LINKED_LIST:
    for(Int32T i = 0; i < uhash->hashTableSize; i++){
      PGBucketT bucket = uhash->hashTable.llHashTable[i];
      while(bucket != NULL){
	PGBucketT tempBucket = bucket;
	bucket = bucket->nextGBucketInChain;
	tempBucket->nextGBucketInChain = uhash->unusedPGBuckets;
	uhash->unusedPGBuckets = tempBucket;
	
	PBucketEntryT bucketEntry = tempBucket->firstEntry.nextEntry;
	while(bucketEntry != NULL){
	  PBucketEntryT tempEntry = bucketEntry;
	  bucketEntry = bucketEntry->nextEntry;
	  tempEntry->nextEntry = uhash->unusedPBucketEntrys;
	  uhash->unusedPBucketEntrys = tempEntry;
	}
      }
      
      uhash->hashTable.llHashTable[i] = NULL;
    }
    break;
  case HT_STATISTICS:
    ASSERT(FALSE); // Not supported.
//     for(IntT i = 0; i < uhash->hashTableSize; i++){
//       uhash->hashTable.linkHashTable[i][0].indexStart = INDEX_START_EMPTY;
//     }
    break;
  default:
    ASSERT(FALSE);
  }
  uhash->nHashedPoints = 0;
  uhash->nHashedBuckets = 0;
}

// Reorders uhash (of type HT_STATISTICS) to optimize for cache
// behavior. If <auxPtsList> is NULL, it is allocated, but not freed at
// the end (so, the caller has to free <auxPtsList>).
void optimizeUHashStructure(PUHashStructureT uhash, PointsListEntryT *(&auxPtsList)){
  ASSERT(FALSE); // HT_STATISTICS not supported
  ASSERT(uhash->typeHT == HT_STATISTICS);
  
  if (auxPtsList == NULL){
    FAILIF(NULL == (auxPtsList = (PointsListEntryT*)MALLOC(uhash->hashTableSize * sizeof(PointsListEntryT))));
  }
  
  Int32T newP = 0;
  for(Int32T i = 0; i < uhash->hashTableSize; i++){
    for(IntT j = 0; j < uhash->chainSizes[i] && uhash->hashTable.linkHashTable[i][j].indexStart != INDEX_START_EMPTY; j++){
      Int32T p = uhash->hashTable.linkHashTable[i][j].indexStart;
      uhash->hashTable.linkHashTable[i][j].indexStart = newP;
      while (p != INDEX_START_EMPTY) {
	auxPtsList[newP].point = uhash->bucketPoints.pointsList[p].point;
	auxPtsList[newP].nextPoint = newP + 1;
	newP++;
	p = uhash->bucketPoints.pointsList[p].nextPoint;
      }
      auxPtsList[newP - 1].nextPoint = INDEX_START_EMPTY;
    }
  }
  PointsListEntryT *tempList = auxPtsList;
  auxPtsList = uhash->bucketPoints.pointsList;
  uhash->bucketPoints.pointsList = tempList;
}

// Frees the <uhash> structure. If <freeHashFunctions>==FALSE, then
// the hash functions are not freed (because they might be reused, and
// therefore shared by several PUHashStructureT structures).
void freeUHashStructure(PUHashStructureT uhash, BooleanT freeHashFunctions){
  if (uhash == NULL){
    return;
  }

  switch (uhash->typeHT) {
  case HT_LINKED_LIST:
    for(IntT i = 0; i < uhash->hashTableSize; i++){
      PGBucketT bucket = uhash->hashTable.llHashTable[i];
      while (bucket != NULL){
	PGBucketT tempBucket = bucket;
	bucket = bucket->nextGBucketInChain;
	PBucketEntryT bucketEntry = tempBucket->firstEntry.nextEntry;
	while (bucketEntry != NULL){
	  PBucketEntryT tempEntry = bucketEntry;
	  bucketEntry = bucketEntry->nextEntry;
	  free(tempEntry);
	}
	free(tempBucket);
      }
    }
    free(uhash->hashTable.llHashTable);
    if (uhash->unusedPGBuckets != NULL){
      PGBucketT bucket = uhash->unusedPGBuckets;
      while (bucket != NULL){
	PGBucketT tempBucket = bucket;
	bucket = bucket->nextGBucketInChain;
	free(tempBucket);
      }
    }
    if (uhash->unusedPBucketEntrys != NULL){
      PBucketEntryT bucketEntry = uhash->unusedPBucketEntrys;
      while (bucketEntry != NULL){
	PBucketEntryT tempEntry = bucketEntry;
	bucketEntry = bucketEntry->nextEntry;
	free(tempEntry);
      }
    }
    ASSERT(uhash->chainSizes == NULL);
    break;
  case HT_HYBRID_CHAINS:
    free(uhash->hashTable.hybridHashTable);
    free(uhash->hybridChainsStorage);
    ASSERT(uhash->chainSizes == NULL);
    break;
  default:
    ASSERT(FALSE);
  }

  if (freeHashFunctions){
    free(uhash->mainHashA);
    free(uhash->controlHash1);
  }
  free(uhash);
}

// Computes (a.b)mod UH_PRIME_DEFAULT. b is coded as <nBPieces> blocks
// of size totaling <size>. <a> is of length <size>.
inline Uns32T computeBlockProductModDefaultPrime(Uns32T *a, Uns32T *(b[]), IntT nBPieces, IntT size){
//计算a b累加点积，求模，得到hash值，使用默认的 prime=模 （2^32-5了）
  LongUns64T h = 0;
  IntT j = 0;
  IntT bPiece = 0;
  IntT bPieceSize = size / nBPieces;
  IntT i = 0;
  for(IntT bPiece = 0; bPiece < nBPieces; bPiece++){
    for(IntT j = 0; j < bPieceSize; j++, i++){
		//做积分点积  不过，她是一边点积，一边就模 （2^32-5了）
      h = h + (LongUns64T)a[i] * (LongUns64T)b[bPiece][j];
      h = (h & TWO_TO_32_MINUS_1) + 5 * (h >> 32);

      if (h >= UH_PRIME_DEFAULT) {
		h = h - UH_PRIME_DEFAULT;
      }
      CR_ASSERT(h < UH_PRIME_DEFAULT);
    }
  }
  return h;
}

// Computes (a.b)mod UH_PRIME_DEFAULT.
inline Uns32T computeProductModDefaultPrime(Uns32T *a, Uns32T *b, IntT size)
{//计算a。b累加， 然后模（2^32-5）
  LongUns64T h = 0;
  IntT i = 0;
  for(IntT i = 0; i < size; i++){
    h = h + (LongUns64T)a[i] * (LongUns64T)b[i];
    h = (h & TWO_TO_32_MINUS_1) + 5 * (h >> 32);
    if (h >= UH_PRIME_DEFAULT) {
      h = h - UH_PRIME_DEFAULT;
    }
    CR_ASSERT(h < UH_PRIME_DEFAULT);
  }
  return h;
}

// Compute fuction ((rndVector . data)mod prime)mod hashTableSize
// Vectors <rndVector> and <data> are assumed to have length <size>.
inline Uns32T computeUHashFunction(
Uns32T *rndVector, Uns32T *(data[]), 
IntT nDataPieces, IntT size, 
Uns32T prime, Int32T hashTableSize)

{//计算h1主函数值，
	//只是判断参数，然后调用
  ASSERT(prime == UH_PRIME_DEFAULT);
  ASSERT(rndVector != NULL);
  ASSERT(data != NULL);

  Uns32T h = computeBlockProductModDefaultPrime(rndVector, data, nDataPieces, size) % hashTableSize;

  ASSERT(h >= 0 && h < hashTableSize);

  return h;
}

inline Uns32T combinePrecomputedHashes(Uns32T *firstBucketVector,
	Uns32T *secondBucketVector, IntT nBucketVectorPieces, IntT uhfIndex)
{
  // CR_ASSERT(bucketVector != NULL);
//   if (nBucketVectorPieces == 1) {
//     // using normal <g> functions.
//     CR_ASSERT(bucketVector[1] != NULL);
//     return (bucketVector[1][uhfIndex] % UH_PRIME_DEFAULT);
//   } else {
//     CR_ASSERT(nBucketVectorPieces == 2); // each of the <g> functions is a pair of 2 <u> functions.
//     //CR_ASSERT(bucketVector[2] != NULL);
//     //CR_ASSERT(bucketVector[3] != NULL);
//     LongUns64T r = (LongUns64T)(bucketVector[2][uhfIndex]) + (LongUns64T)(bucketVector[3][uhfIndex + UHF_NUMBER_OF_HASHES]);
//     if (r >= UH_PRIME_DEFAULT) {
//       r -= UH_PRIME_DEFAULT;
//     }
//     CR_ASSERT(r < UH_PRIME_DEFAULT);
//     return (Uns32T)r;
//   }
  if (nBucketVectorPieces == 1) {
    // using normal <g> functions.
    Uns32T h = firstBucketVector[uhfIndex];
    if (h > UH_PRIME_DEFAULT){
      h = h - UH_PRIME_DEFAULT;
    }
    return h;
  } else {
    CR_ASSERT(nBucketVectorPieces == 2); // each of the <g> functions is a pair of 2 <u> functions.
    //CR_ASSERT(bucketVector[2] != NULL);
    //CR_ASSERT(bucketVector[3] != NULL);
    LongUns64T r = (LongUns64T)(firstBucketVector[uhfIndex]) + (LongUns64T)(secondBucketVector[uhfIndex + UHF_NUMBER_OF_HASHES]);
    if (r >= UH_PRIME_DEFAULT) {
      r -= UH_PRIME_DEFAULT;
    }
    CR_ASSERT(r < UH_PRIME_DEFAULT);
    return (Uns32T)r;
  }
}

// Adds the bucket entry (a point <point>) to the bucket defined by
// bucketVector in the uh structure with number uhsNumber. If no such
// bucket exists, then it is first created.
void addBucketEntry(PUHashStructureT uhash, IntT nBucketVectorPieces,
Uns32T firstBucketVector[], Uns32T secondBucketVector[]
/*, PPointT point*/ ,Int32T pointIndex)
{//根据vector来计算hash值，然后查找桶，添加桶，点？？？


  CR_ASSERT(uhash != NULL);
  // CR_ASSERT(bucketVector != NULL);

  Uns32T hIndex;
  Uns32T control1;

  if (!USE_PRECOMPUTED_HASHES){
    // if not using the same hash functions across multiple
    // UHashStructureT, then we need to compute explicitly the hases.
    Uns32T *tempVector[2];
    tempVector[0] = firstBucketVector;
    tempVector[1] = secondBucketVector;
    hIndex = computeUHashFunction(uhash->mainHashA, tempVector, nBucketVectorPieces, uhash->hashedDataLength, uhash->prime, uhash->hashTableSize);
	//计算第一个hansh表索引值
    control1 = computeBlockProductModDefaultPrime(uhash->controlHash1, tempVector, nBucketVectorPieces, uhash->hashedDataLength);
  } else {
    // if using the same hash functions across multiple
    // UHashStructureT, then we can use the (possibly partially)
    // precomputed hash values.
    CR_ASSERT(uhash->prime == UH_PRIME_DEFAULT);
    hIndex = combinePrecomputedHashes(firstBucketVector, secondBucketVector, nBucketVectorPieces, UHF_MAIN_INDEX) % uhash->hashTableSize;
    control1 = combinePrecomputedHashes(firstBucketVector, secondBucketVector, nBucketVectorPieces, UHF_CONTROL1_INDEX);
	//获取原来计算好的  两个值，相加
  }

  PGBucketT p;
  BooleanT found;
  Int32T j;
  Int32T temp;
  switch (uhash->typeHT) {
  case HT_LINKED_LIST://链式的表，先索引到第一层，然后用遍历链表找第二层（用第二个hash值为关键字）
    p = uhash->hashTable.llHashTable[hIndex];
    while(p != NULL && 
	  (p->controlValue1 != control1)) {
      p = p->nextGBucketInChain;
    }
    if (p == NULL) {//是空的就建立
      // new bucket to add to the hash table
      uhash->nHashedBuckets++;
      uhash->hashTable.llHashTable[hIndex] = newGBucket(uhash,
							control1, 
							pointIndex, 
							uhash->hashTable.llHashTable[hIndex]);
    } else {
      // add this bucket entry to the existing bucket
      addPointToGBucket(uhash, p, pointIndex);
    }
    break;
  case HT_PACKED:
//     // The bucket should already exist.
//     IntT i;
//     for(i = 0; i < uhash->chainSizes[hIndex] && uhash->hashTable.packedHashTable[hIndex][i].nPointsInBucket > 0; i++){
//       if (uhash->hashTable.packedHashTable[hIndex][i].controlValue1 == control1){
// 	break;
//       }
//     }
//     uhash->hashTable.packedHashTable[hIndex][i].nPointsInBucket++;
    break;
  case HT_STATISTICS:
    ASSERT(FALSE);
//     uhash->bucketPoints.pointsList[uhash->nHashedPoints].point = point;
//     found = FALSE;
//     for(j = 0; j < uhash->chainSizes[hIndex] && uhash->hashTable.linkHashTable[hIndex][j].indexStart != INDEX_START_EMPTY; j++){
//       if (uhash->hashTable.linkHashTable[hIndex][j].controlValue1 == control1){
// 	found = TRUE;
// 	break;
//       }
//     }
//     if (!found) {
//       // new bucket
//       if (j >= uhash->chainSizes[hIndex]) {
// 	// dont have enough space in pREALLOCated memory.
// 	if (uhash->chainSizes[hIndex] > 0) {
// 	  uhash->chainSizes[hIndex] = CEIL(uhash->chainSizes[hIndex] * CHAIN_RESIZE_RATIO);
// 	}else{
// 	  uhash->chainSizes[hIndex] = 1;
// 	}
// 	uhash->hashTable.linkHashTable[hIndex] = (LinkPackedGBucketT*)REALLOC(uhash->hashTable.linkHashTable[hIndex], uhash->chainSizes[hIndex] * sizeof(LinkPackedGBucketT));
// 	uhash->hashTable.linkHashTable[hIndex][j].controlValue1 = control1;
//       }
//       uhash->hashTable.linkHashTable[hIndex][j].controlValue1 = control1;
//       uhash->hashTable.linkHashTable[hIndex][j].indexStart = INDEX_START_EMPTY;
//       uhash->nHashedBuckets++;
//       if (j + 1 < uhash->chainSizes[hIndex]){
// 	uhash->hashTable.linkHashTable[hIndex][j + 1].indexStart = INDEX_START_EMPTY;
//       }
//     }
    
//     temp = uhash->hashTable.linkHashTable[hIndex][j].indexStart;
//     uhash->hashTable.linkHashTable[hIndex][j].indexStart = uhash->nHashedPoints;
//     uhash->bucketPoints.pointsList[uhash->nHashedPoints].nextPoint = temp;

    break;
  default:
    ASSERT(FALSE);
  }
  uhash->nHashedPoints++;
}

// Returns the bucket defined by the vector <bucketVector> in the UH
// structure number <uhsNumber>.
GeneralizedPGBucket getGBucket
	(PUHashStructureT uhash, IntT nBucketVectorPieces,
	Uns32T firstBucketVector[], Uns32T secondBucketVector[])
{//通过两个向量，计算主副索引。然后遍历二级索引，提取对应的桶

  Uns32T hIndex;
  Uns32T control1;

  //TIMEV_START(timeGBHash);

  //计算主hansh，辅助hash
  if (!USE_PRECOMPUTED_HASHES)
  {
    // if not using the same hash functions across multiple
    // UHashStructureT, then we need to compute explicitly the hases.
    Uns32T *tempVector[2];
    tempVector[0] = firstBucketVector;
    tempVector[1] = secondBucketVector;
	//具体算法没看
    hIndex = computeUHashFunction(uhash->mainHashA, tempVector, nBucketVectorPieces, uhash->hashedDataLength, uhash->prime, uhash->hashTableSize);
    control1 = computeBlockProductModDefaultPrime(uhash->controlHash1, tempVector, nBucketVectorPieces, uhash->hashedDataLength);
  }
  else 
  {
    // if using the same hash functions across multiple
    // UHashStructureT, then we can use the (possibly partially)
    // precomputed hash values.
    CR_ASSERT(uhash->prime == UH_PRIME_DEFAULT);

	//当使用了提前计算时： 一般长度* 原长度向量 = 分成了两个向量保存：   precomputedHashesOfULSHs【0->1 高位的一二级索引   2->3 低位的一二级索引】
	//这个函数只是将高低位索引拼接，
    hIndex = combinePrecomputedHashes(firstBucketVector, secondBucketVector, nBucketVectorPieces, UHF_MAIN_INDEX) % uhash->hashTableSize;
    control1 = combinePrecomputedHashes(firstBucketVector, secondBucketVector, nBucketVectorPieces, UHF_CONTROL1_INDEX);
  }
  //TIMEV_END(timeGBHash);

  GeneralizedPGBucket result;
  PGBucketT p;
  PHybridChainEntryT indexHybrid = NULL;
  //TIMEV_START(timeChainTraversal);
  switch(uhash->typeHT)
  {
  case HT_LINKED_LIST:
    p = uhash->hashTable.llHashTable[hIndex];
    while(p != NULL && 
	  (p->controlValue1 != control1)) {
      p = p->nextGBucketInChain;
      nBucketsInChains++;
    }
    result.llGBucket = p;
    return result;
  case HT_STATISTICS:
    ASSERT(FALSE); // HT_STATISTICS not supported anymore
    for(IntT j = 0; j < uhash->chainSizes[hIndex] && uhash->hashTable.linkHashTable[hIndex][j].indexStart != INDEX_START_EMPTY; j++){
      if (uhash->hashTable.linkHashTable[hIndex][j].controlValue1 == control1){
	result.linkGBucket = &(uhash->hashTable.linkHashTable[hIndex][j]);
	return result;
      }
    }
    result.linkGBucket = NULL;
    return result;
  case HT_PACKED:
    ASSERT(FALSE); // HT_PACKED not supported anymore
    for(IntT j = 0; j < uhash->chainSizes[hIndex]; j++){
      if (uhash->hashTable.packedHashTable[hIndex][j].controlValue1 == control1){
	result.packedGBucket = &(uhash->hashTable.packedHashTable[hIndex][j]);
	return result;
      }
    }
    result.packedGBucket = NULL;
    return result;

  case HT_HYBRID_CHAINS://默认
    indexHybrid = uhash->hashTable.hybridHashTable[hIndex];
	//通过主索引值，得到第二层索引的链表
    while (indexHybrid != NULL)
	{ //遍历数组的每一个点，看第二层索引的值一不一致
      if (indexHybrid->controlValue1 == control1)
	  {
		  result.hybridGBucket = indexHybrid + 1;
		  return result;
      }
	  else
	  {
		  indexHybrid = indexHybrid + 1;//偏移到了下一个
		  if (indexHybrid->point.isLastBucket != 0)
		  {//到最后了
			  result.hybridGBucket = NULL;
			  return result;
		  }

		  //这句话是核心的操作啊！   由于二级桶和实体都是一个数组上，所以，查询
		  //就是每个二级桶的的开始值比较一下
		  //然后和  当前二级桶+当前二级桶长度 =下一个二级桶的开始
		  //在比较二级桶
		  indexHybrid = indexHybrid + indexHybrid->point.bucketLength;//为何还要加一个10？
		  //是不是一个桶的大小就是10个，公用一个二级索引值，一个一级索引指向一个二级索引数组
      }
    }//while

    result.hybridGBucket = NULL;
    return result;
    break;
  default:
    ASSERT(FALSE);
  }
  //TIMEV_END(timeChainTraversal);

}


void precomputeUHFsForULSH(PUHashStructureT uhash, Uns32T *uVector, IntT length, Uns32T *result){
	//计算两个hash值，返回到resutl数组
  if (length == uhash->hashedDataLength){
	  //计算第一个向量*第二个向量 然后模 2^32-5
    result[UHF_MAIN_INDEX] = computeProductModDefaultPrime(uhash->mainHashA, uVector, length);//主hash索引值
    result[UHF_CONTROL1_INDEX] = computeProductModDefaultPrime(uhash->controlHash1, uVector, length);//辅助hash索引值
  } else {
    ASSERT(2 * length == uhash->hashedDataLength); // the length is 1/2 of the bucket length
    result[UHF_MAIN_INDEX] = computeProductModDefaultPrime(uhash->mainHashA, uVector, length);
    result[UHF_CONTROL1_INDEX] = computeProductModDefaultPrime(uhash->controlHash1, uVector, length);
	//？？为何还有后面一部分？

	//由于在nnnstrcut中 ，每组只取了8，是hashedDataLength的一半， 而uhash->mainHashA 是16维度的向量，
	//所以这里先计算了hansh表向量的 前8维度 点乘  降维后的 8维uVector；
	//然后再次用后8维点乘
    result[UHF_MAIN_INDEX + UHF_NUMBER_OF_HASHES] = computeProductModDefaultPrime(uhash->mainHashA + length, uVector, length);
    result[UHF_CONTROL1_INDEX + UHF_NUMBER_OF_HASHES] = computeProductModDefaultPrime(uhash->controlHash1 + length, uVector, length);
  }
}

