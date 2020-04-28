#include "aesemath.h"

/********************  �w�q�禡  ********************/ 

unsigned char bitCount(unsigned char value){
    unsigned char count = 0;
    while(value > 0){
        if((value & 1) == 1){  //  �P�_�̧C�줸�O�_�� 1 
            count++;
		}
        value >>= 1;
    }
    return count;
}

unsigned char mulInv(unsigned char x){
	//  ����W������۰��k - �X�i�ڴX���o��k GF(2^8)
	unsigned short i[7] = {0, 1, 0, 0x11B, 0, 1, x};  //   Q A1 A2 A3 B1 B2 B3
	unsigned short v[7] = {0};  //  Q' A1' A2' A3' B1' B2' B3'
	while(true){
		if(i[6] == 0){  //  B3 = 0�A x �S���Ϥ��� 
			return 0;
		} 
		if(i[6] == 1){  //  B3 = 1�AB2 �� x �Ϥ��� 
			return i[5];
		} 
		v[0] = GfPolyDiv(i[3], i[6]);  //  Q' = A3 / B3
		//  �� B �ƻs�� A'
		v[1] = i[4];
		v[2] = i[5];
		v[3] = i[6];
		//  B' = A - Q * B 
		v[4] = i[1] ^ GfPolyMul(v[0], i[4]);
		v[5] = i[2] ^ GfPolyMul(v[0], i[5]); 
		v[6] = i[3] ^ GfPolyMul(v[0], i[6]);
		memcpy(i, v, 14);  //  �N�p��n�� v ��^ i 
	}
}

unsigned char SubByte(unsigned char subByte){
	unsigned char s_box, S_inv, byte = 0;
	S_inv = mulInv(subByte);  //  ���k�Ϥ��� 
    s_box = 0xF1; 
    for(unsigned char j = 0; j < 8; j++) {
            s_box = s_box >> 1 | s_box << 7;  //  ��������첾 1 �줸 
            byte += bitCount(S_inv & s_box) % 2 * (1 << 7 - j);  //  ���k�Ϥ��� AND S����p��̭����X�� 1�A�p�G���_�ƫh�� 1�F���ƫh�� 0�A�å����۹�����m 
    }
    return byte ^ 99;
}

unsigned char SubByteInv(unsigned char subByte){
	unsigned char s_box, byte = 0;
    s_box = 0xA4;
    for(unsigned char j = 0; j < 8; j++) {
            s_box = s_box >> 1 | s_box << 7;  //  ��������첾 1 �줸 
            byte += bitCount(subByte & s_box) % 2 * (1 << 7 - j);  //  ���k�Ϥ��� AND S����p��̭����X�� 1�A�p�G���_�ƫh�� 1�F���ƫh�� 0�A�å����۹�����m 
    }
    return mulInv(byte ^ 5);  //  �έ��k�Ϥ����٭� 
}

unsigned char xtime(unsigned char x, unsigned char y){
	unsigned char s = 0;
	for(unsigned char i = 0; i < 7; i++){
		s = (s << 1) ^ (((s >> 7) & 1) * 0x1B);  //  ���� 1�A�p�G��̰��쬰 1�A�h�A���� 0x1B 
		x <<= 1; 
		if(x >> 7 == 1){  //  �P�_�̥���O�_�� 1 
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
		if(x >> 15 == 1){  //  �P�_�̥���O�_�� 1
			s ^= y;
		}
	}
	return s;
}

unsigned short GfPolyDiv(unsigned short x, unsigned short y){
	unsigned char s = 0;
	unsigned char i;
	//  �T�w y �ĴX�줸�� 1 
	for(i = 0; i < 16; i++){
		if(y >> 15 == 1){
			break;
		}
		y <<= 1;
	}
	//  �p�G�̰��쬰 1�A�h���� y�A�� s �[ 1 
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

