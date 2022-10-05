#include <stdio.h>
#include <string.h>

int initializeInitFile(int index, char *shm_key)
{
	FILE *fp;
	char temp[100];
	char shm_buff[30];
	char status_buff[30];
	int size = 0;

	fp = fopen("initialize.ini", "r+");

	while (1)
	{
		memset(temp, 0x00, sizeof(temp));
		fgets(temp, 1024, fp);

		if (strncmp(temp, "SHARED_INFO", 11) == 0)
		{
			size = ftell(fp);
			break;
		}

		if (feof(fp)) break;
	}

	memset(shm_buff, 0x00, sizeof(shm_buff));
	memset(status_buff, 0x00, sizeof(status_buff));
	sprintf(shm_buff, "SHM_KEY[%d] = %s\n", index, shm_key);
	sprintf(status_buff, "%s", "SHARED_STATUS = 0\n");

	fseek(fp, size - 2, SEEK_SET);
	fputs(shm_buff, fp);
	fputs(status_buff, fp);

	fclose(fp);

	return 0;
}

int writeInitFile(int index, int isShared)
{
	FILE *fp;
	char temp[100];
	char shm_buff[2];
	char isShared_buff[2];
	int size = 0;

	fp = fopen("initialize.ini", "r+");

	while (1)
	{   
		memset(temp, 0x00, sizeof(temp));
		fgets(temp, 1024, fp);

		if (strncmp(temp, "SHM_KEY", 7) == 0)
		{   
			memcpy(shm_buff, temp + 8, sizeof(shm_buff));

			if (atoi(shm_buff) == index)
			{   
				size = ftell(fp);
				break;
			}
		}

		if (feof(fp)) break;
	}

	memset(isShared_buff, 0x00, sizeof(isShared_buff));
	sprintf(isShared_buff, "%d", isShared);

	fseek(fp, size + 16, SEEK_SET);
	fputs(isShared_buff, fp);

	fclose(fp);
	return 0;
}

