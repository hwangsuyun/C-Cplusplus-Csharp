#ifndef __COMMON_H__
#define __COMMON_H__

#include "tlink.h"
#include "db_apply.h"

#define TRY_CNT         3
#define TRY_TIME        5

typedef struct
{
	char ora_info[ DBSTR_LEN + 1 ];
	long sleep_time;
} INFO;

INFO info;

typedef struct
{
	char ip[ IP_LEN + 1 ];
	int port;
	char id[ ID_LEN + 1 ];
	char passwd[ PASSWD_LEN + 1 ];
	char source[ PATH_LEN + 1 ];
	char backup[ PATH_LEN + 1 ];
	char target[ PATH_LEN + 1 ];
	char error[ PATH_LEN + 1 ];
	char process[ PROCESS_NM_LEN ];
} SITE_INFO;

#endif
