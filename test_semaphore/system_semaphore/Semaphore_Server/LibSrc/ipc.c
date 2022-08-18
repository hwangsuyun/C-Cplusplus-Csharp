#include	<errno.h>
#include	<string.h>
#include	<stdio.h>

#include	"ipc.h"
#include	"semun.h"

#define ERR		((void *) -1)

// SEM_UNDO �ɼ��� ���μ����� ����Ǹ� �ý��ۿ��� �������� ������ ���� ���·� �ǵ���. �׷��Ƿ� ���� �� �ɼ��� ��� 
// IPC_NOWAIT �ɼ��� ȣ�� ��� �������� ������ ��� ��ٸ��� �ʰ� ���з� �ٷ� ����
// SEM_UNDO�� �ɼ����� �Ͽ� semLock�� �ϰ� semUnLock�� ���������� ���ŷ ��尡 ��, �׷��� �� ���¿��� �� �ٸ� �� ��û�� �ö� ������ �ֱ� ���ؼ��� IPC_NOWAIT �ɼ��� ����ؾ���
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
	// semop() �Լ��� ���������� ���� ���� ������� ����ϱ� ���ؼ��� ���� �������� ���� 1 ���ҽ�Ű��, ��� �Ŀ��� 1 ������Ű�µ�, �̷��� ��������� �����ϴ� ���� ��û�ϴ� �Լ�
	if (semop(semid, &semLock, 1) == -1) 
		return -1;
	return 0;
}

int sem_unlock(int semid)
{
	// semop() �Լ��� ���������� ���� ���� ������� ����ϱ� ���ؼ��� ���� �������� ���� 1 ���ҽ�Ű��, ��� �Ŀ��� 1 ������Ű�µ�, �̷��� ��������� �����ϴ� ���� ��û�ϴ� �Լ�
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

	/* �������� ����*/
	ipc_key = ftok(".", sem_key);
	if (ipc_key < 0) {
		dbg(9,"ERR:init_ipc key Error(%#010x)", ipc_key);
		return -1;
	}

	// IPC_EXCL ���� KEY�� ������ ����, �����ϸ� ���� ����, IPC_CREAT ���� �ܵ� ��� �Ұ�
	semid = semget(ipc_key, sem_num, IPC_EXCL | IPC_CREAT | SHM_PERM);  
	dbg(1, "semget option is IPC_EXCL | IPC_CREAT | 0666");
	if (semid == -1) {
		// IPC_CREAT ���� KEY�� ������ ����, �����ϸ� ���� KEY �� ����, �ܵ� ��� ����
		semid = semget(ipc_key, sem_num, IPC_CREAT | SHM_PERM);
		dbg(1, "semget option is IPC_CREAT | 0666");
		if (semid == -1) {
			dbg(9,"ERR:init_ipc semget Error");
			return -1;
		}
		// ���� KEY�� ������ semctl(�������������Լ�)���� �����Ͽ� semop(������������Լ�)���
		return semid;
	}

	dbg(5, "init_ipc SEMAPHORE ID(%d)", semid);

	// semtcl()�Լ��� ������� �����ϴ� �Լ��μ� �����������չ�ȣ, �����������ճ���������ġ, ������(SETVAL�� �������������), semun���Ͽº���(��������ý��ۺ���) �� ���ڷ� ���
	sem_union.val = 1;
	if (semctl(semid, 0, SETVAL, sem_union) == -1) {
		dbg(9,"ERR:init_ipc semctl Error");
		return -1;
	}

	dbg(5,"OK:Semaphore Set Succ SEMID(%d)", semid);

	dbg(1,"END:init_ipc()");

	return semid;
}

// Desc : ������� �̿��Ͽ� ��ȣ������ ����
// @semid : �������� id
int setLockSemaphore(int semid)
{
	int ret;

	dbg(5, "setLockSemaphore(%d)", semid);
	
	// �����޸� �̿��Ͽ� �����ϱ�
	ret = sem_lock(semid); // �������� �̿��Ͽ� ��ŷ �ϱ�
	if (ret < 0)
	{
		dbg(9, "ERR:sem_lock Error"); 
		return -1;
	}	
	return 0;
}

// Desc : ������� �̿��Ͽ� ��ȣ������ ����
// @semid : �������� id
int setUnLockSemaphore(int semid)
{
	int ret;

	dbg(5, "setUnLockSemaphore(%d)", semid);
	
	// �����޸� �̿��Ͽ� �����ϱ�
	ret = sem_unlock(semid); // �������� �̿��Ͽ� ���ŷ �ϱ�
	if (ret < 0)
	{ 
		dbg(9, "ERR:sem_unlock Error"); 
		return -1;
	} 
	return 0;
}
