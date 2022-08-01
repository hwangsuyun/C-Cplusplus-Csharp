#include	<errno.h>
#include	<string.h>
#include	<stdio.h>

#include	"ipc.h"
#include	"semun.h"

#define ERR		((void *) -1)

struct sembuf semLock = {0, -1, SEM_UNDO};
struct sembuf semUnLock = {0, 1, SEM_UNDO};

#define   SEM_LOCK(id)   (!semop(id, &semLock, 1)) ? 1 : 0
#define   SEM_UNLOCK(id) (!semop(id, &semUnLock, 1)) ? 1 : 0
	
unsigned int semid[10];
unsigned int shmid[10];

static ShmChk *sharedMemory;

int sem_lock(unsigned int semid)
{
	if (semop(semid, &semLock, 1) == 0) 
		return 1;
	return 0;
}

int sem_unlock(unsigned int semid)
{
	if (semop(semid, &semUnLock, 1) == 0) 
		return 1;
	return 0;
}

int init_ipc(long *sem_key, long *shm_key)
{
	key_t   ipc_key;
	int	sem_num = 1;	/* semaphore count */
	void * shm_addr = (void *)0;
	union semun 	sem_union;
	int i;
	char shm_key_str[30];

	dbg(1, "START:init_ipc()");

	//for (i = 0; i < sizeof(ShmChk); i++)
	for (i = 0; i < sizeof(shmid) / sizeof(int); i++)
	{
		/* 세마포어 설정*/
		ipc_key = ftok(".", sem_key[i]);
		if (ipc_key < 0) {
			dbg(9,"ERR:init_ipc key Error(%#010x)", ipc_key);
			return -1;
		}

		semid[i] = semget(ipc_key, sem_num, IPC_CREAT | SHM_PERM);  
		if (semid[i] == -1) {
			dbg(9,"ERR:init_ipc semget Error");
			return -1;
		}

		dbg(5, "init_ipc SEMAPHORE ID (%d)(%d)", i, semid[i]);
		sem_union.val = 1;
		if (semctl(semid[i], 0, SETVAL, sem_union) == -1) {
			dbg(9,"ERR:init_ipc semctl Error");
			return -1;
		}

		dbg(5,"OK:Semaphoer Set Succ SEMID(%d)(%d)", i, semid[i]);
	
		/* 공유메모리 설정*/
		ipc_key = ftok(".", shm_key[i]);
		if (ipc_key < 0) {
			dbg(9,"ERR:get_shm_id key Error(%#010x)", ipc_key);
			return -1;
		}

		dbg(5, "OK:get_shm_id ipc_key = %#010x", ipc_key);
		shmid[i] = shmget(ipc_key, sizeof(char), IPC_CREAT | SHM_PERM);
		if (shmid[i] == -1) {
			dbg(9,"ERR:get_shm_id shmid(%d)(%d) Error", i, shmid[i]);
			return -1;
		}

		dbg(5,"OK:get_shm_id shmid(%d)(%d)", i, shmid[i]);

		sharedMemory = 0x00;
		shm_addr = shmat(shmid[i], (void *)0, 0);
		//sharedMemory->shmChk[i] = (char)shmat(shmid[i], (void *)0, 0);
		if (shm_addr == (void *)-1) {
			dbg(9,"ERR:shm_addr is nagative. shmid(%d)(%d)", i, shmid[i]);
			return -1;
		}
		sharedMemory = (ShmChk *)shm_addr;
		memset((char *)sharedMemory, 0x00, sizeof(ShmChk));

		// file write (ipc_key)
		memset(shm_key_str, 0x00, sizeof(shm_key_str));
		sprintf(shm_key_str, "%#010x", ipc_key);
		initializeInitFile(i, shm_key_str);
	}

	dbg(1,"END:init_ipc()");
	return 0;
}

// Desc : 세마포어를 이용하여 공유메모리에 접근하여 상호배제를 실현
//        공유메모리의 인덱스에 해당하는 주소의 내용을 수정하는 함수
// @semid : 세마포어 id
// @shmid : 공유메모리 id
// @index : index number
// @value : 세팅되는 값
void setShmSegData(int semid, int shmid, int index, char value)
{
	dbg(5, "%s-%d-%s-setShmSegData", __FILE__, __LINE__, __func__);
	ShmChk *tIndex;
	char temp[10];
	
	// 공유메모리 이용하여 변경하기
	if (!sem_lock(semid)) { dbg(9, "ERR:sem_lock Error"); }      // 세마포어 이용하여 락킹/언락킹 하기
	getShmSeg(&tIndex, shmid);                                   // 공유메모리의 정보를 가져옴
	tIndex->shmChk[index] = value;                      	     // 공유메모리의 인덱스에 해당하는 상태 정보를 value 값으로
	if (!sem_unlock(semid)) { dbg(9, "ERR:sem_unlock Error"); }  // 공유메모리 작업이 끝나면 언락    
}

// Desc : 세마포어를 이용하여 공유메모리에 접근하여 상호배제를 실현
//        공유메모리의 인덱스에 해당하는 주소의 내용을 얻어오는 함수
// @semid : 세마포어 id
// @shmid : 공유메모리 id
// @index : index number
int getShmSegData(int semid, int shmid, int index)
{
	dbg(5, "%s-%d-%s-getShmSegData", __FILE__, __LINE__, __func__);

	int result;
	ShmChk *tIndex;

	memset((char *)&tIndex, 0x00, sizeof(tIndex));

	// 공유메모리 이용하여 가져오기
	if (!sem_lock(semid)) { dbg(9, "ERR:sem_lock Error"); }      // 세마포어 이용하여 락킹/언락킹 하기
	getShmSeg(&tIndex, shmid);                                   // 공유메모리의 정보를 가져옴
	result = tIndex->shmChk[index];        			     // 공유메모리의 인덱스에 해당하는 상태 정보를 가져오기
	dbg(5, "tIndex->shmChk(%s)", tIndex->shmChk);
	dbg(5, "tIndex->shmChk(%d)(%c)", 0, tIndex->shmChk[0]);
	dbg(5, "tIndex->shmChk(%d)(%c)", 1, tIndex->shmChk[1]);
	dbg(5, "tIndex->shmChk(%d)(%c)", 2, tIndex->shmChk[2]);
	dbg(5, "tIndex->shmChk(%d)(%c)", 3, tIndex->shmChk[3]);
	dbg(5, "tIndex->shmChk(%d)(%c)", 4, tIndex->shmChk[4]);
	dbg(5, "tIndex->shmChk(%d)(%c)", 5, tIndex->shmChk[5]);
	dbg(5, "tIndex->shmChk(%d)(%c)", 6, tIndex->shmChk[6]);
	dbg(5, "tIndex->shmChk(%d)(%c)", 7, tIndex->shmChk[7]);
	dbg(5, "tIndex->shmChk(%d)(%c)", 8, tIndex->shmChk[8]);
	dbg(5, "tIndex->shmChk(%d)(%c)", 9, tIndex->shmChk[9]);
	if (!sem_unlock(semid)) { dbg(9, "ERR:sem_unlock Error"); }  // 공유메모리 작업이 끝나면 언락    

	if (result == 48) result = 0;
        else if (result == 49) result = 1;	

	return result;
}

/////////////////////////////////////////////
// Desc : 공유메모리 정보를 얻어오는 함수
// ShmChk **pIn : 공유메모리에 붙어서 작업을 하는 포인터 매개변수
int getShmSeg(ShmChk **pIn, unsigned int shmid)
{
	dbg(5, "%s-%d-%s-getShmSeg", __FILE__, __LINE__, __func__);

	//if (( *pIn = (ShmChk *) shmat(shmid, (void *)0, 0)) == ERR) {
	if (( *pIn = (ShmChk *) shmat(shmid, (void *)0, 0)) == ERR) {
		dbg(9,"%s-%d-%s-ERR:getShmSeg Error", __FILE__, __LINE__, __func__);
		return 0;
	}
	return 1;
}

