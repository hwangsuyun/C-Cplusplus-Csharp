#include <stdio.h>
#include <string.h>

#include "parse_data.h"
#include "cJSON.h"

int parseRecvData(char *buff, RecvMsg *data)
{
	cJSON *item		= NULL;
	cJSON *pgm		= NULL;
	cJSON *siteCd	= NULL;
	cJSON *ip		= NULL;
	cJSON *flag		= NULL;
	cJSON *key		= NULL;

	item	= cJSON_Parse((char *)buff);
	pgm     = cJSON_GetObjectItem(item, "pgm");
	siteCd	= cJSON_GetObjectItem(item, "siteCd");
	ip      = cJSON_GetObjectItem(item, "ip");
	flag    = cJSON_GetObjectItem(item, "flag");
	key		= cJSON_GetObjectItem(item, "key");

	memcpy(data->pgm,		cJSON_Print(pgm) + 1,		strlen(cJSON_Print(pgm)) - 2);
	memcpy(data->siteCd,	cJSON_Print(siteCd) + 1,	strlen(cJSON_Print(siteCd)) - 2);
	memcpy(data->ip,		cJSON_Print(ip) + 1,		strlen(cJSON_Print(ip)) - 2);
	memcpy(data->flag,		cJSON_Print(flag) + 1,		strlen(cJSON_Print(flag)) - 2);
	memcpy(data->key,		cJSON_Print(key) + 1,		strlen(cJSON_Print(key)) - 2);

	return 0;
}

int changeCharToNumber(char *temp)
{
	int i;

	for (i = 0; i < sizeof(temp); i++)
	{
		if (temp[i] == 'A') return 65;
		else if (temp[i] == 'B') return 66;
		else if (temp[i] == 'C') return 67;
		else if (temp[i] == 'D') return 68;
		else if (temp[i] == 'E') return 69;
		else if (temp[i] == 'F') return 70;
		else if (temp[i] == 'G') return 71;
		else if (temp[i] == 'H') return 72;
		else if (temp[i] == 'I') return 73;
		else if (temp[i] == 'J') return 74;
		else if (temp[i] == 'K') return 75;
		else if (temp[i] == 'L') return 76;
		else if (temp[i] == 'M') return 77;
		else if (temp[i] == 'N') return 78;
		else if (temp[i] == 'O') return 79;
		else if (temp[i] == 'P') return 80;
		else if (temp[i] == 'Q') return 81;
		else if (temp[i] == 'R') return 82;
		else if (temp[i] == 'S') return 83;
		else if (temp[i] == 'T') return 84;
		else if (temp[i] == 'U') return 85;
		else if (temp[i] == 'V') return 86;
		else if (temp[i] == 'W') return 87;
		else if (temp[i] == 'X') return 88;
		else if (temp[i] == 'Y') return 89;
		else if (temp[i] == 'Z') return 90;
	}
	return 0;
}       

