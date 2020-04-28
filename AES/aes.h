/*
 *     檔名： aes.h
 *     全名： aes
 *     作者： Cerasus
 *     描述： 
 */

#ifndef _AES_H_ 
#define _AES_H_

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "aes08lu.h"
#include "aes32lu.h"
#include "aes08math.h"
#include "aes32math.h"

#define MATH 0
#define LU 1


/********************  宣告函式  ********************/ 


/********************
 *  列印 AES 
 ********************/ 
void printAES(const char name[], unsigned char data[], long size);

/********************
 *  字串比較 
 ********************/ 
bool strCMP(const char* cmp1, const char* cmp2, int count = 3);

/********************
 *  08 - bit 
 ********************/ 
void AES_padding(PAES_context ctx, bool crypt);
void AES(PAES_context ctx, void (*ke)(PAES_context), void (*ecb)(PAES_context), void (*cbc)(PAES_context), void (*ctr)(PAES_context), void (*ofb)(PAES_context), void (*cfb)(PAES_context), bool crypt, bool print = false);
void AES_C(PAES_context ctx, void (*ke)(PAES_context), void (*ecb)(PAES_context, bool), void (*cbc)(PAES_context, bool), void (*ctr)(PAES_context, bool), void (*ofb)(PAES_context, bool), void (*cfb)(PAES_context, bool), bool crypt, bool print);
void AES_Encrypt(PAES_context ctx, bool is08b, bool opm, bool print = false);
void AES_Decrypt(PAES_context ctx, bool is08b, bool opm, bool print = false);
void AES_Crypt(PAES_context ctx, bool crypt, bool is08b, bool opm, bool print = false);
void AES_I_Crypt(PAES_context ctx, bool crypt, bool is08b, bool opm, bool print = false);

#endif
