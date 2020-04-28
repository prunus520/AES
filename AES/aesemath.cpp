#include "aesemath.h"

/********************  定義函式  ********************/ 

unsigned char bitCount(unsigned char value){
    unsigned char count = 0;
    while(value > 0){
        if((value & 1) == 1){  //  判斷最低位元是否為 1 
            count++;
		}
        value >>= 1;
    }
    return count;
}

unsigned char mulInv(unsigned char x){
	//  限域上的輾轉相除法 - 擴展歐幾里得算法 GF(2^8)
	unsigned short i[7] = {0, 1, 0, 0x11B, 0, 1, x};  //   Q A1 A2 A3 B1 B2 B3
	unsigned short v[7] = {0};  //  Q' A1' A2' A3' B1' B2' B3'
	while(true){
		if(i[6] == 0){  //  B3 = 0， x 沒有反元素 
			return 0;
		} 
		if(i[6] == 1){  //  B3 = 1，B2 為 x 反元素 
			return i[5];
		} 
		v[0] = GfPolyDiv(i[3], i[6]);  //  Q' = A3 / B3
		//  把 B 複製到 A'
		v[1] = i[4];
		v[2] = i[5];
		v[3] = i[6];
		//  B' = A - Q * B 
		v[4] = i[1] ^ GfPolyMul(v[0], i[4]);
		v[5] = i[2] ^ GfPolyMul(v[0], i[5]); 
		v[6] = i[3] ^ GfPolyMul(v[0], i[6]);
		memcpy(i, v, 14);  //  將計算好的 v 放回 i 
	}
}

unsigned char SubByte(unsigned char subByte){
	unsigned char s_box, S_inv, byte = 0;
	S_inv = mulInv(subByte);  //  乘法反元素 
    s_box = 0xF1; 
    for(unsigned char j = 0; j < 8; j++) {
            s_box = s_box >> 1 | s_box << 7;  //  往左旋轉位移 1 位元 
            byte += bitCount(S_inv & s_box) % 2 * (1 << 7 - j);  //  乘法反元素 AND S盒後計算裡面有幾個 1，如果為奇數則為 1；偶數則為 0，並左移相對應位置 
    }
    return byte ^ 99;
}

unsigned char SubByteInv(unsigned char subByte){
	unsigned char s_box, byte = 0;
    s_box = 0xA4;
    for(unsigned char j = 0; j < 8; j++) {
            s_box = s_box >> 1 | s_box << 7;  //  往左旋轉位移 1 位元 
            byte += bitCount(subByte & s_box) % 2 * (1 << 7 - j);  //  乘法反元素 AND S盒後計算裡面有幾個 1，如果為奇數則為 1；偶數則為 0，並左移相對應位置 
    }
    return mulInv(byte ^ 5);  //  用乘法反元素還原 
}

unsigned char xtime(unsigned char x, unsigned char y){
	unsigned char s = 0;
	for(unsigned char i = 0; i < 7; i++){
		s = (s << 1) ^ (((s >> 7) & 1) * 0x1B);  //  左移 1，如果原最高位為 1，則再異或 0x1B 
		x <<= 1; 
		if(x >> 7 == 1){  //  判斷最左邊是否為 1 
			s ^= y;
		}
	}
	return s;
}

unsigned short GfPolyMul(unsigned short x, unsigned short y){
	unsigned short s = 0;
	for(unsigned char i = 0; i < 15; i++){
		s <<= 1;
		x <<= 1;
		if(x >> 15 == 1){  //  判斷最左邊是否為 1
			s ^= y;
		}
	}
	return s;
}

unsigned short GfPolyDiv(unsigned short x, unsigned short y){
	unsigned char s = 0;
	unsigned char i;
	//  確定 y 第幾位元為 1 
	for(i = 0; i < 16; i++){
		if(y >> 15 == 1){
			break;
		}
		y <<= 1;
	}
	//  如果最高位為 1，則異或 y，並 s 加 1 
	for(unsigned char j = 0; j < i + 1; j++){
		s <<= 1;
		if(x >> 15 == 1){
			x ^= y;
			s += 1;
		}
		x <<= 1;
	}
	return s;
}

