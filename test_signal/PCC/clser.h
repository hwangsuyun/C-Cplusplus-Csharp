#include 	<sys/types.h>
#include 	<sys/socket.h>
#include	<pthread.h>
#include	<signal.h>
#include	<unistd.h>
#include	<stdio.h>
#include	<netinet/in.h>
#include	<errno.h>
#include	<fcntl.h>
#include	<sys/time.h>
#include	<sys/stat.h>
#include	<pthread.h>

#include	"myipc.h"

#define	SIZE	sizeof(struct sockaddr_in)
#define	QUEUESIZ	5120

typedef struct {
	int 	client_fd;			/*	클라이언트 접속 fd 		*/
	int		seq_no;				/*	POOL 사용 순번			*/
	int 	status;				/*	POOL 사용여부 상태		*/
}CLIENTFD_POOL;

CLIENTFD_POOL *clientfd_pool;
int 	clientfd_cnt;
int 	port_num;
int		clser_pid;

PROCESSPOOL			*processpool;
INNERJUNMUNDATA		innerjunmundata;
