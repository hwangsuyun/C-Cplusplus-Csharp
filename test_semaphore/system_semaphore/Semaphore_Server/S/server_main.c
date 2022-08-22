#include <stdio.h>
#include <string.h>

#include "ipc.h"
#include "parse_data.h"

#define STX     0x02
#define ETX     0x03
#define FS      0x1c

int server_main(int sockFd)
{
	char *__fn = "server_main";
	char rBuff[1024];
	char sBuff[1024];
	int len, r;
	PROTOCOL data;

	memset(rBuff, 0x00, sizeof(rBuff));
	memset(sBuff, 0x00, sizeof(sBuff));
	memset((char *)&data, 0x00, sizeof(data));

	len = readSocket(sockFd, rBuff, sizeof(rBuff), 20);
	if (len == 0)
	{
		dbg(9, "ERR: Client Closed .. <%s>", __fn);
		return -1;
	}
	else if (len == -1)
	{
		dbg(9, "ERR: Client Timeout <%s>", __fn);
		return -1;
	}
	else if (len == -2)
	{
		dbg(9, "ERR: Cliser Recv Err recv_buff(%s) <%s>", rBuff, __fn);
		return -1;
	}
	dbg(6, "Recv from Client [%d][%d][%s] <%s>", sockFd, len, rBuff, __fn);

	// 1. parsing client data
	parseRecvData(rBuff, &data);

	// 2. check semid, shmid
	r = checkSemKey(&data, sBuff);

	r = write(sockFd, sBuff, sizeof(sBuff));
	dbg(6, "Send to Client [%d][%d][%s] <%s>", sockFd, r, sBuff, __fn);

	return 0;	
}

int checkSemKey(PROTOCOL *data, char *sBuff)
{
	int ret = 0;
	key_t ipc_key;
	int numSemKey, isSemaphored = 0;
	int semid;
			
	dbg(5,"OK:data->semKey(%s)", data->semKey);

	numSemKey = changeCharToNumber(data->semKey);
	if (numSemKey == 0) return -1;

	dbg(5,"OK:numSemKey(%d)", numSemKey);

	semid = init_ipc(numSemKey);
	if (semid < 0) 
	{
		sprintf(sBuff, "%s", "[ERROR] don't create key");
		return -1;
	}
	
	writeInitFile(semid, isSemaphored);	

	// 3. check Lock or UnLock
	if (strcmp(data->flag, "1") == 0)
	{
		// Lock 시도
		ret = setLockSemaphore(semid);
		if (ret < 0)
		{
			sprintf(sBuff, "%s", "[ERROR] fail Locking");
		}
		else 
		{
			sprintf(sBuff, "%s", "[OK] succeed Locking");
		}

		// fileWrite
		isSemaphored = 1;
		markInitFile(semid, isSemaphored);

	}                                                               
	else if (strcmp(data->flag, "0") == 0)
	{
		// UnLock 시도
		ret = setUnLockSemaphore(semid);
		if (ret < 0)
		{
			sprintf(sBuff, "%s", "[ERROR] fail UnLocking");
		}
		else
		{
			sprintf(sBuff, "%s", "[OK] succeed UnLocking");
		}

		// fileWrite
		isSemaphored = 0;
		markInitFile(semid, isSemaphored);
	}
	else 
	{
		sprintf(sBuff, "%s", "[ERROR] not correct message");
	}

	return 0;
}
