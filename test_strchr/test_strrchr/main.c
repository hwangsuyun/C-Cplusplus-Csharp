#include <stdio.h>

int main()
{
	char *tSizeString;
	char tStr[64];

	memset(tStr, 0x00, sizeof(tStr));
	//memcpy(tStr, "yyyymmdd=xxx", sizeof(tStr));
	memcpy(tStr, "=yyyymmdd=xxx", sizeof(tStr));

	tSizeString = strchr(tStr, '=') + 1;
	printf("strchr : tSizeString(%s)\n", tSizeString);
	tSizeString = strrchr(tStr, '=') + 1;
	printf("strrchr : tSizeString(%s)\n", tSizeString);

	return 0;
}
