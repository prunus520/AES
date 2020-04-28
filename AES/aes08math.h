/*
 *     檔名： aes08math.h
 *     全名： aes 08-bit mathematical operation
 *     作者： Cerasus
 *     描述： 
 */

#ifndef _AES08MATH_H_ 
#define _AES08MATH_H_

#include "aesemath.h"
#include "aesctx.h"

namespace math08{
	#undef wordEKey
	#undef LOOKUP
	
	
	/********************  宣告函式  ********************/ 
	
	
	/********************
	 *  陣列處理 
	 ********************/
	void xorArray(unsigned char dst[], unsigned char src[], unsigned char size);
	void copyArray(unsigned char dst[], unsigned char src[], unsigned char size);
	
	/********************
	 *  金鑰擴展
	 ********************/
	unsigned short RotByte(unsigned short shiftByte);
	unsigned char Rcon(unsigned char round);
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
	
	#include "aesmd.h"
}

#endif
