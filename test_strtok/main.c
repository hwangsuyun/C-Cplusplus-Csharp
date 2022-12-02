#include <stdio.h>
#include <string.h>

int main()
{
	char temp[25];
	char *ptr = NULL;

	memset(temp, 0x00, sizeof(temp));
	memcpy(temp, "2020-01-09", sizeof(temp));
	ptr = strtok(temp, "-");
	if (ptr) 
	{
		printf("%s\n", ptr);
	}

	while (ptr != NULL) 
	{
		ptr = strtok(NULL, "-");
		if (ptr)
		{
			printf("%s\n", ptr);
		}
	}
	return 0;
}
