#ifndef __SHM_H__

#define __SHM_H__



void *initShmFile( unsigned char **shm, char sub_key, int size ); 

void *initShmKey( unsigned char **shm, key_t key , int size );

int accessShm( key_t shmkey);

int makeShm( key_t shmkey, long nbyte );

unsigned char *attachShm( int shmid, char *shmaddr, int flag );

int detachShm( char *vaddr );

int removeShm( key_t shmkey );



#endif

