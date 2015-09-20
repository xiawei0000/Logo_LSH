#ifndef MYSQL_MAIN_H
#define MYSQL_MAIN_H
#include "mysql_tools.h"

extern int   NUMBER_URLBASEINDEX;

void mySqlInsertResult(int URLImage_idURLImage,double maxsim);

void mySqlInsertResultWithLogoId(int URLImage_idURLImage,int  logoid);

void mySqlInsertLogoFeats(int LogoId,int Cheng, LogoFeatStruct logoFeat);
int mySqlGetLogoFeats(int idLogo,int Cheng, LogoFeatStruct *logoFeat);

void mySqlGetPictureAddress
	(long long idProgect,
	MYSQL *sock, 
	srcIndexStruct **srcIndex,
	int *numberOfRows_ALL
	);



void mySqlGetLogoAddress
	(int idBrand,
	MYSQL *sock, 
	srcIndexStruct **srcIndex,int* numberOfRows
	);


int mySqlGetidBrand
(long long idProgect ,
MYSQL *sock);

void mySqlLogoSrc(long long idProgect,int*numberOfRows,srcIndexStruct **srcIndex);


void mySqlPictureSrc(long long idProgect,int*numberOfRows,srcIndexStruct **srcIndex);


int  mysqlGetAllRowsForPicture(long long idProgect,
	MYSQL *sock, 
	int *numberOfRows,
	char **idURLBase_Index//[][_MAX_PATH]
	);
#endif
