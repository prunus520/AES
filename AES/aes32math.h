/*
 *     檔名： aes08math.h
 *     全名： aes 08-bit mathematical operation
 *     作者： Cerasus
 *     描述： 
 */

#ifndef _AES32MATH_H_ 
#define _AES32MATH_H_

#include "aesemath.h"
#include "aesctx.h"

namespace math32{
	#undef wordEKey
	#define wordEKey
	#undef LOOKUP
	
	
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
	unsigned RotWord(unsigned shiftWord);
	unsigned SubWord(unsigned subWord);
	unsigned Rcon(unsigned char round);
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
	
	#include "aesmd.h"
}

#endif
