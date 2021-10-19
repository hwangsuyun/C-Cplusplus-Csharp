#include <stdio.h>
#include <string.h>
#include "cJSON.h"

int main()
{
	cJSON *item = NULL; 
	cJSON *server = NULL; 
	cJSON *user = NULL; 

	char temp[512];
	int r;

	char tempServer[20];
	char tempUser[20];

	memset(temp, 0x00, sizeof(temp));
	memset(tempServer, 0x00, sizeof(tempServer));
	memset(tempUser, 0x00, sizeof(tempUser));
	//memcpy(temp, "{\"server\":\"korea\",\"user\":\"hsy\"}", sizeof(temp));
	sprintf(temp, "{\"server\":\"%s\", \"user\":\"%s\"}", "korea", "hsy");

	r = strlen(temp);
	
	printf("temp(%s)\n", temp);
	printf("r(%d)\n", r);

	item = cJSON_Parse((char *)temp);
	if (item == NULL)
	{
		printf("ERROR : cJSONParse\n");
		return -1;
	}
		
	server = cJSON_GetObjectItem(item, "server");
	user = cJSON_GetObjectItem(item, "user");

	memcpy(tempServer, cJSON_Print(server), sizeof(tempServer));
	memcpy(tempUser, cJSON_Print(user), sizeof(tempUser));;

	printf("tempServer(%s)\n", tempServer);
	printf("tempUser(%s)\n", tempUser);

	return 0;
}
