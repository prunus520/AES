#include "rrdsgn.h"

unsigned char md = 0;
const char* MODE[] = {"ECB", "CBC", "CTR", "OFB", "OFB8", "OFB64", "CFB", "CFB8", "CFB64"};
typedef INIT *PINIT;

/********************  �w�q�禡  ********************/ 

//  ��L�ƥ� 
bool keyEvent(PINIT pinit, PAES_context ctx){
	DWORD cnt;
    INPUT_RECORD input;
    char c;
	
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_LINE_INPUT | ENABLE_INSERT_MODE | ENABLE_MOUSE_INPUT);  //  ���o�r�����Ϋ��U Enter�B�T�Υ��k�� 
	
    while(true){  //  Ū����J���ʧ@ 
        if(kbhit() != 0){  //  �P�_�O�_����L�ƥ�A�B�����U�����A 
        	c = getch();
            if(c == 0x48 || c == 0x4B){  //  ���U Up �M Left
            	//  �ﶵ���W���ʡA�C�� 0 �h�^�� sum - 1 
				pinit->num--;
				if(pinit->num < 0){
					pinit->num = pinit->sum - 1;
				}
                break;
			}else if(c == 0x50 || c == 0x4D){  //  ���U Down �M Right
				//  �ﶵ���U���ʡA�W�L�h�^�� 0 
				pinit->num++;
				pinit->num %= pinit->sum;
                break;
			}else if(c == 0x0D){  //  ���U Enter 
				return 1;
			}else if(c == 0x1B){  //  ���U Esc�A�B���O��歶�� 
				if(pinit->page != MENU){
					pinit->num = pinit->sum - 1;  //  �C�ӭ����̫�@�ӿﶵ���W�@�� 
					return 1;
				}else{
					exit(0);
				}
			}else if(c > 0x3A && c < 0x3F && pinit->page != c - 0x3A){  //  ���U F1 ~ F2 �B���O�ۤv������ 
				pinit->page = c - 0x3A;  //  �]�w�ۤv������ MENU = 1�A F1 = 0x70�A0x70 - 0x6F = 0x01(16) = 1(10) 
				pinit->num = -1;  //  �׶}��L�ﶵ 
				pinit->parm = 2;
				return 1;
			}else if(pinit->page != AESM){  //  �P�_�O���O�[�ѱK�e�� 
				//  ���U�Ʀr��A�������ػP ASCII ���N�X�A�]�w AES �Ѽƪ��ֱ��� 
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

//  �Ӽг]�p 
void logo(void){
	printf("\n");
	textHR();
	textCenter("");
	textCenter("Advanced Encryption Standard �[�ѱK");
	textCenter("");
	textCenter("by  Cerasus");
	textCenter("");
	textHR();
	textCenter("");
}

//  �ѼƳ]�p 
void parms(PINIT pinit, PAES_context ctx){
	const char* pad[] = {"No", "Zero", "Space", "ISOIEC78164", "ZeroLen", "ANSIX923", "ISO10126", "PKCS7"};
	setCursorXY(0, 9);
	textCenterf("%s %s�k %s�Ҳ�", pinit->is08b ? "8-bit" : "32-bit", pinit->isLU ? "�d��" : "�ƾ�", pinit->crypt ? pinit->crypt == 2 ? "�P����X" : "�P��" : "�@��");
	textCenter("");
	textCenterf("�϶�-���_-�Ҧ�-��R�G%d-%d-%s-%sPadding", ctx->blockSize, ctx->keySize, ctx->mode, pad[ctx->padding]);
	textCenter("");
	textCenterf("key�G%s", ctx->key);
	textCenter("");
	textCenterf("IV�G%s", ctx->iv);
	textCenter("");
	textHR();
	textCenter("");
	pinit->parm = 2;
}

//  ���]�p 
void menu(PINIT pinit, PAES_context ctx){
	const char* option[] = {"  �[�K�ɮ�  ", "  �ѱK�ɮ�  ", " �[�ѱK�ɮ� ", "    �]�w    ", " �M�Ÿ�Ƨ� ", "  �i���u��  ", "    ���}    "};
	pinit->sum = sizeof(option) / 8;  //  �@�@���h�֭ӿﶵ 
	do{
		pageMenu("���G", option, pinit->sum, pinit->num);
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

//  �]�w�ѼƳ]�p 
void set(PINIT pinit, PAES_context ctx){
	const char* option[] = {"  �줸�B��  ", "  �ާ@�Ҧ�  ", " �[�ѱK�Ҳ� ", "  �϶��j�p  ", "  ���_����  ", " �[�ѱK�Ҧ� ", "  ��R�Ҧ�  ", "  �]�wKey   ", "   �]�wIV   ", "   �W�@��   "};
	pinit->sum = sizeof(option) / 8;  //  �@�@���h�֭ӿﶵ
	do{
		pageMenu("�]�w�G", option, pinit->sum, pinit->num);
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

//  �줸�B��]�p 
void opb(PINIT pinit, PAES_context ctx){
	const char* option[] = {" 08-bit ", " 32-bit ", " �W�@�� "};
	pinit->sum = sizeof(option) / 8;  //  �@�@���h�֭ӿﶵ
	do{
		pageMenu("�줸�B��G", option, pinit->sum, pinit->num);
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

//  �ާ@�Ҧ��]�p 
void opmd(PINIT pinit, PAES_context ctx){
	const char* option[] = {" �ƾǾާ@ ", " �d��ާ@ ", "  �W�@��  "};
	pinit->sum = sizeof(option) / 8;  //  �@�@���h�֭ӿﶵ
	do{
		pageMenu("�ާ@�Ҧ��G", option, pinit->sum, pinit->num);
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

//  �[�ѱK�Ҳճ]�p 
void cmdl(PINIT pinit, PAES_context ctx){
	const char* option[] = {"   �@��Ҳ�   ", "   �P���Ҳ�   ", " �P����X�Ҳ� ", "    �W�@��    "};
	pinit->sum = sizeof(option) / 8;  //  �@�@���h�֭ӿﶵ
	do{
		pageMenu("�[�ѱK�ҲաG", option, pinit->sum, pinit->num);
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

//  �϶��j�p�]�p 
void bs(PINIT pinit, PAES_context ctx){
	const char* option[] = {" 128-bit ", " 192-bit ", " 256-bit ", " �W�@��  "};
	pinit->sum = sizeof(option) / 8;  //  �@�@���h�֭ӿﶵ
	do{
		pageMenu("�϶��j�p�G", option, pinit->sum, pinit->num);
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

//  ���_���׳]�p 
void ks(PINIT pinit, PAES_context ctx){
	const char* option[] = {" 128-bit ", " 192-bit ", " 256-bit ", " �W�@��  "};
	pinit->sum = sizeof(option) / 8;  //  �@�@���h�֭ӿﶵ
	do{
		pageMenu("���_���סG", option, pinit->sum, pinit->num);
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

//  �[�ѱK�Ҧ��]�p 
void cmd(PINIT pinit, PAES_context ctx){
	const char* option[] = {"  ECB �Ҧ�  ", "  CBC �Ҧ�  ", "  CTR �Ҧ�  ", "  OFB �Ҧ�  ", " OFB-8 �Ҧ� ", " OFB-64 �Ҧ�", "  CFB �Ҧ�  ", " CFB-8 �Ҧ� ", " CFB-64 �Ҧ�", "   �W�@��   "};
	pinit->sum = sizeof(option) / 8;  //  �@�@���h�֭ӿﶵ
	do{
		pageMenu("�[�ѱK�Ҧ��G", option, pinit->sum, pinit->num);
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

//  ��R�Ҧ��]�p 
void pding(PINIT pinit, PAES_context ctx){
	const char* option[] = {"        No        ", "       Zero       ", "      Space       ", " ISO / IEC 7816-4 ", "     ZeroLen      ",
							"    ANSI X.923    ", "    ISO 10126     ", "      PKCS7       ", "      �W�@��      "};
	pinit->sum = sizeof(option) / 8;  //  �@�@���h�֭ӿﶵ
	do{
		pageMenu("��R�Ҧ��G", option, pinit->sum, pinit->num);
	}while(!keyEvent(pinit, ctx));
	if(pinit->parm == 2){
		if(pinit->num > -1 && pinit->num < 8){
			setPadding(ctx, pinit->num);
		}
		pinit->parm = 1;
		pinit->num = 6;
	}
}

//  �]�w���_ 
void keys(PINIT pinit, PAES_context ctx){
	const char* option[] = {" Key ( ASCII ) ", "  Key ( Hex )  ", "    �W�@��     "};
	pinit->sum = sizeof(option) / 8;  //  �@�@���h�֭ӿﶵ
	do{
		pageMenu("�]�wKey�G", option, pinit->sum, pinit->num);
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

//  �]�wIV 
void ivs(PINIT pinit, PAES_context ctx){
	const char* option[] = {" IV ( ASCII ) ", "  IV ( Hex )  ", "    �W�@��    "};
	pinit->sum = sizeof(option) / 8;  //  �@�@���h�֭ӿﶵ
	do{
		pageMenu("�]�wIV�G", option, pinit->sum, pinit->num);
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

//  �M�Ÿ�Ƨ� 
void clf(PINIT pinit, PAES_context ctx){
	const char* option[] = {" �M�� Encrypt ��Ƨ� ", " �M�� Decrypt ��Ƨ� ", "  �M�� Done ��Ƨ�   ", "   �M�ť�����Ƨ�    ", "       �W�@��        "};
	pinit->sum = sizeof(option) / 8;  //  �@�@���h�֭ӿﶵ
	do{
		pageMenu("�M�Ÿ�Ƨ��G", option, pinit->sum, pinit->num);
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
			printf("  �Ҧ���Ƨ��w�M�ŧ���");
		}
		if(pinit->num > -1 && pinit->num < 4){
			setCursorXY(21, 25);
			textHR(34, false);
			setCursorXY(21, 26);
			textCenter("  �T�w  ", 0xF0, 35, false);
			while(!keyEvent(pinit));
			pinit->parm = 1;
		}else if(pinit->num != -1){
			pinit->page = MENU;
			pinit->num = 4;
		}
	}
}

//  �i���u�� 
void pro(PINIT pinit, PAES_context ctx){
	const char* option[] = {"  ���ե[�K  ", "  ���ոѱK  ", " �����[�ѱK ", "   �W�@��   "};
	pinit->sum = sizeof(option) / 8;  //  �@�@���h�֭ӿﶵ
	do{
		pageMenu("�i���u��G", option, pinit->sum, pinit->num);
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
		textCenter("  �T�w  ", 0xF0);
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

//  �[�ѱK����ɮ׭��� 
void dencrypt(char cpath[], char cfile[], PINIT pinit, PAES_context ctx, bool print){
	char inpath[256], outpath[256];
	//  �[�K 
	if(pinit->num == 0 || pinit->num == 4){
		setCursorXY(5, 19);
		if(pinit->num == 0){
			printf("�[�K");
			setCursorXY(5, 20);
			printf("�ɮ׸��|�G %s", cpath);
			setCursorXY(5, 21);
			printf("�ɮצW�١G %s", cfile);
		}
		sprintf(inpath, "%s/%s", cpath, cfile);  //  �[�K�ɶ}���ɮת��a�� 
		sprintf(outpath, "Decrypt/en_%s", cfile);  //  �[�K���x�s�ɮת��a�� 
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
	//  �ѱK
	else if(pinit->num == 1 || pinit->num == 5){
		setCursorXY(5, 19);
		if(pinit->num == 1){
			printf("�ѱK");
			setCursorXY(5, 20);
			printf("�ɮ׸��|�G %s", cpath);
			setCursorXY(5, 21);
			printf("�ɮצW�١G %s", cfile);
		}
		sprintf(inpath, "%s/%s", cpath, cfile);  //  �ѱK�ɶ}���ɮת��a�� 
		sprintf(outpath, "Done/de_%s", strCMP(cfile, "en_") ? cfile + 3 : cfile);  //  �ѱK���x�s�ɮת��a��A�B�p�G�e���� en_ �h�h��
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
	//  �[�K��ѱK 
	else if(pinit->num == 2 || pinit->num == 6){
		setCursorXY(5, 19);
		printf("�[�K");
		setCursorXY(5, 20);
		printf("�ɮ׸��|�G %s", cpath);
		setCursorXY(5, 21);
		printf("�ɮצW�١G %s", cfile);
		sprintf(inpath, "%s/%s", cpath, cfile);  //  �[�K�ɶ}���ɮת��a�� 
		if(pinit->num == 2){
			sprintf(outpath, "Decrypt/en_%s", cfile);  //  �[�K���x�s�ɮת��a�� 
		}else if(pinit->num == 6){
			sprintf(outpath, "Decrypt/en_%d_%d_%d_%s_%d_%d_%d_%s", pinit->is08b, pinit->isLU, pinit->crypt, ctx->mode, ctx->blockSize, ctx->keySize, ctx->padding, cfile);  //  �[�K���x�s�ɮת��a��A�üе��Ҧ��Ѽ� 
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
		printf("�ѱK");
		if(pinit->num == 2){
			setCursorXY(5, 31);
			printf("�ɮ׸��|�G Decrypt");
			setCursorXY(5, 32);
			printf("�ɮצW�١G %s", cfile);
			sprintf(inpath, "Decrypt/en_%s", cfile);  //  �ѱK�ɶ}���ɮת��a�� 
			sprintf(outpath, "Done/de_%s", cfile);  //  �ѱK���x�s�ɮת��a�� 
		}else if(pinit->num == 6){
			setCursorXY(5, 31);
			printf("�ɮ׸��|�G Decrypt");
			setCursorXY(5, 32);
			printf("�ɮצW�١G en_%d_%d_%d_%s_%d_%d_%d_%s", pinit->is08b, pinit->isLU, pinit->crypt, ctx->mode, ctx->blockSize, ctx->keySize, ctx->padding, cfile);
			sprintf(inpath, "Decrypt/en_%d_%d_%d_%s_%d_%d_%d_%s", pinit->is08b, pinit->isLU, pinit->crypt, ctx->mode, ctx->blockSize, ctx->keySize, ctx->padding, cfile);  //  �ѱK�ɶ}���ɮת��a�� 
			sprintf(outpath, "Done/de_%d_%d_%d_%s_%d_%d_%d_%s", pinit->is08b, pinit->isLU, pinit->crypt, ctx-> mode, ctx->blockSize, ctx->keySize, ctx->padding, cfile);  //  �ѱK���x�s�ɮת��a�� �A�üе��Ҧ��Ѽ�
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
	bool flagf = 0;  //  �P�_�O�_���ɮ� 
	int i = 0;  //  �P�_���h�֭��ɮ� 
	int numf = 0;  //  �P�_�ثe�O����ĴX�� 
	strcpy(dir, cpath);  //  ��ܬ��[�K�٬O�ѱK�}�Ҫ��a�� 
	strcpy(filter, "*.*");  //  �H . �����ΡA�P�_�O�_���ɮ� 
	_chdir(dir);  //  �i�J�l�ؿ� 
	bool enter = 0;
	while(!enter){
		setCursorXY(0, 19);
		if((File = _findfirst(filter, &file)) != -1){  //  �P�_�O�_�����\�}�� 
			_findnext(File, &file);  //  ���L�o�� . 
			i = 0;
			textCenter("����ɮסG");
			textCenter("");
			while(!_findnext(File, &file) && i < 10){  //  �Ĥ@�����L�o�� .. 
				flagf = 1;  //  ��ܦ��ɮ� 
				textCenterf(numf == i ? 0xF0 : 0x0F, " %s ", file.name);  //  �P�_�O�_�����ۤv�A�O�h�����C��
				if(numf == i){
					strcpy(cfile, file.name);  //  ���o�ثe���쪺�ɮצW�r 
				}
				textCenter("");
				i++;
			}
			textCenter(" ���w�ɮ� ", numf == i ? 0xF0 : 0x0F);
			textCenter("");
			i++;
			//  ��ܨS���ɮ� 
			if(!flagf){
				textCenter("�S���ɮ�");
				textCenter("");
				pinit->num = -1;  //  �׶}��L�ﶵ
				textHR();
				textCenter("  �T�w  ", 0xF0);  //  �P�_�O�_�����ۤv�A�O�h�����C��
			}else{
				textCenter("  �W�@��  ", numf == i ? 0xF0 : 0x0F);  //  �P�_�O�_�����ۤv�A�O�h�����C��
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
			if(!targetFile("�ؼ��ɮסG", &tpath, 64)){
				if(access(tpath, F_OK) != -1){
					splitFile(tpath, cpath, cfile);
				}else{
					enter = 0;
					COORD cursorPosition = getCursorXY();
					int y =  cursorPosition.Y - 3;  //  �p�����\�񪺦�m
					box(30, 5, 24, y);
					setCursorXY(34, y + 1);
					printf("�䤣����|");
					setCursorXY(24, y + 2);
					textHR(28, false);
					setCursorXY(24, y + 3);
					textCenter("  �T�w  ", 0xF0, 29, false);
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
		pinit->num = -1;  //  �׶}��L�ﶵ
	}else{
		textCenterf("%s", cfile);
	}
	_chdir("..");  //  ��^��ؿ� 
}

