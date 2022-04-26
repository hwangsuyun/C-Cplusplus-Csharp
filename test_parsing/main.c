#include <stdio.h>

typedef struct
{
	char type               [3];
	char number             [3];
	char percent            [6];
	char tmpSize            [14];
	char size               [15];
} LIST;

int checkSize(char *buff, LIST *list);
int parseData(char *buff, char ch);
char *PAD( char *src );
char *RPAD( char *src );
char *LPAD( char *src );

int main()
{
	LIST list;
	char buff[1024];
	int checkYn = 0;

	memset((char *)&list, 0x00, sizeof(list));
	memset(buff, 0x00, sizeof(buff));
	//memcpy(buff, "SND  4  100%           0/6             6  18-10-08 16:48  TST01.F83.D20181001TEST.0000103359  BCGATOR AGENT 테스트 자료(CJCGV)", sizeof(buff));	
	//memcpy(buff, "SND  4  100%     0/2315181       2315181  18-10-08 12:47  TST01.F83.D20181008TEST.0000103341  BCGATOR AGENT 테스트 자료(CJCGV)", sizeof(buff));	
	memcpy(buff, "SND  4  100%    0/25412376      25412376  18-10-10 10:47  TST01.F83.D20181010TEST.0000103686  BCGATOR AGENT 테스트 자료(CJCGV)", sizeof(buff));
	
	checkSize(buff, &list);
	printf("(%s)\n", list.size);
	if (strncmp(list.percent, "100", 3) == 0) checkYn = 1;
	printf("checkYn(%d)\n", checkYn);	
	return 0;
}


int checkSize(char *buff, LIST *list)
{
	int dataLen = 0;
	int sizeLen = 0;

	dataLen = parseData(buff, 0x20);
	memcpy(list->type, buff + sizeLen, dataLen);
	printf("list->type(%s)\n", list->type);
	sizeLen += dataLen;

	dataLen = parseData(buff + sizeLen, 0x20);
	memcpy(list->number, buff + sizeLen, dataLen);
	printf("list->number(%s)\n", list->number);
	sizeLen += dataLen;

	dataLen = parseData(buff + sizeLen, 0x20);
	memcpy(list->percent, buff + sizeLen, dataLen);
	printf("list->percent(%s)\n", list->percent);
	sizeLen += dataLen;

	dataLen = parseData(buff + sizeLen, 0x20);
	memcpy(list->tmpSize, buff + sizeLen, dataLen);
	printf("list->tmpSize(%s)\n", list->tmpSize);
	sizeLen += dataLen;

	dataLen = parseData(buff + sizeLen, 0x20);
	memcpy(list->size, buff + sizeLen, dataLen);
	printf("list->size(%s)\n", list->size);
	sizeLen += dataLen;

	PAD(list->type);
	PAD(list->number);
	PAD(list->percent);
	PAD(list->tmpSize);
	PAD(list->size);

	return 0;
}


int parseData(char *buff, char ch)
{
	int index = 0;
	int len = 0;
	char *ptr = NULL;

	while (1)
	{
		if (buff[index] == 0x20)
		{
			index++;
			continue;
		}
		ptr = strchr(buff + index, ch);
		len = ptr - buff;
		break;
	}
	return len;
}

char *PAD( char *src )
{
	LPAD( src );
	RPAD( src );

	return src;
}

char *LPAD( char *src )
{
	int i = 0;
	int j = 0;

	char *tmpStr;

	if( *src == 0x00 ) return;

	tmpStr = ( char * )malloc( strlen( src ) + 1 );
	strcpy( tmpStr, src );

	for( ;; ) {
		if( !(*( tmpStr + i ) == 0x20 || *( tmpStr + i ) == 0x09 ) ) break;

		i++;
	}

	strcpy( src, tmpStr + i );
	free( tmpStr );

	return src;
}

char *RPAD( char *src )
{
	int i = 0;
	int j = 0;
	int len;

	char *tmpStr;

	if( *src == 0x00 ) return;

	len                     = strlen( src );
	tmpStr          = ( char * )malloc( len + 1 );
	strcpy( tmpStr, src );

	for( i=len - 1; i != 0; i-- ) {
		if( *( tmpStr + i ) == 0x20 || *( tmpStr + i ) == 0x09 ) {
			src[ i ] = 0x00;
		} else {
			break;
		}
	}

	free( tmpStr );

	return src;
}

