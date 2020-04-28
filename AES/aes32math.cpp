#include "aes32math.h"

namespace math32{
	
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
	unsigned RotWord(unsigned shiftWord){
		return shiftWord << 8 ^ shiftWord >> 24;
	}
	
	//  �r�`���N
	unsigned SubWord(unsigned subWord){
		unsigned temp = 0;
		for(unsigned char i = 0; i < 4; i++){
			//  �N�r���ন�۹������줸�աA�ô��N�A��^�۹������r�ո�
		    temp ^= SubByte(word2byte(subWord, i)) << i * 8;
		}
	    return temp;
	}
	
	//  ���_�^�X�`�� 
	unsigned Rcon(unsigned char round){
		unsigned char Rc = 1;  //  ��l�Ȭ� 1 
		for(unsigned char i = 0; i < round; i++){
			Rc = Rc << 1 ^ ((Rc >> 7) * 0x1B);  //  �p�G���X�h���� 0x1B 
		}
		return Rc << 24;  //  ��^�r�`���A 
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
				temp = SubWord(RotWord(ctx->ExpandedKey.EKey32[i - 1])) ^ Rcon(i / N - 1);  //  �O���e�@�Ӧr�աA�åB������첾 1 �Ӧ줸�աA�A�줸�մ����A�M�Ყ�Ϊ��_�^�X�`��
			}else if(i % N == 0 && ctx->Nk == 8){
				temp = SubWord(ctx->ExpandedKey.EKey32[i - 1]);  //  �p�G���_�j�p�� 256 - bit�A�B�r�� ( i / 4 ) �� Nb �����ơA�h�O���e�@�Ӧr�աA�æ줸�մ��� 
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
		    temp_state[i] = SubByte(temp_state[i]);
		}
	}
	
	//  �f�줸�մ���
	void SubBytesInv(PAES_context ctx, unsigned state[]){
		unsigned char* temp_state = (unsigned char*)state;  //  �N�r�����ܦ��줸��
		for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
		    temp_state[i] = SubByteInv(temp_state[i]);
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
		for(unsigned char i = 0; i < ctx->Nb; i++) {
			state[i] = (xtime(2, word2byte(temp[i], 3)) ^ xtime(3, word2byte(temp[i], 2)) ^ xtime(1, word2byte(temp[i], 1)) ^ xtime(1, word2byte(temp[i], 0))) << 24;  //  2 3 1 1
			state[i] ^= (xtime(1, word2byte(temp[i], 3)) ^ xtime(2, word2byte(temp[i], 2)) ^ xtime(3, word2byte(temp[i], 1)) ^ xtime(1, word2byte(temp[i], 0))) << 16;  //  1 2 3 1
			state[i] ^= (xtime(1, word2byte(temp[i], 3)) ^ xtime(1, word2byte(temp[i], 2)) ^ xtime(2, word2byte(temp[i], 1)) ^ xtime(3, word2byte(temp[i], 0))) << 8;  //  1 1 2 3
			state[i] ^= xtime(3, word2byte(temp[i], 3)) ^ xtime(1, word2byte(temp[i], 2)) ^ xtime(1, word2byte(temp[i], 1)) ^ xtime(2, word2byte(temp[i], 0));  //  3 1 1 2
		}
	}
	
	//  �f�V�X�� 
	void MixColumnsInv(PAES_context ctx, unsigned state[]){
		unsigned temp[ctx->Nb];
		copyArray(temp, state, ctx->Nb);
		for(unsigned char i = 0; i < ctx->Nb; i++) {
			state[i] = (xtime(14, word2byte(temp[i], 3)) ^ xtime(11, word2byte(temp[i], 2)) ^ xtime(13, word2byte(temp[i], 1)) ^ xtime(9, word2byte(temp[i], 0))) << 24;  //  14 11 13 9
			state[i] ^= (xtime(9, word2byte(temp[i], 3)) ^ xtime(14, word2byte(temp[i], 2)) ^ xtime(11, word2byte(temp[i], 1)) ^ xtime(13, word2byte(temp[i], 0))) << 16;  //  9 14 11 13
			state[i] ^= (xtime(13, word2byte(temp[i], 3)) ^ xtime(9, word2byte(temp[i], 2)) ^ xtime(14, word2byte(temp[i], 1)) ^ xtime(11, word2byte(temp[i], 0))) << 8;  //  13 9 14 11
			state[i] ^= xtime(11, word2byte(temp[i], 3)) ^ xtime(13, word2byte(temp[i], 2)) ^ xtime(9, word2byte(temp[i], 1)) ^ xtime(14, word2byte(temp[i], 0));  //  11 13 9 14
		}
	}
	
	//  �P�� - �^�X���_�[�k
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
}
