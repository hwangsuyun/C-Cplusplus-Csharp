#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "extern.h"
#include "line_check.h"

#define OK  0
#define ERR 1


int setErrorShmLineInfo( unsigned char **shm, int server_no )
{
	*(*shm + server_no) = ERR;

	return 0;
}

int setOkShmLineInfo( unsigned char **shm, int server_no )
{
	*(*shm + server_no) = OK;

	return 0;
}

// server_cnt 서버의 총 개수
// server_no  서버의 순번/순서
int getErrorServer( unsigned char **shm, int server_cnt, int server_no, int *server_index )
{	
	int i;

	if(server_no <= 0 || server_no >= server_cnt + 1)
		return -1;

	i = server_no;
	for( i = server_no; i <= server_cnt; i++ )
	{
		if(*(*shm + i) == ERR)
		{
			*server_index = i;
			return 1;
		}
	}
	
	return 0;
}

int getNormalServer( unsigned char **shm, int server_cnt )
{
	int i, cur_idx, next_idx;
	char cur_svr_num;
	char next_svr_num;

	cur_svr_num = **shm;
	cur_idx = atoi(&cur_svr_num);

//printf("Current Server Number:%c\n", cur_svr_num);

	for( i=0; i<server_cnt; i++ ) {
		next_idx = (cur_idx % server_cnt) +1;

		if( *(*shm + next_idx) == OK ){
			sprintf(&next_svr_num, "%d", next_idx);
			**shm = next_svr_num;

//printf("Next Server Number:%c\n", next_svr_num);
			return 0;
		}
		cur_idx = next_idx;
	}

//printf("Does not exist, the available server");
	return -1;
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

