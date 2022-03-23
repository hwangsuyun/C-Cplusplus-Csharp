#ifndef __SAMSUNG_LIB_H__
#define __SAMSUNG_LIB_H__

#include "cJSON.h"

/****************************HTTP 데이터*/
typedef struct {
        char ip[15 + 1];
        int port;
        char appId[30 + 1];
	char command[10 + 1];
        char serviceId[50 + 1];
        char processNm[30 + 1];
        int sleepTime;
} HTTPDATA;

/****************************JSON 송신 포멧*/
/*
typedef struct
{
	cJson *item;
	cJson *authKey;
	cJson *command;
	cJson *siteDir;
	cJson *cardDir;
	cJson *srcFileNm;
	cJson *desFileNm;
	cJson *dummy1;
	cJson *dummy2;
} REQJSON;
*/
/****************************JSON 수신 포멧*/
typedef struct
{
        cJSON *item;
        cJSON *authKey;
        cJSON *command;
        cJSON *resCode; 
        cJSON *resultMsg;
} RESJSON;

#endif
