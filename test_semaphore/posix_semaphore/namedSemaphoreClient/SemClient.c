#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include "cJSON.h"
#include "SemClient.h"

/**
 * �������� ��ݸ� ��û�ϴ� ��ȣ�� �����ϴ�.
 *
 * History:
 *		��â��, 1.0 2016.02.16 �����ۼ�
 *
 * @param		pgm		���μ�����
 * @param		siteCd	��ü��
 * @param		ip		������ ������ ����
 * @param		port	������ ��Ʈ ����
 * @return		������ 0�� ��ȯ, ���н� -1�� ��ȯ
 * @author		��â��
 */
int lockSem(char *pgm, char *siteCd, char *ip, int port)
{
	int sLen, retVal;
	char key[128];
	char sendMessage[128];

	memset( key, 0x00, sizeof(key) ); 
	memset( sendMessage, 0x00, sizeof(sendMessage) ); 

	makeJsonData4Req(pgm, siteCd, ip, "LOCK", "DUMMY", sendMessage);
	printf("sendMessage = (%s)\n", sendMessage);
	sLen = strlen(sendMessage);

	retVal = sendRecvMsg(ip, port, sendMessage, sLen, key);
	printf("key = (%s)\n", key);

	if (retVal >= 0)
		createKeyFile(key, pgm);			//	��ŷ�� �����ϸ� ������ �����Ѵ�.

	return retVal;
}

/**
 * �������� ������ ��û�ϴ� ��ȣ�� �����ϴ�.
 *
 * History:
 *		��â��, 1.0 2016.02.16 �����ۼ�
 *
 * @param		pgm		���μ�����
 * @param		siteCd	��ü��
 * @param		ip		������ ������ ����
 * @param		port	������ ��Ʈ ����
 * @return		������ 0�� ��ȯ, ���н� -1�� ��ȯ
 * @author		��â��
 */
int unlockSem(char *pgm, char *siteCd, char *ip, int port, char *key)
{
	int sLen;
	char sendMessage[128];
	memset( sendMessage, 0x00, sizeof(sendMessage) ); 
	makeJsonData4Req(pgm, siteCd, ip, "UNLOCK", "", sendMessage);
	sLen = strlen(sendMessage);

	//return sendRecvMsg(ip, port, sendMessage, sLen);
	return 0;
}


/**
 * json ���� String�� �����մϴ�.
 *
 * History:
 *		��â��, 1.0 2016.02.16 �����ۼ�
 *
 * @param		pgm		���μ�����
 * @param		siteCd	��ü��
 * @param		ip		������ ������ ����
 * @param		port	������ ��Ʈ ����
 * @param		msg		������� ����(output)
 * @author		��â��
 */
void makeJsonData4Req(char *pgm, char *siteCd, char *ip, char *flag, char *key, char *msg)
{
	sprintf(msg, "{\
			\"pgm\":\"%s\", \
			\"siteCd\":\"%s\", \
			\"ip\":\"%s\", \
			\"flag\":\"%s\", \
			\"key\":\"%s\" \
			}"
			, pgm, siteCd, ip, flag, key);
}

/**
 * �Էµ� ip, port ���ڿ� �ش��ϴ� ��Ʈ�� sendMessage�� �����ϰ�, ����޽����� �޽��ϴ�.
 *
 * History:
 *		��â��, 1.0 2016.02.16 �����ۼ�
 *
 * @param		ip				������ ������ ����
 * @param		port			������ ��Ʈ ����
 * @param		sendMessage		�����޽���
 * @param		sLen			�����޽����� ũ��
 * @author		��â��
 *
 */
int sendRecvMsg(char *ip, int port, char *sendMessage, int sLen, char *key)
{
	int i;
	int sockfd;
	int length;
	char receiveMessage[1024];

	memset(receiveMessage, 0x00, sizeof(receiveMessage));

	if( (sockfd = ConnectSemServer( ip, port ))==FALSE ) {
		printf(">>>>> connection error! [%s:%d]\n", ip, port ); 
		exit(1);
	}
	printf("connect complete...\n");

	if( ( write( sockfd, sendMessage, sLen ) ) <= 0 ) {
		printf(">>>>> write error! [%s:%d]\n", ip, port );
		close( sockfd );
		return -1;
	}
	printf("write complete...\n");
	printf("write message : [%d][", sLen);

	for (i = 0 ; i < sLen ; i++ )
		printf("%c", (unsigned char)sendMessage[i]);
	printf("]\n");

	length = read( sockfd, receiveMessage, 1024 );

	if( length < 0 ) {
		fprintf(stderr, "ERROR : Receive Error(%d)\n", errno); fflush(stderr);
		close( sockfd );
		exit(1);
	}       
	else if( length == 0 ) {
		fprintf(stderr, "ERROR : Disconnected while receiving(%d)\n", errno); fflush(stderr);
		close( sockfd );
		exit(1);
	}       
	printf("read complete...\n");
	printf("read message : (%s)\n", receiveMessage);

	RecvMsg *parsedData = (RecvMsg *) malloc(sizeof(RecvMsg));
	memset(parsedData, 0x00, sizeof(RecvMsg));
	parseRecvData(receiveMessage, parsedData);

	//printf("(%s)(%d) msg = %s\n", __FILE__, __LINE__, parsedData->msg );
	//printf("(%s)(%d) msgCd = %s\n", __FILE__, __LINE__, parsedData->msgCd );
	printf("(%s)(%d) key = %s\n", __FILE__, __LINE__, parsedData->key );

	if (strcmp(parsedData->msgCd, "ERROR") == 0)		// ��ŷ/���ŷ�� ������ �����
		return -1;
	
	strcpy(key, parsedData->key);
	printf("(%s)(%d) key = %s\n", __FILE__, __LINE__, key );


	close( sockfd );
	return 0;
}

int ConnectSemServer(char *IpAddr, int Port)
{
	int                 sockfd;
	struct linger       ling;
	struct sockaddr_in  addr;
	char                errbuf[1024];

	memset(errbuf, 0x00, sizeof(errbuf));

	if( (sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0 ) {
		fprintf(stderr, "ConnectSemSever >> Socket Open Error : %s\n", strerror(errno));
		return FALSE;
	}

	ling.l_onoff  = 1;
	ling.l_linger = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling));

	bzero((char *)&addr, sizeof(addr));
	addr.sin_family      = PF_INET;
	addr.sin_addr.s_addr = inet_addr(IpAddr);;
	addr.sin_port        = htons(Port);

	if( connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0 ) {
		fprintf(stderr, "ConnectSemServer >> connect Error : %s\n", strerror(errno));
		fprintf(stderr, "   >> IP = %s, Port = %d\n", IpAddr, Port);
		close(sockfd);
		return FALSE;
	}
	return sockfd;
}

void parseRecvData(char *in, RecvMsg *out)
{
	cJSON *wholeMsg = NULL;
	cJSON *msg		= NULL;
	cJSON *msgCd	= NULL;
	cJSON *key		= NULL;

	wholeMsg		= cJSON_Parse((char *)in);
	msg				= cJSON_GetObjectItem(wholeMsg, "msg");
	msgCd			= cJSON_GetObjectItem(wholeMsg, "msgCd");
	key				= cJSON_GetObjectItem(wholeMsg, "key");

	memcpy(out->msg,	cJSON_Print(msg) + 1,		strlen(cJSON_Print(msg)) - 2); 
	memcpy(out->msgCd,	cJSON_Print(msgCd) + 1,		strlen(cJSON_Print(msgCd)) - 2); 
	memcpy(out->key,    cJSON_Print(key) + 1,		strlen(cJSON_Print(key)) - 2); 
}

int createKeyFile(char *key, char *currProcName)
{
	char fileName[150];

	memset(fileName, 0x00, sizeof(fileName));

	sprintf(fileName, "./keyfile/%s_key", currProcName );
	printf("Source(%s), Line(%d) : fileName = %s\n", __FILE__, __LINE__, fileName);
	//dbg(1, "Source(%s), Line(%d) : fileName = %s", __FILE__, __LINE__, fileName);

	printf("Source(%s), Line(%d) : key = %s\n", __FILE__, __LINE__, key);

	struct stat st = {0};

	if (stat("./keyfile/", &st) == -1) {                        // ���丮 ���� ���� üũ
		mkdir("./keyfile/", 0777);                              // ���丮 ������ ���Ӱ� ����
		//dbg(1, "Source(%s), Line(%d) : Directory \"keyfile\" is created.", __FILE__, __LINE__);
	} 

	int fd = open(fileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR );    // ���μ��� �̸��� �����̸����� �ϴ� Ű���� ����
	//printf("Source(%s), Line(%d) : fd(%d)\n", __FILE__, __LINE__, fd);

	if (fd > 0 ) {
		printf("Source(%s), Line(%d) : keyfile is created.\n", __FILE__, __LINE__);
		write(fd, key, strlen(key));
	}
	//else 																// ���� ���� ���н� �����ʿ�?

	close(fd);
	
	/*
	fd = open(fileName, O_RDONLY );    // ���μ��� �̸��� �����̸����� �ϴ� Ű���� ����
	char temp[100];
	memset(temp, 0x00, sizeof(temp));
	read(fd, temp, sizeof(temp));
	printf("Source(%s), Line(%d) : read = (%s)\n", __FILE__, __LINE__, temp);
	close(fd);
	*/

	return fd;
}

