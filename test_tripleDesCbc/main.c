#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char dual_vec[] = "qyrzs1t2u3v4w5x6aibjckdlemfngohpMUNVOWPXQYRXSKDL7E8F9G0HAIBJC+T/";

#define MAX_DATA_LENGTH		1024
#define MODE_DUAL_DECRYPT    0

unsigned char pRND[16][16] = {
	91, 92, 26, 17, 172, 55, 29, 55, 93, 77, 253, 23, 21, 148, 220, 84,
	101, 101, 42, 69, 166, 12, 77, 187, 195, 168, 209, 197, 223, 64, 156, 24,
	77, 209, 147, 65, 184, 153, 104, 33, 215, 9, 180, 140, 194, 138, 194, 92,
	134, 174, 94, 23, 27, 205, 76, 239, 1, 11, 137, 112, 12, 121, 244, 133,
	79, 76, 171, 94, 207, 94, 4, 157, 23, 118, 2, 109, 41, 101, 46, 82,
	197, 106, 115, 55, 174, 253, 144, 156, 37, 196, 127, 243, 145, 206, 222, 210,
	55, 88, 169, 197, 246, 163, 215, 148, 214, 77, 69, 80, 202, 54, 209, 2,
	213, 3, 165, 29, 200, 170, 97, 81, 141, 253, 202, 165, 179, 224, 222, 165,
	112, 225, 109, 88, 14, 97, 253, 66, 25, 187, 208, 68, 81, 96, 132, 131,
	57, 233, 206, 194, 117, 194, 160, 95, 95, 179, 145, 98, 55, 252, 149, 19,
	42, 115, 33, 203, 205, 18, 40, 155, 102, 240, 72, 239, 85, 138, 245, 158,
	172, 1, 61, 27, 109, 62, 87, 53, 184, 31, 62, 76, 24, 137, 176, 51,
	27, 11, 122, 129, 203, 222, 248, 52, 23, 84, 29, 95, 210, 193, 126, 182,
	88, 22, 175, 191, 247, 102, 226, 96, 48, 186, 216, 130, 201, 221, 108, 98,
	51, 123, 96, 61, 239, 22, 96, 11, 195, 21, 99, 236, 93, 13, 100, 177,
	19, 147, 106, 95, 195, 81, 83, 56, 233, 101, 211, 8, 16, 137, 158, 195
};

// CTDES
#define TYPE_2KEY 			0
#define TYPE_3KEY			1

#define DES_ENCRYPT     0
#define DES_DECRYPT     1

#define DES_KEY_SIZE    8

/**
 * \brief          DES context structure
 */
typedef struct
{
	int mode;                   /*!<  encrypt/decrypt   */
	unsigned long sk[32];       /*!<  DES subkeys       */
}
des_context;

/**
 * \brief          Triple-DES context structure
 */
typedef struct
{
	int mode;                   /*!<  encrypt/decrypt   */
	unsigned long sk[96];       /*!<  3DES subkeys      */
}
des3_context;

// CTDES 메시지 처리기입니다.
#ifndef GET_ULONG_BE
#define GET_ULONG_BE(n,b,i)                             \
{                                                       \
	(n) = ( (unsigned long) (b)[(i)    ] << 24 )        \
	| ( (unsigned long) (b)[(i) + 1] << 16 )        \
	| ( (unsigned long) (b)[(i) + 2] <<  8 )        \
	| ( (unsigned long) (b)[(i) + 3]       );       \
}
#endif

#ifndef PUT_ULONG_BE
#define PUT_ULONG_BE(n,b,i)                             \
{                                                       \
	(b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
	(b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
	(b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
	(b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

/*
 * Expanded DES S-boxes
 */
static const unsigned long SB1[64] =
{
	0x01010400, 0x00000000, 0x00010000, 0x01010404,
	0x01010004, 0x00010404, 0x00000004, 0x00010000,
	0x00000400, 0x01010400, 0x01010404, 0x00000400,
	0x01000404, 0x01010004, 0x01000000, 0x00000004,
	0x00000404, 0x01000400, 0x01000400, 0x00010400,
	0x00010400, 0x01010000, 0x01010000, 0x01000404,
	0x00010004, 0x01000004, 0x01000004, 0x00010004,
	0x00000000, 0x00000404, 0x00010404, 0x01000000,
	0x00010000, 0x01010404, 0x00000004, 0x01010000,
	0x01010400, 0x01000000, 0x01000000, 0x00000400,
	0x01010004, 0x00010000, 0x00010400, 0x01000004,
	0x00000400, 0x00000004, 0x01000404, 0x00010404,
	0x01010404, 0x00010004, 0x01010000, 0x01000404,
	0x01000004, 0x00000404, 0x00010404, 0x01010400,
	0x00000404, 0x01000400, 0x01000400, 0x00000000,
	0x00010004, 0x00010400, 0x00000000, 0x01010004
};

static const unsigned long SB2[64] =
{
	0x80108020, 0x80008000, 0x00008000, 0x00108020,
	0x00100000, 0x00000020, 0x80100020, 0x80008020,
	0x80000020, 0x80108020, 0x80108000, 0x80000000,
	0x80008000, 0x00100000, 0x00000020, 0x80100020,
	0x00108000, 0x00100020, 0x80008020, 0x00000000,
	0x80000000, 0x00008000, 0x00108020, 0x80100000,
	0x00100020, 0x80000020, 0x00000000, 0x00108000,
	0x00008020, 0x80108000, 0x80100000, 0x00008020,
	0x00000000, 0x00108020, 0x80100020, 0x00100000,
	0x80008020, 0x80100000, 0x80108000, 0x00008000,
	0x80100000, 0x80008000, 0x00000020, 0x80108020,
	0x00108020, 0x00000020, 0x00008000, 0x80000000,
	0x00008020, 0x80108000, 0x00100000, 0x80000020,
	0x00100020, 0x80008020, 0x80000020, 0x00100020,
	0x00108000, 0x00000000, 0x80008000, 0x00008020,
	0x80000000, 0x80100020, 0x80108020, 0x00108000
};

static const unsigned long SB3[64] =
{
	0x00000208, 0x08020200, 0x00000000, 0x08020008,
	0x08000200, 0x00000000, 0x00020208, 0x08000200,
	0x00020008, 0x08000008, 0x08000008, 0x00020000,
	0x08020208, 0x00020008, 0x08020000, 0x00000208,
	0x08000000, 0x00000008, 0x08020200, 0x00000200,
	0x00020200, 0x08020000, 0x08020008, 0x00020208,
	0x08000208, 0x00020200, 0x00020000, 0x08000208,
	0x00000008, 0x08020208, 0x00000200, 0x08000000,
	0x08020200, 0x08000000, 0x00020008, 0x00000208,
	0x00020000, 0x08020200, 0x08000200, 0x00000000,
	0x00000200, 0x00020008, 0x08020208, 0x08000200,
	0x08000008, 0x00000200, 0x00000000, 0x08020008,
	0x08000208, 0x00020000, 0x08000000, 0x08020208,
	0x00000008, 0x00020208, 0x00020200, 0x08000008,
	0x08020000, 0x08000208, 0x00000208, 0x08020000,
	0x00020208, 0x00000008, 0x08020008, 0x00020200
};

static const unsigned long SB4[64] =
{
	0x00802001, 0x00002081, 0x00002081, 0x00000080,
	0x00802080, 0x00800081, 0x00800001, 0x00002001,
	0x00000000, 0x00802000, 0x00802000, 0x00802081,
	0x00000081, 0x00000000, 0x00800080, 0x00800001,
	0x00000001, 0x00002000, 0x00800000, 0x00802001,
	0x00000080, 0x00800000, 0x00002001, 0x00002080,
	0x00800081, 0x00000001, 0x00002080, 0x00800080,
	0x00002000, 0x00802080, 0x00802081, 0x00000081,
	0x00800080, 0x00800001, 0x00802000, 0x00802081,
	0x00000081, 0x00000000, 0x00000000, 0x00802000,
	0x00002080, 0x00800080, 0x00800081, 0x00000001,
	0x00802001, 0x00002081, 0x00002081, 0x00000080,
	0x00802081, 0x00000081, 0x00000001, 0x00002000,
	0x00800001, 0x00002001, 0x00802080, 0x00800081,
	0x00002001, 0x00002080, 0x00800000, 0x00802001,
	0x00000080, 0x00800000, 0x00002000, 0x00802080
};

static const unsigned long SB5[64] =
{
	0x00000100, 0x02080100, 0x02080000, 0x42000100,
	0x00080000, 0x00000100, 0x40000000, 0x02080000,
	0x40080100, 0x00080000, 0x02000100, 0x40080100,
	0x42000100, 0x42080000, 0x00080100, 0x40000000,
	0x02000000, 0x40080000, 0x40080000, 0x00000000,
	0x40000100, 0x42080100, 0x42080100, 0x02000100,
	0x42080000, 0x40000100, 0x00000000, 0x42000000,
	0x02080100, 0x02000000, 0x42000000, 0x00080100,
	0x00080000, 0x42000100, 0x00000100, 0x02000000,
	0x40000000, 0x02080000, 0x42000100, 0x40080100,
	0x02000100, 0x40000000, 0x42080000, 0x02080100,
	0x40080100, 0x00000100, 0x02000000, 0x42080000,
	0x42080100, 0x00080100, 0x42000000, 0x42080100,
	0x02080000, 0x00000000, 0x40080000, 0x42000000,
	0x00080100, 0x02000100, 0x40000100, 0x00080000,
	0x00000000, 0x40080000, 0x02080100, 0x40000100
};

static const unsigned long SB6[64] =
{
	0x20000010, 0x20400000, 0x00004000, 0x20404010,
	0x20400000, 0x00000010, 0x20404010, 0x00400000,
	0x20004000, 0x00404010, 0x00400000, 0x20000010,
	0x00400010, 0x20004000, 0x20000000, 0x00004010,
	0x00000000, 0x00400010, 0x20004010, 0x00004000,
	0x00404000, 0x20004010, 0x00000010, 0x20400010,
	0x20400010, 0x00000000, 0x00404010, 0x20404000,
	0x00004010, 0x00404000, 0x20404000, 0x20000000,
	0x20004000, 0x00000010, 0x20400010, 0x00404000,
	0x20404010, 0x00400000, 0x00004010, 0x20000010,
	0x00400000, 0x20004000, 0x20000000, 0x00004010,
	0x20000010, 0x20404010, 0x00404000, 0x20400000,
	0x00404010, 0x20404000, 0x00000000, 0x20400010,
	0x00000010, 0x00004000, 0x20400000, 0x00404010,
	0x00004000, 0x00400010, 0x20004010, 0x00000000,
	0x20404000, 0x20000000, 0x00400010, 0x20004010
};

static const unsigned long SB7[64] =
{
	0x00200000, 0x04200002, 0x04000802, 0x00000000,
	0x00000800, 0x04000802, 0x00200802, 0x04200800,
	0x04200802, 0x00200000, 0x00000000, 0x04000002,
	0x00000002, 0x04000000, 0x04200002, 0x00000802,
	0x04000800, 0x00200802, 0x00200002, 0x04000800,
	0x04000002, 0x04200000, 0x04200800, 0x00200002,
	0x04200000, 0x00000800, 0x00000802, 0x04200802,
	0x00200800, 0x00000002, 0x04000000, 0x00200800,
	0x04000000, 0x00200800, 0x00200000, 0x04000802,
	0x04000802, 0x04200002, 0x04200002, 0x00000002,
	0x00200002, 0x04000000, 0x04000800, 0x00200000,
	0x04200800, 0x00000802, 0x00200802, 0x04200800,
	0x00000802, 0x04000002, 0x04200802, 0x04200000,
	0x00200800, 0x00000000, 0x00000002, 0x04200802,
	0x00000000, 0x00200802, 0x04200000, 0x00000800,
	0x04000002, 0x04000800, 0x00000800, 0x00200002
};

static const unsigned long SB8[64] =
{
	0x10001040, 0x00001000, 0x00040000, 0x10041040,
	0x10000000, 0x10001040, 0x00000040, 0x10000000,
	0x00040040, 0x10040000, 0x10041040, 0x00041000,
	0x10041000, 0x00041040, 0x00001000, 0x00000040,
	0x10040000, 0x10000040, 0x10001000, 0x00001040,
	0x00041000, 0x00040040, 0x10040040, 0x10041000,
	0x00001040, 0x00000000, 0x00000000, 0x10040040,
	0x10000040, 0x10001000, 0x00041040, 0x00040000,
	0x00041040, 0x00040000, 0x10041000, 0x00001000,
	0x00000040, 0x10040040, 0x00001000, 0x00041040,
	0x10001000, 0x00000040, 0x10000040, 0x10040000,
	0x10040040, 0x10000000, 0x00040000, 0x10001040,
	0x00000000, 0x10041040, 0x00040040, 0x10000040,
	0x10040000, 0x10001000, 0x10001040, 0x00000000,
	0x10041040, 0x00041000, 0x00041000, 0x00001040,
	0x00001040, 0x00040040, 0x10000000, 0x10041000
};

/*
 * PC1: left and right halves bit-swap
 */
static const unsigned long LHs[16] =
{
	0x00000000, 0x00000001, 0x00000100, 0x00000101,
	0x00010000, 0x00010001, 0x00010100, 0x00010101,
	0x01000000, 0x01000001, 0x01000100, 0x01000101,
	0x01010000, 0x01010001, 0x01010100, 0x01010101
};

static const unsigned long RHs[16] =
{
	0x00000000, 0x01000000, 0x00010000, 0x01010000,
	0x00000100, 0x01000100, 0x00010100, 0x01010100,
	0x00000001, 0x01000001, 0x00010001, 0x01010001,
	0x00000101, 0x01000101, 0x00010101, 0x01010101,
};

/*
 * Initial Permutation macro
 */
#define DES_IP(X,Y)                                             \
{                                                               \
	T = ((X >>  4) ^ Y) & 0x0F0F0F0F; Y ^= T; X ^= (T <<  4);   \
	T = ((X >> 16) ^ Y) & 0x0000FFFF; Y ^= T; X ^= (T << 16);   \
	T = ((Y >>  2) ^ X) & 0x33333333; X ^= T; Y ^= (T <<  2);   \
	T = ((Y >>  8) ^ X) & 0x00FF00FF; X ^= T; Y ^= (T <<  8);   \
	Y = ((Y << 1) | (Y >> 31)) & 0xFFFFFFFF;                    \
	T = (X ^ Y) & 0xAAAAAAAA; Y ^= T; X ^= T;                   \
	X = ((X << 1) | (X >> 31)) & 0xFFFFFFFF;                    \
}

/*
 * Final Permutation macro
 */
#define DES_FP(X,Y)                                             \
{                                                               \
	X = ((X << 31) | (X >> 1)) & 0xFFFFFFFF;                    \
	T = (X ^ Y) & 0xAAAAAAAA; X ^= T; Y ^= T;                   \
	Y = ((Y << 31) | (Y >> 1)) & 0xFFFFFFFF;                    \
	T = ((Y >>  8) ^ X) & 0x00FF00FF; X ^= T; Y ^= (T <<  8);   \
	T = ((Y >>  2) ^ X) & 0x33333333; X ^= T; Y ^= (T <<  2);   \
	T = ((X >> 16) ^ Y) & 0x0000FFFF; Y ^= T; X ^= (T << 16);   \
	T = ((X >>  4) ^ Y) & 0x0F0F0F0F; Y ^= T; X ^= (T <<  4);   \
}

/*
 * DES round macro
 */
#define DES_ROUND(X,Y)                          \
{                                               \
	T = *SK++ ^ X;                              \
	Y ^= SB8[ (T      ) & 0x3F ] ^              \
	SB6[ (T >>  8) & 0x3F ] ^              \
	SB4[ (T >> 16) & 0x3F ] ^              \
	SB2[ (T >> 24) & 0x3F ];               \
	\
	T = *SK++ ^ ((X << 28) | (X >> 4));         \
	Y ^= SB7[ (T      ) & 0x3F ] ^              \
	SB5[ (T >>  8) & 0x3F ] ^              \
	SB3[ (T >> 16) & 0x3F ] ^              \
	SB1[ (T >> 24) & 0x3F ];               \
}

#define SWAP(a,b) { unsigned long t = a; a = b; b = t; t = 0; }

static const unsigned char odd_parity_table[128] = { 1,  2,  4,  7,  8,
	11, 13, 14, 16, 19, 21, 22, 25, 26, 28, 31, 32, 35, 37, 38, 41, 42, 44,
	47, 49, 50, 52, 55, 56, 59, 61, 62, 64, 67, 69, 70, 73, 74, 76, 79, 81,
	82, 84, 87, 88, 91, 93, 94, 97, 98, 100, 103, 104, 107, 109, 110, 112,
	115, 117, 118, 121, 122, 124, 127, 128, 131, 133, 134, 137, 138, 140,
	143, 145, 146, 148, 151, 152, 155, 157, 158, 161, 162, 164, 167, 168,
	171, 173, 174, 176, 179, 181, 182, 185, 186, 188, 191, 193, 194, 196,
	199, 200, 203, 205, 206, 208, 211, 213, 214, 217, 218, 220, 223, 224,
	227, 229, 230, 233, 234, 236, 239, 241, 242, 244, 247, 248, 251, 253,
	254 };
/*
 * Table of weak and semi-weak keys
 *
 * Source: http://en.wikipedia.org/wiki/Weak_key
 *
 * Weak:
 * Alternating ones + zeros (0x0101010101010101)
 * Alternating 'F' + 'E' (0xFEFEFEFEFEFEFEFE)
 * '0xE0E0E0E0F1F1F1F1'
 * '0x1F1F1F1F0E0E0E0E'
 *
 * Semi-weak:
 * 0x011F011F010E010E and 0x1F011F010E010E01
 * 0x01E001E001F101F1 and 0xE001E001F101F101
 * 0x01FE01FE01FE01FE and 0xFE01FE01FE01FE01
 * 0x1FE01FE00EF10EF1 and 0xE01FE01FF10EF10E
 * 0x1FFE1FFE0EFE0EFE and 0xFE1FFE1FFE0EFE0E
 * 0xE0FEE0FEF1FEF1FE and 0xFEE0FEE0FEF1FEF1
 *
 */

#define WEAK_KEY_COUNT 16

static const unsigned char weak_key_table[WEAK_KEY_COUNT][DES_KEY_SIZE] =
{
	{ 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 },
	{ 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE },
	{ 0x1F, 0x1F, 0x1F, 0x1F, 0x0E, 0x0E, 0x0E, 0x0E },
	{ 0xE0, 0xE0, 0xE0, 0xE0, 0xF1, 0xF1, 0xF1, 0xF1 },

	{ 0x01, 0x1F, 0x01, 0x1F, 0x01, 0x0E, 0x01, 0x0E },
	{ 0x1F, 0x01, 0x1F, 0x01, 0x0E, 0x01, 0x0E, 0x01 },
	{ 0x01, 0xE0, 0x01, 0xE0, 0x01, 0xF1, 0x01, 0xF1 },
	{ 0xE0, 0x01, 0xE0, 0x01, 0xF1, 0x01, 0xF1, 0x01 },
	{ 0x01, 0xFE, 0x01, 0xFE, 0x01, 0xFE, 0x01, 0xFE },
	{ 0xFE, 0x01, 0xFE, 0x01, 0xFE, 0x01, 0xFE, 0x01 },
	{ 0x1F, 0xE0, 0x1F, 0xE0, 0x0E, 0xF1, 0x0E, 0xF1 },
	{ 0xE0, 0x1F, 0xE0, 0x1F, 0xF1, 0x0E, 0xF1, 0x0E },
	{ 0x1F, 0xFE, 0x1F, 0xFE, 0x0E, 0xFE, 0x0E, 0xFE },
	{ 0xFE, 0x1F, 0xFE, 0x1F, 0xFE, 0x0E, 0xFE, 0x0E },
	{ 0xE0, 0xFE, 0xE0, 0xFE, 0xF1, 0xFE, 0xF1, 0xFE },
	{ 0xFE, 0xE0, 0xFE, 0xE0, 0xFE, 0xF1, 0xFE, 0xF1 }
};

void des_key_set_parity(unsigned char key[DES_KEY_SIZE])
{

	int i;

	for (i = 0; i < DES_KEY_SIZE; i++)
		key[i] = odd_parity_table[key[i] / 2];
}

/*
 * Check the given key's parity, returns 1 on failure, 0 on SUCCESS
 */
int des_key_check_key_parity(const unsigned char key[DES_KEY_SIZE])
{
	int i;

	for (i = 0; i < DES_KEY_SIZE; i++)
		if (key[i] != odd_parity_table[key[i] / 2])
			return(1);

	return(0);
}

int des_key_check_weak(const unsigned char key[DES_KEY_SIZE])
{
	int i;

	for (i = 0; i < WEAK_KEY_COUNT; i++)
		if (memcmp(weak_key_table[i], key, DES_KEY_SIZE) == 0)
			return(1);

	return(0);
}

void des_setkey(unsigned long SK[32], const unsigned char key[DES_KEY_SIZE], unsigned char makenoise)
{
	int i;
	unsigned long X, Y, T;

	GET_ULONG_BE(X, key, 0);
	GET_ULONG_BE(Y, key, 4);

	/*
	 * Permuted Choice 1
	 */
	T = ((Y >> 4) ^ X) & 0x0F0F0F0F;  X ^= T; Y ^= (T << 4);
	T = ((Y) ^ X) & 0x10101010;  X ^= T; Y ^= (T);

	X = (LHs[(X) & 0xF] << 3) | (LHs[(X >> 8) & 0xF] << 2)
		| (LHs[(X >> 16) & 0xF] << 1) | (LHs[(X >> 24) & 0xF])
		| (LHs[(X >> 5) & 0xF] << 7) | (LHs[(X >> 13) & 0xF] << 6)
		| (LHs[(X >> 21) & 0xF] << 5) | (LHs[(X >> 29) & 0xF] << 4);

	Y = (RHs[(Y >> 1) & 0xF] << 3) | (RHs[(Y >> 9) & 0xF] << 2)
		| (RHs[(Y >> 17) & 0xF] << 1) | (RHs[(Y >> 25) & 0xF])
		| (RHs[(Y >> 4) & 0xF] << 7) | (RHs[(Y >> 12) & 0xF] << 6)
		| (RHs[(Y >> 20) & 0xF] << 5) | (RHs[(Y >> 28) & 0xF] << 4);

	X &= 0x0FFFFFFF;
	Y &= 0x0FFFFFFF;

	/*
	 * calculate subkeys
	 */
	for (i = 0; i < 16; i++)
	{
		if (i < 2 || i == 8 || i == 15)
		{
			X = ((X << 1) | (X >> 27)) & 0x0FFFFFFF;
			Y = ((Y << 1) | (Y >> 27)) & 0x0FFFFFFF;
		}
		else
		{
			X = ((X << 2) | (X >> 26)) & 0x0FFFFFFF;
			Y = ((Y << 2) | (Y >> 26)) & 0x0FFFFFFF;
		}

		*SK++ = ((X << 4) & 0x24000000) | ((X << 28) & 0x10000000)
			| ((X << 14) & 0x08000000) | ((X << 18) & 0x02080000)
			| ((X << 6) & 0x01000000) | ((X << 9) & 0x00200000)
			| ((X >> 1) & 0x00100000) | ((X << 10) & 0x00040000)
			| ((X << 2) & 0x00020000) | ((X >> 10) & 0x00010000)
			| ((Y >> 13) & 0x00002000) | ((Y >> 4) & 0x00001000)
			| ((Y << 6) & 0x00000800) | ((Y >> 1) & 0x00000400)
			| ((Y >> 14) & 0x00000200) | ((Y) & 0x00000100)
			| ((Y >> 5) & 0x00000020) | ((Y >> 10) & 0x00000010)
			| ((Y >> 3) & 0x00000008) | ((Y >> 18) & 0x00000004)
			| ((Y >> 26) & 0x00000002) | ((Y >> 24) & 0x00000001);

		*SK++ = ((X << 15) & 0x20000000) | ((X << 17) & 0x10000000)
			| ((X << 10) & 0x08000000) | ((X << 22) & 0x04000000)
			| ((X >> 2) & 0x02000000) | ((X << 1) & 0x01000000)
			| ((X << 16) & 0x00200000) | ((X << 11) & 0x00100000)
			| ((X << 3) & 0x00080000) | ((X >> 6) & 0x00040000)
			| ((X << 15) & 0x00020000) | ((X >> 4) & 0x00010000)
			| ((Y >> 2) & 0x00002000) | ((Y << 8) & 0x00001000)
			| ((Y >> 14) & 0x00000808) | ((Y >> 9) & 0x00000400)
			| ((Y) & 0x00000200) | ((Y << 7) & 0x00000100)
			| ((Y >> 7) & 0x00000020) | ((Y >> 3) & 0x00000011)
			| ((Y << 2) & 0x00000004) | ((Y >> 21) & 0x00000002);
	}
}

/*
 * DES key schedule (56-bit, encryption)
 */
int des_setkey_enc(des_context *ctx, const unsigned char key[DES_KEY_SIZE])
{
	des_setkey(ctx->sk, key, 1);

	return(0);
}

/*
 * DES key schedule (56-bit, decryption)
 */
int des_setkey_dec(des_context *ctx, const unsigned char key[DES_KEY_SIZE])
{
	int i;

	des_setkey(ctx->sk, key, 1);

	for (i = 0; i < 16; i += 2)
	{
		SWAP(ctx->sk[i], ctx->sk[30 - i]);
		SWAP(ctx->sk[i + 1], ctx->sk[31 - i]);
	}

	return(0);
}

void des3_set2key(unsigned long esk[96],
		unsigned long dsk[96],
		const unsigned char key[DES_KEY_SIZE * 2])
{
	int i;

	des_setkey(esk, key, 0);
	des_setkey(dsk + 32, key + 8, 1);

	for (i = 0; i < 32; i += 2)
	{
		dsk[i] = esk[30 - i];
		dsk[i + 1] = esk[31 - i];

		esk[i + 32] = dsk[62 - i];
		esk[i + 33] = dsk[63 - i];

		esk[i + 64] = esk[i];
		esk[i + 65] = esk[i + 1];

		dsk[i + 64] = dsk[i];
		dsk[i + 65] = dsk[i + 1];
	}
}

/*
 * Triple-DES key schedule (112-bit, encryption)
 */
int des3_set2key_enc(des3_context *ctx, const unsigned char key[DES_KEY_SIZE * 2])
{
	unsigned long sk[96];

	des3_set2key(ctx->sk, sk, key);
	memset(sk, 0, sizeof(sk));

	return(0);
}

/*
 * Triple-DES key schedule (112-bit, decryption)
 */
int des3_set2key_dec(des3_context *ctx, const unsigned char key[DES_KEY_SIZE * 2])
{
	unsigned long sk[96];

	des3_set2key(sk, ctx->sk, key);
	memset(sk, 0, sizeof(sk));

	return(0);
}

void des3_set3key(unsigned long esk[96],
		unsigned long dsk[96],
		const unsigned char key[24])
{
	int i;

	des_setkey(esk, key, 0);

	des_setkey(dsk + 32, key + 8, 1);

	des_setkey(esk + 64, key + 16, 0);

	for (i = 0; i < 32; i += 2)
	{
		dsk[i] = esk[94 - i];
		dsk[i + 1] = esk[95 - i];

		esk[i + 32] = dsk[62 - i];
		esk[i + 33] = dsk[63 - i];

		dsk[i + 64] = esk[30 - i];
		dsk[i + 65] = esk[31 - i];
	}

}

/*
 * Triple-DES key schedule (168-bit, encryption)
 */
int des3_set3key_enc(des3_context *ctx, const unsigned char key[DES_KEY_SIZE * 3])
{
	unsigned long sk[96];

	memset(sk, 0, sizeof(sk));
	memset(ctx->sk, 0x00, sizeof(ctx->sk));
	des3_set3key(ctx->sk, sk, key);


	return(0);
}

/*
 * Triple-DES key schedule (168-bit, decryption)
 */
int des3_set3key_dec(des3_context *ctx, const unsigned char key[DES_KEY_SIZE * 3])
{
	unsigned long sk[96];

	des3_set3key(sk, ctx->sk, key);
	memset(sk, 0, sizeof(sk));

	return(0);
}

/*
 * DES-ECB block encryption/decryption
 */
int des_crypt_ecb(des_context *ctx,
		const unsigned char input[8],
		unsigned char output[8])
{
	int i;
	unsigned long X, Y, T, *SK;

	SK = ctx->sk;

	GET_ULONG_BE(X, input, 0);

	GET_ULONG_BE(Y, input, 4);

	DES_IP(X, Y);

	for (i = 0; i < 8; i++)
	{
		DES_ROUND(Y, X);
		DES_ROUND(X, Y);
	}

	DES_FP(Y, X);

	PUT_ULONG_BE(Y, output, 0);

	PUT_ULONG_BE(X, output, 4);

	return(0);
}

/*
 * DES-CBC buffer encryption/decryption
 */
int des_crypt_cbc(des_context *ctx,
		int mode,
		size_t length,
		unsigned char iv[8],
		const unsigned char *input,
		unsigned char *output)
{
	int i;
	unsigned char temp[8];

	if (length % 8)
		return(-1);

	if (mode == DES_ENCRYPT)
	{
		while (length > 0)
		{
			for (i = 0; i < 8; i++)
				output[i] = (unsigned char)(input[i] ^ iv[i]);

			des_crypt_ecb(ctx, output, output);
			memcpy(iv, output, 8);

			input += 8;
			output += 8;
			length -= 8;
		}
	}
	else /* DES_DECRYPT */
	{
		while (length > 0)
		{
			memcpy(temp, input, 8);
			des_crypt_ecb(ctx, input, output);

			for (i = 0; i < 8; i++)
				output[i] = (unsigned char)(output[i] ^ iv[i]);

			memcpy(iv, temp, 8);

			input += 8;
			output += 8;
			length -= 8;
		}
	}

	return(0);
}

/*
 * 3DES-ECB block encryption/decryption
 */
int des3_crypt_ecb(des3_context *ctx,
		const unsigned char input[8],
		unsigned char output[8])
{
	int i;
	unsigned long X, Y, T, *SK;

	SK = ctx->sk;

	GET_ULONG_BE(X, input, 0);

	GET_ULONG_BE(Y, input, 4);


	DES_IP(X, Y);

	for (i = 0; i < 8; i++)
	{
		DES_ROUND(Y, X);
		DES_ROUND(X, Y);
	}

	for (i = 0; i < 8; i++)
	{
		DES_ROUND(X, Y);
		DES_ROUND(Y, X);
	}

	for (i = 0; i < 8; i++)
	{
		DES_ROUND(Y, X);
		DES_ROUND(X, Y);
	}

	DES_FP(Y, X);

	PUT_ULONG_BE(Y, output, 0);
	PUT_ULONG_BE(X, output, 4);

	//DbgPrintf("------------> ecb out [%02X%02X%02X%02X%02X%02X%02X%02X]\n"
	//,output[0],output[1],output[2],output[3],output[4],output[5],output[6],output[7]);


	return(0);
}

/*
 * 3DES-CBC buffer encryption/decryption
 */
int des3_crypt_cbc(des3_context *ctx,
		int mode,
		size_t length,
		unsigned char iv[8],
		const unsigned char *input,
		unsigned char *output)
{
	int i;
	unsigned char temp[8];

	if (length % 8)
		return(-1);

	if (mode == DES_ENCRYPT)
	{
		while (length > 0)
		{
			for (i = 0; i < 8; i++)
				output[i] = (unsigned char)(input[i] ^ iv[i]);

			des3_crypt_ecb(ctx, output, output);
			memcpy(iv, output, 8);

			input += 8;
			output += 8;
			length -= 8;
		}
	}

	/*
	   for (int i = 0; i < 260; i++) {
	   printf("output(%d)\n", output[i]);
	   }
	 */

	else /* DES_DECRYPT */
	{
		while (length > 0)
		{
			memcpy(temp, input, 8);
			des3_crypt_ecb(ctx, input, output);

			for (i = 0; i < 8; i++)
				output[i] = (unsigned char)(output[i] ^ iv[i]);

			memcpy(iv, temp, 8);

			input += 8;
			output += 8;
			length -= 8;
		}
	}

	return(0);
}

int de_des_ecb(unsigned char *dataOut, unsigned char *dataIn, unsigned char *key, unsigned int data_byteLen, int mode)
{
	des_context ctx;
	int i;
	int cnt = data_byteLen / DES_KEY_SIZE;

	if (mode == DES_ENCRYPT)
		des_setkey_enc(&ctx, (unsigned char *)key);
	else
		des_setkey_dec(&ctx, (unsigned char *)key);

	for (i = 0; i < cnt; i++)
		des_crypt_ecb(&ctx, dataIn + (DES_KEY_SIZE*i), dataOut + (DES_KEY_SIZE*i));
	return 0;
}

int de_des_cbc(unsigned char *dataOut, unsigned char *dataIn, unsigned char *key, unsigned char *IV, unsigned int data_byteLen, int mode)
{
	des_context ctx;

	if (mode == DES_ENCRYPT)
		des_setkey_enc(&ctx, (unsigned char *)key);
	else
		des_setkey_dec(&ctx, (unsigned char *)key);

	des_crypt_cbc(&ctx, mode, data_byteLen, IV, dataIn, dataOut);

	return 0;
}
/*
   int CTDES::de_des(unsigned char *dataOut, unsigned char *dataIn, unsigned char *key, int mode)
   {
   des_context ctx;

   if(mode == DES_ENCRYPT)
   des_setkey_enc(&ctx, (unsigned char *) key );
   else
   des_setkey_dec(&ctx, (unsigned char *) key );

   des_crypt_ecb( &ctx, dataIn, dataOut);

   return 0;
   }
 */
int de_3des_ecb(unsigned char *dataOut, unsigned char *dataIn, unsigned char *key, int keytype, unsigned int data_byteLen, int mode)
{
	int i, cnt = data_byteLen / DES_KEY_SIZE;
	des3_context ctx;

	if (data_byteLen % 8)
		return(-1);

	if (mode == DES_ENCRYPT)
	{
		if (keytype == TYPE_2KEY)
			des3_set2key_enc(&ctx, (unsigned char *)key);
		else
			des3_set3key_enc(&ctx, (unsigned char *)key);
	}
	else
	{
		if (keytype == TYPE_2KEY)
			des3_set2key_dec(&ctx, (unsigned char *)key);
		else
			des3_set3key_dec(&ctx, (unsigned char *)key);
	}

	for (i = 0; i < cnt; i++)
		des3_crypt_ecb(&ctx, dataIn + (DES_KEY_SIZE*i), dataOut + (DES_KEY_SIZE*i));

	return 0;
}

int de_3des_cbc(unsigned char *dataOut, unsigned char *dataIn, unsigned char *key, int keytype, unsigned char *IV, unsigned int data_byteLen, int mode)
{
	des3_context ctx;

	if (mode == DES_ENCRYPT)
	{
		if (keytype == TYPE_2KEY)
			des3_set2key_enc(&ctx, (unsigned char *)key);
		else
			des3_set3key_enc(&ctx, (unsigned char *)key);
	}
	else
	{
		if (keytype == TYPE_2KEY)
			des3_set2key_dec(&ctx, (unsigned char *)key);
		else
			des3_set3key_dec(&ctx, (unsigned char *)key);
	}

	des3_crypt_cbc(&ctx, mode, data_byteLen, IV, dataIn, dataOut);

	return 0;
}

void XOR(unsigned char *a, unsigned char *b, int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		a[i] ^= b[i];
	}
}

unsigned char ShiftRight_4bit(unsigned char* in, unsigned char* out, int len)
{
	unsigned char ctmp[2], start, end;
	int i = 0;

	if (len < 1)
		return 0;

	start = in[0];
	end = in[len - 1];

	for (i = 0; i < len - 1; i++)
	{
		ctmp[0] = (in[i] & 0x0F);
		ctmp[1] = (in[i + 1] & 0xF0);
		out[i] = (ctmp[0] << 4) + (ctmp[1] >> 4);
	}

	ctmp[0] = (end & 0x0F);
	ctmp[1] = (start & 0xF0);
	out[len - 1] = (ctmp[0] << 4) + (ctmp[1] >> 4);

	return 1;
}

unsigned char ShiftLeft_4bit(unsigned char* in, unsigned char* out, int len)
{
	unsigned char ctmp[2], start, end;
	int i = 0;

	if (len < 1)
		return 0;

	start = in[0];
	end = in[len - 1];

	for (i = 1; i < len; i++)
	{
		ctmp[0] = (in[i - 1] & 0x0F);
		ctmp[1] = (in[i] & 0xF0);
		out[i] = (ctmp[0] << 4) + (ctmp[1] >> 4);
	}

	ctmp[0] = (end & 0x0F);
	ctmp[1] = (start & 0xF0);
	out[0] = (ctmp[0] << 4) + (ctmp[1] >> 4);

	return 1;
}

int GetKey(unsigned char* ct, unsigned char* k)
{
	unsigned char ctmp[10], ctmp2[15];
	int i;

	for (i = 0; i < 14; i++)
	{
		ctmp2[i] = (ct[i] - 0x30) % 10;
	}
	memcpy(ct, ctmp2, 14);

	memset(k, 0x00, 24);

	k[0] = pRND[5][ct[13]] ^ (pRND[ct[9]][5] + pRND[10][10]) % 256;
	k[1] = pRND[5][ct[13]] ^ ((pRND[ct[12]][0] ^ pRND[1][11]) + (pRND[0][8] | pRND[14][7])) % 256;
	k[2] = pRND[5][ct[13]] ^ (pRND[5][ct[10]] + pRND[13][ct[6]] + pRND[4][6]) % 256;
	k[3] = pRND[5][ct[13]] ^ pRND[7][4] ^ (pRND[11][ct[8]] | pRND[0][9]);
	k[4] = pRND[5][ct[13]] ^ (pRND[1][15] ^ pRND[7][ct[2]] ^ pRND[ct[11]][3]);
	k[5] = pRND[5][ct[13]] ^ ((pRND[7][11] + pRND[2][ct[8]]) % 256) ^ pRND[10][10];
	k[6] = pRND[5][ct[13]] ^ pRND[15][0] ^ (pRND[13][6] | pRND[10][0]);
	k[7] = pRND[5][ct[13]] ^ ((k[2] ^ k[4]) + pRND[11][ct[0]]) % 256;
	k[8] = pRND[5][ct[13]] ^ (pRND[5][15] + pRND[ct[10]][8]) % 256;
	k[9] = pRND[5][ct[13]] ^ (pRND[1][11] + pRND[9][0] + pRND[8][7]) % 256;
	k[10] = pRND[5][ct[13]] ^ (pRND[9][ct[13]] + pRND[0][5] + pRND[1][1]) % 256;
	k[11] = pRND[5][ct[13]] ^ (pRND[7][2] + pRND[ct[3]][2] + pRND[15][13]) % 256;
	k[12] = pRND[5][ct[13]] ^ pRND[2][6] ^ (pRND[6][13] + pRND[2][0]) % 256;
	k[13] = pRND[5][ct[13]] ^ (pRND[1][7] | pRND[10][2] | pRND[ct[1]][6]);
	k[14] = pRND[5][ct[13]] ^ (pRND[2][15] + pRND[12][11] + pRND[2][ct[4]]) % 256;
	k[15] = pRND[5][ct[13]] ^ (pRND[10][2] + pRND[1][ct[10]] + pRND[1][9]) % 256;
	k[16] = pRND[5][ct[13]] ^ pRND[15][ct[5]] ^ pRND[11][3] ^ pRND[10][1];
	k[17] = pRND[5][ct[13]] ^ (pRND[10][8] + pRND[9][ct[9]]) % 256;
	k[18] = pRND[5][ct[13]] ^ (pRND[3][0] + pRND[0][1] + pRND[3][1]) % 256;
	k[19] = pRND[5][ct[13]] ^ (pRND[1][12] + pRND[3][9] + pRND[ct[12]][4]) % 256;
	k[20] = pRND[5][ct[13]] ^ (pRND[ct[6]][ct[3]] + pRND[4][3] + pRND[1][0]) % 256;
	k[21] = pRND[5][ct[13]] ^ (pRND[4][10] + pRND[2][13]) % 256 ^ pRND[3][0];
	k[22] = pRND[5][ct[13]] ^ pRND[1][ct[10]] ^ pRND[13][7] ^ pRND[14][13];
	k[23] = pRND[5][ct[13]] ^ (pRND[8][ct[13]] | pRND[ct[12]][0]) ^ pRND[15][13];

	XOR(k + 3, k + 13, 5);
	ShiftLeft_4bit(k + 1, ctmp, 10);
	memcpy(k + 1, ctmp, 10);
	ShiftRight_4bit(k + 20, ctmp, 3);


	/*
	   for (int i = 0; i < 24; i++) {
	   printf("key[%d][%d]\n", i, k[i]);
	   }


	   for (int i = 0; i < 10; i++) {
	   printf("ctmp[%d][%d]\n", i, ctmp[i]);
	   }
	 */

	memcpy(k + 20, ctmp, 3);
	XOR(k + 11, k + 6, 10);
	ShiftRight_4bit(k + 15, ctmp, 8);
	memcpy(k + 15, ctmp, 8);
	ShiftLeft_4bit(k + 8, ctmp, 5);
	memcpy(k + 8, ctmp, 5);

	return 24;
}

int DualDecode_ENC(unsigned char* ctime, unsigned char* inbuf, int ninbuf, unsigned char* outbuf)
{
	unsigned char key[24], IV[8], pData[260];
	int len = GetKey(ctime, key);
	int i;

	for (i = 0; i < 8; i++)
	{
		IV[i] = i;
	}

	len = ninbuf;
	if (len % 8 != 0)
		len += (8 - len % 8);

	//de_3des_cbc(pData, inbuf, key, TYPE_3KEY, IV, len, DES_DECRYPT);

	if (pData[0] == 'D' && pData[1] == 'E')
	{
		memcpy(outbuf, pData + 3, pData[2]);
		return pData[2];
	}

	memset(key, 0x00, sizeof(key));
	memset(pData, 0x00, sizeof(pData));

	return -1;
}

int DecodeData_POS(unsigned char* ctime, unsigned char* inbuf, int ninbuf, unsigned char* outbuf)
{
	unsigned char key[24], IV[8], pData[260];
	int len = GetKey(ctime, key);
	int i;

	for (i = 0; i < 8; i++)
	{
		IV[i] = i;
	}

	if (ninbuf < 16)
	{//PAN 번호 6자리를 return하려면 암호화 데이터 길이는 16자리 이하이면 에러 리턴
		return -3;
	}

	//PAN 번호 앞 6자리만 사용하기 위해 
	//len = 16;

	if (len % 8 != 0)
		len += (8 - len % 8);

	de_3des_cbc(pData, inbuf, key, TYPE_3KEY, IV, len, DES_DECRYPT);

	if (pData[0] == 'D' && pData[1] == 'E')
	{
		len = pData[2];
		/*
		   if (len < 6)
		   {//PAN 번호 6자리를 return해야 하므로 암호화 데이터가 6자리 이하이면 에러 리턴
		   return -4;
		   }
		 */

		//memcpy(outbuf, pData + 3, 6);
		memcpy(outbuf, pData + 3, 16);
		//len = 6;
		len = 16;
		memset(pData, 0x00, sizeof(pData));
		memset(key, 0x00, sizeof(key));
		memset(IV, 0x00, sizeof(IV));
		return len;
	}

	memset(pData, 0x00, sizeof(pData));
	memset(key, 0x00, sizeof(key));
	memset(IV, 0x00, sizeof(IV));
	return -1;
}

char ASCII2HEX(char* str)
{
	char temp;
	char a[3];

	memcpy(a, str, 2);

	if ((a[0] >= '0') && (a[0] <= '9')) 		temp = (a[0] - '0');
	else if ((a[0] >= 'a') && (a[0] <= 'f')) 	temp = (a[0] - 'W');	//Capital
	else 									temp = (a[0] - '7');	//small
	temp <<= 4;
	if ((a[1] >= '0') && (a[1] <= '9')) 		temp |= (a[1] - '0');
	else if ((a[1] >= 'a') && (a[1] <= 'f')) 	temp |= (a[1] - 'W');	//Capital
	else									temp |= (a[1] - '7');	//small

	return(temp);
}

void STRING2HEX(const char* str, unsigned char* hex)
{
	int i, len;
	char pBuf[3];
	len = strlen(str);
	if (len % 2 != 0)
	{
		strcat((char *)str, "0");
		len++;
	}

	for (i = 0; i < len / 2; i++)
	{
		memset(pBuf, 0x00, sizeof(pBuf));
		memcpy(pBuf, str + (i * 2), 2);
		hex[i] = ASCII2HEX(pBuf);
		//printf("hex(%d)\n", hex[i]);
	}
}

int Usage( int argc, char **argv )
{
	int     i;

	printf( "========================================================================\n" );
	for( i=0; i<argc; i++ ) {
		printf( "argument[%d]=%s\n", i, argv[ i ] );
	}
	printf( "========================================================================\n" );

	return 0;
}

int main(int argc, char **argv)
{
	printf("main.c arg values : %d, %s\n", argc, argv);

	const char* data = "CFFA62545EC4E6700A5930351479681A8A26E0155AE77D79";
	unsigned char ctime[14] = { '2', '0', '1', '8', '1', '1', '2', '7', '1', '7', '1', '8', '1', '8' };
	unsigned char inbuf[48];
	unsigned char outbuf[300];
	int ninbuf = 0;
	int i;
/*
	if (argc != 2)
	{
		printf( "입력 값이 정확하지 않습니다!!\n" );
		printf( "argv[ 1 ]=암호화 카드번호\n" );
		return -1;
	}
*/
	Usage( argc, argv );

	if (strlen(data) % 2 != 0)
	{
		printf("Data 길이 확인\n");
	}

	ninbuf = strlen(data) / 2;
	if (ninbuf > 256)
		return 0;
	STRING2HEX(data, inbuf);

	/*
	   for (int i = 0; i < 24; i++) {
	   printf("key[%d][%d]\n", i, key[i]);
	   }
	 */

	DecodeData_POS(ctime, inbuf, ninbuf, outbuf);


	for (i = 0; i < 48; i++) {
		printf("out(%d)\n", outbuf[i]);
	}

	return 0;
}
