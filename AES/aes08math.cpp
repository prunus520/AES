#include "aes08math.h"

namespace math08{
		
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
	
	//  �e���|�Ӫ��줸�� ( ���e���@�Ӧr�� )�A�å�����첾 1 �Ӧ줸�աA�A�^�ǭp��᪺��m 
	unsigned short RotByte(unsigned short shiftByte){
		return shiftByte / 4 * 4 - 4 + (shiftByte + 1) % 4;
	}
	
	//  ���_�^�X�`�� 
	unsigned char Rcon(unsigned char round){
		unsigned char Rc = 1;  //  ��l�Ȭ� 1 
		for(unsigned char i = 0; i < round; i++){
			Rc = Rc << 1 ^ ((Rc >> 7) * 0x1B);  //  �p�G���X�h���� 0x1B 
		}
		return Rc;
	}
	
	//  ���_�X�i 
	void KeyExpansion(PAES_context ctx){
		unsigned short i = 0;
		unsigned char temp;
		unsigned char N = ctx->Nr - 6;  //  ���o max(Nk, Nb) 
		//  �ƻs�e Nk �����_
		while(i < 4 * ctx->Nk){
			ctx->ExpandedKey.EKey08[i] = ctx->key[i];
			i++;
		}
		//  ���᪺�^�X���_ 
		while(i < 4 * ctx->Nb * (1 + ctx->Nr)){
			if (i / 4 % N == 0){
				temp = SubByte(ctx->ExpandedKey.EKey08[RotByte(i)]);  //  �O�����e���|�Ӫ��줸�� ( ���e���@�Ӧr�� ) �A�åB������첾 1 �Ӧ줸�աA�A�줸�մ��� 
				if(i % (4 * N) == 0){
					temp ^= Rcon(i / (4 * N) - 1);  //  ���p i �� ���_�j�p�����ơA�h���Ϊ��_�^�X�`�ơA�Ĥ@�^�� Rcon[0] �H������ 
				}
			}else if(i / 4 % N == 0 && ctx->Nk == 8){
				temp = SubByte(ctx->ExpandedKey.EKey08[i - 4]);  //  �p�G���_�j�p�� 256 - bit�A�B�r�� ( i / 4 ) �� Nb �����ơA�h�O�����e���|�Ӫ��줸�� ( ���e���@�Ӧr�� )�A�æ줸�մ��� 
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
			state[i] = SubByte(state[i]);
		}
	}
	
	//  �f�줸�մ���
	void SubBytesInv(PAES_context ctx, unsigned char state[]){
		for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
			state[i] = SubByteInv(state[i]);
		}
	}
	
	//  �첾�C
	void ShiftRows(PAES_context ctx, unsigned char state[]){
		unsigned char temp[4 * ctx->Nb];
		copyArray(temp, state, 4 * ctx->Nb);
		for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
			//  ���o�C�@�C�۹�������m�A�å����A�B���W�L Nb�A�W�L�h�q 0 �}�l 
			state[i] = temp[(i + i % 4 * 4) % (4 * ctx->Nb)]; 
		}
	}
	
	//  �f�첾�C
	void ShiftRowsInv(PAES_context ctx, unsigned char state[]){
		unsigned char temp[4 * ctx->Nb];
		copyArray(temp, state, 4 * ctx->Nb);
		for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
			//  ���o�C�@�C�۹�������m�A�åk���A�B���p�� 0�A�W�L�h�q Nb �}�l
			state[i] = temp[(4 * ctx->Nb + i - i % 4 * 4) % (4 * ctx->Nb)];
		}
	}
	
	//  �V�X�� 
	void MixColumns(PAES_context ctx, unsigned char state[]){
		unsigned char temp[4 * ctx->Nb];
		copyArray(temp, state, 4 * ctx->Nb);
		for(unsigned char i = 0; i < 4 * ctx->Nb; i += 4) {
			for(unsigned char j = 0; j < 4; j++){
				//  �C��H���P�{�ש��W���� N �줸�� ( 0 1 2 3 ( 0 ) -> 1 2 3 0 ( 1 ) -> 2 3 1 0 ( 2 ) �`���U�h ) 
				//  �h������ 2 3 1 1 
				state[i + j] = xtime(2, temp[i + j]) ^ xtime(3, temp[i + (j + 1) % 4]) ^ xtime(1, temp[i + (j + 2) % 4]) ^ xtime(1, temp[i + (j + 3) % 4]);
			}
		}
	}
	
	//  �f�V�X�� 
	void MixColumnsInv(PAES_context ctx, unsigned char state[]){
		unsigned char temp[4 * ctx->Nb];
		copyArray(temp, state, 4 * ctx->Nb);
		for(unsigned char i = 0; i < 4 * ctx->Nb; i += 4) {
			for(unsigned char j = 0; j < 4; j++){
				//  �C��H���P�{�ש��W���� N �줸�� ( 0 1 2 3 ( 0 ) -> 1 2 3 0 ( 1 ) -> 2 3 1 0 ( 2 ) �`���U�h ) 
				//  �h������ 14 11 13 9 
				state[i + j] = xtime(14, temp[i + j]) ^ xtime(11, temp[i + (j + 1) % 4]) ^ xtime(13, temp[i + (j + 2) % 4]) ^ xtime(9, temp[i + (j + 3) % 4]);
			}
		}
	}
	
	//  �P�� - �^�X���_�[�k
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
}
