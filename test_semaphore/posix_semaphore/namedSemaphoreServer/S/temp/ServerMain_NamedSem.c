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
	char *__fn = "server_main";
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

	// 1. parsing client data
	parseRecvData(rBuff, &data);
	dbg(1, "parseRecvData");

	//sem_unlink(data.pgm);

	/*
	if ((sem = sem_open(data.pgm, O_CREAT, 0777, 1)) == NULL) {
		perror("Semaphore Open Error");
		dbg(9, "Semaphore Open Error");
		return 1;
	}
	*/

	//dbg(1, "After sem_unlink");

	// 3. check Lock or UnLock
	if (strcmp(data.flag, "1") == 0) {
		// check Lock or UnLock 
		//isShared = getShmSegData(temp_semid, temp_shmid, index);
		//isSemUsed = sem_open("data.

		dbg(1, "Before sem create");
		// 세마포어 생성시 에러 체크
		if ((sem = sem_open(data.pgm, O_CREAT, 0777, 1)) == NULL) {
			dbg(9, "Semaphore Open Error(NULL)");
			return 1;
		}
		dbg(1, "After sem create");

		dbg(1, "Before sem access");
		// 클라이언트가 접속후 세마포어 접근시
		if ((sem = sem_open(data.pgm, 0, 0777, 0)) == SEM_FAILED) {
			dbg(9, "Semaphore Open Error(SEM_FAILED)");
			return 1;
		}
		dbg(1, "After sem access");

		//sem_unlink(data.pgm);
		sem_wait(sem);
		memcpy(sBuff, "[OK] succeed Locking", sizeof(sBuff));

		/*
		if (isShared == 0) {
			// Lock 시도
			setShmSegData(temp_semid, temp_shmid, index, '1');
			memcpy(sBuff, "[OK] succeed Locking", sizeof(sBuff));

			// fileWrite
			isShared = 1;
			r = writeInitFile(index, isShared);
		}
		else {
			memcpy(sBuff, "[ERROR] fail Locking", sizeof(sBuff));
		}
		*/
	}
	else if (strcmp(data.flag, "0") == 0) {
		sem_post(sem);
		memcpy(sBuff, "[OK] succeed UnLocking", sizeof(sBuff));
		/*
		// UnLock 시도
		setShmSegData(temp_semid, temp_shmid, index, '0');
		memcpy(sBuff, "[OK] succeed UnLocking", sizeof(sBuff));

		// fileWrite
		isShared = 0;
		r = writeInitFile(index, isShared);
		*/
	}
	else {
		memcpy(sBuff, "[ERROR] not correct message", sizeof(sBuff));
	}

	r = write(sockFd, sBuff, sizeof(sBuff));
	dbg(6, "Send to Client [%d][%d][%s] <%s>", sockFd, r, sBuff, __fn);

	return 0;	
}

