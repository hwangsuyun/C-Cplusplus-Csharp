#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#include "tlink.h"
#include "sftp_lib.h"

extern char fileName[100][1024];
extern int cnt;

size_t callbackWriteMemory(void *ptr, size_t size, size_t nmemb, void *stream)
{
	char temp1[1024];
	char temp2[1024];
	char *str;
	size_t realsize = size * nmemb;
	CONTEXTDATA *mem = (CONTEXTDATA *)stream;

	memset(temp1, 0x00, sizeof(temp1));
	memset(temp2, 0x00, sizeof(temp2));
	
	mem->data = (char*)realloc(mem->data, mem->size + realsize + 1);
	if (mem->data) {
		memcpy(&(mem->data[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->data[mem->size] = '\0';
	}

	memcpy(temp1, ptr, sizeof(temp1));

	if (strstr(temp1, ".end") != NULL)
	{
		strncpy(temp2, temp1, strstr(temp1, ".end") - temp1);
		str = strrchr(temp2, 0x20) + 1;
		strcpy(fileName[cnt], str);
		cnt++;
	}

	return realsize;
}

static void freeContextData(CONTEXTDATA *p)
{
	p ? (free(p->data), p->data = NULL, p->size = 0) : 1;
}

static size_t callbackWriteFile(void *buffer, size_t size, size_t nmemb, void *stream)
{
	struct FtpFile *out = (struct FtpFile *)stream;
	if(out && !out->stream) {
		/* open file for writing */
		out->stream = fopen(out->filename, "wb");
		if(!out->stream) return -1; /* failure, can't open file to write */
	}
	return fwrite(buffer, size, nmemb, out->stream);
}

int checkEndFile(char *url)
{
	LOG( 5, _OK_, "START checkEndFile" );

	CURL *curl;
	
	// curl 초기화
	curl_global_init(CURL_GLOBAL_DEFAULT);
	// context 생성
	curl = curl_easy_init();
	if (curl) {
		CURLcode res;

		curl_easy_setopt(curl, CURLOPT_URL, url);

		// 요청한 결과 data를 메모리에 받아 오기
		CONTEXTDATA mem = {0, 0};
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callbackWriteMemory);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mem);

		// curl 라이브러리는 멀티 쓰레드 환경에서 SIGALRM 시그널을 발생시킴 그래서 사용안하게 변경
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

		// 페이지 읽어오기
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			LOG( 9, _ERROR_, "curl_easy_perform() failed: (%d)(%s)", res, curl_easy_strerror(res));
			return res;
		}

		// 사용한 메모리 해제
		freeContextData( &mem );
	}
	// context 해제
	curl_easy_cleanup(curl);
	// lib 해제
	curl_global_cleanup();

	if (cnt == 0) return -1;

	LOG( 5, _OK_, "END checkEndFile" );

	return 0;
}

int communicateSftpProtocol(char *fullUrl, char *path, char* backPath, char *targetPath)
{
	LOG( 5, _OK_, "START communicateSftpProtocol" );

	CURL *curl;
	double speed_download, total_time;

	// curl 초기화
	curl_global_init(CURL_GLOBAL_DEFAULT);
	// context 생성
	curl = curl_easy_init();
	if (curl) {
		CURLcode res;
		struct curl_slist *chunk = NULL;
		char renameFile[100];
		char rmEndFile[100];
		struct FtpFile ftpfile = {targetPath,  NULL};  

		memset(renameFile, 0x00, sizeof(renameFile));
		memset(rmEndFile, 0x00, sizeof(rmEndFile));
		sprintf(renameFile, "%s%s%s%s", "rename ", path, " ", backPath);
		sprintf(rmEndFile, "%s%s%s", "rm ", path, ".end");
		
		//LOG( 5, _OK_, "renameFile: (%s)", renameFile);
		//LOG( 5, _OK_, "rmEndFile: (%s)", rmEndFile);
			
		chunk = curl_slist_append(chunk, renameFile);
		chunk = curl_slist_append(chunk, rmEndFile);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callbackWriteFile);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);

		curl_easy_setopt(curl, CURLOPT_URL, fullUrl);
		curl_easy_setopt(curl, CURLOPT_POSTQUOTE, chunk);

		// 페이지 읽어오기
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			LOG( 9, _ERROR_, "curl_easy_perform() failed: (%d)(%s)", res, curl_easy_strerror(res));
			return res;
		}

		curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD, &speed_download);
		curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);
		//printf("Speed: %.3f bytes/sec during %.3f seconds\n", speed_download, total_time);

		// 헤더 리스트 해제
		curl_slist_free_all (chunk);

		if(ftpfile.stream) fclose(ftpfile.stream); /* close the local file */
	}

	// context 해제
	curl_easy_cleanup(curl);
	// lib 해제
	curl_global_cleanup();
	
	LOG( 5, _OK_, "END communicateSftpProtocol" );

	return 0;
}
