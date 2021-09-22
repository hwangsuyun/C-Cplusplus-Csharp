#include <stdio.h>
#include <string.h>
#include "zlib.h"
#include "Head.h"
#include    "Aes.h"

int  Compression( int flag, char *source, int sourceLen, char *dest, int *destLen )
{
	int result = 0;
	Bytef rBuff[ COMPRESS_BUFF_SIZE ];
	uLongf rBuffLen = COMPRESS_BUFF_SIZE;
	memset( rBuff, 0x00, sizeof( rBuff ) );

	switch( flag )
	{
		case COMPRESS :
			//int ZEXPORT compress (Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
			result = compress( rBuff, &rBuffLen, (const Bytef*) source, sourceLen );
			break;

		case UNCOMPRESS :
			//int ZEXPORT uncompress (Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
			result = uncompress( rBuff, &rBuffLen, (const Bytef*) source, sourceLen );
			break;

		default :
			return -1;
			break;
	}

	if( result != Z_OK )
	{
		return result;
	}
    
	memcpy( dest, rBuff, rBuffLen );
	*destLen = rBuffLen;

	return Z_OK;
}

// Aes 암복호화
// type : E=암호화, D=복호화
// key : user key
// iv : iv
// cryptByte : 16, 24, 32 bytes 
// in : source data
// result : result data
// len : 데이터 길이
// iMode : ECB=0, CBC=1, CFB=2
int nasAesCrypt(char *type, char const* key, char const* iv, int cryptByte, char const* in, char* result, size_t len, int iMode)
{
	memcpy( result, in, len);

	if( len % cryptByte != 0 )
	{
		len = len + ( cryptByte - ( len % cryptByte ) ) ;
		printf("len(%d)\n", len);
	}   

	if( strcmp( type, "E" ) == 0 )
	{
		aes_128_cbc_encrypt((BYTE const*)key, (BYTE const*)iv, (BYTE *)result, len);
	}
	else if( strcmp( type, "D" ) == 0 )
	{
		aes_128_cbc_decrypt((BYTE const*)key, (BYTE const*)iv, (BYTE *)result, len);
	}

	return len;

}

int main()
{
	// 16의 배수가아니면 제대로 복호화가 안됨... 아마 패딩 방식으로 공백 넣고 해서 16의 배수를 맞추던지
	// 노패딩으로 하면서 16의 배수로 맞춰야 하는 듯
	//char szPlainData[256] = "Single block msg 1234567890 aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffff0";
	char szPlainData[256] = "12345678901234561234567890123456";
	char szKey[16] = { 0x06, 0xa9, 0x21, 0x40, 0x36, 0xb8, 0xa1, 0x5b,
		                    0x51, 0x2e, 0x03, 0xd5, 0x34, 0x12, 0x00, 0x06 };
	char szIV[16] =  { 0x3d, 0xaf, 0xba, 0x42, 0x9d, 0x9e, 0xb4, 0x30,
		                      0xb4, 0x22, 0xda, 0x80, 0x2c, 0x9f, 0xac, 0x41 };

	char temp1[256];
	char result[256];
	int len;
	char szEncrypted[256] = "";
	char szDecrypted[256] = "";


	len = strlen(szPlainData);

	memset(szEncrypted , 0x00, sizeof(szEncrypted));
	memset(szDecrypted , 0x00, sizeof(szDecrypted));

	printf("[%d][%s]\n", len, szPlainData);
	nasAesCrypt("E", szKey, szIV, 16, szPlainData, szEncrypted, len, 1);
	memset(temp1, 0x00, sizeof(temp1));
	//CharStr2HexStr( (unsigned char*) szEncrypted, temp1, len);
	printf("======hex[%s]org[%s]\n\n\n", temp1, szEncrypted);

	nasAesCrypt("D", szKey, szIV, 16, szEncrypted, szDecrypted, len, 1);
	printf("======[%s]\n", szDecrypted );

	return 0;
}
