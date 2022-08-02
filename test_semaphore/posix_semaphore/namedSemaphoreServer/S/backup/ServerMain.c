#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "ipc.h"
#include "parse_data.h"

#define STX     0x02
#define ETX     0x03
#define FS      0x1c

int serverMain(int sockFd)
{
	char *__fn = "serverMain";
	char rBuff[1024];
	char sBuff[1024];
	int len, r, isShared;
	PROTOCOL data;
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
	// ����ó��
	len = readSocket(sockFd, rBuff, sizeof(rBuff), 20);
	if (len == 0) {										// �о���� ���ߴٸ�
		dbg(9, "ERR: Client Closed .. <%s>", __fn);
		return -1;
	}
	else if (len == -1) {								// Ÿ�� �ƿ� �Ǵ� select ����
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
	parseRecvData(rBuff, &data);						// Parsing client data
	dbg(1, "Called Func() parseRecvData");

	if (strcmp(data.flag, "1") == 0) {					// �� ��û��
		dbg(1, "Before sem create");
		// �������� ������ ���� üũ
		if ((sem = sem_open(data.pgm, O_CREAT, 0777, 1)) == NULL) {
			dbg(9, "Semaphore Open Error(NULL)");
			return 1;
		}
		dbg(1, "After sem create");

		int sval;
		if (sem) sem_getvalue(sem, &sval);				// ���������� ���� ���´�.
		dbg(1, "%d : sval = %d", __LINE__, sval);

		if (sval <= 0 ) {								// �������� ���� ��� �� ���ٸ�, �������� ������� �޽��� �Ǿ ������. 
			memcpy(sBuff, "[ERROR] There is no Semaphore to lock.", sizeof(sBuff));
		} else {
			sem_wait(sem);
			memcpy(sBuff, "[OK] succeed Locking", sizeof(sBuff));
		}
	}
	else if (strcmp(data.flag, "0") == 0) {
		dbg(1, "Before sem access");
		// Ŭ���̾�Ʈ�� ������ �������� ���ٽ�
		if ((sem = sem_open(data.pgm, 0, 0777, 0)) == SEM_FAILED) {
			dbg(9, "Semaphore Open Error(SEM_FAILED)");
			return 1;
		}
		dbg(1, "After sem access");

		int sval;
		dbg(1, "%d : sval = %d", __LINE__, sval);
		if (sem) sem_getvalue(sem, &sval);				 // ���������� ���� ���´�.

		if (sval >= 1 ) {
			memcpy(sBuff, "[ERROR] There is no Semaphore to unlock.", sizeof(sBuff));
		} else {
			sem_post(sem);
			memcpy(sBuff, "[OK] succeed UnLocking", sizeof(sBuff));
		}
	}
	else {		// Ŭ���̾�Ʈ�� "0", "1"�� �ƴ� �޽����� �����Ͽ��� ��� 
		memcpy(sBuff, "[ERROR] It's not correct message", sizeof(sBuff));
	}

	r = write(sockFd, sBuff, sizeof(sBuff));
	dbg(6, "Send to Client [%d][%d][%s] <%s>", sockFd, r, sBuff, __fn);

	return 0;	
}

int reqLockSem(char *inPgm, sem_t *inSem, char **inBuff )
{
	dbg(1, "Before opening semaphore");
	// �������� ������ ���� üũ
	if ((inSem = sem_open(inPgm, O_CREAT, 0777, 1)) == NULL) {
		dbg(9, "Semaphore Open Error(NULL)");
		return 1;
	}
	dbg(1, "After opening semaphore");

	int semVal;
	if (inSem) sem_getvalue(inSem, &semVal);			// ���������� ���� ���´�.
	dbg(1, "%d : semVal = %d", __LINE__, semVal);

	if (semVal <= 0 ) {									// �������� ���� ��� �� ���ٸ�, �������� ������� �޽��� �Ǿ ������. 
		memcpy(inBuff, "[ERROR] There is no Semaphore to lock.", sizeof(inBuff));
	} else {
		sem_wait(inSem);
		memcpy(inBuff, "[OK] succeed Locking", sizeof(inBuff));
	}

}
