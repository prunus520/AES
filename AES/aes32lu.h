/*
 *     檔名： aes32lu.h
 *     全名： aes 32-bit lookup operation
 *     作者： Cerasus
 *     描述： 
 */

#ifndef _AES32LU_H_
#define _AES21LU_H_

#include "aesctx.h"

namespace lu32{
	#undef wordEKey
	#define wordEKey
	#undef LOOKUP
	#define LOOKUP
	#include "aes32lut.h"
	
	
	/********************  宣告函式  ********************/ 
	
	
	/********************
	 *  陣列處理 
	 ********************/
	void xorArray(unsigned dst[], unsigned char src[], unsigned char size);
	void xorArray(unsigned dst[], unsigned src[], unsigned char size);
	void copyArray(unsigned char dst[], unsigned char src[], unsigned char size);
	void copyArray(unsigned char dst[], unsigned src[], unsigned char size);
	void copyArray(unsigned dst[], unsigned char src[], unsigned char size);
	void copyArray(unsigned dst[], unsigned src[], unsigned char size);
	
	/********************
	 *  字組與位元組轉換 
	 ********************/
	unsigned char word2byte(unsigned state, unsigned char offset);
	
	/********************
	 *  金鑰擴展
	 ********************/
	unsigned RotWord(unsigned shiftByte);
	unsigned SubWord(const unsigned box[], unsigned subWord);
	void KeyExpansion(PAES_context ctx);
	
	/********************
	 *  一般模式 
	 ********************/
	void AddRoundKey(PAES_context ctx, unsigned state[], unsigned char roundKey);
	void SubBytes(PAES_context ctx, unsigned state[]);
	void SubBytesInv(PAES_context ctx, unsigned state[]);
	void ShiftRows(PAES_context ctx, unsigned state[]);
	void ShiftRowsInv(PAES_context ctx, unsigned state[]);
	void MixColumns(PAES_context ctx, unsigned state[]);
	void MixColumnsInv(PAES_context ctx, unsigned state[]);
	
	/********************
	 *  同型模式
	 ********************/
	void AddRoundKey(bool crypt, PAES_context ctx, unsigned state[], unsigned char roundKey);
	void SubBytes(bool crypt, PAES_context ctx, unsigned state[]);
	void ShiftRows(bool crypt, PAES_context ctx, unsigned state[]);
	void MixColumns(bool crypt, PAES_context ctx, unsigned state[]);
	
	/********************
	 *  整合同型模式
	 ********************/
	void I_SubBytes(bool crypt, PAES_context ctx, unsigned state[]);
	void I_MixColumns(bool crypt, PAES_context ctx, unsigned state[]);
	
	#include "aesmd.h"
}

#endif
