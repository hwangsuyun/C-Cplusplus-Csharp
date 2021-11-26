#include <stdio.h>

int main()
{
	FILE *fp;
	char file[100 + 1];
	
	memset(file, 0x00, sizeof(file));
	memcpy(file, "test.txt2", sizeof(file));
	
	if ((fp = fopen(file, "w")) == NULL)
	{
		printf("file not found\n");
		return -1;
	}

	while (1)
	{
		fprintf(fp, "%s\n", "I am boy");
	}
	
	return 0;
}
