#include <stdio.h>
#include <string.h>

int writeInitFile(int semid, int isSemaphored)
{
	FILE *fp;
	char temp[100];
	char sem_buff[30];
	char status_buff[30];

	fp = fopen("initialize.ini", "r+");

	while (1)
	{
		memset(temp, 0x00, sizeof(temp));
		fgets(temp, 1024, fp);

		if (strncmp(temp, "SEM_KEY", 7) == 0)
		{   
		    memcpy(sem_buff, temp + 10, sizeof(sem_buff));

		    if (atoi(sem_buff) == semid)
			{
				return 0;
			}
		}

		if (feof(fp)) break;
	}

	memset(sem_buff, 0x00, sizeof(sem_buff));
	memset(status_buff, 0x00, sizeof(status_buff));
	sprintf(sem_buff, "SEM_KEY = %d\n", semid);
	sprintf(status_buff, "SEMAPHORED_STATUS = %d\n", isSemaphored);

	fseek(fp, 0, SEEK_END);
	fputs(sem_buff, fp);
	fputs(status_buff, fp);

	fclose(fp);

	return 0;
}

int markInitFile(int semid, int isSemaphored)
{
	FILE *fp;
	char temp[100];
	char sem_buff[10];
	char isSemaphored_buff[2];
	int size = 0;

	fp = fopen("initialize.ini", "r+");

	while (1)
	{   
		memset(temp, 0x00, sizeof(temp));
		fgets(temp, 1024, fp);

		if (strncmp(temp, "SEM_KEY", 7) == 0)
		{   
			memcpy(sem_buff, temp + 10, sizeof(sem_buff));

			if (atoi(sem_buff) == semid)
			{   
				size = ftell(fp);
				break;
			}
		}

		if (feof(fp)) break;
	}

	memset(isSemaphored_buff, 0x00, sizeof(isSemaphored_buff));
	sprintf(isSemaphored_buff, "%d", isSemaphored);

	fseek(fp, size + 20, SEEK_SET);
	fputs(isSemaphored_buff, fp);

	fclose(fp);
	return 0;
}

