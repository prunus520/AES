#include "rrdsgn.h"

unsigned char md = 0;
const char* MODE[] = {"ECB", "CBC", "CTR", "OFB", "OFB8", "OFB64", "CFB", "CFB8", "CFB64"};
typedef INIT *PINIT;

/********************  定義函式  ********************/ 

//  鍵盤事件 
bool keyEvent(PINIT pinit, PAES_context ctx){
	DWORD cnt;
    INPUT_RECORD input;
    char c;
	
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_LINE_INPUT | ENABLE_INSERT_MODE | ENABLE_MOUSE_INPUT);  //  取得字元不用按下 Enter、禁用左右鍵 
	
    while(true){  //  讀取輸入的動作 
        if(kbhit() != 0){  //  判斷是否為鍵盤事件，且為按下的狀態 
        	c = getch();
            if(c == 0x48 || c == 0x4B){  //  按下 Up 和 Left
            	//  選項往上移動，低於 0 則回到 sum - 1 
				pinit->num--;
				if(pinit->num < 0){
					pinit->num = pinit->sum - 1;
				}
                break;
			}else if(c == 0x50 || c == 0x4D){  //  按下 Down 和 Right
				//  選項往下移動，超過則回到 0 
				pinit->num++;
				pinit->num %= pinit->sum;
                break;
			}else if(c == 0x0D){  //  按下 Enter 
				return 1;
			}else if(c == 0x1B){  //  按下 Esc，且不是選單頁面 
				if(pinit->page != MENU){
					pinit->num = pinit->sum - 1;  //  每個頁面最後一個選項為上一頁 
					return 1;
				}else{
					exit(0);
				}
			}else if(c > 0x3A && c < 0x3F && pinit->page != c - 0x3A){  //  按下 F1 ~ F2 且不是自己的頁面 
				pinit->page = c - 0x3A;  //  設定自己的頁面 MENU = 1， F1 = 0x70，0x70 - 0x6F = 0x01(16) = 1(10) 
				pinit->num = -1;  //  避開其他選項 
				pinit->parm = 2;
				return 1;
			}else if(pinit->page != AESM){  //  判斷是不是加解密畫面 
				//  按下數字鍵，為虛擬建與 ASCII 的代碼，設定 AES 參數的快捷鍵 
				if(c == 0x31){
					pinit->is08b = !pinit->is08b;
					pinit->parm = 1;
					return 1;
				}else if(c == 0x32){
					pinit->isLU = !pinit->isLU;
					pinit->parm = 1;
					return 1;
				}else if(c == 0x33){
					pinit->crypt++;
					pinit->crypt %= 3;
					pinit->parm = 1;
					return 1;
				}else if(c == 0x34){
					if(getBlockSize(ctx) == 128){
						setBlockSize(ctx, 192);
					}else if(getBlockSize(ctx) == 192){
						setBlockSize(ctx, 256);
					}else if(getBlockSize(ctx) == 256){
						setBlockSize(ctx, 128);
					}
					pinit->parm = 1;
					return 1;
				}else if(c == 0x35){
					if(getKeySize(ctx) == 128){
						setKeySize(ctx, 192);
					}else if(getKeySize(ctx) == 192){
						setKeySize(ctx, 256);
					}else if(getKeySize(ctx) == 256){
						setKeySize(ctx, 128);
					}
					pinit->parm = 1;
					return 1;
				}else if(c == 0x36){
					md++;
					md %= 9;
					setMode(ctx, MODE[md]);
					pinit->parm = 1;
					return 1;
				}else if(c == 0x37){
					setPadding(ctx, getPadding(ctx) + 1);
					pinit->parm = 1;
					return 1;
				}else if(c == 0x38){
					const char* key = (char*)ctx->key;
					setpws("Key ( ASCII )", &key, ctx->keySize / 8);
					setKEY(ctx, (unsigned char*)key);
					pinit->parm = 1;
					return 1;
				}else if(c == 0x39){
					const char* iv = (char*)ctx->iv;
					setpws("IV ( ASCII )", &iv, ctx->blockSize / 8);
					setIV(ctx, (unsigned char*)iv);
					pinit->parm = 1;
					return 1;
				}
			}
        }
    }
    return 0;
}

//  商標設計 
void logo(void){
	printf("\n");
	textHR();
	textCenter("");
	textCenter("Advanced Encryption Standard 加解密");
	textCenter("");
	textCenter("by  Cerasus");
	textCenter("");
	textHR();
	textCenter("");
}

//  參數設計 
void parms(PINIT pinit, PAES_context ctx){
	const char* pad[] = {"No", "Zero", "Space", "ISOIEC78164", "ZeroLen", "ANSIX923", "ISO10126", "PKCS7"};
	setCursorXY(0, 9);
	textCenterf("%s %s法 %s模組", pinit->is08b ? "8-bit" : "32-bit", pinit->isLU ? "查表" : "數學", pinit->crypt ? pinit->crypt == 2 ? "同型整合" : "同型" : "一般");
	textCenter("");
	textCenterf("區塊-金鑰-模式-填充：%d-%d-%s-%sPadding", ctx->blockSize, ctx->keySize, ctx->mode, pad[ctx->padding]);
	textCenter("");
	textCenterf("key：%s", ctx->key);
	textCenter("");
	textCenterf("IV：%s", ctx->iv);
	textCenter("");
	textHR();
	textCenter("");
	pinit->parm = 2;
}

//  選單設計 
void menu(PINIT pinit, PAES_context ctx){
	const char* option[] = {"  加密檔案  ", "  解密檔案  ", " 加解密檔案 ", "    設定    ", " 清空資料夾 ", "  進階工具  ", "    離開    "};
	pinit->sum = sizeof(option) / 8;  //  一共有多少個選項 
	do{
		pageMenu("選單：", option, pinit->sum, pinit->num);
	}while(!keyEvent(pinit, ctx));
	if(pinit->parm == 2){
		if(pinit->num == 0 || pinit->num == 1 || pinit->num == 2){
			pinit->page = AESM;
		}else if(pinit->num == 3){
			pinit->page = SET;
			pinit->num = 0;
		}else if(pinit->num == 4){
			pinit->page = CLF;
			pinit->num = 0;
		}else if(pinit->num == 5){
			pinit->page = PRO;
			pinit->num = 0;
		}else if(pinit->num == 7){
			exit(0);
		}
	}
}

//  設定參數設計 
void set(PINIT pinit, PAES_context ctx){
	const char* option[] = {"  位元運算  ", "  操作模式  ", " 加解密模組 ", "  區塊大小  ", "  金鑰長度  ", " 加解密模式 ", "  填充模式  ", "  設定Key   ", "   設定IV   ", "   上一頁   "};
	pinit->sum = sizeof(option) / 8;  //  一共有多少個選項
	do{
		pageMenu("設定：", option, pinit->sum, pinit->num);
	}while(!keyEvent(pinit, ctx));
	if(pinit->parm == 2){
		if(pinit->num == 0){
			pinit->page = OPB;
			pinit->num = 0;
		}else if(pinit->num == 1){
			pinit->page = OPMD;
			pinit->num = 0;
		}else if(pinit->num == 2){
			pinit->page = CMDL;
			pinit->num = 0;
		}else if(pinit->num == 3){
			pinit->page = BS;
			pinit->num = 0;
		}else if(pinit->num == 4){
			pinit->page = KS;
			pinit->num = 0;
		}else if(pinit->num == 5){
			pinit->page = CMD;
			pinit->num = 0;
		}else if(pinit->num == 6){
			pinit->page = PDING;
			pinit->num = 0;
		}else if(pinit->num == 7){
			pinit->page = KEYS;
			pinit->num = 0;
		}else if(pinit->num == 8){
			pinit->page = IVS;
			pinit->num = 0;
		}else if(pinit->num == 9){
			pinit->page = MENU;
			pinit->num = 3;
		}
	}
}

//  位元運算設計 
void opb(PINIT pinit, PAES_context ctx){
	const char* option[] = {" 08-bit ", " 32-bit ", " 上一頁 "};
	pinit->sum = sizeof(option) / 8;  //  一共有多少個選項
	do{
		pageMenu("位元運算：", option, pinit->sum, pinit->num);
	}while(!keyEvent(pinit, ctx));
	if(pinit->parm == 2){
		if(pinit->num == 0){
			pinit->is08b = 1;
		}else if(pinit->num == 1){
			pinit->is08b = 0;
		}
		pinit->parm = 1;
		pinit->num = 0;
	}
}

//  操作模式設計 
void opmd(PINIT pinit, PAES_context ctx){
	const char* option[] = {" 數學操作 ", " 查表操作 ", "  上一頁  "};
	pinit->sum = sizeof(option) / 8;  //  一共有多少個選項
	do{
		pageMenu("操作模式：", option, pinit->sum, pinit->num);
	}while(!keyEvent(pinit, ctx));
	if(pinit->parm == 2){
		if(pinit->num == 0){
			pinit->isLU = MATH;
		}else if(pinit->num == 1){
			pinit->isLU = LU;
		}
		pinit->parm = 1;
		pinit->num = 1;
	}
}

//  加解密模組設計 
void cmdl(PINIT pinit, PAES_context ctx){
	const char* option[] = {"   一般模組   ", "   同型模組   ", " 同型整合模組 ", "    上一頁    "};
	pinit->sum = sizeof(option) / 8;  //  一共有多少個選項
	do{
		pageMenu("加解密模組：", option, pinit->sum, pinit->num);
	}while(!keyEvent(pinit, ctx));
	if(pinit->parm == 2){
		if(pinit->num == 0){
			pinit->crypt = 0;
		}else if(pinit->num == 1){
			pinit->crypt = 1;
		}else if(pinit->num == 2){
			pinit->crypt = 2;
		}
		pinit->parm = 1;
		pinit->num = 2;
	}
}

//  區塊大小設計 
void bs(PINIT pinit, PAES_context ctx){
	const char* option[] = {" 128-bit ", " 192-bit ", " 256-bit ", " 上一頁  "};
	pinit->sum = sizeof(option) / 8;  //  一共有多少個選項
	do{
		pageMenu("區塊大小：", option, pinit->sum, pinit->num);
	}while(!keyEvent(pinit, ctx));
	if(pinit->parm == 2){
		if(pinit->num == 0){
			setBlockSize(ctx, 128);
		}else if(pinit->num == 1){
			setBlockSize(ctx, 192);
		}else if(pinit->num == 2){
			setBlockSize(ctx, 256);
		}
		pinit->parm = 1;
		pinit->num = 3;
	}
}

//  金鑰長度設計 
void ks(PINIT pinit, PAES_context ctx){
	const char* option[] = {" 128-bit ", " 192-bit ", " 256-bit ", " 上一頁  "};
	pinit->sum = sizeof(option) / 8;  //  一共有多少個選項
	do{
		pageMenu("金鑰長度：", option, pinit->sum, pinit->num);
	}while(!keyEvent(pinit, ctx));
	if(pinit->parm == 2){
		if(pinit->num == 0){
			setKeySize(ctx, 128);
		}else if(pinit->num == 1){
			setKeySize(ctx, 192);
		}else if(pinit->num == 2){
			setKeySize(ctx, 256);
		}
		pinit->parm = 1;
		pinit->num = 4;
	}
}

//  加解密模式設計 
void cmd(PINIT pinit, PAES_context ctx){
	const char* option[] = {"  ECB 模式  ", "  CBC 模式  ", "  CTR 模式  ", "  OFB 模式  ", " OFB-8 模式 ", " OFB-64 模式", "  CFB 模式  ", " CFB-8 模式 ", " CFB-64 模式", "   上一頁   "};
	pinit->sum = sizeof(option) / 8;  //  一共有多少個選項
	do{
		pageMenu("加解密模式：", option, pinit->sum, pinit->num);
	}while(!keyEvent(pinit, ctx));
	if(pinit->parm == 2){
		if(pinit->num > -1 && pinit->num < 9){
			md = pinit->num;
			setMode(ctx, MODE[md]);
		}
		pinit->parm = 1;
		pinit->num = 5;
	}
}

//  填充模式設計 
void pding(PINIT pinit, PAES_context ctx){
	const char* option[] = {"        No        ", "       Zero       ", "      Space       ", " ISO / IEC 7816-4 ", "     ZeroLen      ",
							"    ANSI X.923    ", "    ISO 10126     ", "      PKCS7       ", "      上一頁      "};
	pinit->sum = sizeof(option) / 8;  //  一共有多少個選項
	do{
		pageMenu("填充模式：", option, pinit->sum, pinit->num);
	}while(!keyEvent(pinit, ctx));
	if(pinit->parm == 2){
		if(pinit->num > -1 && pinit->num < 8){
			setPadding(ctx, pinit->num);
		}
		pinit->parm = 1;
		pinit->num = 6;
	}
}

//  設定金鑰 
void keys(PINIT pinit, PAES_context ctx){
	const char* option[] = {" Key ( ASCII ) ", "  Key ( Hex )  ", "    上一頁     "};
	pinit->sum = sizeof(option) / 8;  //  一共有多少個選項
	do{
		pageMenu("設定Key：", option, pinit->sum, pinit->num);
	}while(!keyEvent(pinit, ctx));
	if(pinit->parm == 2){
		if(pinit->num == 0){
			const char* key = (char*)ctx->key;
			setpws("Key ( ASCII )", &key, ctx->keySize / 8);
			setKEY(ctx, (unsigned char*)key);
		}else if(pinit->num == 1){
			const char* key = (char*)ctx->key;
			setpws("Key ( Hex )", &key, ctx->keySize / 8, 1);
			setKEY(ctx, (unsigned char*)key);
		}
		pinit->parm = 1;
		pinit->num = 7;
	}
}

//  設定IV 
void ivs(PINIT pinit, PAES_context ctx){
	const char* option[] = {" IV ( ASCII ) ", "  IV ( Hex )  ", "    上一頁    "};
	pinit->sum = sizeof(option) / 8;  //  一共有多少個選項
	do{
		pageMenu("設定IV：", option, pinit->sum, pinit->num);
	}while(!keyEvent(pinit, ctx));
	if(pinit->parm == 2){
		if(pinit->num == 0){
			const char* iv = (char*)ctx->iv;
			setpws("IV ( ASCII )", &iv, ctx->blockSize / 8);
			setIV(ctx, (unsigned char*)iv);
		}else if(pinit->num == 1){
			const char* iv = (char*)ctx->iv;
			setpws("IV ( Hex )", &iv, ctx->blockSize / 8, 1);
			setIV(ctx, (unsigned char*)iv);
		}
		pinit->parm = 1;
		pinit->num = 8;
	}
}

//  清空資料夾 
void clf(PINIT pinit, PAES_context ctx){
	const char* option[] = {" 清空 Encrypt 資料夾 ", " 清空 Decrypt 資料夾 ", "  清空 Done 資料夾   ", "   清空全部資料夾    ", "       上一頁        "};
	pinit->sum = sizeof(option) / 8;  //  一共有多少個選項
	do{
		pageMenu("清空資料夾：", option, pinit->sum, pinit->num);
	}while(!keyEvent(pinit, ctx));
	if(pinit->parm == 2){
		if(pinit->num == 0){
			delBox("Encrypt");
		}else if(pinit->num == 1){
			delBox("Decrypt");
		}else if(pinit->num == 2){
			delBox("Done");
		}else if(pinit->num == 3){
			delBox("Encrypt");
			delBox("Decrypt");
			delBox("Done");
			setCursorXY(27, 23);
			printf("  所有資料夾已清空完畢");
		}
		if(pinit->num > -1 && pinit->num < 4){
			setCursorXY(21, 25);
			textHR(34, false);
			setCursorXY(21, 26);
			textCenter("  確定  ", 0xF0, 35, false);
			while(!keyEvent(pinit));
			pinit->parm = 1;
		}else if(pinit->num != -1){
			pinit->page = MENU;
			pinit->num = 4;
		}
	}
}

//  進階工具 
void pro(PINIT pinit, PAES_context ctx){
	const char* option[] = {"  測試加密  ", "  測試解密  ", " 全部加解密 ", "   上一頁   "};
	pinit->sum = sizeof(option) / 8;  //  一共有多少個選項
	do{
		pageMenu("進階工具：", option, pinit->sum, pinit->num);
	}while(!keyEvent(pinit, ctx));
	if(pinit->parm == 2){
		if(pinit->num == 0){
			pinit->page = AESM;
			pinit->num = 4;
		}else if(pinit->num == 1){
			pinit->page = AESM;
			pinit->num = 5;
		}else if(pinit->num == 2){
			pinit->page = AESM;
			pinit->num = 6;
		}else if(pinit->num == 3){
			pinit->page = MENU;
			pinit->num = 5;
		}
	} 
}

void aesm(PINIT pinit, PAES_context ctx){
	char cpath[260], cfile[260];
	int temp_num = pinit->num;
	if(pinit->num % 2 == 0){
		strcpy(cpath, "Encrypt");
	}else{
		strcpy(cpath, "Decrypt");
	}
	findFile(pinit, cpath, cfile);
	if(pinit->num != -1){
		pinit->num = temp_num;
	}
	cls();
	if(pinit->num == 0 || pinit->num == 1){
		box(72, 12, 3, 17);
		dencrypt(cpath, cfile, pinit, ctx);
	}else if(pinit->num == 2){
		box(72, 12, 3, 17);
		box(72, 12, 3, 28);
		dencrypt(cpath, cfile, pinit, ctx);
	}else if(pinit->num == 4 || pinit->num == 5){
		dencrypt(cpath, cfile, pinit, ctx, true);
	}else if(pinit->num == 6){
		box(72, 12, 3, 17);
		box(72, 12, 3, 28);
		dencryptAll(cpath, cfile, pinit, ctx);
	}
	if(pinit->num != -1 && (pinit->num < 3 || pinit->num == 6)){
		keyFlag = 0;
		textCenter("  確定  ", 0xF0);
		textHR();
	}
	if(pinit->num != -1){
		while(!keyEvent(pinit));
	}
	pinit->num = temp_num;
	if(pinit->num < 3){
		pinit->page = MENU;
	}else if(pinit->num < 7){
		pinit->page = PRO;
		pinit->num -= 4;
	}
	pinit->parm = 2;
}

//  加解密選取檔案頁面 
void dencrypt(char cpath[], char cfile[], PINIT pinit, PAES_context ctx, bool print){
	char inpath[256], outpath[256];
	//  加密 
	if(pinit->num == 0 || pinit->num == 4){
		setCursorXY(5, 19);
		if(pinit->num == 0){
			printf("加密");
			setCursorXY(5, 20);
			printf("檔案路徑： %s", cpath);
			setCursorXY(5, 21);
			printf("檔案名稱： %s", cfile);
		}
		sprintf(inpath, "%s/%s", cpath, cfile);  //  加密時開啟檔案的地方 
		sprintf(outpath, "Decrypt/en_%s", cfile);  //  加密後儲存檔案的地方 
	    setFile(ctx, inpath, outpath);
	    switch(pinit->crypt){
	    	case 0:
				AES_Encrypt(ctx, pinit->is08b, pinit->isLU, print);
	    		break;
	    	case 1:
				AES_Crypt(ctx, encrypt, pinit->is08b, pinit->isLU, print);
	    		break;
	    	case 2:
				AES_I_Crypt(ctx, encrypt, pinit->is08b, pinit->isLU, print);
	    		break;
		}
		setCursorXY(0, 29);
	}
	//  解密
	else if(pinit->num == 1 || pinit->num == 5){
		setCursorXY(5, 19);
		if(pinit->num == 1){
			printf("解密");
			setCursorXY(5, 20);
			printf("檔案路徑： %s", cpath);
			setCursorXY(5, 21);
			printf("檔案名稱： %s", cfile);
		}
		sprintf(inpath, "%s/%s", cpath, cfile);  //  解密時開啟檔案的地方 
		sprintf(outpath, "Done/de_%s", strCMP(cfile, "en_") ? cfile + 3 : cfile);  //  解密後儲存檔案的地方，且如果前面為 en_ 則去掉
	    setFile(ctx, inpath, outpath);
	    switch(pinit->crypt){
	    	case 0:
	    		AES_Decrypt(ctx, pinit->is08b, pinit->isLU, print);
	    		break;
	    	case 1:
				AES_Crypt(ctx, decrypt, pinit->is08b, pinit->isLU, print);
	    		break;
	    	case 2:
				AES_I_Crypt(ctx, decrypt, pinit->is08b, pinit->isLU, print);
	    		break;
		}
		setCursorXY(0, 29);
	}
	//  加密後解密 
	else if(pinit->num == 2 || pinit->num == 6){
		setCursorXY(5, 19);
		printf("加密");
		setCursorXY(5, 20);
		printf("檔案路徑： %s", cpath);
		setCursorXY(5, 21);
		printf("檔案名稱： %s", cfile);
		sprintf(inpath, "%s/%s", cpath, cfile);  //  加密時開啟檔案的地方 
		if(pinit->num == 2){
			sprintf(outpath, "Decrypt/en_%s", cfile);  //  加密後儲存檔案的地方 
		}else if(pinit->num == 6){
			sprintf(outpath, "Decrypt/en_%d_%d_%d_%s_%d_%d_%d_%s", pinit->is08b, pinit->isLU, pinit->crypt, ctx->mode, ctx->blockSize, ctx->keySize, ctx->padding, cfile);  //  加密後儲存檔案的地方，並標註所有參數 
		}
	    setFile(ctx, inpath, outpath);
    	switch(pinit->crypt){
	    	case 0:
				AES_Encrypt(ctx, pinit->is08b, pinit->isLU, print);
	    		break;
	    	case 1:
				AES_Crypt(ctx, encrypt, pinit->is08b, pinit->isLU, print);
	    		break;
	    	case 2:
				AES_I_Crypt(ctx, encrypt, pinit->is08b, pinit->isLU, print);
	    		break;
		}
		if(keyFlag){
			setCursorXY(0, 29);
			return;
		}
		setCursorXY(5, 30);
		printf("解密");
		if(pinit->num == 2){
			setCursorXY(5, 31);
			printf("檔案路徑： Decrypt");
			setCursorXY(5, 32);
			printf("檔案名稱： %s", cfile);
			sprintf(inpath, "Decrypt/en_%s", cfile);  //  解密時開啟檔案的地方 
			sprintf(outpath, "Done/de_%s", cfile);  //  解密後儲存檔案的地方 
		}else if(pinit->num == 6){
			setCursorXY(5, 31);
			printf("檔案路徑： Decrypt");
			setCursorXY(5, 32);
			printf("檔案名稱： en_%d_%d_%d_%s_%d_%d_%d_%s", pinit->is08b, pinit->isLU, pinit->crypt, ctx->mode, ctx->blockSize, ctx->keySize, ctx->padding, cfile);
			sprintf(inpath, "Decrypt/en_%d_%d_%d_%s_%d_%d_%d_%s", pinit->is08b, pinit->isLU, pinit->crypt, ctx->mode, ctx->blockSize, ctx->keySize, ctx->padding, cfile);  //  解密時開啟檔案的地方 
			sprintf(outpath, "Done/de_%d_%d_%d_%s_%d_%d_%d_%s", pinit->is08b, pinit->isLU, pinit->crypt, ctx-> mode, ctx->blockSize, ctx->keySize, ctx->padding, cfile);  //  解密後儲存檔案的地方 ，並標註所有參數
		}
	    setFile(ctx, inpath, outpath);
		switch(pinit->crypt){
	    	case 0:
	    		AES_Decrypt(ctx, pinit->is08b, pinit->isLU, print);
	    		break;
	    	case 1:
				AES_Crypt(ctx, decrypt, pinit->is08b, pinit->isLU, print);
	    		break;
	    	case 2:
				AES_I_Crypt(ctx, decrypt, pinit->is08b, pinit->isLU, print);
	    		break;
		}
		setCursorXY(0, 40);
	}
}

void dencryptAll(char cpath[], char cfile[], PINIT pinit, PAES_context ctx){
	for(int i = 0; i < 2; i++){
		pinit->is08b = i;
		for(int j = 0; j < 2; j++){
			pinit->isLU = j;
			for(int k = 0; k < 3; k++){
				pinit->crypt = k;
				for(int l = 0; l < 9; l++){
					ctx->mode = MODE[l];
					for(int m = 0; m < 3; m++){
						ctx->blockSize = m == 0 ? 128 : (m == 1 ? 192 : 256);
						for(int n = 0; n < 3; n++){
							ctx->keySize = n == 0 ? 128 : (n == 1 ? 192 : 256);
							for(int o = 0; o < 8; o++){
								ctx->padding = o;
								keyFlag = 0;
								if(rearKeyEvent()) return;
								dencrypt(cpath, cfile, pinit, ctx);
								if(keyFlag) return; 
							}
						}
					}
				}
			}
		}
	}
}

void findFile(PINIT pinit, char cpath[], char cfile[]){
	struct _finddata_t file;
	char dir[256], filter[256];
	long File;
	bool flagf = 0;  //  判斷是否有檔案 
	int i = 0;  //  判斷有多少個檔案 
	int numf = 0;  //  判斷目前是指到第幾個 
	strcpy(dir, cpath);  //  選擇為加密還是解密開啟的地方 
	strcpy(filter, "*.*");  //  以 . 為切割，判斷是否為檔案 
	_chdir(dir);  //  進入子目錄 
	bool enter = 0;
	while(!enter){
		setCursorXY(0, 19);
		if((File = _findfirst(filter, &file)) != -1){  //  判斷是否有成功開啟 
			_findnext(File, &file);  //  先過濾掉 . 
			i = 0;
			textCenter("選擇檔案：");
			textCenter("");
			while(!_findnext(File, &file) && i < 10){  //  第一次先過濾掉 .. 
				flagf = 1;  //  表示有檔案 
				textCenterf(numf == i ? 0xF0 : 0x0F, " %s ", file.name);  //  判斷是否為選到自己，是則改變顏色
				if(numf == i){
					strcpy(cfile, file.name);  //  取得目前指到的檔案名字 
				}
				textCenter("");
				i++;
			}
			textCenter(" 指定檔案 ", numf == i ? 0xF0 : 0x0F);
			textCenter("");
			i++;
			//  表示沒有檔案 
			if(!flagf){
				textCenter("沒有檔案");
				textCenter("");
				pinit->num = -1;  //  避開其他選項
				textHR();
				textCenter("  確定  ", 0xF0);  //  判斷是否為選到自己，是則改變顏色
			}else{
				textCenter("  上一頁  ", numf == i ? 0xF0 : 0x0F);  //  判斷是否為選到自己，是則改變顏色
				textCenter("");
				i++;
			}
		}
		textHR();
		pinit->num = numf;
		pinit->sum = i;
		enter = keyEvent(pinit);
		numf = pinit->num;
		if(numf == i - 2 && enter == 1){
			const char* tpath;
			if(!targetFile("目標檔案：", &tpath, 64)){
				if(access(tpath, F_OK) != -1){
					splitFile(tpath, cpath, cfile);
				}else{
					enter = 0;
					COORD cursorPosition = getCursorXY();
					int y =  cursorPosition.Y - 3;  //  小視窗擺放的位置
					box(30, 5, 24, y);
					setCursorXY(34, y + 1);
					printf("找不到路徑");
					setCursorXY(24, y + 2);
					textHR(28, false);
					setCursorXY(24, y + 3);
					textCenter("  確定  ", 0xF0, 29, false);
					pinit->num = numf;
					pinit->sum = i;
					enter = keyEvent(pinit);
					numf = pinit->num;
					enter = 0;
				}
			}else{
				enter = 0;
			}
		}
	}
	if(numf == i - 1){
		pinit->num = -1;  //  避開其他選項
	}else{
		textCenterf("%s", cfile);
	}
	_chdir("..");  //  返回原目錄 
}

