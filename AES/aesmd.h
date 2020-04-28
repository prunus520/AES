/*
 *     檔名： aesmd.h
 *     全名： aes mode
 *     作者： Cerasus
 *     描述： 
 */

/********************
 *  定義加密為 1，解密為 0 
 ********************/ 

#define encrypt 1
#define decrypt 0

//  判斷是否為字節型的擴展金鑰 
#ifndef wordEKey
	typedef unsigned char uXXb;
#else
	typedef unsigned uXXb;
#endif 


/********************  宣告函式  ********************/ 


/********************
 *  加解密部分 
 ********************/
static void Encrypt(PAES_context ctx, uXXb state[]);
static void Decrypt(PAES_context ctx, uXXb state[]);
static void Crypt(PAES_context ctx, uXXb state[], bool crypt);
#ifdef LOOKUP
	static void I_Crypt(PAES_context ctx, uXXb state[], bool crypt);
#endif 

/********************
 *  AES - ECB 模式
 ********************/
static void ECB(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt);
static void ECB_Encrypt(PAES_context ctx);
static void ECB_Decrypt(PAES_context ctx);
static void ECB_Crypt(PAES_context ctx, bool crypt);
#ifdef LOOKUP
	static void ECB_I_Crypt(PAES_context ctx, bool crypt);
#endif 

/********************
 *  AES - CBC 模式
 ********************/
static void CBC(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt);
static void CBC_Encrypt(PAES_context ctx);
static void CBC_Decrypt(PAES_context ctx);
static void CBC_Crypt(PAES_context ctx, bool crypt);
#ifdef LOOKUP
	static void CBC_I_Crypt(PAES_context ctx, bool crypt);
#endif 

/********************
 *  AES - CTR 模式
 ********************/
static void CTR(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt);
static void CTR_Encrypt(PAES_context ctx);
static void CTR_Decrypt(PAES_context ctx);
static void CTR_Crypt(PAES_context ctx, bool crypt);
#ifdef LOOKUP
	static void CTR_I_Crypt(PAES_context ctx, bool crypt);
#endif 

/********************
 *  AES - OFB 模式
 ********************/
static void OFB(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt);
static void OFB_Encrypt(PAES_context ctx);
static void OFB_Decrypt(PAES_context ctx);
static void OFB_Crypt(PAES_context ctx, bool crypt);
#ifdef LOOKUP
	static void OFB_I_Crypt(PAES_context ctx, bool crypt);
#endif 

/********************
 *  AES - CFB 模式
 ********************/
static void CFB(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt);
static void CFB_Encrypt(PAES_context ctx);
static void CFB_Decrypt(PAES_context ctx);
static void CFB_Crypt(PAES_context ctx, bool crypt);
#ifdef LOOKUP
	static void CFB_I_Crypt(PAES_context ctx, bool crypt);
#endif 

/********************
 *  IV偏移量 
 ********************/ 
static void ivOffset(PAES_context ctx, uXXb iv[], uXXb padding[], unsigned short offset);

/********************  定義函式  ********************/ 


//  一般加密 
void Encrypt(PAES_context ctx, uXXb state[]){
	AddRoundKey(ctx, state, 0);
	for(unsigned char i = 1; i < ctx->Nr; i++){
		if(rearKeyEvent()) return;  //  中離程式
		SubBytes(ctx, state);
		ShiftRows(ctx, state);
		MixColumns(ctx, state);
		AddRoundKey(ctx, state, i);
	}
	SubBytes(ctx, state);
	ShiftRows(ctx, state);
	AddRoundKey(ctx, state, ctx->Nr);
}

//  一般解密 
void Decrypt(PAES_context ctx, uXXb state[]){
	AddRoundKey(ctx, state, ctx->Nr);
	ShiftRowsInv(ctx, state);
	SubBytesInv(ctx, state);
	for(unsigned char i = ctx->Nr - 1; i > 0; i--){
		if(rearKeyEvent()) return;  //  中離程式
		AddRoundKey(ctx, state, i);
		MixColumnsInv(ctx, state);
		ShiftRowsInv(ctx, state);
		SubBytesInv(ctx, state);
	}
	AddRoundKey(ctx, state, 0);
}

//  同型加解密 
void Crypt(PAES_context ctx, uXXb state[], bool crypt){
	for(unsigned char i = 0; i < ctx->Nr - 1; i++){
		if(rearKeyEvent()) return;  //  中離程式
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
	//  整合同型加解密 
	void I_Crypt(PAES_context ctx, uXXb state[], bool crypt){
		for(unsigned char i = 0; i < ctx->Nr - 1; i++){
			if(rearKeyEvent()) return;  //  中離程式
			AddRoundKey(crypt, ctx, state, i);
			I_MixColumns(crypt, ctx, state);  //  包含 SubBytes、 ShiftRows
		}
		AddRoundKey(crypt, ctx, state, ctx->Nr - 1);
		I_SubBytes(crypt, ctx, state);  // 包含 ShiftRows 
		AddRoundKey(crypt, ctx, state, ctx->Nr);
	}
#endif

//  ECB 模型，前函式為一般用，後函式為同型用 
void ECB(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt){
	uXXb state[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  體 
	for(long dataBlock = 0; dataBlock < ctx->dataLength; dataBlock += ctx->dataOffset){  //  資料塊移動 
		copyArray(state, ctx->data + dataBlock, 4 * ctx->Nb);  //  資料區塊複製到體 
		//  如果兩個都選，則同型先 
		if(i_crypt != NULL){
			(*i_crypt)(ctx, state, crypt);
		}else{
			(*en_de)(ctx, state);
		}
		if(keyFlag) return;
		copyArray(ctx->data + dataBlock, state, 4 * ctx->Nb);  //  體複製到資料區塊 
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

//  CBC 模型，前函式為一般用，後函式為同型用 
void CBC(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt){
	uXXb state[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  體
	uXXb iv[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  將 iv 轉到不同型態
	uXXb temp[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  解密暫存用 
	copyArray(iv, ctx->iv, 4 * ctx->Nb);  //  將 iv 轉到不同型態
	for(long dataBlock = 0; dataBlock < ctx->dataLength; dataBlock += ctx->dataOffset){  //  資料塊移動 
		copyArray(state, ctx->data + dataBlock, 4 * ctx->Nb);  //  資料區塊複製到體 
		if(crypt){
			xorArray(state, iv, 4 * ctx->Nb / sizeof(uXXb));  //  異或 iv
		}else{
			copyArray(temp, state, 4 * ctx->Nb / sizeof(uXXb));  //  體複製到暫存 
		}
		//  如果兩個都選，則同型先，都不選擇回傳 
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
			copyArray(iv, state, 4 * ctx->Nb / sizeof(uXXb));  //  複製體到 iv 
		}else{
			xorArray(state, iv, 4 * ctx->Nb / sizeof(uXXb));  //  體與 iv 異或 
			copyArray(iv, temp, 4 * ctx->Nb / sizeof(uXXb));  //  將體複製到 iv 
		}
		copyArray(ctx->data + dataBlock, state, 4 * ctx->Nb);  //  體複製到資料區塊
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

//  CTR 模型，前函式為一般用，後函式為同型用 
void CTR(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt){
	uXXb state[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  體
	uXXb iv[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  將 iv 轉到不同型態
	uXXb temp[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  暫存資料區塊 
	copyArray(iv, ctx->iv, 4 * ctx->Nb);  //  將 iv 轉到不同型態
	for(long dataBlock = 0; dataBlock < ctx->dataLength; dataBlock += ctx->dataOffset){  //  資料塊移動 
		copyArray(state, iv, 4 * ctx->Nb / sizeof(uXXb));  //  資料區塊複製到體 
		//  如果兩個都選，則同型先，都不選擇回傳 
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
		copyArray(temp, ctx->data + dataBlock, 4 * ctx->Nb);  //  資料區塊複製到暫存 
		xorArray(state, temp, 4 * ctx->Nb / sizeof(uXXb));  //  體異或資料區塊 
		copyArray(ctx->data + dataBlock, state, 4 * ctx->Nb);  //  體複製到資料區塊
		//  看一共有幾個小區塊，從小的區塊計數到大的區塊，如果大於 255 ( 也就是 256 = 0 )，則前一個小區塊 + 1 
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

//  OFB 模型，前函式為一般用，後函式為同型用 
void OFB(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt){
	uXXb state[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  體
	uXXb iv[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  將 iv 轉到不同型態
	uXXb block[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  防止資料區塊預先更動 
	copyArray(iv, ctx->iv, 4 * ctx->Nb);  //  將 iv 轉到不同型態
	for(long dataBlock = 0; dataBlock < ctx->dataLength; dataBlock += ctx->dataOffset){  //  資料塊移動 
		copyArray(state, iv, 4 * ctx->Nb / sizeof(uXXb));  //  資料區塊複製到體 
		//  如果兩個都選，則同型先，都不選擇回傳 
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
		ivOffset(ctx, iv, state, ctx->bitOffset);  //  IV偏移量，體其後填充 
		copyArray(block, ctx->data + dataBlock, 4 * ctx->Nb);  //  複製 data 防止 data 被更改 
		xorArray(state, block, 4 * ctx->Nb / sizeof(uXXb));  //  異或區塊，依照 dataOffset 大小來異或 
		copyArray(ctx->data + dataBlock, state, ctx->dataOffset);  //  體複製到資料區塊
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

//  CFB 模型，前函式為一般用，後函式為同型用 
void CFB(PAES_context ctx, void (*en_de)(PAES_context, uXXb[]), void (*i_crypt)(PAES_context, uXXb[], bool), bool crypt){
	uXXb state[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  體
	uXXb iv[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  將 iv 轉到不同型態 
	uXXb block[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  防止資料區塊預先更動
	copyArray(iv, ctx->iv, 4 * ctx->Nb);  //  將 iv 轉到不同型態
	for(long dataBlock = 0; dataBlock < ctx->dataLength; dataBlock += ctx->dataOffset){  //  資料塊移動 
		copyArray(state, iv, 4 * ctx->Nb / sizeof(uXXb));  //  資料區塊複製到體 
		//  如果兩個都選，則同型先，都不選擇回傳 
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
		copyArray(block, ctx->data + dataBlock, 4 * ctx->Nb);  //  複製 data 防止 data 被更改 
		xorArray(state, block, 4 * ctx->Nb / sizeof(uXXb));  //  異或區塊，依照 dataOffset 大小來異或 
		if(crypt){
			ivOffset(ctx, iv, state, ctx->bitOffset);  //  IV偏移量，體其後填充 
		}else{
			uXXb temp[4 * ctx->Nb / sizeof(uXXb)] = {0};  //  暫存資料區塊 
			copyArray(temp, ctx->data + dataBlock, 4 * ctx->Nb);  //  複製資料區塊到暫存
			ivOffset(ctx, iv, temp, ctx->bitOffset);  //  IV偏移量，資料區塊其後填充 
		}
		copyArray(ctx->data + dataBlock, state, ctx->dataOffset);  //  體複製到資料區塊
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

//  OFB 與 CFB 的IV偏移量 
void ivOffset(PAES_context ctx, uXXb iv[], uXXb padding[], unsigned short offset){
	unsigned char xblock = offset / (8 * sizeof(uXXb));  //  確定可以先移動幾個小區塊 ( 08 - bit 每 8 bit 為一個小區塊， 32 - bit 則每 32 bit 為一個小區塊 ) 
	unsigned char xbit = offset % (8 * sizeof(uXXb));  //  確定還剩餘幾個 bit 需要移動 
	if(xblock != 0){
		//  往左位移 N 個小區塊 
		copyArray(iv, iv + xblock, 4 * ctx->Nb / sizeof(uXXb) - xblock);  //  先位移 N 個小區塊 
		copyArray(iv + 4 * ctx->Nb / sizeof(uXXb) - xblock, padding, xblock);  //  填充 N 個資料小區塊 
	}
	if(xbit != 0){
		//  往左位移 N 個 bit 
		for(int i = 0; i < 4 * ctx->Nb / sizeof(uXXb); i++){
			iv[i] <<= xbit;//  每個小區塊先往左位移 N 個 bit 
			if(i != 4 * ctx->Nb / sizeof(uXXb) - 1){
				iv[i] ^= iv[i + 1] >> (8 * sizeof(uXXb) - xbit);  //  先取得下一個小區塊溢出的資料，並填充到當前小區塊的後面 
			}else{
				iv[i] ^= padding[xblock] >> (8 * sizeof(uXXb) - xbit);  //  如果為最後一塊，則直接填充 N 個資料 bit 
			} 
		}
	}
}

