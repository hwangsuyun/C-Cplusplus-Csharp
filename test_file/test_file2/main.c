#include <stdio.h>

int main()
{
	FILE *oldFp, *newFp;
	char fileName[100 + 1];
	char newFileName[100 + 1];
	char ReadRec[1024 + 1];
	int str_len = 0;
	int cnt = 0;
	int isSecondLine = 0;
	int size = 0;

	memset(fileName, 0x00, sizeof(fileName));
	memset(newFileName, 0x00, sizeof(newFileName));
	memset(ReadRec, 0x00, sizeof(ReadRec));
	memcpy(fileName, "F1820824.dat", sizeof(fileName));
	memcpy(newFileName, "F1820824.dat.new", sizeof(newFileName));

	if ((oldFp = fopen(fileName, "r")) == NULL)
	{
		printf("[ERROR] OLD FILE OPEN\n");
		return -1;
	}
	if ((newFp = fopen(newFileName, "w")) == NULL)
	{
		printf("[ERROR] NEW FILE OPEN\n");
		return -1;
	}
	

	while( !feof(oldFp) )
	{
		if (cnt < 2)
		{
			memset(ReadRec, 0x00, sizeof(ReadRec));
			str_len = fread( ReadRec, 200, 1, oldFp );
			ReadRec[200] = 0x00;
			fprintf(newFp, "%s\n", ReadRec );
			size += 200;
			
			if ((isSecondLine) && (cnt == 1))
			{
				size += 1; //\r\n
				fseek(oldFp, size, SEEK_SET);
				isSecondLine = 0;
				cnt = 0;
			} 
			else
			{
				cnt++;
			}
		}
		else
		{
			char tempStr3[100 + 1];
			char tempStr4[100 + 1];

			memset(tempStr3, 0x00, sizeof(tempStr3));
			memset(tempStr4, 0x00, sizeof(tempStr4));
			str_len = fread( tempStr3, 100, 1, oldFp );
			size += 100;
			size += 1; //\r\n
			fseek(oldFp, size, SEEK_SET);
			str_len = fread( tempStr4, 100, 1, oldFp );
			size += 100;
			tempStr3[100] = 0x00;
			tempStr4[100] = 0x00;
			
			memset(ReadRec, 0x00, sizeof(ReadRec));
			sprintf(ReadRec, "%s%s", tempStr3, tempStr4);
			fprintf(newFp, "%s\n", ReadRec);
			cnt = 0;
			isSecondLine = 1;
		}
	}
	fclose( oldFp );
	fclose( newFp );


	return 0;
};
