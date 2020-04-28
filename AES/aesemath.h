/*
 *     檔名： aesemath.h
 *     全名： aes expansion math
 *     作者： Cerasus
 *     描述： 
 */

#ifndef _AESEMATH_H_ 
#define _AESEMATH_H_

#include <string.h>


/********************  宣告函式  ********************/ 

/********************
 *  計算位元組裡面共有幾個 1 
 ********************/ 
unsigned char bitCount(unsigned char value);

/********************
 *  有限域上的乘法反元素 GF(2^8)
 ********************/ 
unsigned char mulInv(unsigned char x);

/********************
 *  位元組替換 GF(2^8)
 ********************/ 
unsigned char SubByte(unsigned char subByte);

/********************
 *  位元組替換 GF(2^8)
 ********************/ 
unsigned char SubByteInv(unsigned char subByte);

/********************
 *  有限域上的乘法 GF(2^8)
 ********************/ 
unsigned char xtime(unsigned char x, unsigned char y);

/********************
 *  有限域上的多項式乘法 GF(2^8)
 ********************/ 
unsigned short GfPolyMul(unsigned short x, unsigned short y);

/********************
 *  有限域上的多項式除法 GF(2^8)
 ********************/ 
unsigned short GfPolyDiv(unsigned short x, unsigned short y);

#endif
