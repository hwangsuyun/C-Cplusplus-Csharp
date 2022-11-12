#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// SMTP 단계
// 1. telnet localhost 25 (25 : smtp port)
// 2. MAIL FROM: "xxx@naver.com" (보내는 사람)
// 3. RCPT TO: "yyy@naver.com" (받는 사람)
// 4. DATA (본문 내용)
// 5. To: "yyy@naver.com" (받는 사람 화면에 표시)
// 6. Cc: "zzz@naver.com" (참조 화면에 표시)
// 7. Subject: TEST MAIL SEND (제목 화면에 표시)
// 8. . (본문 내용 입력 완료)
// 9. QUIT 명령어 (통신 종료)

FILE *sfp;
FILE *rfp;

typedef struct {
	char smtp_host[50];
	char send_mail_addr[50];
	char send_name[50];
	char recv_to_addr[50][50];
	char recv_cc_addr[50][50];
	char tot_recv_to_addr[500];
	char tot_recv_cc_addr[500];
	char outData[1024];
} MAILINFO;

int get_response();
int send_recv(char *fmt, ...);


int main()
{
	char smwbinPath[30];
	char sIniPath[100];
	MAILINFO mInfo;
	int ret = 0;

	memset((char *) &mInfo, 0x00, sizeof(mInfo));

	memset(sIniPath, 0x00, sizeof(sIniPath));
	sprintf(sIniPath, "./%s", "MailSender.properties");
	ret = initialize(&mInfo, sIniPath);
	if (ret < 0)
	{
		printf("error initialize\n");
		return -1;
	}

	// 3. sendSmtp
	ret = send_smtp(&mInfo);
	if (ret < 0)
	{
		printf("error send_smtp\n");		        
		return -1;
	}

	return 0;
}

int initialize(MAILINFO *mInfo, char *sIniPath)
{
	int i;

	// SMTP IP
	memcpy(mInfo->smtp_host, "133.9.100.5", sizeof(mInfo->smtp_host));


	// FROM ADDR
	memcpy(mInfo->send_mail_addr, "nastech_op@nastech.co.kr", sizeof(mInfo->send_mail_addr));

	// FROM NAME
	memcpy(mInfo->send_name, "나스텍운영팀", sizeof(mInfo->send_name));

	// TO ADDR
	memcpy(mInfo->recv_to_addr[0], "syhwang@nastech.co.kr", 50);
//	memcpy(mInfo->recv_to_addr[1], "hspark@nastech.co.kr", 50);
//	memcpy(mInfo->recv_to_addr[2], "hwangsuyun@naver.com", 50);
	// CC ADDR
//	memcpy(mInfo->recv_cc_addr[0], "syhwang@nastech.co.kr", 50);
//	memcpy(mInfo->recv_cc_addr[1], "hspark@nastech.co.kr", 50);
	memcpy(mInfo->recv_cc_addr[0], "hwangsuyun@naver.com", 50);

	memcpy(mInfo->tot_recv_to_addr, "syhwang@nastech.co.kr, hspark@nastech.co.kr, hwangsuyun@naver.com", sizeof(mInfo->tot_recv_to_addr));
	memcpy(mInfo->tot_recv_cc_addr, "syhwang@nastech.co.kr, hspark@nastech.co.kr, hwangsuyun@naver.com", sizeof(mInfo->tot_recv_cc_addr));

	printf("mInfo->smtp_host(%s)\n", mInfo->smtp_host );
	printf("mInfo->send_mail_addr(%s)\n", mInfo->send_mail_addr );
	printf("mInfo->send_name(%s)\n", mInfo->send_name );
	printf("mInfo->recv_to_addr(%s)\n", mInfo->recv_to_addr[0]);
	printf("mInfo->tot_recv_to_addr(%s)\n", mInfo->tot_recv_to_addr);
	printf("mInfo->tot_recv_cc_addr(%s)\n", mInfo->tot_recv_cc_addr);

	return 0;
}

int send_smtp(MAILINFO *mInfo)
{
	struct sockaddr_in sin;
	struct hostent *hp;
	struct servent *sp;
	int send;
	int recv;
	int i;

	//  메일 서버에 연결
	if ((hp = gethostbyname(mInfo->smtp_host)) == NULL) {
		printf("Unknown Host (%s)\n", mInfo->smtp_host );
		return -1;
	}
	if (hp->h_addrtype != AF_INET) {
		printf("Unknown Address Family (%s)\n", hp->h_addrtype );
		return -1;
	}

	memset((char *)&sin, 0, sizeof(sin));
	memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);

	sin.sin_family = hp->h_addrtype;
	sin.sin_port = htons(25);

	if ((send = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket Error\n" );
		return -1;
	}
	if (connect(send, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		printf("connect Error\n" );
		return -1;
	}
	if ((recv = dup(send)) < 0) {
		printf("dup Error\n" );
		return -1;
	}
	if ((sfp = fdopen(send, "w")) == 0) {
		printf("fdopen Error\n" );
		return -1;
	}
	if ((rfp = fdopen(recv, "r")) == 0) {
		printf("fdopen Error\n" );
		return -1;
	}

	// 접속 메시지 받기
	get_response();

	// SMTP 헤더보내기
	send_recv("MAIL FROM: <%s>\r\n", mInfo->send_mail_addr);
	for (i = 0; i < sizeof(mInfo->recv_to_addr) / sizeof(mInfo->recv_to_addr[0]); i++)
	{
		if (strncmp(mInfo->recv_to_addr[i], "", 1) != 0)
		{
			// 받는 사람 확인
			send_recv("RCPT TO: <%s>\r\n", mInfo->recv_to_addr[i]);
		}
		else 
		{
			break;
		}
	}

	// 참조 확인
	for (i = 0; i < sizeof(mInfo->recv_cc_addr) / sizeof(mInfo->recv_cc_addr[0]); i++)
	{
		if (strncmp(mInfo->recv_cc_addr[i], "", 1) != 0)
		{
			// 받는 사람 확인
			send_recv("RCPT TO: <%s>\r\n", mInfo->recv_cc_addr[i]);
		}
		else
		{
			break;
		}
	}

	// 본문 보내기
	send_recv("DATA\r\n");

	// 받는 사람 화면에 표시
	fprintf(sfp, "To: %s\r\n", mInfo->tot_recv_to_addr);
	fprintf(sfp, "Cc: %s\r\n", mInfo->tot_recv_cc_addr);

	// 메일 제목 화면에 표시
	fprintf(sfp, "Subject: %s\r\n", "[SEVEN, BTW] 정산파일 전송 결과");
	fprintf(sfp, "\r\n");

	// 메일 내용 화면에 표시
	fprintf(sfp, "%s%s%s%s%s%s%s%s", 
			"안녕하세요. 나스텍 입니다.\r\n",
			"세븐, 바이더웨이 정산파일 전송 결과입니다.\r\n",
			"\r\n",
			"1. SEVEN 서버 정산파일 데이터 전송 리스트\r\n", 
			mInfo->outData, 
			"\r\n",
			"\r\n",
			"감사합니다.\r\n"
		   );

	//  내용 종료 및 통신 종료
	send_recv(".\r\n");
	send_recv("QUIT\r\n");

	fclose(sfp);
	fclose(rfp);
	printf("Your mail is sended successfully !!\n" );

	return 0;
}

// 메일서버에서 응답받기
int get_response()
{
	char buf[BUFSIZ];

	while (fgets(buf, sizeof(buf), rfp)) {
		buf[strlen(buf) - 1] = 0;
		if (!isdigit(buf[0]) || buf[0] > '3') {
			printf("Unexpected Reply (%s)\n", buf );
			return -1;
		}
		if (buf[4] != '-')
			break;
	}
	return 0;
}

// 메일서버로 내용 전송하고 응답받기
int send_recv(char *fmt, ...)
{
	int ret = 0;

	va_list ap;

	va_start(ap, fmt);
	vfprintf(sfp, fmt, ap);
	va_end(ap);

	fflush(sfp);
	ret = get_response();
	if (ret < 0) return -1;
}
