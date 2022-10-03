#ifndef _SEMUN_H
#define	_SEMUN_H	1
union semun {
	int	val;
	struct	semid_ds *buf;
	unsigned short int *array;
	struct seminfo *__buf;
   };
#endif
