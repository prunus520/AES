#include "aesctx.h"

typedef AES_context *PAES_context;

/********************  �w�q�禡  ********************/ 


//  �Ы� context 
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

//  �]�w�Ҳ� 
void setMode(PAES_context ctx, const char* mode){
	ctx->mode = mode;
}

const char* getMode(PAES_context ctx){
	return ctx->mode;
}

//  �]�w�϶��M IV�AIV ���ūh�]�� 0 
void setBlock(PAES_context ctx, unsigned short blockSize, unsigned char* iv){
	setBlockSize(ctx, blockSize);
	setIV(ctx, iv);
}

void setBlockSize(PAES_context ctx, unsigned short blockSize){
	ctx->blockSize = blockSize;
	ctx->Nb = blockSize / 32;
	ctx->bitOffset = atoi(ctx->mode + 3) ?: blockSize;  //  ���o�Ҧ��᪺�Ʀr�A�p�G�� 0 �h���϶��j�p 
	//  �����q���W�X�ŦX�d��A�_�h���϶��j�p / 8 
	if(ctx->bitOffset > 0 && ctx->bitOffset < blockSize){
		ctx->dataOffset = ctx->bitOffset / 8 + (ctx->bitOffset % 8 > 0 ? 1 : 0);  //  �p�G����n���p�϶��A�h�ɨ��n 
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
	//  ���p IV ���ŦX�W��άO NULL�A�h�᭱�� 0�A�Ϥ� 
	unsigned char* iv_temp = (unsigned char*)malloc(sizeof(unsigned char) * ctx->blockSize / 8 + 1);  //  ���s�}�O���� 
	int i = 0;
	for(; i < strlen((char*)iv); i++){
		iv_temp[i] = iv[i];  //  �ƻs���e�� IV 
	}
	for(; i < ctx->blockSize / 8 + 1; i++){
		iv_temp[i] = 0x00;  //  �ƻs���e�� IV 
	}
	if(ctx->iv != NULL)
		free(ctx->iv);
	ctx->iv = iv_temp;
}

unsigned char* getIV(PAES_context ctx){
	return ctx->iv;
}

//  �]�w���_�M key�Akey ���ūh�]�� 0
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
	ctx->ExpandedKey.EKey08 = (unsigned char*)malloc(sizeof(unsigned char) * 4 * ctx->Nb * (1 + ctx->Nr));  //  �}�Ҭ۹��������_�X�i�j�p 
}

unsigned short getKeySize(PAES_context ctx){
	return ctx->keySize;
}

void setKEY(PAES_context ctx, unsigned char* key){
	//  ���p key ���ŦX�W��άO NULL�A�h�᭱�� 0�A�Ϥ� 
	unsigned char* key_temp = (unsigned char*)malloc(sizeof(unsigned char) * ctx->keySize / 8 + 1);  //  ���s�}�O���� 
	int i = 0;
	for(; i < strlen((char*)key); i++){
		key_temp[i] = key[i];  //  �ƻs���e�� key 
	}
	for(; i < ctx->keySize / 8 + 1; i++){
		key_temp[i] = 0x00;  //  �ƻs���e�� IV 
	}
	if(ctx->key != NULL)
		free(ctx->key);
	ctx->key = key_temp;
}

unsigned char* getKey(PAES_context ctx){
	return ctx->key;
}

//  �]�w��R
void setPadding(PAES_context ctx, unsigned char padding){
	ctx->padding = padding % PADDING_MAX;
}

unsigned char getPadding(PAES_context ctx){
	return ctx->padding;
}

//  �]�w��J��X�ɮצ�m 
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

//  Ū�� 
void readFile(PAES_context ctx){
	FILE* file;
	unsigned char count;
	COORD cursorPosition = getCursorXY();
	setCursorXY(5, cursorPosition.Y + 1);
	if(file = fopen(ctx->inFile, "rb")){
		ctx->dataLength = filelength(fileno(file));  //  ���o�ɮת��� 
		//  �p�G��n���϶��j�p�h���h�}�A�Ϥ��h�h�}�϶��j�p / 8 
		count = ctx->dataLength % (ctx->blockSize / 8);
		ctx->blockLength = ctx->dataLength + ctx->blockSize / 8 - (count ?: 0);
		ctx->data = (unsigned char*)malloc(sizeof(unsigned char) * ctx->blockLength);
		fread(ctx->data, 1, ctx->dataLength, file);
		fclose(file);
		printf("�ɮפj�p�G %lu �줸��", ctx->dataLength);
	}else{
		printf("�ɮפj�p�G Ū�ɥ���");
	}
}

// �g�� 
void writeFile(PAES_context ctx){
	FILE* file;
	setCursorXY(5, getCursorXY().Y + 4);
	if(file = fopen(ctx->outFile, "wb")){
		fwrite(ctx->data, 1, ctx->dataLength, file);
		fclose(file);
		free(ctx->data);
		if(keyFlag)
			printf("����");
		else
			printf("����");
	}
}

