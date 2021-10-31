#include <stdio.h>
#include <string.h>
#include "cJSON.h"

#define STX     0x02
#define ETX     0x03
#define FS      0x1c

typedef struct 
{
	char pgm[30 + 1];
	char site_cd[10 + 1];
	char ip[15 + 1];
	char flag[3 + 1];
	char semKey[3 + 1];
	char shmKey[3 + 1];
} PROTOCOL;

void makeStringData(char *, char *, char *, char *, char *, char *, char *);
void makeJsonData(char *, char *, char *, char *, char *, char *, char *);
int parseRecvData(char *buff, PROTOCOL *data);

int main()
{
	char sendMessage[1024];
	PROTOCOL data;

	memset(sendMessage, 0x00, sizeof(sendMessage));
	memset((char *)&data, 0x00, sizeof(data));

	//makeStringData("client", "HSY", "127.0.0.1", "0", "S", "M", sendMessage);
	makeJsonData("client", "HSY", "127.0.0.1", "0", "S", "M", sendMessage);

	printf("sendMessage(%s)\n", sendMessage);

	parseRecvData(sendMessage, &data);
	
	printf("data.pgm(%s)\n", data.pgm);
	printf("data.site_cd(%s)\n", data.site_cd);
	printf("data.ip(%s)\n", data.ip);
	printf("data.flag(%s)\n", data.flag);
	printf("data.semKey(%s)\n", data.semKey);
	printf("data.shmKey(%s)\n", data.shmKey);

	return 0;
}

#if 0
int parseRecvData(char *buff, PROTOCOL *data)
{
	int i, j, cnt = 0;
	int index[10] = {0, };
	
	if (buff[0] != STX) return -1;

	for (i = 0; i < strlen(buff); i++)
	{	
		if (buff[1 + i] == FS)
		{
			index[cnt] = i + 1;
			cnt++;
		}
	}

	memcpy(data->pgm,     buff + 1,                       index[0] - 1);
	memcpy(data->site_cd, buff + 1 + index[0], index[1] - index[0] - 1);
	memcpy(data->ip,      buff + 1 + index[1], index[2] - index[1] - 1);
	memcpy(data->flag,    buff + 1 + index[2], index[3] - index[2] - 1);
	memcpy(data->semKey,  buff + 1 + index[3], index[4] - index[3] - 1);
	memcpy(data->shmKey,  buff + 1 + index[4], index[5] - index[4] - 1);

	return 0;
}
#endif

#if 1
int parseRecvData(char *buff, PROTOCOL *data)
{
	cJSON *item    = NULL; 
	cJSON *pgm     = NULL; 
	cJSON *site_cd = NULL; 
	cJSON *ip      = NULL; 
	cJSON *flag    = NULL; 
	cJSON *semKey  = NULL; 
	cJSON *shmKey  = NULL; 

	item = cJSON_Parse((char *)buff);
	if (item == NULL)
	{
	    printf("ERROR : cJSONParse\n");
	    return -1;
	}
	
	pgm     = cJSON_GetObjectItem(item, "pgm");
	site_cd = cJSON_GetObjectItem(item, "site_cd");
	ip      = cJSON_GetObjectItem(item, "ip");
	flag    = cJSON_GetObjectItem(item, "flag");
	semKey  = cJSON_GetObjectItem(item, "semKey");
	shmKey  = cJSON_GetObjectItem(item, "shmKey");
	
	memcpy(data->pgm,     cJSON_Print(pgm),     sizeof(data->pgm));
	memcpy(data->site_cd, cJSON_Print(site_cd), sizeof(data->site_cd));
	memcpy(data->ip,      cJSON_Print(ip),      sizeof(data->ip));
	memcpy(data->flag,    cJSON_Print(flag),    sizeof(data->flag));
	memcpy(data->semKey,  cJSON_Print(semKey),  sizeof(data->semKey));
	memcpy(data->shmKey,  cJSON_Print(shmKey),  sizeof(data->shmKey));
	
	return 0;
}
#endif

void makeStringData(char *pgm, char *site_cd, char *ip, char *flag, char *semKey, char *shmKey, char *msg)
{
	sprintf(msg, "%c%s%c%s%c%s%c%s%c%s%c%s%c%c", 
			STX, pgm, FS, site_cd, FS, ip, FS, flag, FS, semKey, FS, shmKey, FS, ETX);
}

void makeJsonData(char *pgm, char *site_cd, char *ip, char *flag, char *semKey, char *shmKey, char *msg)
{
	sprintf(msg, "{                       \
			\"pgm\"     : \"%s\", \
			\"site_cd\" : \"%s\", \
			\"ip\"      : \"%s\", \
			\"flag\"    : \"%s\", \
			\"semKey\"  : \"%s\", \
			\"shmKey\"  : \"%s\"  \
		      }"
		      , pgm, site_cd, ip, flag, semKey, shmKey);
}
