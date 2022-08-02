#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "ipc.h"
#include "parse_data.h"

#define STX     0x02
#define ETX     0x03
#define FS      0x1c

void reqLockSem(RecvMsg *recvMsg, sem_t *inSem, char *inBuff);
void reqUnlockSem(RecvMsg recvMsg, sem_t *inSem, char *inBuff);
int createKeyFile(RecvMsg *recvMsg, char *out);
void makeJsonData4Res(char *resMsg, char *resCd, char *key, char *msg);
void makeKey4Res(char *pgm, char *siteCd, char *ip, char *out);

int serverMain(int sockFd)
{
	char *__fn = "serverMain";
	char rBuff[1024];
	char sBuff[1024];
	int len, r, isShared;
	RecvMsg *data;
	int temp_semid, temp_shmid, index;

	sem_t *sem;
	int isSemUsed;

	///////////////////////////////////////////////////////////////////////////////
	// �ʱ�ȭ
	memset(rBuff, 0x00, sizeof(rBuff));
	memset(sBuff, 0x00, sizeof(sBuff));
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////
	// ����ó��(�б�, Ÿ�Ӿƿ�, select)
	len = readSocket(sockFd, rBuff, sizeof(rBuff), 20);
	if (len == 0) {														// �о���� ���ߴٸ�
		dbg(9, "ERR: Client Closed .. <%s>", __fn);
		return -1;
	}
	else if (len == -1) {												// Ÿ�� �ƿ� �Ǵ� select ����
		dbg(9, "ERR: Client Timeout <%s>", __fn);
		return -1;
	}
	else if (len == -2) {
		dbg(9, "ERR: Cliser Recv Err recv_buff(%s) <%s>", rBuff, __fn);
		return -1;
	}
	dbg(6, "Recv from Client [%d][%d][%s] <%s>", sockFd, len, rBuff, __fn);
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////
	data = (RecvMsg *)malloc(sizeof(RecvMsg));
	memset(data, 0x00, sizeof(RecvMsg));

	dbg(1, "Before Calling Func() parseRecvData");
	parseRecvData(rBuff, data);											// Parsing client data
	dbg(1, "After  Calling Func() parseRecvData");

	if (strcmp(data->flag, "LOCK") == 0) {								// �� ��û��
		reqLockSem(data, sem, sBuff );									// �� ��û �Լ� ȣ��
	}
	else if (strcmp(data->flag, "UNLOCK") == 0) {						// ��� ��û��
		// Ű ���� üũ�� Ű�� ���� ���� �ʴٸ� 
		// ERROR�޽��� ����
		// Ű ���� üũ�� Ű�� ���� �´ٸ�
		// ��� ��û �Լ� ȣ��
		//reqUnlockSem(data, sem, sBuff );								// ��� ��û �Լ� ȣ��
	}
	else {																// Ŭ���̾�Ʈ�� "0", "1"�� �ƴ� �޽����� �����Ͽ��� ��� 
		memcpy(sBuff, "[ERROR] It's not correct message", sizeof(sBuff));
	}

	r = write(sockFd, sBuff, sizeof(sBuff));
	dbg(6, "Send to Client [%d][%d][%s] <%s>", sockFd, r, sBuff, __fn);

	return 0;	
}

void reqLockSem(RecvMsg *recvMsg, sem_t *inSem, char *inBuff)
{
	char key[128];
	char semFileName[128];

	memset(key, 0x00, sizeof(key));
	memset(semFileName, 0x00, sizeof(semFileName));

	sprintf(semFileName, "%s.%s", recvMsg->pgm, recvMsg->siteCd);

	dbg(1, "LineNum(%d) : Start function reqLockSem()", __LINE__);
	if ((inSem = sem_open(semFileName, O_CREAT, 0777, 1)) == NULL) {			// �������� ���Ͽ��½� ���� üũ
		dbg(9, "Semaphore Open Error(NULL)");
		makeJsonData4Res("Semaphore Open Error(NULL)", "ERROR", "", inBuff);	// Ŭ���̾�Ʈ�� ���� JSON ������ ����
																				// �����ÿ��� Ű�� ���� 
	} else {																	// �������� ������ �� �Ǿ��ٸ�
		dbg(1, "After opening Semaphore");
	
		int semVal;
		if (inSem) sem_getvalue(inSem, &semVal);						// �������� ���� ���´�.
		dbg(1, "%d : semVal = %d", __LINE__, semVal);
	
		if (semVal <= 0 ) {												// ������� ���� �� ���ٸ�, �������� ������� �����޽��� �Ǿ ������. 
			makeJsonData4Res("There isn't any Semaphore to lock", "ERROR", "", inBuff);	// Ŭ���̾�Ʈ�� ���� JSON ������ ����
		} else {														// �������� ��� �����Ͽ�, ��� �õ�
			sem_wait(inSem);
			
			char keyFileName[128];										
			int retVal = 0;
			retVal = createKeyFile(recvMsg, keyFileName);				// Ű ���ϻ���

			if (retVal < 0) makeJsonData4Res("It's failed to create a key file.", "ERROR", "", inBuff);	// Ŭ���̾�Ʈ�� ���� JSON ������ ����
			else 			makeJsonData4Res("Succeed Locking", "OK", keyFileName, inBuff);	// Ŭ���̾�Ʈ�� ���� JSON ������ ����
		}
	}
}

void reqUnlockSem(RecvMsg recvMsg, sem_t *inSem, char *inBuff)
{
	char key[128];
	char semFileName[128];

	memset(key, 0x00, sizeof(key));
	memset(semFileName, 0x00, sizeof(semFileName));

	strcat(semFileName, recvMsg.pgm);
	strcat(semFileName, ".");
	strcat(semFileName, recvMsg.siteCd);

	//if ((inSem = sem_open(inPgm, 0, 0777, 0)) == SEM_FAILED) {		// �������� ���Ͽ��½� ���� üũ
	if ((inSem = sem_open(semFileName, 0, 0777, 0)) == SEM_FAILED) {	// �������� ���Ͽ��½� ���� üũ
		dbg(9, "Semaphore Open Error(SEM_FAILED)");
		strcpy(inBuff, "[ERROR] Semephore Open Error(SEM_FAILED)");
	} else {
		dbg(1, "After opening semaphore");								// �������� ������ �� �Ǿ��ٸ�

		int semVal;
		dbg(1, "%d : semVal = %d", __LINE__, semVal);
		if (inSem) sem_getvalue(inSem, &semVal); 						// ���������� ���� ���´�.

		if (semVal >= 1 ) {												// ������� ������ �� ���ٸ�, �������� ������� �����޽��� �Ǿ ������. 
			strcpy(inBuff, "[ERROR] There is no Semaphore to unlock.");
		} else {
			// Ű���� ���� ���� �߰� �ʿ�
			sem_post(inSem);
			strcpy(inBuff, "[OK] succeed UnLocking");
		}
	}
}

int createKeyFile(RecvMsg *recvMsg, char *out)
{
	char keyName[100];
	char fileName[150];

	memset(keyName, 0x00, sizeof(keyName));
	memset(fileName, 0x00, sizeof(fileName));

	makeKey4Res(recvMsg, keyName);
	dbg(1, "Source(%s), Line(%d) : keyName = %s", __FILE__, __LINE__, keyName);

	strcpy(fileName, "./keyfile/");
	strcat(fileName, keyName);
	dbg(1, "Source(%s), Line(%d) : fileName = %s", __FILE__, __LINE__, fileName);

	struct stat st = {0};

	if (stat("./keyfile/", &st) == -1) {                        // ���丮 ���� ���� üũ
		mkdir("./keyfile/", 0777);                              // ���丮 ������ ���Ӱ� ����
		dbg(1, "Source(%s), Line(%d) : Directory \"keyfile\" is created.", __FILE__, __LINE__);
	} 

	int fd = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH );	// Ű�� �����̸����� �ϴ� Ű���� ����

	if (fd != -1)	 strcpy(out, keyName);
	else			 strcpy(out, "");

	return fd;
}

// ���䰪 Json ���ڿ� ����
void makeJsonData4Res(char *msg, char *msgCd, char *key, char *out)
{
	sprintf(out, "{\"msg\":\"%s\",\"msgCd\":\"%s\",\"key\":\"%s\"}"
			, msg, msgCd, key, out);
}

// Ű�� ����
void makeKey4Res(char *pgm, char *siteCd, char *ip, char *out)
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char date[10];
	char time[10];
	sprintf(date, "%04d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	sprintf(time, "%02d%02d%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);

	sprintf(out, "%s_%s_%s_%s_%s", pgm, siteCd, ip, date, time);
}
