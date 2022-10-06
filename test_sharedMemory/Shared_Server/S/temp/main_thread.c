#define	__TF	"main_thread.c"

#include        <sys/socket.h>
#include        <sys/types.h>
#include        <sys/time.h>
#include        <stdlib.h>
#include        <signal.h>
#include        <wait.h>
#include        <errno.h>
#include        <unistd.h>
#include        <sys/ipc.h>
#include	<stdio.h>

#include	"extern.h"

extern char		inifile[];
extern int     max_process_cnt;

int	listenfd;

int	bMain_Running;

int dbg_fork_cnt;
extern unsigned char *shm_line_info;
extern int g_svr_num;

int	Server_Daemon()
{
static char     *__fn = "DaemonThr_Rtn";

	char	cliaddr_buff[100];
	int		nsockfd;
	int     i, r;
	int     Server_Port_No;

	bMain_Running = 1;
	
	setRunCnt(&shm_line_info, g_svr_num, 0);

	Server_Port_No = ini_readinteger(inifile, "SERVER_INFO", "SERVER_PORT", 0);
	if(Server_Port_No <= 0) {
		dbg(9, "ERR: inifile(%s) get Server_Port_No. Server_Port_No(%ld). <%s:%s>",
		inifile, Server_Port_No, __fn, __TF);
		exit(9);
	}
	dbg(6, "DAEMON Server_Port_No(%d)", Server_Port_No);

	listenfd = Open_Server_Socket(Server_Port_No);
	if(listenfd < 0){
		dbg(9, "ERR: Open_Server_Socket failed. Server_Port_No(%d) <%s:%s>",  Server_Port_No, __fn, __TF);
		exit(9);
	}
	dbg(6, "OK: Open_Server_Socket. port_no(%d). <%s:%s>",  Server_Port_No, __fn, __TF);

	while(bMain_Running){
		memset(cliaddr_buff, 0x0, sizeof(cliaddr_buff));

		dbg(2, "CALL: sock_accept. <%s:%s>", __fn, __TF);
		nsockfd = sock_accept(listenfd, cliaddr_buff);
		if (nsockfd < 0 ) {
			dbg(9, "ERR: Socket Accept Fail(%d) <%s:%s>", nsockfd, __fn, __TF);
			continue;
		}

		dbg_fork_cnt = (dbg_fork_cnt+1) % 100000;
		if(fork() == 0) {
			dbg(9, "Client %s CONNECTED. <%s:%s>", cliaddr_buff, __fn, __TF);
			close(listenfd);
			break;
		}
		close(nsockfd);
	}


	r = incRunCnt(&shm_line_info, g_svr_num);
	dbg(9, "run cnt[%d]", r);
	if( r <= max_process_cnt )
	{
		Server_Main(dbg_fork_cnt, nsockfd);
	}
	else
	{
		dbg(9, "NOT Server_Main");
	}
	r = decRunCnt(&shm_line_info, g_svr_num);

	close(nsockfd);
	exit(0);
}
