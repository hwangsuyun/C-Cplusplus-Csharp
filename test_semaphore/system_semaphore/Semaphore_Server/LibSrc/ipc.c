#include	<errno.h>
#include	<string.h>
#include	<stdio.h>

#include	"ipc.h"
#include	"semun.h"

#define ERR		((void *) -1)

// SEM_UNDO 옵션은 프로세스가 종료되면 시스템에서 세마포어 설정을 원래 상태로 되돌림. 그러므로 보통 이 옵션을 사용 
// IPC_NOWAIT 옵션은 호출 즉시 실행하지 못했을 경우 기다리지 않고 실패로 바로 복귀
// SEM_UNDO를 옵션으로 하여 semLock만 하고 semUnLock을 하지않으면 블로킹 모드가 됨, 그래서 락 상태에서 또 다른 락 요청이 올때 응답을 주기 위해서는 IPC_NOWAIT 옵션을 사용해야함
//struct sembuf semLock = {0, -1, SEM_UNDO};
//struct sembuf semUnLock = {0, 1, SEM_UNDO};
struct sembuf semLock = {0, -1, IPC_NOWAIT};
struct sembuf semUnLock = {0, 1, IPC_NOWAIT};
//struct sembuf semLock = {0, -1, IPC_NOWAIT | SEM_UNDO};
//struct sembuf semUnLock = {0, 1, IPC_NOWAIT | SEM_UNDO};

#define   SEM_LOCK(id)   (!semop(id, &semLock, 1)) ? 1 : 0
#define   SEM_UNLOCK(id) (!semop(id, &semUnLock, 1)) ? 1 : 0

int sem_lock(int semid)
{
	// semop() 함수는 세마포어의 값을 변경 세마포어를 사용하기 위해서는 먼저 세마포어 값을 1 감소시키고, 사용 후에는 1 증가시키는데, 이렇게 세마포어값을 증감하는 것을 요청하는 함수
	if (semop(semid, &semLock, 1) == -1) 
		return -1;
	return 0;
}

int sem_unlock(int semid)
{
	// semop() 함수는 세마포어의 값을 변경 세마포어를 사용하기 위해서는 먼저 세마포어 값을 1 감소시키고, 사용 후에는 1 증가시키는데, 이렇게 세마포어값을 증감하는 것을 요청하는 함수
	if (semop(semid, &semUnLock, 1) == -1) 
		return -1;
	return 1;
}

int init_ipc(int sem_key)
{
	key_t ipc_key;
	int	sem_num = 1;	/* semaphore count */
	union semun sem_union;
	int semid;

	dbg(1, "START:init_ipc()");

	/* 세마포어 설정*/
	ipc_key = ftok(".", sem_key);
	if (ipc_key < 0) {
		dbg(9,"ERR:init_ipc key Error(%#010x)", ipc_key);
		return -1;
	}

	// IPC_EXCL 사용시 KEY가 없으면 생성, 존재하면 에러 리턴, IPC_CREAT 없이 단독 사용 불가
	semid = semget(ipc_key, sem_num, IPC_EXCL | IPC_CREAT | SHM_PERM);  
	dbg(1, "semget option is IPC_EXCL | IPC_CREAT | 0666");
	if (semid == -1) {
		// IPC_CREAT 사용시 KEY가 없으면 생성, 존재하면 기존 KEY 값 리턴, 단독 사용 가능
		semid = semget(ipc_key, sem_num, IPC_CREAT | SHM_PERM);
		dbg(1, "semget option is IPC_CREAT | 0666");
		if (semid == -1) {
			dbg(9,"ERR:init_ipc semget Error");
			return -1;
		}
		// 기존 KEY가 있으면 semctl(세마포어제어함수)없이 리턴하여 semop(세마포어값변경함수)사용
		return semid;
	}

	dbg(5, "init_ipc SEMAPHORE ID(%d)", semid);

	// semtcl()함수는 세마포어를 제어하는 함수로서 세마포어집합번호, 세마포어집합내에서의위치, 제어명령(SETVAL은 세마포어값을설정), semun유니온변수(세마포어시스템변수) 를 인자로 사용
	sem_union.val = 1;
	if (semctl(semid, 0, SETVAL, sem_union) == -1) {
		dbg(9,"ERR:init_ipc semctl Error");
		return -1;
	}

	dbg(5,"OK:Semaphore Set Succ SEMID(%d)", semid);

	dbg(1,"END:init_ipc()");

	return semid;
}

// Desc : 세마포어를 이용하여 상호배제를 실현
// @semid : 세마포어 id
int setLockSemaphore(int semid)
{
	int ret;

	dbg(5, "setLockSemaphore(%d)", semid);
	
	// 공유메모리 이용하여 변경하기
	ret = sem_lock(semid); // 세마포어 이용하여 락킹 하기
	if (ret < 0)
	{
		dbg(9, "ERR:sem_lock Error"); 
		return -1;
	}	
	return 0;
}

// Desc : 세마포어를 이용하여 상호배제를 실현
// @semid : 세마포어 id
int setUnLockSemaphore(int semid)
{
	int ret;

	dbg(5, "setUnLockSemaphore(%d)", semid);
	
	// 공유메모리 이용하여 변경하기
	ret = sem_unlock(semid); // 세마포어 이용하여 언락킹 하기
	if (ret < 0)
	{ 
		dbg(9, "ERR:sem_unlock Error"); 
		return -1;
	} 
	return 0;
}
