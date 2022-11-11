#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

static char *from_addr  = "syhwang@nastech.co.kr";                /* 보내는 사람 주소        */
static char *reply_addr = "syhwang@nastech.co.kr";                /* 받는 사람 주소        */
static char *mailhost   = "133.9.100.5";                /* 메일서버 주소        */
static int   mailport   = 25;                /* 메일서버 포트        */
static char *subject    = "seven";                /* 메일 제목                 */
static FILE *sfp;
static FILE *rfp;

/*
 ** 메일서버에서 응답받기
 */
void Get_Response(void)
{
	char buf[BUFSIZ];

	while (fgets(buf, sizeof(buf), rfp)) {
		buf[strlen(buf)-1] = 0;
		if (!isdigit(buf[0]) || buf[0] > '3') {
			printf("unexpected reply: %s\n", buf);
			exit(-1);
		}
		if (buf[4] != '-')
			break;
	}
	return;
}

/*
 ** 메일서버로 내용 전송하고 응답받기
 */
void Chat(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(sfp, fmt, ap);
	va_end(ap);

	fflush(sfp);
	Get_Response();
}

int main(int argc, char **argv)
{
	FILE *fpFile;
	char buf[BUFSIZ];
	char my_name[BUFSIZ];
	struct sockaddr_in sin;
	struct hostent *hp;
	struct servent *sp;
	int c;
	int s;
	int r;
	char *cp;
	/*
	 *  메일 서버에 연결
	 */

	if ((hp = gethostbyname(mailhost)) == NULL) {
		printf("%s: unknown host\n", mailhost);
		exit(-1);
	}
	if (hp->h_addrtype != AF_INET) {
		printf("unknown address family: %d\n", hp->h_addrtype);
		exit(-1);
	}

	memset((char *)&sin, 0, sizeof(sin));
	memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);

	sin.sin_family = hp->h_addrtype;
	sin.sin_port = htons(mailport);

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket: error\n");
		exit(-1);
	}
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		printf("connect: error\n");
		exit(-1);
	}
	if ((r = dup(s)) < 0) {
		printf("dup: error\n");
		exit(-1);
	}
	if ((sfp = fdopen(s, "w")) == 0) {
		printf("fdopen: error\n");
		exit(-1);
	}
	if ((rfp = fdopen(r, "r")) == 0) {
		printf("fdopen: error\n");
		exit(-1);
	}

	/* 
	 *  SMTP 헤더보내기
	 */
	Get_Response(); /* 초기 메시지 받기 */
	/*
	   Chat("HELO %s\r\n", my_name);
	 */
	Chat("MAIL FROM: <%s>\r\n", from_addr);
	Chat("RCPT TO: <%s>\r\n", reply_addr);
	Chat("DATA\r\n");

	/* 
	 *  메일 제목
	 */
	if( subject != NULL)
	{
		fprintf(sfp, "Subject: %s\r\n", subject);
		fprintf(sfp, "\r\n");
	}

	/* 
	 *  메일 내용 읽기
	 */
	if ((fpFile = fopen(argv[argc - 1], "r")) == NULL )
	{
		printf("no message file \n");
		exit(-1);
	}
	else
	{
		while(fgets(buf, sizeof(buf), fpFile))
		{
			buf[strlen(buf)-1] = 0;
			if (strcmp(buf, ".") == 0)
			{
				fprintf(sfp, ".\r\n");
			}
			else
			{
				fprintf(sfp, "%s\r\n", buf);
			}
		}
		fclose(fpFile);
	}

	/* 
	 *  내용 종료 및 통신 종료
	 */
	Chat(".\r\n");
	Chat("QUIT\r\n");

	printf("Your mail is sended successfully !!\n");
	exit(0);
}

/*EOF*/
