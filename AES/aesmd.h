/*
 *     �ɦW�G aesmd.h
 *     ���W�G aes mode
 *     �@�̡G Cerasus
 *     �y�z�G 
 */

/********************
 *  �w�q�[�K�� 1�A�ѱK�� 0 
 ********************/ 

#define encrypt 1
#define decrypt 0

//  �P�_�O�_���r�`�����X�i���_ 
#ifndef wordEKey
	typedef unsigned char uXXb;
#else
	typedef unsigned uXXb;
#endif 


/********************  �ŧi�禡  ********************/ 


/********************
 *  �[�ѱK���� 
 ********************/
static void Encrypt(PAES_context ctx, uXXb state[]);
static void Decrypt(PAES_context ctx, uXXb state[]);
static void Crypt(PAES_context ctx, uXXb state[], bool crypt);
#ifdef LOOKUP
	static void I_Crypt(PAES_context ctx, uXXb state[], bool crypt);
#endif 

/********************
 *  AES - ECB �Ҧ�
 ********************/
static void ECB(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt);
static void ECB_Encrypt(PAES_context ctx);
static void ECB_Decrypt(PAES_context ctx);
static void ECB_Crypt(PAES_context ctx, bool crypt);
#ifdef LOOKUP
	static void ECB_I_Crypt(PAES_context ctx, bool crypt);
#endif 

/********************
 *  AES - CBC �Ҧ�
 ********************/
static void CBC(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt);
static void CBC_Encrypt(PAES_context ctx);
static void CBC_Decrypt(PAES_context ctx);
static void CBC_Crypt(PAES_context ctx, bool crypt);
#ifdef LOOKUP
	static void CBC_I_Crypt(PAES_context ctx, bool crypt);
#endif 

/********************
 *  AES - CTR �Ҧ�
 ********************/
static void CTR(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt);
static void CTR_Encrypt(PAES_context ctx);
static void CTR_Decrypt(PAES_context ctx);
static void CTR_Crypt(PAES_context ctx, bool crypt);
#ifdef LOOKUP
	static void CTR_I_Crypt(PAES_context ctx, bool crypt);
#endif 

/********************
 *  AES - OFB �Ҧ�
 ********************/
static void OFB(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt);
static void OFB_Encrypt(PAES_context ctx);
static void OFB_Decrypt(PAES_context ctx);
static void OFB_Crypt(PAES_context ctx, bool crypt);
#ifdef LOOKUP
	static void OFB_I_Crypt(PAES_context ctx, bool crypt);
#endif 

/********************
 *  AES - CFB �Ҧ�
 ********************/
static void CFB(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt);
static void CFB_Encrypt(PAES_context ctx);
static void CFB_Decrypt(PAES_context ctx);
static void CFB_Crypt(PAES_context ctx, bool crypt);
#ifdef LOOKUP
	static void CFB_I_Crypt(PAES_context ctx, bool crypt);
#endif 

/********************
 *  IV�����q 
 ********************/ 
static void ivOffset(PAES_context ctx, uXXb iv[], uXXb padding[], unsigned short offset);

/********************  �w�q�禡  ********************/ 


//  �@��[�K 
void Encrypt(PAES_context ctx, uXXb state[]){
	AddRoundKey(ctx, state, 0);
	for(unsigned char i = 1; i < ctx->Nr; i++){
		if(rearKeyEvent()) return;  //  �����{��
		SubBytes(ctx, state);
		ShiftRows(ctx, state);
		MixColumns(ctx, state);
		AddRoundKey(ctx, state, i);
	}
	SubBytes(ctx, state);
	ShiftRows(ctx, state);
	AddRoundKey(ctx, state, ctx->Nr);
}

//  �@��ѱK 
void Decrypt(PAES_context ctx, uXXb state[]){
	AddRoundKey(ctx, state, ctx->Nr);
	ShiftRowsInv(ctx, state);
	SubBytesInv(ctx, state);
	for(unsigned char i = ctx->Nr - 1; i > 0; i--){
		if(rearKeyEvent()) return;  //  �����{��
		AddRoundKey(ctx, state, i);
		MixColumnsInv(ctx, state);
		ShiftRowsInv(ctx, state);
		SubBytesInv(ctx, state);
	}
	AddRoundKey(ctx, state, 0);
}

//  �P���[�ѱK 
void Crypt(PAES_context ctx, uXXb state[], bool crypt){
	for(unsigned char i = 0; i < ctx->Nr - 1; i++){
		if(rearKeyEvent()) return;  //  �����{��
		AddRoundKey(crypt, ctx, state, i);
		SubBytes(crypt, ctx, state);
		ShiftRows(crypt, ctx, state);
		MixColumns(crypt, ctx, state);
	}
	AddRoundKey(crypt, ctx, state, ctx->Nr - 1);
	SubBytes(crypt, ctx, state);
	ShiftRows(crypt, ctx, state);
	AddRoundKey(crypt, ctx, state, ctx->Nr);
}

#ifdef LOOKUP
	//  ��X�P���[�ѱK 
	void I_Crypt(PAES_context ctx, uXXb state[], bool crypt){
		for(unsigned char i = 0; i < ctx->Nr - 1; i++){
			if(rearKeyEvent()) return;  //  �����{��
			AddRoundKey(crypt, ctx, state, i);
			I_MixColumns(crypt, ctx, state);  //  �]�t SubBytes�B ShiftRows
		}
		AddRoundKey(crypt, ctx, state, ctx->Nr - 1);
		I_SubBytes(crypt, ctx, state);  // �]�t ShiftRows 
		AddRoundKey(crypt, ctx, state, ctx->Nr);
	}
#endif

//  ECB �ҫ��A�e�禡���@��ΡA��禡���P���� 
void ECB(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt){
	uXXb state[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  �� 
	for(long dataBlock = 0; dataBlock < ctx->dataLength; dataBlock += ctx->dataOffset){  //  ��ƶ����� 
		copyArray(state, ctx->data + dataBlock, 4 * ctx->Nb);  //  ��ư϶��ƻs���� 
		//  �p�G��ӳ���A�h�P���� 
		if(i_crypt != NULL){
			(*i_crypt)(ctx, state, crypt);
		}else{
			(*en_de)(ctx, state);
		}
		if(keyFlag) return;
		copyArray(ctx->data + dataBlock, state, 4 * ctx->Nb);  //  ��ƻs���ư϶� 
		perbar(dataBlock, ctx->dataLength, crypt);
	}
	perbar(ctx->dataLength, ctx->dataLength, crypt);
}

void ECB_Encrypt(PAES_context ctx){
	ECB(ctx, &Encrypt, NULL, encrypt); 
}

void ECB_Decrypt(PAES_context ctx){
	ECB(ctx, &Decrypt, NULL, decrypt);
}

void ECB_Crypt(PAES_context ctx, bool crypt){
	ECB(ctx, NULL, &Crypt, crypt);
}

#ifdef LOOKUP
	void ECB_I_Crypt(PAES_context ctx, bool crypt){
		ECB(ctx, NULL, &I_Crypt, crypt);
	}
#endif

//  CBC �ҫ��A�e�禡���@��ΡA��禡���P���� 
void CBC(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt){
	uXXb state[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  ��
	uXXb iv[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  �N iv ��줣�P���A
	uXXb temp[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  �ѱK�Ȧs�� 
	copyArray(iv, ctx->iv, 4 * ctx->Nb);  //  �N iv ��줣�P���A
	for(long dataBlock = 0; dataBlock < ctx->dataLength; dataBlock += ctx->dataOffset){  //  ��ƶ����� 
		copyArray(state, ctx->data + dataBlock, 4 * ctx->Nb);  //  ��ư϶��ƻs���� 
		if(crypt){
			xorArray(state, iv, 4 * ctx->Nb / sizeof(uXXb));  //  ���� iv
		}else{
			copyArray(temp, state, 4 * ctx->Nb / sizeof(uXXb));  //  ��ƻs��Ȧs 
		}
		//  �p�G��ӳ���A�h�P�����A������ܦ^�� 
		if(i_crypt != NULL){
			(*i_crypt)(ctx, state, crypt);
		}else{
			if(en_de != NULL){
				(*en_de)(ctx, state);
			}else{
				return;
			}
		}
		if(keyFlag) return;
		if(crypt){
			copyArray(iv, state, 4 * ctx->Nb / sizeof(uXXb));  //  �ƻs��� iv 
		}else{
			xorArray(state, iv, 4 * ctx->Nb / sizeof(uXXb));  //  ��P iv ���� 
			copyArray(iv, temp, 4 * ctx->Nb / sizeof(uXXb));  //  �N��ƻs�� iv 
		}
		copyArray(ctx->data + dataBlock, state, 4 * ctx->Nb);  //  ��ƻs���ư϶�
		perbar(dataBlock, ctx->dataLength, crypt);
	}
	perbar(ctx->dataLength, ctx->dataLength, crypt);
}

void CBC_Encrypt(PAES_context ctx){
	CBC(ctx, &Encrypt, NULL, encrypt);
}

void CBC_Decrypt(PAES_context ctx){
	CBC(ctx, &Decrypt, NULL, decrypt);
}

void CBC_Crypt(PAES_context ctx, bool crypt){
	CBC(ctx, NULL, &Crypt, crypt);
}

#ifdef LOOKUP
	void CBC_I_Crypt(PAES_context ctx, bool crypt){
		CBC(ctx, NULL, &I_Crypt, crypt);
	}
#endif

//  CTR �ҫ��A�e�禡���@��ΡA��禡���P���� 
void CTR(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt){
	uXXb state[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  ��
	uXXb iv[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  �N iv ��줣�P���A
	uXXb temp[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  �Ȧs��ư϶� 
	copyArray(iv, ctx->iv, 4 * ctx->Nb);  //  �N iv ��줣�P���A
	for(long dataBlock = 0; dataBlock < ctx->dataLength; dataBlock += ctx->dataOffset){  //  ��ƶ����� 
		copyArray(state, iv, 4 * ctx->Nb / sizeof(uXXb));  //  ��ư϶��ƻs���� 
		//  �p�G��ӳ���A�h�P�����A������ܦ^�� 
		if(i_crypt != NULL){
			(*i_crypt)(ctx, state, encrypt);
		}else{
			if(en_de != NULL){
				(*en_de)(ctx, state);
			}else{
				return;
			}
		}
		if(keyFlag) return;
		copyArray(temp, ctx->data + dataBlock, 4 * ctx->Nb);  //  ��ư϶��ƻs��Ȧs 
		xorArray(state, temp, 4 * ctx->Nb / sizeof(uXXb));  //  �鲧�θ�ư϶� 
		copyArray(ctx->data + dataBlock, state, 4 * ctx->Nb);  //  ��ƻs���ư϶�
		//  �ݤ@�@���X�Ӥp�϶��A�q�p���϶��p�ƨ�j���϶��A�p�G�j�� 255 ( �]�N�O 256 = 0 )�A�h�e�@�Ӥp�϶� + 1 
		for(unsigned char i = 4 * ctx->Nb / sizeof(uXXb) - 1; i > -1; i--){
			if(++iv[i] != 0){
				break;
			}
		}
		perbar(dataBlock, ctx->dataLength, crypt);
	}
	perbar(ctx->dataLength, ctx->dataLength, crypt);
}

void CTR_Encrypt(PAES_context ctx){
	CTR(ctx, &Encrypt, NULL, encrypt);
}

void CTR_Decrypt(PAES_context ctx){
	CTR(ctx, &Encrypt, NULL, decrypt);
}

void CTR_Crypt(PAES_context ctx, bool crypt){
	CTR(ctx, NULL, &Crypt, crypt);
}

#ifdef LOOKUP
	void CTR_I_Crypt(PAES_context ctx, bool crypt){
		CTR(ctx, NULL, &I_Crypt, crypt);
	}
#endif

//  OFB �ҫ��A�e�禡���@��ΡA��禡���P���� 
void OFB(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt){
	uXXb state[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  ��
	uXXb iv[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  �N iv ��줣�P���A
	uXXb block[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  �����ư϶��w����� 
	copyArray(iv, ctx->iv, 4 * ctx->Nb);  //  �N iv ��줣�P���A
	for(long dataBlock = 0; dataBlock < ctx->dataLength; dataBlock += ctx->dataOffset){  //  ��ƶ����� 
		copyArray(state, iv, 4 * ctx->Nb / sizeof(uXXb));  //  ��ư϶��ƻs���� 
		//  �p�G��ӳ���A�h�P�����A������ܦ^�� 
		if(i_crypt != NULL){
			(*i_crypt)(ctx, state, encrypt);
		}else{
			if(en_de != NULL){
				(*en_de)(ctx, state);
			}else{
				return;
			}
		}
		if(keyFlag) return;
		ivOffset(ctx, iv, state, ctx->bitOffset);  //  IV�����q�A�����R 
		copyArray(block, ctx->data + dataBlock, 4 * ctx->Nb);  //  �ƻs data ���� data �Q��� 
		xorArray(state, block, 4 * ctx->Nb / sizeof(uXXb));  //  ���ΰ϶��A�̷� dataOffset �j�p�Ӳ��� 
		copyArray(ctx->data + dataBlock, state, ctx->dataOffset);  //  ��ƻs���ư϶�
		perbar(dataBlock, ctx->dataLength, crypt);
	}
	perbar(ctx->dataLength, ctx->dataLength, crypt);
}

void OFB_Encrypt(PAES_context ctx){
	OFB(ctx, &Encrypt, NULL, encrypt);
}

void OFB_Decrypt(PAES_context ctx){
	OFB(ctx, &Encrypt, NULL, decrypt);
}

void OFB_Crypt(PAES_context ctx, bool crypt){
	OFB(ctx, NULL, &Crypt, crypt);
}

#ifdef LOOKUP
	void OFB_I_Crypt(PAES_context ctx, bool crypt){
		OFB(ctx, NULL, &I_Crypt, crypt);
	}
#endif

//  CFB �ҫ��A�e�禡���@��ΡA��禡���P���� 
void CFB(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt){
	uXXb state[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  ��
	uXXb iv[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  �N iv ��줣�P���A 
	uXXb block[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  �����ư϶��w�����
	copyArray(iv, ctx->iv, 4 * ctx->Nb);  //  �N iv ��줣�P���A
	for(long dataBlock = 0; dataBlock < ctx->dataLength; dataBlock += ctx->dataOffset){  //  ��ƶ����� 
		copyArray(state, iv, 4 * ctx->Nb / sizeof(uXXb));  //  ��ư϶��ƻs���� 
		//  �p�G��ӳ���A�h�P�����A������ܦ^�� 
		if(i_crypt != NULL){
			(*i_crypt)(ctx, state, encrypt);
		}else{
			if(en_de != NULL){
				(*en_de)(ctx, state);
			}else{
				return;
			}
		}
		if(keyFlag) return;
		copyArray(block, ctx->data + dataBlock, 4 * ctx->Nb);  //  �ƻs data ���� data �Q��� 
		xorArray(state, block, 4 * ctx->Nb / sizeof(uXXb));  //  ���ΰ϶��A�̷� dataOffset �j�p�Ӳ��� 
		if(crypt){
			ivOffset(ctx, iv, state, ctx->bitOffset);  //  IV�����q�A�����R 
		}else{
			uXXb temp[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  �Ȧs��ư϶� 
			copyArray(temp, ctx->data + dataBlock, 4 * ctx->Nb);  //  �ƻs��ư϶���Ȧs
			ivOffset(ctx, iv, temp, ctx->bitOffset);  //  IV�����q�A��ư϶�����R 
		}
		copyArray(ctx->data + dataBlock, state, ctx->dataOffset);  //  ��ƻs���ư϶�
		perbar(dataBlock, ctx->dataLength, crypt);
	}
	perbar(ctx->dataLength, ctx->dataLength, crypt);
}

void CFB_Encrypt(PAES_context ctx){
	CFB(ctx, &Encrypt, NULL, encrypt);
}

void CFB_Decrypt(PAES_context ctx){
	CFB(ctx, &Encrypt, NULL, decrypt);
}

void CFB_Crypt(PAES_context ctx, bool crypt){
	CFB(ctx, NULL, &Crypt, crypt);
}

#ifdef LOOKUP
	void CFB_I_Crypt(PAES_context ctx, bool crypt){
		CFB(ctx, NULL, &I_Crypt, crypt);
	}
#endif

//  OFB �P CFB ��IV�����q 
void ivOffset(PAES_context ctx, uXXb iv[], uXXb padding[], unsigned short offset){
	unsigned char xblock = offset / (8 * sizeof(uXXb));  //  �T�w�i�H�����ʴX�Ӥp�϶� ( 08 - bit �C 8 bit ���@�Ӥp�϶��A 32 - bit �h�C 32 bit ���@�Ӥp�϶� ) 
	unsigned char xbit = offset % (8 * sizeof(uXXb));  //  �T�w�ٳѾl�X�� bit �ݭn���� 
	if(xblock != 0){
		//  �����첾 N �Ӥp�϶� 
		copyArray(iv, iv + xblock, 4 * ctx->Nb / sizeof(uXXb) - xblock);  //  ���첾 N �Ӥp�϶� 
		copyArray(iv + 4 * ctx->Nb / sizeof(uXXb) - xblock, padding, xblock);  //  ��R N �Ӹ�Ƥp�϶� 
	}
	if(xbit != 0){
		//  �����첾 N �� bit 
		for(int i = 0; i < 4 * ctx->Nb / sizeof(uXXb); i++){
			iv[i] <<= xbit;//  �C�Ӥp�϶��������첾 N �� bit 
			if(i != 4 * ctx->Nb / sizeof(uXXb) - 1){
				iv[i] ^= iv[i + 1] >> (8 * sizeof(uXXb) - xbit);  //  �����o�U�@�Ӥp�϶����X����ơA�ö�R���e�p�϶����᭱ 
			}else{
				iv[i] ^= padding[xblock] >> (8 * sizeof(uXXb) - xbit);  //  �p�G���̫�@���A�h������R N �Ӹ�� bit 
			} 
		}
	}
}

