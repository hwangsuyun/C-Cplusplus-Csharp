#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#define SIG_DEBUG 25

void Sig_Debug_Set(int sig_no);

int main(int argc, char **argv)
{
	char *__fn = "main";
	int status, r;
	char initFile[20];
	int shared_status;
	int port;

	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[1]);
		exit(1);
	}

	if (fork() > 0)
	{
		exit(0);
	}

	// 1. file read
	memset(initFile, 0x00, sizeof(initFile));
	memcpy(initFile, "initialize.ini", sizeof(initFile));
	port = atoi(argv[1]);
	
	Sig_Debug_Set(SIG_DEBUG);

	while (1)
	{
		if (fork() == 0)
		{
			break;
		}

		wait(&status);
		
		dbg(9, "PARENT: FATAL_ERR: CHILD Deleted. status(0X%x)(%d)", status, status);
		if(status == 0x900) 
		{
			dbg(9, "ERR: CHILD REQUEST PARENT PGM Exit. EXIT PARENT");
			exit(9);
		}
	}

	setpgrp();
	signal(SIGCLD, SIG_IGN);

	dbg(6, "DAEMON port(%d)", port);
	dbg(6, "DAEMON shared_status(%d)", shared_status);

	// 2. tcp 
	r = server_daemon(port);
	if (r < 0)
	{
		dbg(9, "ERR: server_daemon. <%s>", __fn);
		exit(9);
	}

	return 0;
}

void Sig_Debug_Set(int sig_no)
{
	char iniFile[30];
	char dbg_file[100];
	int  r, dbg_level, dbg_new_flag;

	memset(iniFile, 0x00, sizeof(iniFile));
	memcpy(iniFile, "initialize.ini", sizeof(iniFile));

	ini_readstring(iniFile, "DBG_INFO", "DBG_DIR_NAME", dbg_file, "./Dbg");
	dbg_level = ini_readinteger(iniFile, "DBG_INFO", "DBG_LEVEL", 9);
	dbg_new_flag = ini_readinteger(iniFile, "DBG_INFO", "DBG_NEW_FLAG", 0);

	Daemon_Dbg_Init(dbg_file, dbg_level, dbg_new_flag, "daemon");

	signal(sig_no, Sig_Debug_Set);
}

