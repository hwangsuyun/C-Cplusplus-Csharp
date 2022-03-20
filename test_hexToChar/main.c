#include <stdio.h>
#include <string.h>

char hexToChar(char* str)
{
	char temp;
	char a[3];

	memcpy(a, str, 2);

	if ((a[0] >= '0') && (a[0] <= '9'))         temp = (a[0] - '0');
	else if ((a[0] >= 'a') && (a[0] <= 'f'))    temp = (a[0] - 'W');    //Capital
	else                                    temp = (a[0] - '7');    //small
	temp <<= 4;
	if ((a[1] >= '0') && (a[1] <= '9'))         temp |= (a[1] - '0');
	else if ((a[1] >= 'a') && (a[1] <= 'f'))    temp |= (a[1] - 'W');   //Capital
	else                                    temp |= (a[1] - '7');   //small

	return(temp);
}

void hexToBinary(const char* str, unsigned char* hex)
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
		hex[i] = hexToChar(pBuf);
		//printf("hex(%d)\n", hex[i]);
	}
}

int main()
{
	unsigned char temp[100];
	char out[100];
	int i = 0;

	memset(temp, 0x00, sizeof(temp));
	memset(out, 0x00, sizeof(out));
	//memcpy(temp, "36323130333935302A2A2A2A35393532", sizeof(temp));
	memcpy(temp, "00323130333935302A2A2A2A35393532", sizeof(temp));

	printf("temp(%s)\n", temp);
	hexToBinary(temp, out);
	for (i = 0; i < 16; i++) printf("%02X", out[i]);
	printf("out(%s)\n", out);
	return 0;
}
