/*
 *     �ɦW�G aesemath.h
 *     ���W�G aes expansion math
 *     �@�̡G Cerasus
 *     �y�z�G 
 */

#ifndef _AESEMATH_H_ 
#define _AESEMATH_H_

#include <string.h>


/********************  �ŧi�禡  ********************/ 

/********************
 *  �p��줸�ո̭��@���X�� 1 
 ********************/ 
unsigned char bitCount(unsigned char value);

/********************
 *  ������W�����k�Ϥ��� GF(2^8)
 ********************/ 
unsigned char mulInv(unsigned char x);

/********************
 *  �줸�մ��� GF(2^8)
 ********************/ 
unsigned char SubByte(unsigned char subByte);

/********************
 *  �줸�մ��� GF(2^8)
 ********************/ 
unsigned char SubByteInv(unsigned char subByte);

/********************
 *  ������W�����k GF(2^8)
 ********************/ 
unsigned char xtime(unsigned char x, unsigned char y);

/********************
 *  ������W���h�������k GF(2^8)
 ********************/ 
unsigned short GfPolyMul(unsigned short x, unsigned short y);

/********************
 *  ������W���h�������k GF(2^8)
 ********************/ 
unsigned short GfPolyDiv(unsigned short x, unsigned short y);

#endif
