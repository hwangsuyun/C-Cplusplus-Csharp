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
	// 초기화
	memset(rBuff, 0x00, sizeof(rBuff));
	memset(sBuff, 0x00, sizeof(sBuff));
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////
	// 예외처리
	len = readSocket(sockFd, rBuff, sizeof(rBuff), 20);
	if (len == 0) {										// 읽어오지 못했다면
		dbg(9, "ERR: Client Closed .. <%s>", __fn);
		return -1;
	}
	else if (len == -1) {								// 타임 아웃 또는 select 에러
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

	if (strcmp(data.flag, "1") == 0) {					// 락 요청시
		dbg(1, "Before sem create");
		// 세마포어 생성시 에러 체크
		if ((sem = sem_open(data.pgm, O_CREAT, 0777, 1)) == NULL) {
			dbg(9, "Semaphore Open Error(NULL)");
			return 1;
		}
		dbg(1, "After sem create");

		int sval;
		if (sem) sem_getvalue(sem, &sval);				// 세마포어의 값을 얻어온다.
		dbg(1, "%d : sval = %d", __LINE__, sval);

		if (sval <= 0 ) {								// 세마포어 값을 얻올 수 없다면, 세마포어 연산없이 메시지 실어서 보낸다. 
			memcpy(sBuff, "[ERROR] There is no Semaphore to lock.", sizeof(sBuff));
		} else {
			sem_wait(sem);
			memcpy(sBuff, "[OK] succeed Locking", sizeof(sBuff));
		}
	}
	else if (strcmp(data.flag, "0") == 0) {
		dbg(1, "Before sem access");
		// 클라이언트가 접속후 세마포어 접근시
		if ((sem = sem_open(data.pgm, 0, 0777, 0)) == SEM_FAILED) {
			dbg(9, "Semaphore Open Error(SEM_FAILED)");
			return 1;
		}
		dbg(1, "After sem access");

		int sval;
		dbg(1, "%d : sval = %d", __LINE__, sval);
		if (sem) sem_getvalue(sem, &sval);				 // 세마포어의 값을 얻어온다.

		if (sval >= 1 ) {
			memcpy(sBuff, "[ERROR] There is no Semaphore to unlock.", sizeof(sBuff));
		} else {
			sem_post(sem);
			memcpy(sBuff, "[OK] succeed UnLocking", sizeof(sBuff));
		}
	}
	else {		// 클라이언트가 "0", "1"이 아닌 메시지를 전송하였을 경우 
		memcpy(sBuff, "[ERROR] It's not correct message", sizeof(sBuff));
	}

	r = write(sockFd, sBuff, sizeof(sBuff));
	dbg(6, "Send to Client [%d][%d][%s] <%s>", sockFd, r, sBuff, __fn);

	return 0;	
}

int reqLockSem(char *inPgm, sem_t *inSem, char **inBuff )
{
	dbg(1, "Before opening semaphore");
	// 세마포어 생성시 에러 체크
	if ((inSem = sem_open(inPgm, O_CREAT, 0777, 1)) == NULL) {
		dbg(9, "Semaphore Open Error(NULL)");
		return 1;
	}
	dbg(1, "After opening semaphore");

	int semVal;
	if (inSem) sem_getvalue(inSem, &semVal);			// 세마포어의 값을 얻어온다.
	dbg(1, "%d : semVal = %d", __LINE__, semVal);

	if (semVal <= 0 ) {									// 세마포어 값을 얻올 수 없다면, 세마포어 연산없이 메시지 실어서 보낸다. 
		memcpy(inBuff, "[ERROR] There is no Semaphore to lock.", sizeof(inBuff));
	} else {
		sem_wait(inSem);
		memcpy(inBuff, "[OK] succeed Locking", sizeof(inBuff));
	}

}
