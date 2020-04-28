/*
 *     �ɦW�G aes08math.h
 *     ���W�G aes 08-bit mathematical operation
 *     �@�̡G Cerasus
 *     �y�z�G 
 */

#ifndef _AES32MATH_H_ 
#define _AES32MATH_H_

#include "aesemath.h"
#include "aesctx.h"

namespace math32{
	#undef wordEKey
	#define wordEKey
	#undef LOOKUP
	
	
	/********************  �ŧi�禡  ********************/ 
	
	
	/********************
	 *  �}�C�B�z 
	 ********************/
	void xorArray(unsigned dst[], unsigned char src[], unsigned char size);
	void xorArray(unsigned dst[], unsigned src[], unsigned char size);
	void copyArray(unsigned char dst[], unsigned char src[], unsigned char size);
	void copyArray(unsigned char dst[], unsigned src[], unsigned char size);
	void copyArray(unsigned dst[], unsigned char src[], unsigned char size);
	void copyArray(unsigned dst[], unsigned src[], unsigned char size);
	
	/********************
	 *  �r�ջP�줸���ഫ 
	 ********************/
	unsigned char word2byte(unsigned state, unsigned char offset);
	
	/********************
	 *  ���_�X�i
	 ********************/
	unsigned RotWord(unsigned shiftWord);
	unsigned SubWord(unsigned subWord);
	unsigned Rcon(unsigned char round);
	void KeyExpansion(PAES_context ctx);
	
	/********************
	 *  �@��Ҧ� 
	 ********************/
	void AddRoundKey(PAES_context ctx, unsigned state[], unsigned char roundKey);
	void SubBytes(PAES_context ctx, unsigned state[]);
	void SubBytesInv(PAES_context ctx, unsigned state[]);
	void ShiftRows(PAES_context ctx, unsigned state[]);
	void ShiftRowsInv(PAES_context ctx, unsigned state[]);
	void MixColumns(PAES_context ctx, unsigned state[]);
	void MixColumnsInv(PAES_context ctx, unsigned state[]);
	
	/********************
	 *  �P���Ҧ�
	 ********************/
	void AddRoundKey(bool crypt, PAES_context ctx, unsigned state[], unsigned char roundKey);
	void SubBytes(bool crypt, PAES_context ctx, unsigned state[]);
	void ShiftRows(bool crypt, PAES_context ctx, unsigned state[]);
	void MixColumns(bool crypt, PAES_context ctx, unsigned state[]);
	
	#include "aesmd.h"
}

#endif
