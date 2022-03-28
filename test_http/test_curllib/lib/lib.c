#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "lib.h"

#define CR 0x0d
#define LF 0x0a

/****************************************************************/
/* ���� �۽� ��û(CM01)                                         */
/****************************************************************/
int makeCM01( char *authKey, char *siteDir, char *srcFileNm, char *sSendBuff )
{
	sprintf( sSendBuff, "{                         \
			      \"authKey\"   : \"%s\", \
			      \"command\"   : \"%s\", \
                              \"siteDir\"   : \"%s\", \
                              \"cardDir\"   : \"%s\", \
                              \"srcFileNm\" : \"%s\", \
                              \"desFileNm\" : \"%s\", \
                              \"dummy1\"    : \"%s\", \
                              \"dummy2\"    : \"%s\"  \
			    }"
 		 , authKey, "CM01", siteDir, siteDir, srcFileNm, srcFileNm, NULL, NULL );

	LOG( 5, _OK_, "makeCM01(%s)", sSendBuff );
        return 0;
}

/****************************************************************/
/* ���� ���� ��û(CM02)                                         */
/****************************************************************/
int makeCM02( char *authKey, char *siteDir, char *srcFileNm, char *sSendBuff )
{
	sprintf( sSendBuff, "{                         \
			      \"authKey\"   : \"%s\", \
			      \"command\"   : \"%s\", \
                              \"siteDir\"   : \"%s\", \
                              \"cardDir\"   : \"%s\", \
                              \"srcFileNm\" : \"%s\", \
                              \"desFileNm\" : \"%s\", \
                              \"dummy1\"    : \"%s\", \
                              \"dummy2\"    : \"%s\"  \
                            }"
                 , authKey, "CM02", siteDir, siteDir, srcFileNm, srcFileNm, NULL, NULL );	

	LOG( 5, _OK_, "makeCM02(%s)", sSendBuff );
	return 0;
}

/****************************************************************/
/* ���丮 ��ȸ ��û(CM03)                                     */
/****************************************************************/
int makeCM03( char *authKey, char *cardDir, char *srcFileNm, char *sSendBuff )
{
	sprintf( sSendBuff, "{                         \
 	                      \"authKey\"   : \"%s\", \
	                      \"command\"   : \"%s\", \
	                      \"siteDir\"   : \"%s\", \
	                      \"cardDir\"   : \"%s\", \
	                      \"srcFileNm\" : \"%s\", \
	                      \"desFileNm\" : \"%s\", \
	                      \"dummy1\"    : \"%s\", \
	                      \"dummy2\"    : \"%s\"  \
	                    }"
	         , authKey, "CM03", NULL, cardDir, NULL, NULL, NULL, NULL );

	LOG( 5, _OK_, "makeCM03(%s)", sSendBuff );
	return 0;
}

/****************************************************************/
/* ���μ��� �⵿ ��û(CM04)                                     */
/****************************************************************/
int makeCM04( char *authKey, char *cardDir, char *desFileNm,  char *sSendBuff )
{
	sprintf( sSendBuff, "{                         \
 	                      \"authKey\"   : \"%s\", \
	                      \"command\"   : \"%s\", \
	                      \"siteDir\"   : \"%s\", \
	                      \"cardDir\"   : \"%s\", \
	                      \"srcFileNm\" : \"%s\", \
	                      \"desFileNm\" : \"%s\", \
	                      \"dummy1\"    : \"%s\", \
	                      \"dummy2\"    : \"%s\"  \
	                    }"
	         , authKey, "CM04", NULL, cardDir, NULL, desFileNm, "", "" );

	LOG( 5, _OK_, "makeCM04(%s)", sSendBuff );
	return 0;
}

/****************************************************************/
/* ���ϼ۽� ����(����)                                          */
/****************************************************************/
int parseCM( char *sRecvBuff, char *repCd )
{
        RESJSON rJson;

        rJson.item = cJSON_Parse((char *)sRecvBuff);
	if (rJson.item == NULL) 
	{
        	LOG( 9, _ERROR_, "cJSON_Parse, rJson.item : [%p]", rJson.item );
		return -1;
	}
	
        rJson.authKey = cJSON_GetObjectItem(rJson.item, "authKey");
        rJson.command = cJSON_GetObjectItem(rJson.item, "command");
        rJson.resCode = cJSON_GetObjectItem(rJson.item, "resCode");
        rJson.resultMsg = cJSON_GetObjectItem(rJson.item, "resultMsg");

        memcpy(repCd, cJSON_Print(rJson.resCode) + 1, strlen(cJSON_Print(rJson.resCode)) - 2);

	if (strncmp(repCd, "0000", 4) != 0) return -1;

        return 0;
}
