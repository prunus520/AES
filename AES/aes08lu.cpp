#include "aes08lu.h"

namespace lu08{

	/********************  定義函式  ********************/ 
	
	
	//  異或陣列
	void xorArray(unsigned char dst[], unsigned char src[], unsigned char size){
		for(unsigned char i = 0; i < size; i++){
			dst[i] ^= src[i];
		}
	}
	
	//  複製陣列
	void copyArray(unsigned char dst[], unsigned char src[], unsigned char size){
	    for (int i = 0; i < size; i++){
	        dst[i] = src[i];
	    }
	}
	
	//  金鑰擴展 
	void KeyExpansion(PAES_context ctx){
		unsigned short i = 0;
		unsigned char temp;
		unsigned char N = ctx->Nr - 6;  //  取得 max(Nk, Nb) 
		unsigned char RotWord[] = {3, 3, 3, 7};  //  前移四個的位元組 ( 往前移一個字組 )，並左旋轉位移 1 個位元組 
		//  複製前 Nk 塊金鑰
		while(i < 4 * ctx->Nk){
			ctx->ExpandedKey.EKey08[i] = ctx->key[i];
			i++;
		}
		//  之後的回合金鑰 
		while(i < 4 * ctx->Nb * (1 + ctx->Nr)){
			if (i / 4 % N == 0){
				temp = S_Box[ctx->ExpandedKey.EKey08[i - RotWord[i % 4]]];  //  記錄往前移四個的位元組 ( 往前移一個字組 ) ，並且左旋轉位移 1 個位元組，再位元組替換 
				if(i % (4 * N) == 0){
					temp ^= Rcon[i / (4 * N) - 1];  //  假如 i 為 金鑰大小的倍數，則異或金鑰回合常數，第一回為 Rcon[0] 以此類推 
				}
			}else if(i / 4 % N == 0 && ctx->Nk == 8){
				temp = S_Box[ctx->ExpandedKey.EKey08[i - 4]];  //  如果金鑰大小為 256 - bit，且字組 ( i / 4 ) 為 Nb 的倍數，則記錄往前移四個的位元組 ( 往前移一個字組 )，並位元組替換 
			}else{
				temp = ctx->ExpandedKey.EKey08[i - 4];  //  記錄往前移四個的位元組 ( 往前移一個字組 )
			}
			ctx->ExpandedKey.EKey08[i] = ctx->ExpandedKey.EKey08[i - 4 * N] ^ temp;  //  將往前移 4 * Nb 個的位元組 ( 往前移 Nb 個字組 ) 與記錄的位元組異或，得到新的擴展金鑰 
			i++;
		}
	}
	
	//  回合金鑰加法
	void AddRoundKey(PAES_context ctx, unsigned char state[], unsigned char roundKey){
		xorArray(state, ctx->ExpandedKey.EKey08 + 4 * ctx->Nb * roundKey, 4 * ctx->Nb);
	}
	
	//  位元組替換
	void SubBytes(PAES_context ctx, unsigned char state[]){
		for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
			state[i] = S_Box[state[i]];
		}
	}
	
	//  逆位元組替換
	void SubBytesInv(PAES_context ctx, unsigned char state[]){
		for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
			state[i] = S_Box_Inv[state[i]];
		}
	}
	
	//  位移列
	void ShiftRows(PAES_context ctx, unsigned char state[]){
		unsigned char temp[4 * ctx->Nb];
		copyArray(temp, state, 4 * ctx->Nb);
		if(ctx->blockSize == 128){
			//  區塊為 128 - bit 時 
			for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
				state[i] = temp[Shift_Rows_128b[i]];
			}
		}else if(ctx->blockSize == 192){
			//  區塊為 192 - bit 時
			for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
				state[i] = temp[Shift_Rows_192b[i]];
			}
		}else if(ctx->blockSize == 256){
			//  區塊為 256 - bit 時
			for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
				state[i] = temp[Shift_Rows_256b[i]];
			}
		}
	}
	
	//  逆位移列
	void ShiftRowsInv(PAES_context ctx, unsigned char state[]){
		unsigned char temp[4 * ctx->Nb];
		copyArray(temp, state, 4 * ctx->Nb);
		if(ctx->blockSize == 128){
			//  區塊為 128 - bit 時
			for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
				state[i] = temp[Shift_Rows_128b_Inv[i]];
			}
		}else if(ctx->blockSize == 192){
			//  區塊為 192 - bit 時
			for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
				state[i] = temp[Shift_Rows_192b_Inv[i]];
			}
		}else if(ctx->blockSize == 256){
			//  區塊為 256 - bit 時
			for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
				state[i] = temp[Shift_Rows_256b_Inv[i]];
			}
		}
	}
	
	//  混合行 
	void MixColumns(PAES_context ctx, unsigned char state[]){
		unsigned char temp[4 * ctx->Nb];
		copyArray(temp, state, 4 * ctx->Nb); 
		for(unsigned char i = 0; i < 4 * ctx->Nb; i += 4) {
			state[i] = Multiply_2[temp[i]] ^ Multiply_3[temp[i + 1]] ^ temp[i + 2] ^ temp[i + 3];  //  2 3 1 1 
			state[i + 1] = temp[i] ^ Multiply_2[temp[i + 1]] ^ Multiply_3[temp[i + 2]] ^ temp[i + 3];  //  1 2 3 1
			state[i + 2] = temp[i] ^ temp[i + 1] ^ Multiply_2[temp[i + 2]] ^ Multiply_3[temp[i + 3]];  //  1 1 2 3
			state[i + 3] = Multiply_3[temp[i]] ^ temp[i + 1] ^ temp[i + 2] ^ Multiply_2[temp[i + 3]];  //  3 1 1 2 
		}
	}
	
	//  逆混合行 
	void MixColumnsInv(PAES_context ctx, unsigned char state[]){
		unsigned char temp[4 * ctx->Nb];
		copyArray(temp, state, 4 * ctx->Nb);
		for(unsigned char i = 0; i < 4 * ctx->Nb; i += 4) {
			state[i] = Multiply_14[temp[i]] ^ Multiply_11[temp[i + 1]] ^ Multiply_13[temp[i + 2]] ^ Multiply_9[temp[i + 3]];  //  14 11 13 9 
			state[i + 1] = Multiply_9[temp[i]] ^ Multiply_14[temp[i + 1]] ^ Multiply_11[temp[i + 2]] ^ Multiply_13[temp[i + 3]];  //  9 14 11 13
			state[i + 2] = Multiply_13[temp[i]] ^ Multiply_9[temp[i + 1]] ^ Multiply_14[temp[i + 2]] ^ Multiply_11[temp[i + 3]];  //  13 9 14 11
			state[i + 3] = Multiply_11[temp[i]] ^ Multiply_13[temp[i + 1]] ^ Multiply_9[temp[i + 2]] ^ Multiply_14[temp[i + 3]];  //  11 13 9 14
		}
	}
	
	//  ( 整合 ) 同型 - 回合金鑰加法
	void AddRoundKey(bool crypt, PAES_context ctx, unsigned char state[], unsigned char roundKey){
		if(crypt){
			AddRoundKey(ctx, state, roundKey);
		}else{
			unsigned short offset = 4 * ctx->Nb * (ctx->Nr - roundKey);  //  解密時由 Nr ~ 0 
			if(roundKey == ctx->Nr || roundKey == 0){  //  最初與最後回合不需要逆混合行 
				xorArray(state, ctx->ExpandedKey.EKey08 + offset, 4 * ctx->Nb);
			}else{
				unsigned char temp[4 * ctx->Nb];
				copyArray(temp, ctx->ExpandedKey.EKey08 + offset, 4 * ctx->Nb);  //  防止金鑰擴展被更改 
				MixColumnsInv(ctx, temp);  //  同型需要再逆混合行 
				xorArray(state, temp, 4 * ctx->Nb);
			}
		}
	}
	
	//  同型 - 位元組替換
	void SubBytes(bool crypt, PAES_context ctx, unsigned char state[]){
		if(crypt){
			SubBytes(ctx, state);
		}else{
			SubBytesInv(ctx, state);
		}
	}
	
	//  同型 - 位移列
	void ShiftRows(bool crypt, PAES_context ctx, unsigned char state[]){
		if(crypt){
			ShiftRows(ctx, state);
		}else{
			ShiftRowsInv(ctx, state);
		}
	}
	
	//  同型 - 混合行 
	void MixColumns(bool crypt, PAES_context ctx, unsigned char state[]){
		if(crypt){
			MixColumns(ctx, state);
		}else{
			MixColumnsInv(ctx, state);
		}
	}
	
	//  整合同型 - 位元組替換
	void I_SubBytes(bool crypt, PAES_context ctx, unsigned char state[]){
		if(crypt){
			ShiftRows(ctx, state);
			SubBytes(ctx, state);
		}else{
			ShiftRowsInv(ctx, state);
			SubBytesInv(ctx, state);
		}
	}
	
	//  整合同型 - 混合行 
	void I_MixColumns(bool crypt, PAES_context ctx, unsigned char state[]){
		if(crypt){
			ShiftRows(ctx, state);
			unsigned char temp[4 * ctx->Nb];
			copyArray(temp, state, 4 * ctx->Nb);
			for(unsigned char i = 0; i < 4 * ctx->Nb; i += 4) {
				state[i] = I_Multiply_2[temp[i]] ^ I_Multiply_3[temp[i + 1]] ^ S_Box[temp[i + 2]] ^ S_Box[temp[i + 3]];  //  2 3 1 1
				state[i + 1] = S_Box[temp[i]] ^ I_Multiply_2[temp[i + 1]] ^ I_Multiply_3[temp[i + 2]] ^ S_Box[temp[i + 3]];  //  1 2 3 1
				state[i + 2] = S_Box[temp[i]] ^ S_Box[temp[i + 1]] ^ I_Multiply_2[temp[i + 2]] ^ I_Multiply_3[temp[i + 3]];  //  1 1 2 3
				state[i + 3] = I_Multiply_3[temp[i]] ^ S_Box[temp[i + 1]] ^ S_Box[temp[i +2]] ^ I_Multiply_2[temp[i + 3]];  // 3 1 1 2
			}
		}else{
			ShiftRowsInv(ctx, state);
			unsigned char temp[4 * ctx->Nb];
			copyArray(temp, state, 4 * ctx->Nb);
			for(unsigned char i = 0; i < 4 * ctx->Nb; i += 4) {
				state[i] = I_Multiply_14[temp[i]] ^ I_Multiply_11[temp[i + 1]] ^ I_Multiply_13[temp[i + 2]] ^ I_Multiply_9[temp[i + 3]];  //  14 11 13 9
				state[i + 1] = I_Multiply_9[temp[i]] ^ I_Multiply_14[temp[i + 1]] ^ I_Multiply_11[temp[i + 2]] ^ I_Multiply_13[temp[i + 3]];  //  9 14 11 13
				state[i + 2] = I_Multiply_13[temp[i]] ^ I_Multiply_9[temp[i + 1]] ^ I_Multiply_14[temp[i + 2]] ^ I_Multiply_11[temp[i + 3]];  //  13 9 14 11
				state[i + 3] = I_Multiply_11[temp[i]] ^ I_Multiply_13[temp[i + 1]] ^ I_Multiply_9[temp[i + 2]] ^ I_Multiply_14[temp[i + 3]];  //  11 13 9 14
			}
		}
	}
}
