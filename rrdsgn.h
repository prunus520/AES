/*
 *     檔名： rrdsgn.h
 *     全名： rear design
 *     作者： Cerasus
 *     描述： 
 */

#ifndef _RRDSGN_H_ 
#define _RRDSGN_H_

#include "AES/aes.h"

#define MENU 1
#define SET 2
#define CLF 3
#define PRO 4
#define OPB 5 
#define OPMD 6 
#define CMDL 7 
#define BS 8 
#define KS 9 
#define CMD 10 
#define PDING 11 
#define KEYS 12 
#define IVS 13 
#define AESM 14 

extern unsigned char md;
extern const char* MODE[9];

typedef struct{
	bool is08b;
	bool isLU;
	unsigned char crypt;
	int num;
	int sum;
	int parm;
	int page;
	
}INIT, *PINIT;

/********************  宣告函式  ********************/ 

/********************
 *  視窗與事件設定 
 ********************/
bool keyEvent(PINIT pinit, PAES_context ctx = NULL);

/********************
 *  頁面設計
 ********************/
void logo(void);
void parms(PINIT pinit, PAES_context ctx);
void menu(PINIT pinit, PAES_context ctx);
void set(PINIT pinit, PAES_context ctx);
void opb(PINIT pinit, PAES_context ctx);
void opmd(PINIT pinit, PAES_context ctx);
void cmdl(PINIT pinit, PAES_context ctx);
void bs(PINIT pinit, PAES_context ctx);
void ks(PINIT pinit, PAES_context ctx);
void cmd(PINIT pinit, PAES_context ctx);
void pding(PINIT pinit, PAES_context ctx);
void keys(PINIT pinit, PAES_context ctx);
void ivs(PINIT pinit, PAES_context ctx);
void clf(PINIT pinit, PAES_context ctx);
void pro(PINIT pinit, PAES_context ctx);
void aesm(PINIT pinit, PAES_context ctx);
void dencrypt(char cpath[], char cfile[], PINIT pinit, PAES_context ctx, bool print = false);
void dencryptAll(char cpath[], char cfile[], PINIT pinit, PAES_context ctx);

void findFile(PINIT pinit, char cpath[], char cfile[]);

#endif
