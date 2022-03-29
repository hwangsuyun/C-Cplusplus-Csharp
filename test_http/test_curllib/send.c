#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#include "date.h"
#include "tlink.h"
#include "common.h"
#include "samsung_lib.h"
#include "Curl.h"

int SAMSUNGSend( PROC_INFO *proc_info, HTTPDATA *httpData, char *sFileName )
{
	int ret;
	char url[100 + 1];
	char sFilePath[ BUFSIZ ];
	char sSendBuff[ BUFSIZ + 1 ];
	char sRecvBuff[ BUFSIZ + 1 ];
	char repCd[4 + 1];

	memset( sFilePath, 0x00, sizeof( sFilePath ) );
	sprintf( sFilePath, "%s/%s",proc_info->src_dir, sFileName );
	LOG( 5, _OK_, "���� ��� ����!!(%s)(%s)(%s)", proc_info->src_dir, sFileName, sFilePath );

	memset(url, 0x00, sizeof(url));
	memset(sSendBuff, 0x00, sizeof(sSendBuff));
	
	/* ��û���� ����(JSON) */
	sprintf(url, "%s%s%s%d%s%s", "http://", proc_info->ip, ":", proc_info->port, "/", httpData->serviceId);
	switch (atoi(httpData->command + 3)) {
		case 1:
			ret = makeCM01(httpData->appId, proc_info->src_dir, sFileName, sSendBuff);
			break;
		case 2: 
			ret = makeCM02(httpData->appId, proc_info->src_dir, sFileName, sSendBuff);
			break;
		case 3:
			ret = makeCM03(httpData->appId, proc_info->src_dir, "", sSendBuff);
			break;
		case 4:
			ret = makeCM04(httpData->appId, proc_info->src_dir, httpData->processNm, sSendBuff);
			break;
	}
	
	LOG( 5, _OK_, "URL(%s)", url );
	LOG( 5, _OK_, "sSendBuff(%s)", sSendBuff );

	memset( sRecvBuff, 0x00, sizeof( sRecvBuff ) );
	/* ���(Curl ���̺귯���� �̿��� HTTP ��������) */
	ret = communicateHttpProtocol(url, sSendBuff, sRecvBuff, sizeof(sRecvBuff));
	if (ret < 0)
	{
		LOG( 9, _ERROR_, "communicateHttpProtocol[%d], sSendBuff : (%s)", ret, sSendBuff );
		return -1;
	}	

	memset(repCd, 0x00, sizeof(repCd));
	/* �������� �Ľ�(JSON) */
	ret = parseCM(sRecvBuff, repCd);
	if (ret < 0)
	{
		LOG( 9, _ERROR_, "parseCM[%d], repCd : (%s)", ret, repCd );
		return -1;
	}
	LOG( 5, _OK_, "repCd(%s)", repCd );

	return 0;
}

int communicateHttpProtocol(char *url, char *sSendBuff, char *sRecvBuff, int sRecvBuffLen)
{
	CURL *curl;

	// curl �ʱ�ȭ
	curl_global_init(CURL_GLOBAL_ALL);
	// context ����
	curl = curl_easy_init();
	if (curl) {
		CURLcode res;
		struct curl_slist *chunk = NULL;

		// header ����
		chunk = curl_slist_append(chunk, "Appid:SP_TEST_V1");
		chunk = curl_slist_append(chunk, "Content-Type:text/plain");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

		// ��û�� ��� data�� �޸𸮿� �޾� ����
		HTMLCONTEXTDATA mem = {0, 0};
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mem);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callbackWriteMemory);

		curl_easy_setopt(curl, CURLOPT_URL, url);
		// POST ������ ����
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, sSendBuff);

		// ������ �о����
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			LOG( 9, _ERROR_, "curl_easy_perform() failed" );
			return -1;
		}
		
		memcpy(sRecvBuff, mem.data, sRecvBuffLen);
		LOG( 5, _OK_, "sRecvBuff(%s)", sRecvBuff );
		// ����� �޸� ����
        	freeHtmlContextData( &mem );
		// ��� ����Ʈ ����
		curl_slist_free_all(chunk);
	}
	// context ����
	curl_easy_cleanup(curl);
	// lib ����
	curl_global_cleanup();
	
	return 0;
}
