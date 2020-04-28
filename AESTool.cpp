/*
 *     檔名： AESTool.cpp
 *     全名： AES Tool
 *     作者： Cerasus
 *     描述： 
 */

#include "rrdsgn.h"

int main(){
	const char* key = "abcdefghijklmnop";
	const char* iv = "0000000000000000";
	PAES_context ctx = PAES_create(MODE[md], 128, (unsigned char*)iv, 128, (unsigned char*)key, 7);
	INIT init = {1, LU, 0, 0, 0, 0, MENU};  //  初始化  
	PINIT pinit = &init;
	
	setWindows("AESTool", 77, 47, 500, 10);
	logo();
	while(pinit->page){
		if(pinit->num == -1){
			pinit->num = 0;
		}
		if(pinit->parm != 2){
			parms(pinit, ctx);
		}
		if(pinit->page == MENU){
			menu(pinit, ctx);
		}else if(pinit->page == SET){
			set(pinit, ctx);
		}else if(pinit->page == OPB){
			pinit->page = SET;
			opb(pinit, ctx);
		}else if(pinit->page == OPMD){
			pinit->page = SET;
			opmd(pinit, ctx);
		}else if(pinit->page == CMDL){
			pinit->page = SET;
			cmdl(pinit, ctx);
		}else if(pinit->page == BS){
			pinit->page = SET;
			bs(pinit, ctx);
		}else if(pinit->page == KS){
			pinit->page = SET;
			ks(pinit, ctx);
		}else if(pinit->page == CMD){
			pinit->page = SET;
			cmd(pinit, ctx);
		}else if(pinit->page == PDING){
			pinit->page = SET;
			pding(pinit, ctx);
		}else if(pinit->page == KEYS){
			pinit->page = SET;
			keys(pinit, ctx);
		}else if(pinit->page == IVS){
			pinit->page = SET;
			ivs(pinit, ctx);
		}else if(pinit->page == CLF){
			clf(pinit, ctx);
		}else if(pinit->page == PRO){
			pro(pinit, ctx);
		}else if(pinit->page == AESM){
			aesm(pinit, ctx);
		}
		if(pinit->parm == 2){
			cls();
		}
	}
	PAES_free(ctx);
	 
	return 0;
}

