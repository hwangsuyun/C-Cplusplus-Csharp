#include	"ipc.h"
#include	"semun.h"
#include	<errno.h>
#include	<string.h>

//#define ERR		((ShmChk *) -1)
#define ERR		((void *) -1)

struct sembuf semLock = {0, -1, SEM_UNDO};
struct sembuf semUnLock = {0, 1, SEM_UNDO};

#define   SEM_LOCK(id)   (!semop(id, &semLock, 1)) ? 1 : 0
#define   SEM_UNLOCK(id) (!semop(id, &semUnLock, 1)) ? 1 : 0

static	unsigned int	shmid;
static	unsigned int	semid;

//static  ShmChk  *shmChk;

int sem_lock()
{
	if (semop(semid, &semLock, 1) == 0) 
		return 1;
	return 0;
}

int sem_unlock()
{
	if (semop(semid, &semUnLock, 1) == 0) 
		return 1;
	return 0;
}

/////////////////////////////////////////////
// Desc : 공유메모리 정보를 얻어오는 함수
// ShmChk **pIn : 공유메모리에 붙어서 작업을 하는 포인터 매개변수
//int getShmSeg(ShmChk **pIn)
//int getShmSeg(char **pIn)
int getShmSeg(char **pIn)
{
	//if (( *pIn = (ShmChk *) shmat(shmid, (void *)0, 0)) == ERR) {
	if (( *pIn = (char *) shmat(shmid, (void *)0, 0)) == ERR) {
		dbg(9,"%s-%d-%s-ERR:getShmSeg Error", __FILE__, __LINE__, __func__);
		return 0;
	}
	return 1;
}

/*
int get_van_index(int jobCode)
{
	int	index = 0;
	int	incIndex = 0;

	index = van_index_st->vanIndex[jobCode];
	dbg(1,"get_van_index(jobCode:%d, vanIndex = %d)", jobCode, index);
	if (index < 0) {
		dbg(9,"ERR:get_van_index index(%d) Error", index);
		return index;
	}

	if (!sem_lock()) {
		dbg(9,"ERR:sem_lock Error");
		return -1;
	}

	(van_index_st->vanIndex[jobCode])++;
	van_index_st->vanIndex[jobCode] %= 100;
	if (!sem_unlock()) {
		dbg(9,"ERR:sem_lock Error");
		return -1;
	}
	dbg(1,"END:get_van_index(%d)", index);
	return index;
}
*/

int init_ipc(long sem_key, long shm_key)
{
	key_t	ipc_key;
	int	sem_num = 1;	/* semaphore count */
	void * shm_addr = (void *)0;
	union semun 	sem_union;

	dbg(1, "START:init_ipc()");

	/* 세마포어 설정*/
	ipc_key = ftok(".", sem_key);
	if (ipc_key < 0) {
		dbg(9,"ERR:init_ipc key Error(%#010x)", ipc_key);
		return -1;
	}

	semid = semget(ipc_key, sem_num, IPC_CREAT | SHM_PERM);  
	if (semid == -1) {
		dbg(9,"ERR:init_ipc semget Error");
		return -1;
	}

	dbg(5, "init_ipc SEMAPHORE ID (%d)", semid);
	sem_union.val = 1;
	if (semctl(semid, 0, SETVAL, sem_union) == -1) {
		dbg(9,"ERR:init_ipc semctl Error");
		return -1;
	}

	dbg(5,"OK:Semaphoer Set Succ SEMID(%d)", semid);
	
	/* 공유메모리 설정*/
	ipc_key = ftok(".", shm_key);
	if (ipc_key < 0) {
		dbg(9,"ERR:get_shm_id key Error(%#010x)", ipc_key);
		return -1;
	}

	dbg(5, "OK:get_shm_id ipc_key = %#010x", ipc_key);
	//shmid = shmget(ipc_key, sizeof(ShmChk), IPC_CREAT | SHM_PERM);
	shmid = shmget(ipc_key, sizeof(char), IPC_CREAT | SHM_PERM);
	if (shmid == -1) {
		dbg(9,"ERR:get_shm_id shmid(%d) Error", shmid);
		return -1;
	}

	dbg(5,"OK:get_shm_id shmid(%d)", shmid);

	//shmChk = 0x0;
	shmType0 = 0x0;
	//shmChk = (ShmChk *)shmat(shmid, (void *)0, 0);
	shmType0 = (char *)shmat(shmid, (void *)0, 0);
	if (shm_addr == (void *)-1) {
		dbg(9,"ERR:shm_addr is nagative. shmid(%d)", shmid);
		return -1;
	}

	//memset((char *)shmChk, 0, sizeof(ShmChk));
	memset((char *)shmType0, 0, sizeof(char));

	dbg(1,"END:init_ipc()");
	return 0;
}

// Desc : 세마포어를 이용하여 공유메모리에 접근하여 상호배제를 실현
//        공유메모리의 인덱스에 해당하는 주소의 내용을 수정하는 함수
// @index : 인덱스 번호
// @value  : 세팅되는 값
void setShmSegData(int index, int value)
{
	dbg(5, "%s-%d-%s-setShmSegData", __FILE__, __LINE__, __func__);
	//ShmChk *tIndex;
	char *tIndex;

	// 공유메모리 이용하여 변경하기
	if (!sem_lock()) { dbg(9, "ERR:sem_lock Error"); }      // 세마포어 이용하여 락킹/언락킹 하기
	getShmSeg(&tIndex);                                     // 공유메모리의 정보를 가져옴
	//tIndex->shmChk[index] = value;                      // 공유메모리의 인덱스에 해당하는 상태 정보를 value 값으로
	shmType0[index] = value;                      // 공유메모리의 인덱스에 해당하는 상태 정보를 value 값으로
	if (!sem_unlock()) { dbg(9, "ERR:sem_unlock Error"); }  // 공유메모리 작업이 끝나면 언락    
}

// Desc : 세마포어를 이용하여 공유메모리에 접근하여 상호배제를 실현
//        공유메모리의 인덱스에 해당하는 주소의 내용을 얻어오는 함수
// @index : 인덱스 번호
int getShmSegData(int index)
{
	dbg(5, "%s-%d-%s-getShmSegData", __FILE__, __LINE__, __func__);

	int result;
	//ShmChk *tIndex;
	char *tIndex;

	// 공유메모리 이용하여 가져오기
	if (!sem_lock()) { dbg(9, "ERR:sem_lock Error"); }      // 세마포어 이용하여 락킹/언락킹 하기
	getShmSeg(&tIndex);                                     // 공유메모리의 정보를 가져옴
	//result = tIndex->shmChk[index];                     // 공유메모리의 인덱스에 해당하는 상태 정보를 가져오기
	result = shmType0[index];                     // 공유메모리의 인덱스에 해당하는 상태 정보를 가져오기
	if (!sem_unlock()) { dbg(9, "ERR:sem_unlock Error"); }  // 공유메모리 작업이 끝나면 언락    

	return result;
}

// Desc : 카드서버 상태 값을 초기화 함
//        공유메모리의 인덱스위치에 정보 세팅
// 
/*
int initCardSrvSt()
{
	dbg(1, "%s-%d-%s-initCardSrvSt", __FILE__, __LINE__, __func__);
	setShmSegData(0, lotteCardUseYn);       // 0번은 롯데카드 서버
	setShmSegData(1, shinhanCardUseYn);     // 1번은 신한카드 서버
	// 차후 신한카드, BC카드, 국민카드 추가될 예정
	return 0;
}
*/
