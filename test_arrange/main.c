#include <stdio.h>

int main()
{
	char svrIp[100][20];
	int i;

	printf("svrIp(%d)\n", sizeof(svrIp));
	memset(svrIp, 0x00, sizeof(svrIp));

	memcpy(svrIp[0], "1234", 4);
	printf("svrIp(%s)\n", svrIp);

	for (i = 0; i < 4; i++)
	{
		printf("svrIp(%c)\n", svrIp[0][i]);
	}
	
	return 0;
}
