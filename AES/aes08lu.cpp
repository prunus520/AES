#include "aes08lu.h"

namespace lu08{

	/********************  �w�q�禡  ********************/ 
	
	
	//  ���ΰ}�C
	void xorArray(unsigned char dst[], unsigned char src[], unsigned char size){
		for(unsigned char i = 0; i < size; i++){
			dst[i] ^= src[i];
		}
	}
	
	//  �ƻs�}�C
	void copyArray(unsigned char dst[], unsigned char src[], unsigned char size){
	    for (int i = 0; i < size; i++){
	        dst[i] = src[i];
	    }
	}
	
	//  ���_�X�i 
	void KeyExpansion(PAES_context ctx){
		unsigned short i = 0;
		unsigned char temp;
		unsigned char N = ctx->Nr - 6;  //  ���o max(Nk, Nb) 
		unsigned char RotWord[] = {3, 3, 3, 7};  //  �e���|�Ӫ��줸�� ( ���e���@�Ӧr�� )�A�å�����첾 1 �Ӧ줸�� 
		//  �ƻs�e Nk �����_
		while(i < 4 * ctx->Nk){
			ctx->ExpandedKey.EKey08[i] = ctx->key[i];
			i++;
		}
		//  ���᪺�^�X���_ 
		while(i < 4 * ctx->Nb * (1 + ctx->Nr)){
			if (i / 4 % N == 0){
				temp = S_Box[ctx->ExpandedKey.EKey08[i - RotWord[i % 4]]];  //  �O�����e���|�Ӫ��줸�� ( ���e���@�Ӧr�� ) �A�åB������첾 1 �Ӧ줸�աA�A�줸�մ��� 
				if(i % (4 * N) == 0){
					temp ^= Rcon[i / (4 * N) - 1];  //  ���p i �� ���_�j�p�����ơA�h���Ϊ��_�^�X�`�ơA�Ĥ@�^�� Rcon[0] �H������ 
				}
			}else if(i / 4 % N == 0 && ctx->Nk == 8){
				temp = S_Box[ctx->ExpandedKey.EKey08[i - 4]];  //  �p�G���_�j�p�� 256 - bit�A�B�r�� ( i / 4 ) �� Nb �����ơA�h�O�����e���|�Ӫ��줸�� ( ���e���@�Ӧr�� )�A�æ줸�մ��� 
			}else{
				temp = ctx->ExpandedKey.EKey08[i - 4];  //  �O�����e���|�Ӫ��줸�� ( ���e���@�Ӧr�� )
			}
			ctx->ExpandedKey.EKey08[i] = ctx->ExpandedKey.EKey08[i - 4 * N] ^ temp;  //  �N���e�� 4 * Nb �Ӫ��줸�� ( ���e�� Nb �Ӧr�� ) �P�O�����줸�ղ��ΡA�o��s���X�i���_ 
			i++;
		}
	}
	
	//  �^�X���_�[�k
	void AddRoundKey(PAES_context ctx, unsigned char state[], unsigned char roundKey){
		xorArray(state, ctx->ExpandedKey.EKey08 + 4 * ctx->Nb * roundKey, 4 * ctx->Nb);
	}
	
	//  �줸�մ���
	void SubBytes(PAES_context ctx, unsigned char state[]){
		for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
			state[i] = S_Box[state[i]];
		}
	}
	
	//  �f�줸�մ���
	void SubBytesInv(PAES_context ctx, unsigned char state[]){
		for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
			state[i] = S_Box_Inv[state[i]];
		}
	}
	
	//  �첾�C
	void ShiftRows(PAES_context ctx, unsigned char state[]){
		unsigned char temp[4 * ctx->Nb];
		copyArray(temp, state, 4 * ctx->Nb);
		if(ctx->blockSize == 128){
			//  �϶��� 128 - bit �� 
			for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
				state[i] = temp[Shift_Rows_128b[i]];
			}
		}else if(ctx->blockSize == 192){
			//  �϶��� 192 - bit ��
			for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
				state[i] = temp[Shift_Rows_192b[i]];
			}
		}else if(ctx->blockSize == 256){
			//  �϶��� 256 - bit ��
			for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
				state[i] = temp[Shift_Rows_256b[i]];
			}
		}
	}
	
	//  �f�첾�C
	void ShiftRowsInv(PAES_context ctx, unsigned char state[]){
		unsigned char temp[4 * ctx->Nb];
		copyArray(temp, state, 4 * ctx->Nb);
		if(ctx->blockSize == 128){
			//  �϶��� 128 - bit ��
			for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
				state[i] = temp[Shift_Rows_128b_Inv[i]];
			}
		}else if(ctx->blockSize == 192){
			//  �϶��� 192 - bit ��
			for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
				state[i] = temp[Shift_Rows_192b_Inv[i]];
			}
		}else if(ctx->blockSize == 256){
			//  �϶��� 256 - bit ��
			for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
				state[i] = temp[Shift_Rows_256b_Inv[i]];
			}
		}
	}
	
	//  �V�X�� 
	void MixColumns(PAES_context ctx, unsigned char state[]){
		unsigned char temp[4 * ctx->Nb];
		copyArray(temp, state, 4 * ctx->Nb); 
		for(unsigned char i = 0; i < 4 * ctx->Nb; i += 4) {
			state[i] = Multiply_2[temp[i]] ^ Multiply_3[temp[i + 1]] ^ temp[i + 2] ^ temp[i + 3];  //  2 3 1 1 
			state[i + 1] = temp[i] ^ Multiply_2[temp[i + 1]] ^ Multiply_3[temp[i + 2]] ^ temp[i + 3];  //  1 2 3 1
			state[i + 2] = temp[i] ^ temp[i + 1] ^ Multiply_2[temp[i + 2]] ^ Multiply_3[temp[i + 3]];  //  1 1 2 3
			state[i + 3] = Multiply_3[temp[i]] ^ temp[i + 1] ^ temp[i + 2] ^ Multiply_2[temp[i + 3]];  //  3 1 1 2 
		}
	}
	
	//  �f�V�X�� 
	void MixColumnsInv(PAES_context ctx, unsigned char state[]){
		unsigned char temp[4 * ctx->Nb];
		copyArray(temp, state, 4 * ctx->Nb);
		for(unsigned char i = 0; i < 4 * ctx->Nb; i += 4) {
			state[i] = Multiply_14[temp[i]] ^ Multiply_11[temp[i + 1]] ^ Multiply_13[temp[i + 2]] ^ Multiply_9[temp[i + 3]];  //  14 11 13 9 
			state[i + 1] = Multiply_9[temp[i]] ^ Multiply_14[temp[i + 1]] ^ Multiply_11[temp[i + 2]] ^ Multiply_13[temp[i + 3]];  //  9 14 11 13
			state[i + 2] = Multiply_13[temp[i]] ^ Multiply_9[temp[i + 1]] ^ Multiply_14[temp[i + 2]] ^ Multiply_11[temp[i + 3]];  //  13 9 14 11
			state[i + 3] = Multiply_11[temp[i]] ^ Multiply_13[temp[i + 1]] ^ Multiply_9[temp[i + 2]] ^ Multiply_14[temp[i + 3]];  //  11 13 9 14
		}
	}
	
	//  ( ��X ) �P�� - �^�X���_�[�k
	void AddRoundKey(bool crypt, PAES_context ctx, unsigned char state[], unsigned char roundKey){
		if(crypt){
			AddRoundKey(ctx, state, roundKey);
		}else{
			unsigned short offset = 4 * ctx->Nb * (ctx->Nr - roundKey);  //  �ѱK�ɥ� Nr ~ 0 
			if(roundKey == ctx->Nr || roundKey == 0){  //  �̪�P�̫�^�X���ݭn�f�V�X�� 
				xorArray(state, ctx->ExpandedKey.EKey08 + offset, 4 * ctx->Nb);
			}else{
				unsigned char temp[4 * ctx->Nb];
				copyArray(temp, ctx->ExpandedKey.EKey08 + offset, 4 * ctx->Nb);  //  ������_�X�i�Q��� 
				MixColumnsInv(ctx, temp);  //  �P���ݭn�A�f�V�X�� 
				xorArray(state, temp, 4 * ctx->Nb);
			}
		}
	}
	
	//  �P�� - �줸�մ���
	void SubBytes(bool crypt, PAES_context ctx, unsigned char state[]){
		if(crypt){
			SubBytes(ctx, state);
		}else{
			SubBytesInv(ctx, state);
		}
	}
	
	//  �P�� - �첾�C
	void ShiftRows(bool crypt, PAES_context ctx, unsigned char state[]){
		if(crypt){
			ShiftRows(ctx, state);
		}else{
			ShiftRowsInv(ctx, state);
		}
	}
	
	//  �P�� - �V�X�� 
	void MixColumns(bool crypt, PAES_context ctx, unsigned char state[]){
		if(crypt){
			MixColumns(ctx, state);
		}else{
			MixColumnsInv(ctx, state);
		}
	}
	
	//  ��X�P�� - �줸�մ���
	void I_SubBytes(bool crypt, PAES_context ctx, unsigned char state[]){
		if(crypt){
			ShiftRows(ctx, state);
			SubBytes(ctx, state);
		}else{
			ShiftRowsInv(ctx, state);
			SubBytesInv(ctx, state);
		}
	}
	
	//  ��X�P�� - �V�X�� 
	void I_MixColumns(bool crypt, PAES_context ctx, unsigned char state[]){
		if(crypt){
			ShiftRows(ctx, state);
			unsigned char temp[4 * ctx->Nb];
			copyArray(temp, state, 4 * ctx->Nb);
			for(unsigned char i = 0; i < 4 * ctx->Nb; i += 4) {
				state[i] = I_Multiply_2[temp[i]] ^ I_Multiply_3[temp[i + 1]] ^ S_Box[temp[i + 2]] ^ S_Box[temp[i + 3]];  //  2 3 1 1
				state[i + 1] = S_Box[temp[i]] ^ I_Multiply_2[temp[i + 1]] ^ I_Multiply_3[temp[i + 2]] ^ S_Box[temp[i + 3]];  //  1 2 3 1
				state[i + 2] = S_Box[temp[i]] ^ S_Box[temp[i + 1]] ^ I_Multiply_2[temp[i + 2]] ^ I_Multiply_3[temp[i + 3]];  //  1 1 2 3
				state[i + 3] = I_Multiply_3[temp[i]] ^ S_Box[temp[i + 1]] ^ S_Box[temp[i +2]] ^ I_Multiply_2[temp[i + 3]];  // 3 1 1 2
			}
		}else{
			ShiftRowsInv(ctx, state);
			unsigned char temp[4 * ctx->Nb];
			copyArray(temp, state, 4 * ctx->Nb);
			for(unsigned char i = 0; i < 4 * ctx->Nb; i += 4) {
				state[i] = I_Multiply_14[temp[i]] ^ I_Multiply_11[temp[i + 1]] ^ I_Multiply_13[temp[i + 2]] ^ I_Multiply_9[temp[i + 3]];  //  14 11 13 9
				state[i + 1] = I_Multiply_9[temp[i]] ^ I_Multiply_14[temp[i + 1]] ^ I_Multiply_11[temp[i + 2]] ^ I_Multiply_13[temp[i + 3]];  //  9 14 11 13
				state[i + 2] = I_Multiply_13[temp[i]] ^ I_Multiply_9[temp[i + 1]] ^ I_Multiply_14[temp[i + 2]] ^ I_Multiply_11[temp[i + 3]];  //  13 9 14 11
				state[i + 3] = I_Multiply_11[temp[i]] ^ I_Multiply_13[temp[i + 1]] ^ I_Multiply_9[temp[i + 2]] ^ I_Multiply_14[temp[i + 3]];  //  11 13 9 14
			}
		}
	}
}
