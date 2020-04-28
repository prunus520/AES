#include "aes32lu.h"

namespace lu32{
	
	/********************  �w�q�禡  ********************/ 
	
	
	//  ���ΰ}�C 32 - bit ^ 08 - bit
	void xorArray(unsigned dst[], unsigned char src[], unsigned char size){
		unsigned temp_src[size / 4] = {0};
		copyArray(temp_src, src, size);
		for(unsigned char i = 0; i < size / 4; i++){
			dst[i] ^= temp_src[i];
		}
	}
	
	//  ���ΰ}�C 32 - bit ^ 32 - bit
	void xorArray(unsigned dst[], unsigned src[], unsigned char size){
		for(unsigned char i = 0; i < size; i++){
			dst[i] ^= src[i];
		}
	}
	
	//  �ƻs�}�C 08 - bit <- 08 - bit
	void copyArray(unsigned char dst[], unsigned char src[], unsigned char size){
	    for (int i = 0; i < size; i++){
	        dst[i] = src[i];
	    }
	}
	
	//  �ƻs�}�C 08 - bit <- 32 - bit ( �����ƻs�|�A�ˡA�]�N�O�ѳ̧C�줸�թ��̰��줸�սƻs ) 
	void copyArray(unsigned char dst[], unsigned src[], unsigned char size){
		char* temp_src = (char*)src;  //  �C 1 �Ӧr�թ 4 �Ӧ줸��
	    for(int i = 0; i < size; i++){
	        dst[i] = temp_src[i + 3 - 2 * (i % 4)];  //  �ѳ̰��줸�թ��C�줸�սƻs 
	    }
	}
	
	//  �ƻs�}�C 32 - bit <- 08 - bit ( �����ƻs�|�A�ˡA�]�N�O�ѳ̧C�줸�թ��̰��줸�սƻs ) 
	void copyArray(unsigned dst[], unsigned char src[], unsigned char size){
		char* temp_dst = (char*)dst;  //  �C 1 �Ӧr�թ 4 �Ӧ줸��
	    for(int i = 0; i < size; i++){
	        temp_dst[i] = src[i + 3 - 2 * (i % 4)];  //  �ѳ̰��줸�թ��C�줸�սƻs 
	    }
	}
	
	//  �ƻs�}�C 32 - bit <- 32 - bit
	void copyArray(unsigned dst[], unsigned src[], unsigned char size){
	    for(int i = 0; i < size; i++){
	        dst[i] = src[i];
	    }
	}
	
	//  �N�r�`�ഫ���۹������줸�� 
	unsigned char word2byte(unsigned state, unsigned char offset){
		return state >> offset * 8;
	}
	
	//  ������첾 1 �Ӧ줸��
	unsigned RotWord(unsigned shiftByte){
		return shiftByte << 8 ^ shiftByte >> 24;
	}
	
	//  �r�`���N 
	unsigned SubWord(const unsigned box[], unsigned subWord){
		//  �N�r���ܦ� 4 �Ӭ۹������줸�աA�ô��N�A��^�۹������r�ո� 
		return box[word2byte(subWord, 3)] & 0xFF000000 ^ box[word2byte(subWord, 2)] & 0x00FF0000 ^ box[word2byte(subWord, 1)] & 0x0000FF00 ^ box[word2byte(subWord, 0)] & 0x000000FF;
	}
	
	//  ���_�X�i 
	void KeyExpansion(PAES_context ctx){
		unsigned char i = 0;
		unsigned temp;
		unsigned char N = ctx->Nr - 6;  //  ���o max(Nk, Nb) 
		//  �ƻs�e Nk �����_
		while(i < ctx->Nk){
			copyArray(ctx->ExpandedKey.EKey32 + i, ctx->key + i * 4, 4);
			i++;
		}
		//  ���᪺�^�X���_ 
		while(i < ctx->Nb * (1 + ctx->Nr)){
			if (i % N == 0){
				temp = SubWord(S_Box, RotWord(ctx->ExpandedKey.EKey32[i - 1])) ^ Rcon[i / N - 1];  //  �O���e�@�Ӧr�աA�åB������첾 1 �Ӧ줸�աA�A�줸�մ����A�M�Ყ�Ϊ��_�^�X�`�� 
			}else if(i % N == 0 && ctx->Nk == 8){
				temp = SubWord(S_Box, ctx->ExpandedKey.EKey32[i - 1]);  //  �p�G���_�j�p�� 256 - bit�A�B�r�� ( i / 4 ) �� Nb �����ơA�h�O���e�@�Ӧr�աA�æ줸�մ��� 
			}else{
				temp = ctx->ExpandedKey.EKey32[i - 1];  //  �O���e�@�Ӧr��
			}
			ctx->ExpandedKey.EKey32[i] = ctx->ExpandedKey.EKey32[i - N] ^ temp;  //  �N�e Nb �Ӧr�ջP�O�����r�ղ��ΡA�o��s���X�i���_
			i++;
		}
	}
	
	//  �^�X���_�[�k
	void AddRoundKey(PAES_context ctx, unsigned state[], unsigned char roundKey){
		xorArray(state, ctx->ExpandedKey.EKey32 + ctx->Nb * roundKey, ctx->Nb);
	}
	
	//  �줸�մ���
	void SubBytes(PAES_context ctx, unsigned state[]){
		unsigned char* temp_state = (unsigned char*)state;  //  �N�r�����ܦ��줸�� 
		for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
			temp_state[i] = S_Box[temp_state[i]];
		}
	}
	
	//  �f�줸�մ���
	void SubBytesInv(PAES_context ctx, unsigned state[]){
		unsigned char* temp_state = (unsigned char*)state;  //  �N�r�����ܦ��줸��
		for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
			temp_state[i] = S_Box_Inv[temp_state[i]];
		}
	}
	
	//  �첾�C
	void ShiftRows(PAES_context ctx, unsigned state[]){
		unsigned temp[ctx->Nb];
		copyArray(temp, state, ctx->Nb);
		for(unsigned char i = 0; i < ctx->Nb; i++){
			//  ���o�C�@�C�۹�������m�A�å����A�B���W�L Nb�A�W�L�h�q 0 �}�l 
			state[i] = temp[i] & 0xFF000000 ^ temp[(i + 1) % ctx->Nb] & 0x00FF0000 ^ temp[(i + 2) % ctx->Nb] & 0x0000FF00 ^ temp[(i + 3) % ctx->Nb] & 0x000000FF;
		}
	}
	
	//  �f�첾�C
	void ShiftRowsInv(PAES_context ctx, unsigned state[]){
		unsigned temp[ctx->Nb];
		copyArray(temp, state, ctx->Nb);
		for(unsigned char i = 0; i < ctx->Nb; i++){
			//  ���o�C�@�C�۹�������m�A�åk���A�B���p�� 0�A�W�L�h�q Nb �}�l
			state[i] = temp[i] & 0xFF000000 ^ temp[(i + ctx->Nb - 1) % ctx->Nb] & 0x00FF0000 ^ temp[(i + ctx->Nb - 2) % ctx->Nb] & 0x0000FF00 ^ temp[(i + ctx->Nb - 3) % ctx->Nb] & 0x000000FF;
		}
	}
	
	//  �V�X�� 
	void MixColumns(PAES_context ctx, unsigned state[]){
		unsigned temp[ctx->Nb];
		copyArray(temp, state, ctx->Nb);
		for(unsigned char i = 0; i < ctx->Nb; i++){
			state[i] = (WE0[word2byte(temp[i], 3)] ^ WE1[word2byte(temp[i], 2)] ^ WE2[word2byte(temp[i], 1)] ^ WE3[word2byte(temp[i], 0)]) & 0xFF000000;  //  2 3 1 1
			state[i] ^= (WE0[word2byte(temp[i], 3)] ^ WE1[word2byte(temp[i], 2)] ^ WE2[word2byte(temp[i], 1)] ^ WE3[word2byte(temp[i], 0)]) & 0x00FF0000;  //  1 2 3 1
			state[i] ^= (WE0[word2byte(temp[i], 3)] ^ WE1[word2byte(temp[i], 2)] ^ WE2[word2byte(temp[i], 1)] ^ WE3[word2byte(temp[i], 0)]) & 0x0000FF00;  //  1 1 2 3
			state[i] ^= (WE0[word2byte(temp[i], 3)] ^ WE1[word2byte(temp[i], 2)] ^ WE2[word2byte(temp[i], 1)] ^ WE3[word2byte(temp[i], 0)]) & 0x000000FF;  //  3 1 1 2
		}
	}
	
	//  �f�V�X�� 
	void MixColumnsInv(PAES_context ctx, unsigned state[]){
		unsigned temp[ctx->Nb];
		copyArray(temp, state, ctx->Nb);
		for(unsigned char i = 0; i < ctx->Nb; i++){
			state[i] = (WD0[word2byte(temp[i], 3)] ^ WD1[word2byte(temp[i], 2)] ^ WD2[word2byte(temp[i], 1)] ^ WD3[word2byte(temp[i], 0)]) & 0xFF000000;  //  14 11 13 9
			state[i] ^= (WD0[word2byte(temp[i], 3)] ^ WD1[word2byte(temp[i], 2)] ^ WD2[word2byte(temp[i], 1)] ^ WD3[word2byte(temp[i], 0)]) & 0x00FF0000;  //  9 14 11 13 
			state[i] ^= (WD0[word2byte(temp[i], 3)] ^ WD1[word2byte(temp[i], 2)] ^ WD2[word2byte(temp[i], 1)] ^ WD3[word2byte(temp[i], 0)]) & 0x0000FF00;  //  13 9 14 11 
			state[i] ^= (WD0[word2byte(temp[i], 3)] ^ WD1[word2byte(temp[i], 2)] ^ WD2[word2byte(temp[i], 1)] ^ WD3[word2byte(temp[i], 0)]) & 0x000000FF;  //  11 13 9 14
		}
	}
	
	//  ( ��X ) �P�� - �^�X���_�[�k
	void AddRoundKey(bool crypt, PAES_context ctx, unsigned state[], unsigned char roundKey){
		if(crypt){
			AddRoundKey(ctx, state, roundKey);
		}else{
			unsigned short offset = ctx->Nb * (ctx->Nr - roundKey);  //  �ѱK�ɥ� Nr ~ 0 
			if(roundKey == ctx->Nr || roundKey == 0){  //  �̪�P�̫�^�X���ݭn�f�V�X�� 
				xorArray(state, ctx->ExpandedKey.EKey32 + offset, ctx->Nb);
			}else{
				unsigned temp[ctx->Nb];
				copyArray(temp, ctx->ExpandedKey.EKey32 + offset, ctx->Nb);  //  ������_�X�i�Q��� 
				MixColumnsInv(ctx, temp);  //  �P���ݭn�A�f�V�X�� 
				xorArray(state, temp, ctx->Nb);
			}
		}
	}
	
	//  �P�� - �줸�մ���
	void SubBytes(bool crypt, PAES_context ctx, unsigned state[]){
		if(crypt){
			SubBytes(ctx, state);
		}else{
			SubBytesInv(ctx, state);
		}
	}
	
	//  �P�� - �첾�C
	void ShiftRows(bool crypt, PAES_context ctx, unsigned state[]){
		if(crypt){
			ShiftRows(ctx, state);
		}else{
			ShiftRowsInv(ctx, state);
		}
	}
	
	//  �P�� - �V�X�� 
	void MixColumns(bool crypt, PAES_context ctx, unsigned state[]){
		if(crypt){
			MixColumns(ctx, state);
		}else{
			MixColumnsInv(ctx, state);
		}
	}
	
	//  ��X�P�� - �줸�մ���
	void I_SubBytes(bool crypt, PAES_context ctx, unsigned state[]){
		if(crypt){
			ShiftRows(ctx, state);
			SubBytes(ctx, state);
		}else{
			ShiftRowsInv(ctx, state);
			SubBytesInv(ctx, state);
		}
	}
	
	//  ��X�P�� - �V�X�� 
	void I_MixColumns(bool crypt, PAES_context ctx, unsigned state[]){
		if(crypt){
			ShiftRows(ctx, state);
			unsigned temp[ctx->Nb];
			copyArray(temp, state, ctx->Nb);
			for(unsigned char i = 0; i < ctx->Nb; i++){
				state[i] = (IE0[word2byte(temp[i], 3)] ^ IE1[word2byte(temp[i], 2)] ^ IE2[word2byte(temp[i], 1)] ^ IE3[word2byte(temp[i], 0)]) & 0xFF000000;  //  2 3 1 1
				state[i] ^= (IE0[word2byte(temp[i], 3)] ^ IE1[word2byte(temp[i], 2)] ^ IE2[word2byte(temp[i], 1)] ^ IE3[word2byte(temp[i], 0)]) & 0x00FF0000;  //  1 2 3 1
				state[i] ^= (IE0[word2byte(temp[i], 3)] ^ IE1[word2byte(temp[i], 2)] ^ IE2[word2byte(temp[i], 1)] ^ IE3[word2byte(temp[i], 0)]) & 0x0000FF00;  //  1 1 2 3
				state[i] ^= (IE0[word2byte(temp[i], 3)] ^ IE1[word2byte(temp[i], 2)] ^ IE2[word2byte(temp[i], 1)] ^ IE3[word2byte(temp[i], 0)]) & 0x000000FF;  // 3 1 1 2
			}
		}else{
			ShiftRowsInv(ctx, state);
			unsigned temp[ctx->Nb];
			copyArray(temp, state, ctx->Nb);
			for(unsigned char i = 0; i < ctx->Nb; i++){
				state[i] = (ID0[word2byte(temp[i], 3)] ^ ID1[word2byte(temp[i], 2)] ^ ID2[word2byte(temp[i], 1)] ^ ID3[word2byte(temp[i], 0)]) & 0xFF000000;  //  14 11 13 9
				state[i] ^= (ID0[word2byte(temp[i], 3)] ^ ID1[word2byte(temp[i], 2)] ^ ID2[word2byte(temp[i], 1)] ^ ID3[word2byte(temp[i], 0)]) & 0x00FF0000;  //  9 14 11 13 
				state[i] ^= (ID0[word2byte(temp[i], 3)] ^ ID1[word2byte(temp[i], 2)] ^ ID2[word2byte(temp[i], 1)] ^ ID3[word2byte(temp[i], 0)]) & 0x0000FF00;  //  13 9 14 11 
				state[i] ^= (ID0[word2byte(temp[i], 3)] ^ ID1[word2byte(temp[i], 2)] ^ ID2[word2byte(temp[i], 1)] ^ ID3[word2byte(temp[i], 0)]) & 0x000000FF;  //  11 13 9 14 
			}
		}
	}
}
