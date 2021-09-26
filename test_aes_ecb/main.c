#include <stdio.h> 
#include <openssl/aes.h>   

static const unsigned char key[] = {
	0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
	0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff	
};

#define KEY_BIT128 128
#define KEY_BIT256 256

AES_KEY aes_ks3;

/* AES_Encrypt Process */
int encrypt_block(unsigned char *plainText, unsigned char *cipherText, int length)
{
	AES_KEY encKey;
	int i;
	
	if (AES_set_encrypt_key(key, KEY_BIT128, &encKey) < 0)
	{
		printf("AES_set_encrypt_key fail!!\n");
		return -1;
	}
	//AES_encrypt(plainText, cipherText, &encKey); // aes-128 방식으로만 하면 128비트(16바이트) 밖에 안됨
	// AES_128
	for (i = 0; i <= length / 16; i++)
	{
		AES_ecb_encrypt(plainText + i * 16, cipherText + i * 16, &encKey, AES_ENCRYPT); // aes 에서 ecb or cbc 모드 사용
	}

	//AES_encrypt(plainText, cipherText, &encKey); // aes-256 방식으로만 하면 256비트(32바이트) 밖에 안됨
	// AES_256
	/*
	for (i = 0; i <= length / 32; i++)
	{
		AES_ecb_encrypt(plainText + i * 32, cipherText + i * 32, &encKey, AES_ENCRYPT); // aes 에서 ecb or cbc 모드 사용
	}
	*/
	return 0;
}

/* AES Decrypt Process */
int decrypt_block(unsigned char *plainText, unsigned char *cipherText, int length)
{
	AES_KEY decKey;
	int i;
	
	if (AES_set_decrypt_key(key, KEY_BIT128, &decKey) < 0)
	{
		printf("AES_set_decrypt_key fail!!\n");
		return -1;
	}
	
	//AES_decrypt(cipherText, plainText, &decKey); AES_128
	for (i = 0; i <= length / 16; i++)
	{   
		AES_ecb_encrypt(cipherText + i * 16, plainText + i * 16, &decKey, AES_DECRYPT); // aes 에서 ecb or cbc 모드 사용
	}

	//AES_decrypt(cipherText, plainText, &decKey); AES_256
	/*
	for (i = 0; i <= length / 16; i++)
	{   
		AES_ecb_encrypt(cipherText + i * 16, plainText + i * 16, &decKey, AES_DECRYPT); // aes 에서 ecb or cbc 모드 사용
	}
	*/
	return 0;
}

int main()
{
	char plainText[512];
	char cipherText[512];
	char temp[512];
	int length;

	memset(plainText, 0x00, sizeof(plainText));
	memset(cipherText, 0x00, sizeof(cipherText));
	memset(temp, 0x00, sizeof(temp));
	
	memcpy(plainText, "TEST1234", sizeof(plainText));

	sprintf(plainText, "{                         \
			 			  \"autoKey\"   : \"%s\", \
			    		  \"command\"   : \"%s\", \
						  \"siteDir\"   : \"%s\", \
						  \"cardDir\"   : \"%s\", \
						  \"srcFileNm\" : \"%s\", \
						  \"desFileNm\" : \"%s\", \
						  \"dummy1\"    : \"%s\", \
						  \"dummy2\"    : \"%s\"  \
						}"
			, "autoKey", "CM01", "PATH", "PATH", "NAME", "NAME", "", "" );
	
	length = strlen(plainText);
	printf("length(%d)\n", length);
	printf("plainText(%s)\n", plainText);
	hex_print(plainText, strlen(plainText));
	
	encrypt_block(plainText, cipherText, length);
	printf("encrypt : cipherText(%s)\n", cipherText);
	hex_print(cipherText, strlen(cipherText));
	
	decrypt_block(temp, cipherText, length);
	printf("deccrypt : temp(%s)\n", temp);
	hex_print(temp, strlen(temp));

	return 0;
}

int hex_print(const void* pv, size_t len)
{
	const unsigned char * p = (const unsigned char*)pv;
	if (NULL == pv)
		printf("NULL");
	else
	{
		size_t i = 0;
		for (; i<len;++i)
			printf("%02X ", *p++);
	}
	printf("\n");
	
	return 0;
}
