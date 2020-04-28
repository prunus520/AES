/*
 *     檔名： aesctx.h
 *     全名： aes context
 *     作者： Cerasus
 *     描述： 
 */

#ifndef _AESCTX_H_ 
#define _AESCTX_H_

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../frdsgn.h"

//  定義填充類型 
#define No 0
#define Zero 1
#define Space 2
#define ISOIEC78164 3
#define ZeroLen 4
#define ANSIX923 5
#define ISO10126 6
#define PKCS7 7
#define PADDING_MAX 8

//  定義 context 內容 
typedef struct{
	const char* mode;
	unsigned short blockSize;
	unsigned short keySize;
	unsigned char* key = NULL;
	unsigned char* iv = NULL;
	unsigned char padding;
	const char* inFile;
    const char* outFile;
	long dataLength;
	long blockLength;
	unsigned char* data;
	unsigned char Nb:4;
	unsigned char Nk:4;
	unsigned char Nr:4;
	unsigned char dataOffset;
	unsigned short bitOffset;
	//  08 - bit 與 32 - bit 共用 
	union{
		unsigned char* EKey08 = NULL;
		unsigned* EKey32;
	}ExpandedKey;
}AES_context, *PAES_context;

/********************  宣告函式  ********************/ 


/********************
 *  context 創建與設定 
 ********************/
PAES_context PAES_create(const char* mode, unsigned short blockSize, unsigned char* iv, unsigned short keySize, unsigned char* key, unsigned char padding);
void PAES_free(PAES_context ctx);

void setMode(PAES_context ctx, const char* mode);
const char* getMode(PAES_context ctx);

void setBlock(PAES_context ctx, unsigned short blockSize, unsigned char* iv = NULL);
void setBlockSize(PAES_context ctx, unsigned short blockSize);
unsigned short getBlockSize(PAES_context ctx);
void setIV(PAES_context ctx, unsigned char* iv);
unsigned char* getIV(PAES_context ctx);

void setKey(PAES_context ctx, unsigned short keySize, unsigned char* key = NULL);
void setKeySize(PAES_context ctx, unsigned short keySize);
unsigned short getKeySize(PAES_context ctx);
void setKEY(PAES_context ctx, unsigned char* key);
unsigned char* getKey(PAES_context ctx);

void setPadding(PAES_context ctx, unsigned char padding);
unsigned char getPadding(PAES_context ctx);

void setFile(PAES_context ctx, const char* inFile, const char* outFile);
const char* getInFile(PAES_context ctx);
const char* getOutFile(PAES_context ctx);

/********************
 *  讀寫檔案 
 ********************/
void readFile(PAES_context ctx);
void writeFile(PAES_context ctx);

#endif
