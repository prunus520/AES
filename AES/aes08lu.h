/*
 *     �ɦW�G aes08lu.h
 *     ���W�G aes 08-bit lookup operation
 *     �@�̡G Cerasus
 *     �y�z�G 
 */

#ifndef _AES08LU_H_
#define _AES08LU_H_

#include "aesctx.h"

namespace lu08{
	#undef wordEKey
	#undef LOOKUP
	#define LOOKUP
	#include "aes08lut.h"
	
	
	/********************  �ŧi�禡  ********************/ 
	
	
	/********************
	 *  �}�C�B�z 
	 ********************/
	void xorArray(unsigned char dst[], unsigned char src[], unsigned char size);
	void copyArray(unsigned char dst[], unsigned char src[], unsigned char size);
	
	/********************
	 *  ���_�X�i
	 ********************/
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
	
	/********************
	 *  ��X�P���Ҧ�
	 ********************/
	void I_SubBytes(bool crypt, PAES_context ctx, unsigned char state[]);
	void I_MixColumns(bool crypt, PAES_context ctx, unsigned char state[]);
	
	#include "aesmd.h"
}

#endif
