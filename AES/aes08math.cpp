#include "aes08math.h"

namespace math08{
		
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
	
	//  前移四個的位元組 ( 往前移一個字組 )，並左旋轉位移 1 個位元組，再回傳計算後的位置 
	unsigned short RotByte(unsigned short shiftByte){
		return shiftByte / 4 * 4 - 4 + (shiftByte + 1) % 4;
	}
	
	//  金鑰回合常數 
	unsigned char Rcon(unsigned char round){
		unsigned char Rc = 1;  //  初始值為 1 
		for(unsigned char i = 0; i < round; i++){
			Rc = Rc << 1 ^ ((Rc >> 7) * 0x1B);  //  如果溢出則異或 0x1B 
		}
		return Rc;
	}
	
	//  金鑰擴展 
	void KeyExpansion(PAES_context ctx){
		unsigned short i = 0;
		unsigned char temp;
		unsigned char N = ctx->Nr - 6;  //  取得 max(Nk, Nb) 
		//  複製前 Nk 塊金鑰
		while(i < 4 * ctx->Nk){
			ctx->ExpandedKey.EKey08[i] = ctx->key[i];
			i++;
		}
		//  之後的回合金鑰 
		while(i < 4 * ctx->Nb * (1 + ctx->Nr)){
			if (i / 4 % N == 0){
				temp = SubByte(ctx->ExpandedKey.EKey08[RotByte(i)]);  //  記錄往前移四個的位元組 ( 往前移一個字組 ) ，並且左旋轉位移 1 個位元組，再位元組替換 
				if(i % (4 * N) == 0){
					temp ^= Rcon(i / (4 * N) - 1);  //  假如 i 為 金鑰大小的倍數，則異或金鑰回合常數，第一回為 Rcon[0] 以此類推 
				}
			}else if(i / 4 % N == 0 && ctx->Nk == 8){
				temp = SubByte(ctx->ExpandedKey.EKey08[i - 4]);  //  如果金鑰大小為 256 - bit，且字組 ( i / 4 ) 為 Nb 的倍數，則記錄往前移四個的位元組 ( 往前移一個字組 )，並位元組替換 
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
			state[i] = SubByte(state[i]);
		}
	}
	
	//  逆位元組替換
	void SubBytesInv(PAES_context ctx, unsigned char state[]){
		for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
			state[i] = SubByteInv(state[i]);
		}
	}
	
	//  位移列
	void ShiftRows(PAES_context ctx, unsigned char state[]){
		unsigned char temp[4 * ctx->Nb];
		copyArray(temp, state, 4 * ctx->Nb);
		for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
			//  取得每一列相對應的位置，並左移，且不超過 Nb，超過則從 0 開始 
			state[i] = temp[(i + i % 4 * 4) % (4 * ctx->Nb)]; 
		}
	}
	
	//  逆位移列
	void ShiftRowsInv(PAES_context ctx, unsigned char state[]){
		unsigned char temp[4 * ctx->Nb];
		copyArray(temp, state, 4 * ctx->Nb);
		for(unsigned char i = 0; i < 4 * ctx->Nb; i++){
			//  取得每一列相對應的位置，並右移，且不小於 0，超過則從 Nb 開始
			state[i] = temp[(4 * ctx->Nb + i - i % 4 * 4) % (4 * ctx->Nb)];
		}
	}
	
	//  混合行 
	void MixColumns(PAES_context ctx, unsigned char state[]){
		unsigned char temp[4 * ctx->Nb];
		copyArray(temp, state, 4 * ctx->Nb);
		for(unsigned char i = 0; i < 4 * ctx->Nb; i += 4) {
			for(unsigned char j = 0; j < 4; j++){
				//  每行以不同程度往上旋轉 N 位元組 ( 0 1 2 3 ( 0 ) -> 1 2 3 0 ( 1 ) -> 2 3 1 0 ( 2 ) 循環下去 ) 
				//  則對應到 2 3 1 1 
				state[i + j] = xtime(2, temp[i + j]) ^ xtime(3, temp[i + (j + 1) % 4]) ^ xtime(1, temp[i + (j + 2) % 4]) ^ xtime(1, temp[i + (j + 3) % 4]);
			}
		}
	}
	
	//  逆混合行 
	void MixColumnsInv(PAES_context ctx, unsigned char state[]){
		unsigned char temp[4 * ctx->Nb];
		copyArray(temp, state, 4 * ctx->Nb);
		for(unsigned char i = 0; i < 4 * ctx->Nb; i += 4) {
			for(unsigned char j = 0; j < 4; j++){
				//  每行以不同程度往上旋轉 N 位元組 ( 0 1 2 3 ( 0 ) -> 1 2 3 0 ( 1 ) -> 2 3 1 0 ( 2 ) 循環下去 ) 
				//  則對應到 14 11 13 9 
				state[i + j] = xtime(14, temp[i + j]) ^ xtime(11, temp[i + (j + 1) % 4]) ^ xtime(13, temp[i + (j + 2) % 4]) ^ xtime(9, temp[i + (j + 3) % 4]);
			}
		}
	}
	
	//  同型 - 回合金鑰加法
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
}
