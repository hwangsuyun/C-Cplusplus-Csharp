#define	__TF	"daemon_main.c"

#include	<stdio.h>
#include	<string.h>
#include	<signal.h>
#include	<sys/types.h>
#include	<dirent.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<unistd.h>
#include	<sys/wait.h>

#include	"extern.h"

#define	SIG_DEBUG	25

static char Run_Pgm_Name[100];

char	inifile[100];

void Sig_Debug_Set(int sig_no);

extern int Running_Cnt;

main(int argc, char **argv)
{
char	*__fn = "main";
	int 	r, i, status;
	char	tmp_buff[100];

	if(fork() > 0) {
		exit(0);
	}

	strcpy(Run_Pgm_Name, argv[0]);

	sprintf(inifile, "%s.ini", argv[0]);
	if(access(inifile, F_OK) < 0){
		printf("INI 파일을 찾을 수 없습니다. \n");
		exit(-1);
	}

	Sig_Debug_Set(SIG_DEBUG);

	while(1) {
		if(fork() == 0) {
			break;
		}

		wait(&status);

		dbg(9, "PARENT: FATAL_ERR: CHILD Deleted. status(0X%x)(%d)", status, status);
		if(status == 0x900) {
			dbg(9, "ERR: CHILD REQUEST PARENT PGM Exit. EXIT PARENT");
			exit(9);
		}
		dbg(9, "PARENT: continue RUNNING");
	}

    setpgrp();
	signal(SIGCLD, SIG_IGN);

	Init_Daemon();

	r = Server_Daemon();
        if(r < 0) {
                dbg(9, "ERR: Main_Thread. <%s:%s>", __fn, __TF);
                exit(9);
        }
}

void Sig_Debug_Set(int sig_no)
{
static char *__fn = "*Sig_Debug_Set";

	char	dbg_file[100];
	int	r, dbg_level, dbg_new_flag;

	ini_readstring(inifile, "DBG_INFO", "DBG_DIR_NAME", dbg_file, "./Dbg");
	dbg_level = ini_readinteger(inifile, "DBG_INFO", "DBG_LEVEL", 9);
	dbg_new_flag = ini_readinteger(inifile, "DBG_INFO", "DBG_NEW_FLAG", 0);

	Daemon_Dbg_Init(dbg_file, dbg_level, dbg_new_flag, "daemon");

 	signal(sig_no, Sig_Debug_Set);
}
