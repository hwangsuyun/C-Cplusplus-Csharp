#include	<sys/types.h>
#include	<sys/ipc.h>
#include	<sys/shm.h>
#include	<sys/sem.h>

//#define		SEM_KEY_CH	'S'
//#define		SHM_KEY_CH	'M'
//#define		SHM_KEY_CH2	'N'
#define		SHM_PERM	0666

typedef struct {
	char shmChk[10];
} ShmChk;

//struct sembuf	semLock = {0, -1, SEM_UNDO};
//struct sembuf	semUnLock = {0, 1, SEM_UNDO};

//#define	SEM_LOCK(id)   (!semop(id, &semLock, 1)) ? 1 : 0
//#define	SEM_UNLOCK(id) (!semop(id, &semUnLock, 1)) ? 1 : 0

int init_ipc(long *sem_key, long *shm_key);
