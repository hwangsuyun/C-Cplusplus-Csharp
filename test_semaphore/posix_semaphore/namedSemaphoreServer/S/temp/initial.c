#define	__TF	"initial.c"

#include	<stdio.h>
#include	<string.h>

#include	"extern.h"

int		svr_port[100];
char	svr_ip[100][20];
int		max_server_cnt;
int		tmout;
int		max_process_cnt;

unsigned char *shm_line_info;
int g_svr_num;

extern	char	inifile[];

int Init_Daemon()
{
static char *__fn = "Init_Daemon";
	int	r, i;
	char	tmp_ip[100];
	char	tmp_port[100];

	tmout = ini_readinteger(inifile, "SERVER_INFO", "SVR_TMOUT", 20);
	max_process_cnt = ini_readinteger(inifile, "SERVER_INFO", "MAX_SVR_CNT", 100);

    max_server_cnt = ini_readinteger(inifile, "BYPASS_INFO", "MAX_SERVER_CNT", 0);
	g_svr_num = max_server_cnt;

	for(i=0; i<max_server_cnt; i++){
		memset(tmp_ip, 0x00, sizeof(tmp_ip));
		memset(tmp_port, 0x00, sizeof(tmp_port));
		sprintf(tmp_ip, "BY_IP%d", i+1);
		sprintf(tmp_port, "BY_PORT%d", i+1);

		svr_port[i] = ini_readinteger(inifile, "BYPASS_INFO", tmp_port, 0);
		ini_readstring(inifile, "BYPASS_INFO", tmp_ip, svr_ip[i], "Dbg");
		dbg(4, "IP%d(%s) PORT%d(%d) <%s:%s>", i+1, svr_ip[i], i+1, svr_port[i], __fn, __TF);
	}

	initShmFile(&shm_line_info, 'P', sizeof(unsigned char) * (g_svr_num+2) );

	return(0);
}
