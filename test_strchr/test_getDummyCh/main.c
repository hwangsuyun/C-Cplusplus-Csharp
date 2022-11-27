#include <stdio.h>
#include <string.h>

#define FS 0x1C

int Get_Dummy_CH(char *buf, char data[][100], char ch);

int main()
{
	int r, i;
	char kicc_data[20][100];
	char client_send[512];
	
	memset(client_send, 0x00, sizeof(client_send));
	memset((char *)kicc_data, 0x00, sizeof(kicc_data));
	
	sprintf(client_send, "%c%s%c%s%c%c%s%c%s%c%c",
			0x02, "EF88888888D1PI012440", FS, "1081758", FS, '@', "5387208140166290", FS, "1602", FS, 0x03);
	
	printf("client_send(%s)\n", client_send);
	r = Get_Dummy_CH(client_send, kicc_data, FS);
	
	for (i = 0; i < strlen((char *)kicc_data); i++)
	{
		printf("kicc_data[%d](%s)\n", i, kicc_data[i]);	
	}
	
	return 0;
}

int Get_Dummy_CH( char *buf, char data[][100], char ch )
{
	int  i = 0;  
	char *temp;  

	while(1)
	{
		temp = strchr( buf, ch );
		memset( data[i], 0x00, sizeof(data[i]) );
		if ( temp == NULL ){ 
			memcpy( data[i], buf, strlen(buf) );
			i++;    
			break;  
		}       
		memcpy( data[i], buf, strlen(buf) - strlen(temp) );
		printf("data[i](%s)\n", data[i]);
		i++;    
		strcpy( buf, temp + 1 );
	}

	return i;
}

