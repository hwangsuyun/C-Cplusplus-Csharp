#include <stdio.h>
#include <string.h>

#include "ipc.h"
#include "parse_data.h"

#define STX     0x02
#define ETX     0x03
#define FS      0x1c

extern long sem_key[10];
extern long shm_key[10];
extern unsigned int semid[10];
extern unsigned int shmid[10];

int server_main(int sockFd)
{
	char *__fn = "server_main";
	char rBuff[1024];
	char sBuff[1024];
	int len, r, isShared;
	PROTOCOL data;
	int temp_semid, temp_shmid, index;

	memset(rBuff, 0x00, sizeof(rBuff));
	memset(sBuff, 0x00, sizeof(sBuff));

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
	r = checkSemShmKey(&data, &temp_semid, &temp_shmid, &index);
	if (r <= 0) 
	{
		memcpy(sBuff, "[ERROR] not found key", sizeof(sBuff));
	}
	else
	{
		// 3. check Lock or UnLock
		if (strcmp(data.flag, "1") == 0)
		{
			// check Lock or UnLock 
			isShared = getShmSegData(temp_semid, temp_shmid, index);
			dbg(9, "isShared(%d) <%s>", isShared, __fn);

			if (isShared == 0)
			{
				// Lock 시도
				setShmSegData(temp_semid, temp_shmid, index, '1');
				memcpy(sBuff, "[OK] succeed Locking", sizeof(sBuff));

				// fileWrite
				isShared = 1;
				r = writeInitFile(index, isShared);
			}
			else
			{
				memcpy(sBuff, "[ERROR] fail Locking", sizeof(sBuff));
			}

		}
		else if (strcmp(data.flag, "0") == 0)
		{
			// UnLock 시도
			setShmSegData(temp_semid, temp_shmid, index, '0');
			memcpy(sBuff, "[OK] succeed UnLocking", sizeof(sBuff));

			// fileWrite
			isShared = 0;
			r = writeInitFile(index, isShared);
		}
		else 
		{
			memcpy(sBuff, "[ERROR] not correct message", sizeof(sBuff));
		}
	}

	r = write(sockFd, sBuff, sizeof(sBuff));
	dbg(6, "Send to Client [%d][%d][%s] <%s>", sockFd, r, sBuff, __fn);

	return 0;	
}

int checkSemShmKey(PROTOCOL *data, int *temp_semid, int *temp_shmid, int *index)
{
	int i, j, rtn = 0;
	int sem_num = 0;
	key_t ipc_key;
	int numSemKey;
	int numShmKey;
			
	dbg(5,"OK:data->semKey(%s), data->shmKey(%s)", data->semKey, data->shmKey);

	numSemKey = changeCharToNumber(data->semKey);
	numShmKey = changeCharToNumber(data->shmKey);
	if (numSemKey == 0 || numShmKey == 0) return -1;

	dbg(5,"OK:numSemKey(%d), numShmKey(%d)", numSemKey, numShmKey);

	// get semid, shmid
	for(i = 0; i < sizeof(sem_key) / sizeof(long); i++)
	{
		if (sem_key[i] == numSemKey)
		{
			// key가 있으면
			ipc_key = ftok(".", numSemKey);
			if (ipc_key < 0) {
				dbg(9,"ERR:init_ipc key Error(%#010x)", ipc_key);
				return -1;
			}
			
			// IPC_CREAT -> 키가 없으면 새로 생성, 존재하면 존재하는 ID 반환
			*temp_semid = semget(ipc_key, sem_num, IPC_CREAT);
			if (*temp_semid == -1) {
				dbg(9,"ERR:init_ipc semget Error");
				return -1;
			}

			for (j = 0; j < sizeof(shm_key) / sizeof(long); j++)
			{
				if (shm_key[j] == numShmKey)
				{
					ipc_key = ftok(".", numShmKey);
					if (ipc_key < 0) {
						dbg(9,"ERR:get_shm_id key Error(%#010x)", ipc_key);
						return -1;
					}
					dbg(5, "OK:get_shm_id ipc_shm_key = %#010x", ipc_key);
					*temp_shmid = shmget(ipc_key, sizeof(char), IPC_CREAT);
					if (*temp_shmid == -1) {
						dbg(9,"ERR:get_shm_id temp_shmid(%d) Error", *temp_shmid);
						return -1;
					}
					rtn = 1;
					break;
				}
			} // end of loop
		}	
	} // end of loop

	if (rtn == 1)
	{
		for (i = 0; i < sizeof(shmid) / sizeof(int); i++)
		{
			if (*temp_shmid == shmid[i])
			{
				*index = i;
				dbg(5,"OK:index(%d)", *index);
			}	
		}
	}
	
	return rtn;
}
