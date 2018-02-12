/** ************************************************************************* *
 * @file bits.h
 *
 * @project ADDA
 * Copyright (c) 2015-2017 Heptagon
 *
 * @author BHa <bryant.hansen@mesa-imaging.ch>
 *
 ** ************************************************************************* */

#ifndef _BITS_H_
#define _BITS_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned int
#endif //WIN32

#ifndef TRUE
#define TRUE 1U
#define FALSE 0U
#endif

#define SETBIT(ADDRESS,BIT)   (ADDRESS |= (1<<BIT))
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#define FLIPBIT(ADDRESS,BIT)  (ADDRESS ^= (1<<BIT))
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT))

#define SETBITMASK(x,y)   (x |= (y))
#define CLEARBITMASK(x,y) (x &= (~y))
#define FLIPBITMASK(x,y)  (x ^= (y))
#define CHECKBITMASK(x,y) (x & (y))

#define SETMASKED(val, field) ( ((val) << (field##_SHIFT)) & (field##_MASK) )
#define GETMASKED(val, field) ( ((val) & (field##_MASK)) >> (field##_SHIFT) )
#define SETBOOL(val, field) ( (val) ? (field) : 0 )
#define GETBOOL(val, field) ( ((val) & (field)) /= 0 )

/*Note BIT count is 0-base -- Starts with 0*/
#define BIT0  (1 << 0)
#define BIT1  (1 << 1)
#define BIT2 (1 << 2)
#define BIT3 (1 << 3)
#define BIT4 (1 << 4)
#define BIT5 (1 << 5)
#define BIT6 (1 << 6)
#define BIT7 (1 << 7)
#define BIT8 (1 << 8)
#define BIT9 (1 << 9)
#define BIT10 (1 << 10)
#define BIT11 (1 << 11)
#define BIT12 (1 << 12)
#define BIT13 (1 << 13)
#define BIT14 (1 << 14)
#define BIT15 (1 << 15)
#define BIT16 (1 << 16)
#define BIT17 (1 << 17)
#define BIT18 (1 << 18)
#define BIT19 (1 << 19)
#define BIT20 (1 << 20)
#define BIT21 (1 << 21)
#define BIT22 (1 << 22)
#define BIT23 (1 << 23)
#define BIT24 (1 << 24)
#define BIT25 (1 << 25)
#define BIT26 (1 << 26)
#define BIT27 (1 << 27)
#define BIT28 (1 << 28)
#define BIT29 (1 << 29)
#define BIT30 (1 << 30)
#define BIT31 (1 << 31)

#ifdef __cplusplus
}
#endif

#endif //_BITS_H_

/******************** END of bits.h *****************/