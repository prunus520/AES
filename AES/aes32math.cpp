#include "aes32math.h"

namespace math32{
	
	/********************  定義函式  ********************/ 
	
	
	//  異或陣列 32 - bit ^ 08 - bit
	void xorArray(unsigned dst[], unsigned char src[], unsigned char size){
		unsigned temp_src[size / 4] = {0};
		copyArray(temp_src, src, size);
		for(unsigned char i = 0; i < size / 4; i++){
			dst[i] ^= temp_src[i];
		}
	}
	
	//  異或陣列 32 - bit ^ 32 - bit
	void xorArray(unsigned dst[], unsigned src[], unsigned char size){
		for(unsigned char i = 0; i < size; i++){
			dst[i] ^= src[i];
		}
	}
	
	//  複製陣列 08 - bit <- 08 - bit
	void copyArray(unsigned char dst[], unsigned char src[], unsigned char size){
	    for (int i = 0; i < size; i++){
	        dst[i] = src[i];
	    }
	}
	
	//  複製陣列 08 - bit <- 32 - bit ( 直接複製會顛倒，也就是由最低位元組往最高位元組複製 ) 
	void copyArray(unsigned char dst[], unsigned src[], unsigned char size){
		char* temp_src = (char*)src;  //  每 1 個字組拆成 4 個位元組
	    for(int i = 0; i < size; i++){
	        dst[i] = temp_src[i + 3 - 2 * (i % 4)];  //  由最高位元組往低位元組複製 
	    }
	}
	
	//  複製陣列 32 - bit <- 08 - bit ( 直接複製會顛倒，也就是由最低位元組往最高位元組複製 ) 
	void copyArray(unsigned dst[], unsigned char src[], unsigned char size){
		char* temp_dst = (char*)dst;  //  每 1 個字組拆成 4 個位元組
	    for(int i = 0; i < size; i++){
	        temp_dst[i] = src[i + 3 - 2 * (i % 4)];  //  由最高位元組往低位元組複製 
	    }
	}
	
	//  複製陣列 32 - bit <- 32 - bit
	void copyArray(unsigned dst[], unsigned src[], unsigned char size){
	    for(int i = 0; i < size; i++){
	        dst[i] = src[i];
	    }
	}
	
	//  將字節轉換成相對應的位元組 
	unsigned char word2byte(unsigned state, unsigned char offset){
		return state >> offset * 8;
	}
	
	//  左旋轉位移 1 個位元組 
	unsigned RotWord(unsigned shiftWord){
		return shiftWord << 8 ^ shiftWord >> 24;
	}
	
	//  字節替代
	unsigned SubWord(unsigned subWord){
		unsigned temp = 0;
		for(unsigned char i = 0; i < 4; i++){
			//  將字組轉成相對應的位元組，並替代再放回相對應的字組裡
		    temp ^= SubByte(word2byte(subWord, i)) << i * 8;
		}
	    return temp;
	}
	
	//  金鑰回合常數 
	unsigned Rcon(unsigned char round){
		unsigned char Rc = 1;  //  初始值為 1 
		for(unsigned char i = 0; i < round; i++){
			Rc = Rc << 1 ^ ((Rc >> 7) * 0x1B);  //  如果溢出則異或 0x1B 
		}
		return Rc << 24;  //  轉回字節型態 
	}
	
	//  金鑰擴展 
	void KeyExpansion(PAES_context ctx){
		unsigned char i = 0;
		unsigned temp;
		unsigned char N = ctx->Nr - 6;  //  取得 max(Nk, Nb) 
		//  複製前 Nk 塊金鑰
		while(i < ctx->Nk){
			copyArray(ctx->ExpandedKey.EKey32 + i, ctx->key + i * 4, 4);
			i++;
		}
		//  之後的回合金鑰 
		while(i < ctx->Nb * (1 + ctx->Nr)){
			if (i % N == 0){
				temp = SubWord(RotWord(ctx->ExpandedKey.EKey32[i - 1])) ^ Rcon(i / N - 1);  //  記錄前一個字組，並且左旋轉位移 1 個位元組，再位元組替換，然後異或金鑰回合常數
			}else if(i % N == 0 && ctx->Nk == 8){
				temp = SubWord(ctx->ExpandedKey.EKey32[i - 1]);  //  如果金鑰大小為 256 - bit，且字組 ( i / 4 ) 為 Nb 的倍數，則記錄前一個字組，並位元組替換 
			}else{
				temp = ctx->ExpandedKey.EKey32[i - 1];  //  記錄前一個字組
			}
			ctx->ExpandedKey.EKey32[i] = ctx->ExpandedKey.EKey32[i - N] ^ temp;  //  將前 Nb 個字組與記錄的字組異或，得到新的擴展金鑰
			i++;
		}
	}
	
	//  回合金鑰加法
	void AddRoundKey(PAES_context ctx, unsigned state[], unsigned char roundKey){
		xorArray(state, ctx->ExpandedKey.EKey32 + ctx->Nb * roundKey, ctx->Nb);
	}
	
	//  位元組替換
	void SubBytes(PAES_context ctx, unsigned state[]){
		unsigned char* temp_state = (unsigned char*)state;  //  將字組轉變成位元組
	    for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
		    temp_state[i] = SubByte(temp_state[i]);
		}
	}
	
	//  逆位元組替換
	void SubBytesInv(PAES_context ctx, unsigned state[]){
		unsigned char* temp_state = (unsigned char*)state;  //  將字組轉變成位元組
		for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
		    temp_state[i] = SubByteInv(temp_state[i]);
		}
	}
	
	//  位移列
	void ShiftRows(PAES_context ctx, unsigned state[]){
		unsigned temp[ctx->Nb];
		copyArray(temp, state, ctx->Nb);
		for(unsigned char i = 0; i < ctx->Nb; i++){
			//  取得每一列相對應的位置，並左移，且不超過 Nb，超過則從 0 開始 
			state[i] = temp[i] & 0xFF000000 ^ temp[(i + 1) % ctx->Nb] & 0x00FF0000 ^ temp[(i + 2) % ctx->Nb] & 0x0000FF00 ^ temp[(i + 3) % ctx->Nb] & 0x000000FF;
		}
	}
	
	//  逆位移列
	void ShiftRowsInv(PAES_context ctx, unsigned state[]){
		unsigned temp[ctx->Nb];
		copyArray(temp, state, ctx->Nb);
		for(unsigned char i = 0; i < ctx->Nb; i++){
			//  取得每一列相對應的位置，並右移，且不小於 0，超過則從 Nb 開始
			state[i] = temp[i] & 0xFF000000 ^ temp[(i + ctx->Nb - 1) % ctx->Nb] & 0x00FF0000 ^ temp[(i + ctx->Nb - 2) % ctx->Nb] & 0x0000FF00 ^ temp[(i + ctx->Nb - 3) % ctx->Nb] & 0x000000FF;
		}
	}
	
	//  混合行 
	void MixColumns(PAES_context ctx, unsigned state[]){
		unsigned temp[ctx->Nb];
		copyArray(temp, state, ctx->Nb);
		for(unsigned char i = 0; i < ctx->Nb; i++) {
			state[i] = (xtime(2, word2byte(temp[i], 3)) ^ xtime(3, word2byte(temp[i], 2)) ^ xtime(1, word2byte(temp[i], 1)) ^ xtime(1, word2byte(temp[i], 0))) << 24;  //  2 3 1 1
			state[i] ^= (xtime(1, word2byte(temp[i], 3)) ^ xtime(2, word2byte(temp[i], 2)) ^ xtime(3, word2byte(temp[i], 1)) ^ xtime(1, word2byte(temp[i], 0))) << 16;  //  1 2 3 1
			state[i] ^= (xtime(1, word2byte(temp[i], 3)) ^ xtime(1, word2byte(temp[i], 2)) ^ xtime(2, word2byte(temp[i], 1)) ^ xtime(3, word2byte(temp[i], 0))) << 8;  //  1 1 2 3
			state[i] ^= xtime(3, word2byte(temp[i], 3)) ^ xtime(1, word2byte(temp[i], 2)) ^ xtime(1, word2byte(temp[i], 1)) ^ xtime(2, word2byte(temp[i], 0));  //  3 1 1 2
		}
	}
	
	//  逆混合行 
	void MixColumnsInv(PAES_context ctx, unsigned state[]){
		unsigned temp[ctx->Nb];
		copyArray(temp, state, ctx->Nb);
		for(unsigned char i = 0; i < ctx->Nb; i++) {
			state[i] = (xtime(14, word2byte(temp[i], 3)) ^ xtime(11, word2byte(temp[i], 2)) ^ xtime(13, word2byte(temp[i], 1)) ^ xtime(9, word2byte(temp[i], 0))) << 24;  //  14 11 13 9
			state[i] ^= (xtime(9, word2byte(temp[i], 3)) ^ xtime(14, word2byte(temp[i], 2)) ^ xtime(11, word2byte(temp[i], 1)) ^ xtime(13, word2byte(temp[i], 0))) << 16;  //  9 14 11 13
			state[i] ^= (xtime(13, word2byte(temp[i], 3)) ^ xtime(9, word2byte(temp[i], 2)) ^ xtime(14, word2byte(temp[i], 1)) ^ xtime(11, word2byte(temp[i], 0))) << 8;  //  13 9 14 11
			state[i] ^= xtime(11, word2byte(temp[i], 3)) ^ xtime(13, word2byte(temp[i], 2)) ^ xtime(9, word2byte(temp[i], 1)) ^ xtime(14, word2byte(temp[i], 0));  //  11 13 9 14
		}
	}
	
	//  同型 - 回合金鑰加法
	void AddRoundKey(bool crypt, PAES_context ctx, unsigned state[], unsigned char roundKey){
		if(crypt){
			AddRoundKey(ctx, state, roundKey);
		}else{
			unsigned short offset = ctx->Nb * (ctx->Nr - roundKey);  //  解密時由 Nr ~ 0 
			if(roundKey == ctx->Nr || roundKey == 0){  //  最初與最後回合不需要逆混合行 
				xorArray(state, ctx->ExpandedKey.EKey32 + offset, ctx->Nb);
			}else{
				unsigned temp[ctx->Nb];
				copyArray(temp, ctx->ExpandedKey.EKey32 + offset, ctx->Nb);  //  防止金鑰擴展被更改 
				MixColumnsInv(ctx, temp);  //  同型需要再逆混合行 
				xorArray(state, temp, ctx->Nb);
			}
		}
	}
	
	//  同型 - 位元組替換
	void SubBytes(bool crypt, PAES_context ctx, unsigned state[]){
		if(crypt){
			SubBytes(ctx, state);
		}else{
			SubBytesInv(ctx, state);
		}
	}
	
	//  同型 - 位移列
	void ShiftRows(bool crypt, PAES_context ctx, unsigned state[]){
		if(crypt){
			ShiftRows(ctx, state);
		}else{
			ShiftRowsInv(ctx, state);
		}
	}
	
	//  同型 - 混合行 
	void MixColumns(bool crypt, PAES_context ctx, unsigned state[]){
		if(crypt){
			MixColumns(ctx, state);
		}else{
			MixColumnsInv(ctx, state);
		}
	}
}
