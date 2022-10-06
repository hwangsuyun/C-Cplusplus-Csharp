#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "extern.h"
#include "line_check.h"

#define OK  0
#define ERR 1



int setOkShmLineInfo( unsigned char **shm, int server_no )
{
	*(*shm + server_no) = OK;

	return 0;
}

int incRunCnt( unsigned char **shm, int server_cnt )
{
	
	if ( *(*shm + server_cnt + 1) == 255 )
			return -1;
	
	return ++(*(*shm + server_cnt + 1));
}

int decRunCnt( unsigned char **shm, int server_cnt )
{
	if ( *(*shm + server_cnt + 1) == 0 )
			return -1;
		
	(*(*shm + server_cnt + 1))--;

	return 0;
}

int setRunCnt( unsigned char **shm, int server_cnt, int run_cnt )
{
	if ( run_cnt < 0 || run_cnt > 255)
		return -1;

	*(*shm + server_cnt + 1) = run_cnt;	

	return 0;
}

int getRunCnt( unsigned char **shm, int server_cnt )
{
	return *(*shm + server_cnt + 1);
}

