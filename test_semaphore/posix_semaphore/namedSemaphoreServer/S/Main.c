#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>

// 시그널 25번은 SIGXFSZ
// 의미 : 파일사이즈 초과시 (eXceeded File SiZe)
#define SIG_DEBUG 25

void sigDebugSet(int sigNo);
void attachInitProcess();
void checkArg(int argc, char **argv);
void setPort(int *port, char *inPort);
void waitChildProcess();
void makeParentProcIndependent();

int main(int argc, char **argv)
{
	char *__fn = "main";
	int retVal;
	int port;

	checkArg(argc, argv);				// 아규먼트 개수 체크
	sigDebugSet(SIG_DEBUG);				// dbg 초기화
	setPort(&port, argv[1]);			// 포트번호 세팅
	attachInitProcess();				// Init 프로세스를 부모프로세스로 지정
	waitChildProcess();					// 자식프로세스 작업 끝날때 까지 대기
	makeParentProcIndependent();		// 부모프로세스는 자식프로세스로부터 독립

	retVal = startServer(port);
	if (retVal < 0) {
		dbg(9, "ERR: server_daemon. <%s>", __fn);
		exit(9);
	}

	return 0;
}

/**
  * 로그 설정을 위해 initialize.ini를 읽는다.
  * 시그널 번호를 인자로 받아 해당 이벤트에 대하여 종료되도록 세팅한다.
  * 현재는 파일 용량 초과시에 대비, 시그널을 세팅하여 종료되도록 한다.
  * 25 - SIGXFS(파일 용량 초과에 대한 시그널)
  *
  * History:
  *		곽창섭, 1.0, 2016.02.12 초기작성
  *
  *	@author 곽창섭
  */
void sigDebugSet(int sigNo)
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

	signal(sigNo, sigDebugSet);
}

/**
  * 이 함수는 두가지 기능을 함니다.
  * 1. 부모 프로세스를 종료시킨다.
  * 2. 지식 프로세스는 init(pid=1) 프로세스를 부모프로세스로 합니다.
  * 
  * History:
  *		곽창섭, 1.0, 2016.02.12 초기작성
  *
  *	@author 곽창섭
  */
void attachInitProcess()
{
	if (fork() > 0) {
		exit(0);
	}	
}

/**
  * 아규먼트 개수를 체크합니다. 
  * 
  * History:
  *		곽창섭, 1.0, 2016.02.12 초기작성
  *
  * @param  argc	아규먼트 개수
  * @param  argv	아규먼트 스트링 배열
  *	@author 곽창섭
  */
void checkArg(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
}

/**
  * 입력 받은 포트값을 세팅합니다.
  * 
  * History:
  *		곽창섭, 1.0, 2016.02.16 초기작성
  *
  * @param  port	출력용 포트번호
  * @param  inPort	입력용 포트번호
  *	@author 곽창섭
  */
void setPort(int *port, char *inPort)
{
	*port = atoi(inPort);
	dbg(9, "DAEMON port(%d)", *port);
}

/**
  * 1. 자식프로세스의 종료를 대기한다.
  * 2. 자식프로세스가 종료되면 다시 재시작한다. 
  * 
  * History:
  *		곽창섭, 1.0, 2016.02.16 초기작성
  *
  *	@author 곽창섭
  */
void waitChildProcess()
{
	int status;

	while (1) {
		// 자식 프로세는 다음 로직을 진행한다.
		if (fork() == 0) 
			break;

		wait(&status);				// 자식 프로세스가 종료되기를 기다리며, exit함수 호출시 인자값이 status에 담겨진다.
		dbg(9, "PARENT: FATAL_ERR: CHILD Deleted. status(0X%x)(%d)", status, status);

		if(status == 0x900) {		// 자식 프로세스가 exit함수 인자에 9를 담아서 보냈을 경우 ex) exit(9);
			dbg(9, "ERR: CHILD REQUEST PARENT PGM Exit. EXIT PARENT");
			exit(9);
		}
	}
}

/**
  * 1. 자식 프로세스의 상태로 부터 독립화
  * 
  * History:
  *		곽창섭, 1.0, 2016.02.16 초기작성
  *
  *	@author 곽창섭
  */
void makeParentProcIndependent()
{
	setpgrp();						// 자신의 프로세스 그룹을 자신의 프로세스 아이디로 바꿈
									// 이렇게 하면 로그아웃을 해도, SIGHUP(HangUp Signal)을 받지 않게 됨
	signal(SIGCLD, SIG_IGN);		// SIGCLD(Child Singnal) 자식 프로세스들 중 하나라도 종료/멈춤 발생시 부모에게 보내짐
									// 그리고 부모 프로세스를 종료 시킬 수 있으니 SIG_IGN(Ignore Signal)을 세팅하여 그 신호를 무시함
}
