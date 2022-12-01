#include <stdio.h>
#include <string.h>

int main()
{
	int i;
	char temp[200 + 1];
	char data[10 + 1];

	memset(temp, 0x00, sizeof(temp));
	memset(data, 0x00, sizeof(data));
	//memcpy(temp, "01907SITE_CD000ISPH02509TRAN_DATE0002018080902308TRAN_SEQ000389783201910USE_SVC_FG000Y02107USER_ID000NAS329", sizeof(temp));
	memcpy(temp, "01807SITE_CD000MAC02509TRAN_DATE0002018083102308TRAN_SEQ0002403243", sizeof(temp));
	memcpy(data, "USE_SVC_FG", sizeof(data));
	printf("size(%d)\n", strlen(temp));
	printf("temp(%s)\n", temp);
	printf("data(%s)\n", data);

	if (strstr(temp, data) == NULL) 
	{
		printf("not found\n");
	} 
	else 
	{
		printf("found\n");
	}
	return 0;
}
