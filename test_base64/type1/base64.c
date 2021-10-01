#include <stdio.h>
#include <string.h>

void encodeBASE64( unsigned char *pInput, int nLen, char *pOutput, int *nOutLen )
{
	int i;
	int j = 0;
	char buf[4];
	char vec[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	memset( buf, 0x00, sizeof( buf ) );
	for( i=0; i<nLen; i++ ) {
		if( ( ( i / 3 ) > 0 ) && ( ( i % 3 ) == 0 ) ) {
			*(pOutput+j+0) = vec[(buf[0] & 0xFC) >> 2];
			*(pOutput+j+1) = vec[((buf[0] & 0x03) << 4) | (buf[1] >> 4)];
			*(pOutput+j+2) = vec[((buf[1] & 0x0F) << 2) | (buf[2] >> 6)];
			*(pOutput+j+3) = vec[buf[2] & 0x3F];
			j+=4;
		}
		buf[i%3] = *(pInput+i);
	}

	switch( i % 3 ) {
		case 1:
			buf[1]=0x0;
			*(pOutput+j+0) = vec[(buf[0] & 0xFC) >> 2];
			*(pOutput+j+1) = vec[((buf[0] & 0x03) << 4) | (buf[1] >> 4)];
			*(pOutput+j+2) = '=';
			*(pOutput+j+3) = '=';
			*(pOutput+j+4) = '\0';
			break;
		case 2:
			buf[2]=0x0;
			*(pOutput+j+0) = vec[(buf[0] & 0xFC) >> 2];
			*(pOutput+j+1) = vec[((buf[0] & 0x03) << 4) | (buf[1] >> 4)];
			*(pOutput+j+2) = vec[((buf[1] & 0x0F) << 2) | (buf[2] >> 6)];
			*(pOutput+j+3) = '=';
			*(pOutput+j+4) = '\0';
			break;
		case 0:
			*(pOutput+j+0) = vec[(buf[0] & 0xFC) >> 2];
			*(pOutput+j+1) = vec[((buf[0] & 0x03) << 4) | (buf[1] >> 4)];
			*(pOutput+j+2) = vec[((buf[1] & 0x0F) << 2) | (buf[2] >> 6)];
			*(pOutput+j+3) = vec[buf[2] & 0x3F];
			*(pOutput+j+4) = '\0';
			break;
	}
	*nOutLen = strlen( (char *)pOutput );
}

int decodeBASE64( char *pInput, int len, unsigned char *pOutput, int *nOutLen )
{
	int i;
	int j=0;
	int nLen = 0;
	char buf[5];
	char *pout = pOutput;
	len = len;
	nLen = strlen(pInput);

	memset( buf, 0x00, sizeof( buf ) );
	for( i=0; i<nLen; i++ ) {
		if(((i/4) > 0) && ((i%4) == 0)) {
			pout[j+0] = (buf[0] << 2) | (buf[1] >> 4);
			pout[j+1] = (buf[1] << 4) | (buf[2] >> 2);
			pout[j+2] = (buf[2] << 6) | buf[3];
			j+=3;
		}
		if((*(pInput+i) >= 'A') && (*(pInput+i) <= 'Z'))
			buf[i%4] = *(pInput+i)-'A';
		else if((*(pInput+i) >= 'a') && (*(pInput+i) <= 'z'))
			buf[i%4] = *(pInput+i)-'a'+26;
		else if((*(pInput+i) >= '0') && (*(pInput+i) <= '9'))
			buf[i%4] = *(pInput+i)-'0'+52;
		else if(*(pInput+i) == '+')
			buf[i%4] = 62;
		else if(*(pInput+i) == '/')
			buf[i%4] = 63;
		else if(*(pInput+i) == '=') {
			buf[i%4] = 0;
			break;
		}
		else return -1;
	}
	switch( i % 4 ) {
		case 3:
			pout[j+2] = '\0';
			pout[j+0] = (buf[0] << 2) | (buf[1] >> 4);
			pout[j+1] = (buf[1] << 4) | (buf[2] >> 2);
			*nOutLen = j+2;
			break;
		case 2:
			pout[j+1] = '\0';
			pout[j+0] = (buf[0] << 2) | (buf[1] >> 4);
			*nOutLen = j+1;
			break;
		case 1:
			pout[j+1] = '\0';
			*nOutLen = j;
			break;
		case 0:
			pout[j+0] = (buf[0] << 2) | (buf[1] >> 4);
			pout[j+1] = (buf[1] << 4) | (buf[2] >> 2);
			pout[j+2] = (buf[2] << 6) | buf[3];
			pout[j+3] = '\0';
			*nOutLen = j+3;
	}

	return 0;
}
