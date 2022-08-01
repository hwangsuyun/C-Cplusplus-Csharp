#define	__TF	"util_lib.c"

#include			<stdio.h>
#include			<stdlib.h>
#include			<string.h>
#include			<fcntl.h>

#include	"lib_fn.h"

void	NulltoSpace(char	*buf, int size)
{
	int		i;

	for(i = 0 ; i < size ; i++){
		if(buf[i] == 0x0){
			buf[i] = ' ';
		}
	}
}

void Rm_Mid_Space(char *str_buff, int size)
	{
	int     i, j;
	char    t_buff[5120];

	j = 0;
	for(i=0;i<size;i++) {
		if (*(str_buff+i) == 0x20) {
			continue;
		}
		*(str_buff+j) = *(str_buff+i);
		j++;
	}
	*(str_buff+j) = 0x00;
}

void	LPAD( char *src )
{
	int			i = 0;
	int			j = 0;

	char		*tmpStr;

	tmpStr		= ( char * )malloc( strlen( src ) + 1 );
	strcpy( tmpStr , src );

	for( ;; ) {
		if( !(*( tmpStr + i ) == 0x20 || *( tmpStr + i ) == 0x09 ) ) break;

		i++;
	}

	strcpy( src , tmpStr + i ); 
	free( tmpStr );
}

void	RPAD( char *src )
{
	int			i = 0;
	int			j = 0;
	int			len;

	char		*tmpStr;

	len			= strlen( src );
	tmpStr		= ( char * )malloc( len + 1 );
	strcpy( tmpStr , src );

	for( i = len - 1 ; i != 0 ; i-- ) {
		if( *( tmpStr + i ) == 0x20 || *( tmpStr + i ) == 0x09 ) {
			src[ i ]	= 0x00;
		} else {
			break;
		} 
	}

	free( tmpStr );
}

void	PAD( char *src )
{
	RPAD( src );
	LPAD( src );
}

void	Rm_CRLF( char *src )
{
	int			i = 0;
	int			j = 0;
	int			len;

	char		*tmpStr;

	len			= strlen( src );
	tmpStr		= ( char * )malloc( len + 1 );
	strcpy( tmpStr , src );

	for( i = len - 1 ; i != 0 ; i-- ) {
		if( *( tmpStr + i ) == 0x0d || *( tmpStr + i ) == 0x0a ) {
			src[ i ]	= 0x00;
		} else {
			break;
		} 
	}

	free( tmpStr );
}

int		Get_Dummy_CH( char	*buf, char	data[][100], char	ch )
{
	int		i = 0;
	char	*temp;

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
		i++;
		strcpy( buf, temp + 1 );
	}

	return i;
}

int		Get_Dummy_CH_New( char	*buf, char	*data, int	len, char	ch )
{
	int		i = 0;
	char	*temp;
	int		cp_point;
	int		templen;
	int		buflen;

	while(1)
	{
		cp_point = i * len;

		temp = strchr( buf, ch );
		if ( temp == NULL ){
			templen = 0;
		}
		else
		{
			templen = strlen( temp );
		}
		buflen = strlen( buf );

		memcpy( data+cp_point, buf, buflen - templen );
		if( (buflen-templen) >= len )
		{
			data[cp_point+len-1] = 0x00;
		}
		else
		{
			data[cp_point+buflen-templen] = 0x00;
		}
		i++;

		if ( temp == NULL ){
			break;
		}

		strcpy( buf, temp + 1 );
	}

	return i;
}

int     Get_Serial(long *s_no)
{
	int     r, fd;
	long    val;

	fd = open("./.seqno", O_CREAT | O_RDWR, 0600);
	if ( fd < 0 ){
		return -1;
	}

	lseek(fd, 0, 0);
	r = read(fd, (char*)&val, sizeof(val) );
	if ( val < 0 ){
		val = 0;
	}
	val++;
	val = (val % 1000000 );
	lseek(fd, 0, 0);
	r = write(fd, (char*)&val, sizeof(val) );
	close(fd);
	*s_no = val;
	return 0;
}


int hex2str(unsigned char *r_buff, char *s_buff, int s_len)
{
	int i;
	unsigned char wbuf[3072];

	memset( wbuf, 0x00, sizeof(wbuf) );

	for ( i = 0; i < s_len; i++ )
	{
		sprintf( wbuf+i*2, "%02x", r_buff[i] );
	}

	memcpy( s_buff, wbuf, s_len*2 );

	return s_len * 2;
}


int buff_check(char *dest, char *src)
{
        if ( src[0] != 0x02 )
                return(-1);

        if ( src[strlen(src)-1] != 0x03 )
                return(-2);

        strncpy(dest, src + 1, strlen(src) - 2 );

        return(0);
}
