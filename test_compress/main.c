#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include	<zlib.h>

int compressData(char *in, long inLen, char *out, long outLen)
{
	int ret = 0;

	ret = compress( (Bytef *)out, (uLongf *)&outLen, (Bytef *)in, (uLong)inLen);
	if (ret != Z_OK)
	{
		printf("compress fail.\n");
		printf("Error code = %d\n", ret);
		return -1;
	}
	printf("compress success.\n");
	printf("output = [%s]\n", out); 
	return 0;
}

int uncompressData(char *in, long inLen, char *out, long outLen)
{
	int ret = 0;

	ret = uncompress( (Bytef *)in, (uLongf *)&inLen, (Bytef *)out, (uLong)outLen);
	if (ret != Z_OK)
	{
		printf("uncompress fail\n");
		printf("Error code = %d\n", ret);
		return -1;
	}
	printf("uncompress success.");
	printf("input = [%s]\n", in); 
	return 0;
}

int main()
{
	char	inData[2048];
	char	outData[2048];
	long	inLen, outLen;
	int r;

	strcpy(inData, "ABCDEFG  1122  334 4 55");
	printf("inData(%s)\n", inData);

	inLen = sizeof(inData);
	outLen = sizeof(outData);

//	r = compressData(inData, inLen, outData, outLen);

	r = uncompressData(inData, inLen, outData, outLen);
	if( r < 0 ){
		printf( " ERR: uncompress ret[%d]", r);
		exit(0);
	}
	printf("%s\n", inData);
}

	
