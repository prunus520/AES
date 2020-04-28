#include "aesctx.h"

typedef AES_context *PAES_context;

/********************  定義函式  ********************/ 


//  創建 context 
PAES_context PAES_create(const char* mode, unsigned short blockSize, unsigned char* iv, unsigned short keySize, unsigned char* key, unsigned char padding){
	PAES_context ctx = (AES_context*)malloc(sizeof(AES_context));
	setMode(ctx, mode);
	setBlock(ctx, blockSize, iv);
	setKey(ctx, keySize, key);
    setPadding(ctx, padding);
    
	return ctx;
}

void PAES_free(PAES_context ctx){
	free((void*)ctx->mode);
	free(ctx->key);
	free(ctx->iv);
	free(ctx->data);
	free(ctx->ExpandedKey.EKey08);
	free(ctx);
}

//  設定模組 
void setMode(PAES_context ctx, const char* mode){
	ctx->mode = mode;
}

const char* getMode(PAES_context ctx){
	return ctx->mode;
}

//  設定區塊和 IV，IV 為空則設為 0 
void setBlock(PAES_context ctx, unsigned short blockSize, unsigned char* iv){
	setBlockSize(ctx, blockSize);
	setIV(ctx, iv);
}

void setBlockSize(PAES_context ctx, unsigned short blockSize){
	ctx->blockSize = blockSize;
	ctx->Nb = blockSize / 32;
	ctx->bitOffset = atoi(ctx->mode + 3) ?: blockSize;  //  取得模式後的數字，如果為 0 則為區塊大小 
	//  偏移量不超出符合範圍，否則為區塊大小 / 8 
	if(ctx->bitOffset > 0 && ctx->bitOffset < blockSize){
		ctx->dataOffset = ctx->bitOffset / 8 + (ctx->bitOffset % 8 > 0 ? 1 : 0);  //  如果不剛好滿小區塊，則補到剛好 
	}else{
		ctx->bitOffset = blockSize;
		ctx->dataOffset = blockSize / 8;
	}
	ctx->Nr = 6 + (ctx->Nb > ctx->Nk ? ctx->Nb : ctx->Nk);  //  6 + max ( Nb, Nk )
}

unsigned short getBlockSize(PAES_context ctx){
	return ctx->blockSize;
}

void setIV(PAES_context ctx, unsigned char* iv){
	//  假如 IV 不符合規格或是 NULL，則後面補 0，反之 
	unsigned char* iv_temp = (unsigned char*)malloc(sizeof(unsigned char) * ctx->blockSize / 8 + 1);  //  重新開記憶體 
	int i = 0;
	for(; i < strlen((char*)iv); i++){
		iv_temp[i] = iv[i];  //  複製先前的 IV 
	}
	for(; i < ctx->blockSize / 8 + 1; i++){
		iv_temp[i] = 0x00;  //  複製先前的 IV 
	}
	if(ctx->iv != NULL)
		free(ctx->iv);
	ctx->iv = iv_temp;
}

unsigned char* getIV(PAES_context ctx){
	return ctx->iv;
}

//  設定金鑰和 key，key 為空則設為 0
void setKey(PAES_context ctx, unsigned short keySize, unsigned char* key){
	setKeySize(ctx, keySize);
	setKEY(ctx, key);
}

void setKeySize(PAES_context ctx, unsigned short keySize){
	ctx->keySize = keySize;
	ctx->Nk = keySize / 32;
	ctx->Nr = 6 + (ctx->Nb > ctx->Nk ? ctx->Nb : ctx->Nk);  //  6 + max ( Nb, Nk ) 
	if(ctx->ExpandedKey.EKey08 != NULL)
		free(ctx->ExpandedKey.EKey08);
	ctx->ExpandedKey.EKey08 = (unsigned char*)malloc(sizeof(unsigned char) * 4 * ctx->Nb * (1 + ctx->Nr));  //  開啟相對應的金鑰擴展大小 
}

unsigned short getKeySize(PAES_context ctx){
	return ctx->keySize;
}

void setKEY(PAES_context ctx, unsigned char* key){
	//  假如 key 不符合規格或是 NULL，則後面補 0，反之 
	unsigned char* key_temp = (unsigned char*)malloc(sizeof(unsigned char) * ctx->keySize / 8 + 1);  //  重新開記憶體 
	int i = 0;
	for(; i < strlen((char*)key); i++){
		key_temp[i] = key[i];  //  複製先前的 key 
	}
	for(; i < ctx->keySize / 8 + 1; i++){
		key_temp[i] = 0x00;  //  複製先前的 IV 
	}
	if(ctx->key != NULL)
		free(ctx->key);
	ctx->key = key_temp;
}

unsigned char* getKey(PAES_context ctx){
	return ctx->key;
}

//  設定填充
void setPadding(PAES_context ctx, unsigned char padding){
	ctx->padding = padding % PADDING_MAX;
}

unsigned char getPadding(PAES_context ctx){
	return ctx->padding;
}

//  設定輸入輸出檔案位置 
void setFile(PAES_context ctx, const char* inFile, const char* outFile){
	ctx->inFile = inFile;
	ctx->outFile = outFile;
}

const char* getInFile(PAES_context ctx){
	return ctx->inFile;
}

const char* getOutFile(PAES_context ctx){
	return ctx->outFile;
}

//  讀檔 
void readFile(PAES_context ctx){
	FILE* file;
	unsigned char count;
	COORD cursorPosition = getCursorXY();
	setCursorXY(5, cursorPosition.Y + 1);
	if(file = fopen(ctx->inFile, "rb")){
		ctx->dataLength = filelength(fileno(file));  //  取得檔案長度 
		//  如果剛好為區塊大小則不多開，反之則多開區塊大小 / 8 
		count = ctx->dataLength % (ctx->blockSize / 8);
		ctx->blockLength = ctx->dataLength + ctx->blockSize / 8 - (count ?: 0);
		ctx->data = (unsigned char*)malloc(sizeof(unsigned char) * ctx->blockLength);
		fread(ctx->data, 1, ctx->dataLength, file);
		fclose(file);
		printf("檔案大小： %lu 位元組", ctx->dataLength);
	}else{
		printf("檔案大小： 讀檔失敗");
	}
}

// 寫檔 
void writeFile(PAES_context ctx){
	FILE* file;
	setCursorXY(5, getCursorXY().Y + 4);
	if(file = fopen(ctx->outFile, "wb")){
		fwrite(ctx->data, 1, ctx->dataLength, file);
		fclose(file);
		free(ctx->data);
		if(keyFlag)
			printf("結束");
		else
			printf("完成");
	}
}

