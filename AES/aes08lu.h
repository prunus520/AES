/*
 *     檔名： aes08lu.h
 *     全名： aes 08-bit lookup operation
 *     作者： Cerasus
 *     描述： 
 */

#ifndef _AES08LU_H_
#define _AES08LU_H_

#include "aesctx.h"

namespace lu08{
	#undef wordEKey
	#undef LOOKUP
	#define LOOKUP
	#include "aes08lut.h"
	
	
	/********************  宣告函式  ********************/ 
	
	
	/********************
	 *  陣列處理 
	 ********************/
	void xorArray(unsigned char dst[], unsigned char src[], unsigned char size);
	void copyArray(unsigned char dst[], unsigned char src[], unsigned char size);
	
	/********************
	 *  金鑰擴展
	 ********************/
	void KeyExpansion(PAES_context ctx);
	
	/********************
	 *  一般模式 
	 ********************/
	void AddRoundKey(PAES_context ctx, unsigned char state[], unsigned char roundKey);
	void SubBytes(PAES_context ctx, unsigned char state[]);
	void SubBytesInv(PAES_context ctx, unsigned char state[]);
	void ShiftRows(PAES_context ctx, unsigned char state[]);
	void ShiftRowsInv(PAES_context ctx, unsigned char state[]);
	void MixColumns(PAES_context ctx, unsigned char state[]);
	void MixColumnsInv(PAES_context ctx, unsigned char state[]);
	
	/********************
	 *  同型模式
	 ********************/
	void AddRoundKey(bool crypt, PAES_context ctx, unsigned char state[], unsigned char roundKey);
	void SubBytes(bool crypt, PAES_context ctx, unsigned char state[]);
	void ShiftRows(bool crypt, PAES_context ctx, unsigned char state[]);
	void MixColumns(bool crypt, PAES_context ctx, unsigned char state[]);
	
	/********************
	 *  整合同型模式
	 ********************/
	void I_SubBytes(bool crypt, PAES_context ctx, unsigned char state[]);
	void I_MixColumns(bool crypt, PAES_context ctx, unsigned char state[]);
	
	#include "aesmd.h"
}

#endif
