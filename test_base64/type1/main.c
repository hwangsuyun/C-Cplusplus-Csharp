#include <stdio.h>
#include <string.h>

int main()
{
	int encLen;
	int decLen;
	char temp[100];
	char encData[100];
	char decData[100];

	memset(temp, 0x00, sizeof(temp));
	memset(encData, 0x00, sizeof(encData));
	memset(decData, 0x00, sizeof(decData));
	memcpy(temp, "1111222233334444", sizeof(temp));
	
	encodeBASE64(temp, strlen(temp), encData, &encLen);

	printf("temp(%s)\n", temp);
	printf("encData(%s)\n", encData);
	
	decodeBASE64(encData, encLen, decData, &decLen);

	printf("decData(%s)\n", decData);

	return 0;
}
