#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#include "shm.h"
#include "line_check.h"

int __shmid;

void *initShmFile( unsigned char **shm, char sub_key, int size )
{
	int shmid;
	int i;
	long shm_key;

	shm_key = ftok(".", sub_key );
	if ( shm_key < 0 ) {
		exit(-1);
	}

	// shmget 으로 공유메모리 생성하여 ID 리턴
    shmid = makeShm(shm_key, size );
    if( shmid == -1 ) {
		exit(1);
	}

 	// shmat 으로 공유메모리를 프로세스에 연결
    *shm = (unsigned char *)attachShm( shmid, NULL, 0 );
    if( shm == NULL ) {
		exit(1);
	}

	for(i = 0; i < size; i++)
		setOkShmLineInfo(shm, i);
}

void *initShmKey( unsigned char **shm, key_t key , int size )
{
	int shmid;
	int i;

	// shmget 으로 공유메모리 생성하여 ID 리턴
    shmid = makeShm( key, size );
    if( shmid == -1 ) {
		exit(1);
	}
	printf("==shm_key=[%d], shmid=[%d]\n", key, shmid);

 	// shmat 으로 공유메모리를 프로세스에 연결
    *shm = (unsigned char *)attachShm( shmid, NULL, 0 );
    if( shm == NULL ) {
		exit(1);
	}

	for(i = 0; i < size; i++) {
	    setOkShmLineInfo(shm, i);
	}
}
/*---------------------------------------------------------------------------*
* FUNCTION : SHM Access                                                     *
* Argument : key_t shmkey - Shared Memory key                               *
* Return   : Succes - SHM ID                                                *
*            FAIL   - -1                                                    *
*---------------------------------------------------------------------------*/

int accessShm(shmkey)
    key_t shmkey;
{
    int shmid;
    shmid = shmget(shmkey,0,0666|IPC_EXCL);
    if(shmid==-1) return(-1);
    return shmid;
}


/*---------------------------------------------------------------------------*
 * FUNCTION : SHM Create                                                     *
 * Argument : key_t shmkey - Shared Memory key                               *
 * Return   : Succes - SHM ID                                                *
 *            FAIL   - -1                                                    *
 *---------------------------------------------------------------------------*/

int makeShm(shmkey,nbyte)
	key_t shmkey;
	long  nbyte;
{
	int shmid;

	if(shmkey == IPC_PRIVATE) 
	{
		//printf("Makeshm, IPC_PRIVATE\n");
		return -1;
	}
	if((shmid = shmget(shmkey,nbyte,0666|IPC_CREAT))<0) 
	{
		//printf("Makeshm, shmget Err\n");
		return -1;
	}
	//printf("Makeshm, shmget Ok\n");
	return shmid;
}

/*---------------------------------------------------------------------------*
 * FUNCTION : Shared Memory attach.                                          *
 * Argument : int shmid - shared memory key                                  *
 *            char *shmaddr - shared memory address                          *
 *            int flag - bit flag(system address)                            *
 * Return   : Succes - shared memory address pointer                         *
 *---------------------------------------------------------------------------*/

unsigned char *attachShm(shmid,shmaddr,flag)
	int 	shmid;
	char 	*shmaddr;
	int  	flag;
{
	char *vaddr;
	vaddr = shmat(shmid,shmaddr,flag);
	return (vaddr);
}

/*---------------------------------------------------------------------------*
 * FUNCTION : Shared Memory detach.                                          *
 * Argument : char vaddr - shared memory address point                       *
 * Return   : Succes - 0.                                                    *
 *            FAIL   -  -1                                                   *
 *---------------------------------------------------------------------------*/

int detachShm(vaddr)
	char *vaddr;
{
	int rtn;
	if((rtn = shmdt(vaddr))<0) return -1;
	return 1;
}

/*---------------------------------------------------------------------------*
 * FUNCTION : SHM Remove                                                     *
 * Argument : key_t shmkey - Shared Memory key                               *
 * Return   : Succes - 0.                                                    *
 *            FAIL   - -1.                                                   *
 *---------------------------------------------------------------------------*/

int removeShm(shmkey)
	key_t shmkey;
{
	int shmid;
	int rtn;
	if((shmid=accessShm(shmkey))<0) return -1;
	if(shmctl(shmid,IPC_RMID,0)<0) return -1;
	return 1;
}
