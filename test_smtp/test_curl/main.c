/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2017, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/ 

/* <DESC>
 * Send e-mail with SMTP
 * </DESC>
 */ 

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

/*
 * For an SMTP example using the multi interface please see smtp-multi.c.
 */ 

/* The libcurl options want plain addresses, the viewable headers in the mail
 * can very well get a full name as well.
 */ 
/*
char *payload_text[] = {
	"To: " "syhwang@nastech.co.kr" "\r\n",
	"From: " "nastech_op@nastech.co.kr" "\r\n",
	"Cc: " "syhwang@nastech.co.kr" "\r\n",
	"Subject: SMTP example message\r\n",
	"\r\n", 
	"The body of the message starts here.\r\n",
	"\r\n",
	"It could be a lot of lines, could be MIME encoded, whatever.\r\n",
	NULL
};
*/
typedef struct {
	char smtp_host[100];
	char send_mail_addr[100];
	char send_name[100];
	char recv_to_addr[500];
	char recv_cc_addr[500];
} MAILINFO;
MAILINFO mInfo;

struct upload_status {
	int lines_read;
};

size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
	char *payload_text[] = {
		"To: ", mInfo.recv_to_addr, "\r\n",
		"From: ", mInfo.send_mail_addr, "\r\n",
		"Cc: ", mInfo.recv_cc_addr, "\r\n",
		"Subject: SMTP example message\r\n",
		"\r\n", 
		"The body of the message starts here.\r\n",
		"\r\n",
		"It could be a lot of lines, could be MIME encoded, whatever.\r\n",
		NULL
	};


	struct upload_status *upload_ctx = (struct upload_status *)userp;
	char *data;

	if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
		return 0;
	}

	data = payload_text[upload_ctx->lines_read];

	if(data) {
		size_t len = strlen(data);
		memcpy(ptr, data, len);
		upload_ctx->lines_read++;

		return len;
	}

	return 0;
}

int sendSmtp(MAILINFO *mInfo)
{
	CURL *curl;
	CURLcode res = CURLE_OK;
	struct curl_slist *recipients = NULL;
	struct upload_status upload_ctx;

	upload_ctx.lines_read = 0;

	/*
	printf("smtp_host(%s)\n", mInfo->smtp_host);
	printf("send_mail_addr(%s)\n", mInfo->send_mail_addr);
	printf("send_name(%s)\n", mInfo->send_name);
	printf("recv_to_addr(%s)\n", mInfo->recv_to_addr);
	printf("recv_cc_addr(%s)\n", mInfo->recv_cc_addr);
	*/

	curl = curl_easy_init();
	if(curl) {
		/* This is the URL for your mailserver */ 
		curl_easy_setopt(curl, CURLOPT_URL, "smtp://133.9.100.5");

		/* Note that this option isn't strictly required, omitting it will result
		 * in libcurl sending the MAIL FROM command with empty sender data. All
		 * autoresponses should have an empty reverse-path, and should be directed
		 * to the address in the reverse-path which triggered them. Otherwise,
		 * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
		 * details.
		 */ 
		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, mInfo->send_mail_addr);

		/* Add two recipients, in this particular case they correspond to the
		 * To: and Cc: addressees in the header, but they could be any kind of
		 * recipient. */ 
		recipients = curl_slist_append(recipients, mInfo->recv_to_addr);
		recipients = curl_slist_append(recipients, "sjjung@nastech.co.kr");
		recipients = curl_slist_append(recipients, mInfo->recv_cc_addr);
		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

		/* We're using a callback function to specify the payload (the headers and
		 * body of the message). You could just use the CURLOPT_READDATA option to
		 * specify a FILE pointer to read from. */ 
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
		curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

		/* Send the message */ 
		res = curl_easy_perform(curl);

		/* Check for errors */ 
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));

		/* Free the list of recipients */ 
		curl_slist_free_all(recipients);

		/* curl won't send the QUIT command until you call cleanup, so you should
		 * be able to re-use this connection for additional messages (setting
		 * CURLOPT_MAIL_FROM and CURLOPT_MAIL_RCPT as required, and calling
		 * curl_easy_perform() again. It may not be a good idea to keep the
		 * connection open for a very long time though (more than a few minutes
		 * may result in the server timing out the connection), and you do want to
		 * clean up in the end.
		 */ 
		curl_easy_cleanup(curl);
	}

	return (int)res;
}

int main()
{
	int ret = 0;
	char sIniPath[100];
	FILE *fp;
	
	memset(sIniPath, 0x00, sizeof(sIniPath));
	memcpy(sIniPath, "./MailSender.properties", sizeof(sIniPath));

//	fp = fopen(sIniPath, "r");
//	if (fp == NULL)
//	{
//		printf("ERROR FILE NOT FOUND(%s)\n", sIniPath);
//		return -1;
//	}

//	ret = initialize(sIniPath);
//	if( ret != 0 ) {
//		return -1;
//	}
	memcpy(mInfo.smtp_host, "133.9.100.5", sizeof(mInfo.smtp_host));
	memcpy(mInfo.send_mail_addr, "nastech_op@nastech.co.kr", sizeof(mInfo.send_mail_addr));
	memcpy(mInfo.send_name, "나스텍운영팀", sizeof(mInfo.send_name));
	memcpy(mInfo.recv_to_addr, "syhwang@nastech.co.kr", sizeof(mInfo.recv_to_addr));
	memcpy(mInfo.recv_cc_addr, "syhwang@nastech.co.kr", sizeof(mInfo.recv_cc_addr));
//	fclose(fp);

	ret = sendSmtp(&mInfo);

	return 0;
}

int initialize(char *sIniPath)
{
	int ret;
	char buff[ 100 ];

//	memset((char *)&mInfo, 0x00, sizeof(mInfo));

//	memset(buff, 0x00, sizeof(buff));
	/*
	ReadString( "SMTP", "SMTP_HOST", "", buff, sizeof(buff) - 1, sIniPath);
	PAD(buff);
	memcpy(mInfo.smtp_host, buff, strlen(buff));

	memset(buff, 0x00, sizeof(buff));
	ReadString( "SMTP", "SEND_MAIL_ADDR", "", buff, sizeof(buff) - 1, sIniPath);
	PAD(buff);
	memcpy(mInfo.send_mail_addr, buff, strlen(buff));

	memset(buff, 0x00, sizeof(buff));
	ReadString( "SMTP", "SEND_NAME", "", buff, sizeof(buff) - 1, sIniPath);
	PAD(buff);
	memcpy(mInfo.send_name, buff, strlen(buff));

	memset(buff, 0x00, sizeof(buff));
	ReadString( "SMTP", "RECV_TO_ADDR", "", buff, sizeof(buff) - 1, sIniPath);
	PAD(buff);
	memcpy(mInfo.recv_to_addr, buff, strlen(buff));

	memset(buff, 0x00, sizeof(buff));
	ReadString( "SMTP", "RECV_CC_ADDR", "", buff, sizeof(buff) - 1, sIniPath);
	PAD(buff);
	memcpy(mInfo.recv_cc_addr, buff, strlen(buff));
	*/
//	memcpy(mInfo.smtp_host, "133.9.100.5", sizeof(mInfo.smtp_host));
//	memcpy(mInfo.send_mail_addr, "nastech_op@nastech.co.kr", sizeof(mInfo.send_mail_addr));
//	memcpy(mInfo.send_name, "나스텍운영팀", sizeof(mInfo.send_name));
//	memcpy(mInfo.recv_to_addr, "syhwang@nastech.co.kr, hwangsuyun@naver.com, hwangdkcla@hanmail.net", sizeof(mInfo.recv_to_addr));
//	memcpy(mInfo.recv_cc_addr, "hwangsuyun@nate.com", sizeof(mInfo.recv_cc_addr));

	return 0;
}
