#include "aes.h"

/********************  定義函式  ********************/ 


// 列印AES內容 ( hex )
void printAES(const char name[], unsigned char data[], long size){
	COORD cursorPosition = getCursorXY();
	int y =  cursorPosition.Y;  //  小視窗擺放的位置
	setCursorXY(5, y);
	printf("%s", name);
	setCursorXY(5, y + 1);
	for(long i = 0; i < size; i++){
		printf("%02X  ", data[i]);
		if(i == size - 1){
			printf("     ");  //  補齊用 
		}else if(i % 16 == 15){  //  每 16 個換一行 
			setCursorXY(5, y + 1 + i / 16);
		}
			
	}
	//  補齊用 
	for(int i = 0; i < 16 - size % 16 && size % 16 != 0; i++){
			printf("   ");
	}
	printf("\n");
}

//  字串比較，確定前 3 個是否一樣 
bool strCMP(const char* cmp1, const char* cmp2, int count){
	for(unsigned char i = 0; i < count; i++){
		if(cmp1[i] != cmp2[i]){
			return 0;
		}
	}
	return 1;
}

//  填充字節 
void AES_padding(PAES_context ctx, bool crypt){
	unsigned char count = 0;
	switch(ctx->padding){
		case 0:  //  No
			for(long i = ctx->dataLength; i < ctx->blockLength; i++){
				ctx->data[i] = 0;
			}
			break;
		case 1:  //  Zero 
			if(crypt){
				for(long i = ctx->dataLength; i < ctx->blockLength; i++){
					ctx->data[i] = 0;
				}
				ctx->dataLength = ctx->blockLength;
			}else{
				ctx->blockLength -= ctx->blockSize / 8;
				for(long i = ctx->blockLength - 1; i > -1; i--){
					if(ctx->data[i] == 0){
						count++;
					}else{
						break;
					}
				}
				ctx->blockLength -= count;
				ctx->dataLength = ctx->blockLength;
			}
			break;
		case 2:  //  Space
			if(crypt){
				for(long i = ctx->dataLength; i < ctx->blockLength; i++){
					ctx->data[i] = 0x20;
				}
				ctx->dataLength = ctx->blockLength;
			}else{
				ctx->blockLength -= ctx->blockSize / 8;
				for(long i = ctx->blockLength - 1; i > -1; i--){
					if(ctx->data[i] == 0x20){
						count++;
					}else{
						break;
					}
				}
				ctx->blockLength -= count;
				ctx->dataLength = ctx->blockLength;
			}
			break;
		case 3:  //  ISO / IEC 7816-4
			if(crypt){
				for(long i = ctx->dataLength; i < ctx->blockLength; i++){
					ctx->data[i] = 0;
					if(i == ctx->dataLength){
						ctx->data[i] = 0x80;
					}
				}
				ctx->dataLength = ctx->blockLength;
			}else{
				ctx->blockLength -= ctx->blockSize / 8;
				for(long i = ctx->blockLength - 1; i > -1; i--){
					if(ctx->data[i] == 0x80){
						count++;
						break;
					}else if(ctx->data[i] == 0){
						count++;
					}
				}
				ctx->blockLength -= count;
				ctx->dataLength = ctx->blockLength;
			}
			break;
		case 4:  //  ZeroLen
			if(crypt){
				for(long i = ctx->dataLength; i < ctx->blockLength; i++){
					ctx->data[i] = 0;
					if(i == ctx->blockLength - 1){
						ctx->data[i] = ctx->blockLength - ctx->dataLength - 1;
					}
				}
				ctx->dataLength = ctx->blockLength;
			}else{
				ctx->blockLength -= ctx->blockSize / 8;
				ctx->blockLength -= ctx->data[ctx->blockLength - 1] + 1;
				ctx->dataLength = ctx->blockLength;
			}
			break;
		case 5:  //  ANSI X.923
			if(crypt){
				for(long i = ctx->dataLength; i < ctx->blockLength; i++){
					ctx->data[i] = 0;
				}
				ctx->data[ctx->blockLength - 1] = ctx->blockLength - ctx->dataLength;
				ctx->dataLength = ctx->blockLength;
			}else{
				ctx->blockLength -= ctx->blockSize / 8;
				ctx->blockLength -= ctx->data[ctx->blockLength - 1];
				ctx->dataLength = ctx->blockLength;
			}
			break;
		case 6:  //  ISO 10126
			srand(time(NULL));
			if(crypt){
				for(long i = ctx->dataLength; i < ctx->blockLength; i++){
					ctx->data[i] = rand() % 256;
				}
				ctx->data[ctx->blockLength - 1] = ctx->blockLength - ctx->dataLength;
				ctx->dataLength = ctx->blockLength;
			}else{
				ctx->blockLength -= ctx->blockSize / 8;
				ctx->blockLength -= ctx->data[ctx->blockLength - 1];
				ctx->dataLength = ctx->blockLength;
			}
			break;
		case 7:  //  PKCS7
			if(crypt){
				for(long i = ctx->dataLength; i < ctx->blockLength; i++){
					ctx->data[i] = ctx->blockLength - ctx->dataLength;
				}
				ctx->dataLength = ctx->blockLength;
			}else{
				ctx->blockLength -= ctx->blockSize / 8;
				ctx->blockLength -= ctx->data[ctx->blockLength - 1];
				ctx->dataLength = ctx->blockLength;
			}
			break;
	}
}

void AES(PAES_context ctx, void (*ke)(PAES_context), void (*ecb)(PAES_context), void (*cbc)(PAES_context), void (*ctr)(PAES_context), void (*ofb)(PAES_context), void (*cfb)(PAES_context), bool crypt, bool print){
    readFile(ctx);  //  讀取檔案 
    //  沒有區塊則失敗 
    if(ctx->blockLength == 0){
	    CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		setCursorXY(5, csbi.dwCursorPosition.Y + 1);
		printf("失敗");
    	return;
	}
	//  列印 input 
	if(print){
		box(72, 30, 3, 17);
		setCursorXY(5, 19);
		printAES("input:", ctx->data, ctx->dataLength);
	}
	//  padding 於加密前 
	if(crypt == 1){
		AES_padding(ctx, crypt);
	}
	//  擴充金鑰 
	(*ke)(ctx);
	//  列印擴充金鑰 
	if(print){
		printAES("KeyExpansion:", ctx->ExpandedKey.EKey08, 4 * ctx->Nb * (1 + ctx->Nr));
	}
	//  AES 模式 
	if(strCMP(ctx->mode, "ECB")){
		(*ecb)(ctx);
	}else if(strCMP(ctx->mode, "CBC")){
		(*cbc)(ctx);
	}else if(strCMP(ctx->mode, "CTR")){
		(*ctr)(ctx);
	}else if(strCMP(ctx->mode, "OFB")){
		(*ofb)(ctx);
	}else if(strCMP(ctx->mode, "CFB")){
		(*cfb)(ctx);
	}
	//  padding 於解密後 
	if(crypt == 0){
		AES_padding(ctx, crypt);
	}
	//  列印 output 
	if(print){
		printAES("output:", ctx->data, ctx->dataLength);
		COORD cursorPosition = getCursorXY();
		border(cursorPosition.Y, cursorPosition.Y + 2);
		setCursorXY(5, cursorPosition.Y - 3);
	}
	//  寫入檔案 
	writeFile(ctx);
}

void AES_C(PAES_context ctx, void (*ke)(PAES_context), void (*ecb)(PAES_context, bool), void (*cbc)(PAES_context, bool), void (*ctr)(PAES_context, bool), void (*ofb)(PAES_context, bool), void (*cfb)(PAES_context, bool), bool crypt, bool print){
    readFile(ctx);
    if(ctx->blockLength == 0){
		setCursorXY(5, getCursorXY().Y + 1);
		printf("失敗");
    	return;
	}
	if(print){
		box(72, 30, 3, 17);
		setCursorXY(5, 19);
		printAES("input:", ctx->data, ctx->dataLength);
	}
	
	if(crypt == 1){
		AES_padding(ctx, crypt);
	}
	
	(*ke)(ctx);
	
	if(print){
		printAES("KeyExpansion:", ctx->ExpandedKey.EKey08, 4 * ctx->Nb * (1 + ctx->Nr));
	}
	
	if(strCMP(ctx->mode, "ECB")){
		(*ecb)(ctx, crypt);
	}else if(strCMP(ctx->mode, "CBC")){
		(*cbc)(ctx, crypt);
	}else if(strCMP(ctx->mode, "CTR")){
		(*ctr)(ctx, crypt);
	}else if(strCMP(ctx->mode, "OFB")){
		(*ofb)(ctx, crypt);
	}else if(strCMP(ctx->mode, "CFB")){
		(*cfb)(ctx, crypt);
	}
	
	if(crypt == 0){
		AES_padding(ctx, crypt);
	}
	
	if(print){
		printAES("output:", ctx->data, ctx->dataLength);
		COORD cursorPosition = getCursorXY();
		border(cursorPosition.Y, cursorPosition.Y + 2);
		setCursorXY(5, cursorPosition.Y - 3);
	}
	writeFile(ctx);
}

void AES_Encrypt(PAES_context ctx, bool is08b, bool opm, bool print){
	if(is08b){
		if(opm){
			using namespace lu08;
			AES(ctx, &KeyExpansion, &ECB_Encrypt, &CBC_Encrypt, &CTR_Encrypt, &OFB_Encrypt, &CFB_Encrypt, 1, print);
		}else{
			using namespace math08;
			AES(ctx, &KeyExpansion, &ECB_Encrypt, &CBC_Encrypt, &CTR_Encrypt, &OFB_Encrypt, &CFB_Encrypt, 1, print);
		}
	}else{
		if(opm){
			using namespace lu32;
			AES(ctx, &KeyExpansion, &ECB_Encrypt, &CBC_Encrypt, &CTR_Encrypt, &OFB_Encrypt, &CFB_Encrypt, 1, print);
		}else{
			using namespace math32;
			AES(ctx, &KeyExpansion, &ECB_Encrypt, &CBC_Encrypt, &CTR_Encrypt, &OFB_Encrypt, &CFB_Encrypt, 1, print);
		}
	}
}

void AES_Decrypt(PAES_context ctx, bool is08b, bool opm, bool print){
	if(is08b){
		if(opm){
			using namespace lu08;
			AES(ctx, &KeyExpansion, &ECB_Decrypt, &CBC_Decrypt, &CTR_Decrypt, &OFB_Decrypt, &CFB_Decrypt, 0, print);
		}else{
			using namespace math08;
			AES(ctx, &KeyExpansion, &ECB_Decrypt, &CBC_Decrypt, &CTR_Decrypt, &OFB_Decrypt, &CFB_Decrypt, 0, print);
		}
	}else{
		if(opm){
			using namespace lu32;
			AES(ctx, &KeyExpansion, &ECB_Decrypt, &CBC_Decrypt, &CTR_Decrypt, &OFB_Decrypt, &CFB_Decrypt, 0, print);
		}else{
			using namespace math32;
			AES(ctx, &KeyExpansion, &ECB_Decrypt, &CBC_Decrypt, &CTR_Decrypt, &OFB_Decrypt, &CFB_Decrypt, 0, print);
		}
	}
}

void AES_Crypt(PAES_context ctx, bool crypt, bool is08b, bool opm, bool print){
	if(is08b){
		if(opm){
			using namespace lu08;
			AES_C(ctx, &KeyExpansion, &ECB_Crypt, &CBC_Crypt, &CTR_Crypt, &OFB_Crypt, &CFB_Crypt, crypt, print);
		}else{
			using namespace math08;
			AES_C(ctx, &KeyExpansion, &ECB_Crypt, &CBC_Crypt, &CTR_Crypt, &OFB_Crypt, &CFB_Crypt, crypt, print);
		}
	}else{
		if(opm){
			using namespace lu32;
			AES_C(ctx, &KeyExpansion, &ECB_Crypt, &CBC_Crypt, &CTR_Crypt, &OFB_Crypt, &CFB_Crypt, crypt, print);
		}else{
			using namespace math32;
			AES_C(ctx, &KeyExpansion, &ECB_Crypt, &CBC_Crypt, &CTR_Crypt, &OFB_Crypt, &CFB_Crypt, crypt, print);
		}
	}
}

void AES_I_Crypt(PAES_context ctx, bool crypt, bool is08b, bool opm, bool print){
	if(is08b){
		if(opm){
			using namespace lu08;
			AES_C(ctx, &KeyExpansion, &ECB_I_Crypt, &CBC_I_Crypt, &CTR_I_Crypt, &OFB_I_Crypt, &CFB_I_Crypt, crypt, print);
		}else{
			using namespace math08;
			AES_C(ctx, &KeyExpansion, &ECB_Crypt, &CBC_Crypt, &CTR_Crypt, &OFB_Crypt, &CFB_Crypt, crypt, print);
		}
	}else{
		if(opm){
			using namespace lu32;
			AES_C(ctx, &KeyExpansion, &ECB_I_Crypt, &CBC_I_Crypt, &CTR_I_Crypt, &OFB_I_Crypt, &CFB_I_Crypt, crypt, print);
		}else{
			using namespace math32;
			AES_C(ctx, &KeyExpansion, &ECB_Crypt, &CBC_Crypt, &CTR_Crypt, &OFB_Crypt, &CFB_Crypt, crypt, print);
		}
	}
}

