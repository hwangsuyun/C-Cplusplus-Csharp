#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

static char *from_addr  = "syhwang@nastech.co.kr";                /* ������ ��� �ּ�        */
static char *reply_addr = "syhwang@nastech.co.kr";                /* �޴� ��� �ּ�        */
static char *mailhost   = "133.9.100.5";                /* ���ϼ��� �ּ�        */
static int   mailport   = 25;                /* ���ϼ��� ��Ʈ        */
static char *subject    = "seven";                /* ���� ����                 */
static FILE *sfp;
static FILE *rfp;

/*
 ** ���ϼ������� ����ޱ�
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
 ** ���ϼ����� ���� �����ϰ� ����ޱ�
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
	 *  ���� ������ ����
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
	 *  SMTP ���������
	 */
	Get_Response(); /* �ʱ� �޽��� �ޱ� */
	/*
	   Chat("HELO %s\r\n", my_name);
	 */
	Chat("MAIL FROM: <%s>\r\n", from_addr);
	Chat("RCPT TO: <%s>\r\n", reply_addr);
	Chat("DATA\r\n");

	/* 
	 *  ���� ����
	 */
	if( subject != NULL)
	{
		fprintf(sfp, "Subject: %s\r\n", subject);
		fprintf(sfp, "\r\n");
	}

	/* 
	 *  ���� ���� �б�
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
	 *  ���� ���� �� ��� ����
	 */
	Chat(".\r\n");
	Chat("QUIT\r\n");

	printf("Your mail is sended successfully !!\n");
	exit(0);
}

/*EOF*/
