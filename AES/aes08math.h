/*
 *     �ɦW�G aes08math.h
 *     ���W�G aes 08-bit mathematical operation
 *     �@�̡G Cerasus
 *     �y�z�G 
 */

#ifndef _AES08MATH_H_ 
#define _AES08MATH_H_

#include "aesemath.h"
#include "aesctx.h"

namespace math08{
	#undef wordEKey
	#undef LOOKUP
	
	
	/********************  �ŧi�禡  ********************/ 
	
	
	/********************
	 *  �}�C�B�z 
	 ********************/
	void xorArray(unsigned char dst[], unsigned char src[], unsigned char size);
	void copyArray(unsigned char dst[], unsigned char src[], unsigned char size);
	
	/********************
	 *  ���_�X�i
	 ********************/
	unsigned short RotByte(unsigned short shiftByte);
	unsigned char Rcon(unsigned char round);
	void KeyExpansion(PAES_context ctx);
	
	/********************
	 *  �@��Ҧ� 
	 ********************/
	void AddRoundKey(PAES_context ctx, unsigned char state[], unsigned char roundKey);
	void SubBytes(PAES_context ctx, unsigned char state[]);
	void SubBytesInv(PAES_context ctx, unsigned char state[]);
	void ShiftRows(PAES_context ctx, unsigned char state[]);
	void ShiftRowsInv(PAES_context ctx, unsigned char state[]);
	void MixColumns(PAES_context ctx, unsigned char state[]);
	void MixColumnsInv(PAES_context ctx, unsigned char state[]);
	
	/********************
	 *  �P���Ҧ�
	 ********************/
	void AddRoundKey(bool crypt, PAES_context ctx, unsigned char state[], unsigned char roundKey);
	void SubBytes(bool crypt, PAES_context ctx, unsigned char state[]);
	void ShiftRows(bool crypt, PAES_context ctx, unsigned char state[]);
	void MixColumns(bool crypt, PAES_context ctx, unsigned char state[]);
	
	#include "aesmd.h"
}

#endif
