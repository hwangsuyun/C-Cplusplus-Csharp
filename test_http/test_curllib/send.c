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
	LOG( 5, _OK_, "파일 경로 세팅!!(%s)(%s)(%s)", proc_info->src_dir, sFileName, sFilePath );

	memset(url, 0x00, sizeof(url));
	memset(sSendBuff, 0x00, sizeof(sSendBuff));
	
	/* 요청전문 생성(JSON) */
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
	/* 통신(Curl 라이브러리를 이용한 HTTP 프로토콜) */
	ret = communicateHttpProtocol(url, sSendBuff, sRecvBuff, sizeof(sRecvBuff));
	if (ret < 0)
	{
		LOG( 9, _ERROR_, "communicateHttpProtocol[%d], sSendBuff : (%s)", ret, sSendBuff );
		return -1;
	}	

	memset(repCd, 0x00, sizeof(repCd));
	/* 응답전문 파싱(JSON) */
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

	// curl 초기화
	curl_global_init(CURL_GLOBAL_ALL);
	// context 생성
	curl = curl_easy_init();
	if (curl) {
		CURLcode res;
		struct curl_slist *chunk = NULL;

		// header 설정
		chunk = curl_slist_append(chunk, "Appid:SP_TEST_V1");
		chunk = curl_slist_append(chunk, "Content-Type:text/plain");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

		// 요청한 결과 data를 메모리에 받아 오기
		HTMLCONTEXTDATA mem = {0, 0};
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mem);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callbackWriteMemory);

		curl_easy_setopt(curl, CURLOPT_URL, url);
		// POST 데이터 전송
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, sSendBuff);

		// 페이지 읽어오기
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			LOG( 9, _ERROR_, "curl_easy_perform() failed" );
			return -1;
		}
		
		memcpy(sRecvBuff, mem.data, sRecvBuffLen);
		LOG( 5, _OK_, "sRecvBuff(%s)", sRecvBuff );
		// 사용한 메모리 해제
        	freeHtmlContextData( &mem );
		// 헤더 리스트 해제
		curl_slist_free_all(chunk);
	}
	// context 해제
	curl_easy_cleanup(curl);
	// lib 해제
	curl_global_cleanup();
	
	return 0;
}
