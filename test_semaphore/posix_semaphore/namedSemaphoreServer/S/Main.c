#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>

// �ñ׳� 25���� SIGXFSZ
// �ǹ� : ���ϻ����� �ʰ��� (eXceeded File SiZe)
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

	checkArg(argc, argv);				// �ƱԸ�Ʈ ���� üũ
	sigDebugSet(SIG_DEBUG);				// dbg �ʱ�ȭ
	setPort(&port, argv[1]);			// ��Ʈ��ȣ ����
	attachInitProcess();				// Init ���μ����� �θ����μ����� ����
	waitChildProcess();					// �ڽ����μ��� �۾� ������ ���� ���
	makeParentProcIndependent();		// �θ����μ����� �ڽ����μ����κ��� ����

	retVal = startServer(port);
	if (retVal < 0) {
		dbg(9, "ERR: server_daemon. <%s>", __fn);
		exit(9);
	}

	return 0;
}

/**
  * �α� ������ ���� initialize.ini�� �д´�.
  * �ñ׳� ��ȣ�� ���ڷ� �޾� �ش� �̺�Ʈ�� ���Ͽ� ����ǵ��� �����Ѵ�.
  * ����� ���� �뷮 �ʰ��ÿ� ���, �ñ׳��� �����Ͽ� ����ǵ��� �Ѵ�.
  * 25 - SIGXFS(���� �뷮 �ʰ��� ���� �ñ׳�)
  *
  * History:
  *		��â��, 1.0, 2016.02.12 �ʱ��ۼ�
  *
  *	@author ��â��
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
  * �� �Լ��� �ΰ��� ����� �Դϴ�.
  * 1. �θ� ���μ����� �����Ų��.
  * 2. ���� ���μ����� init(pid=1) ���μ����� �θ����μ����� �մϴ�.
  * 
  * History:
  *		��â��, 1.0, 2016.02.12 �ʱ��ۼ�
  *
  *	@author ��â��
  */
void attachInitProcess()
{
	if (fork() > 0) {
		exit(0);
	}	
}

/**
  * �ƱԸ�Ʈ ������ üũ�մϴ�. 
  * 
  * History:
  *		��â��, 1.0, 2016.02.12 �ʱ��ۼ�
  *
  * @param  argc	�ƱԸ�Ʈ ����
  * @param  argv	�ƱԸ�Ʈ ��Ʈ�� �迭
  *	@author ��â��
  */
void checkArg(int argc, char **argv)
{
	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
}

/**
  * �Է� ���� ��Ʈ���� �����մϴ�.
  * 
  * History:
  *		��â��, 1.0, 2016.02.16 �ʱ��ۼ�
  *
  * @param  port	��¿� ��Ʈ��ȣ
  * @param  inPort	�Է¿� ��Ʈ��ȣ
  *	@author ��â��
  */
void setPort(int *port, char *inPort)
{
	*port = atoi(inPort);
	dbg(9, "DAEMON port(%d)", *port);
}

/**
  * 1. �ڽ����μ����� ���Ḧ ����Ѵ�.
  * 2. �ڽ����μ����� ����Ǹ� �ٽ� ������Ѵ�. 
  * 
  * History:
  *		��â��, 1.0, 2016.02.16 �ʱ��ۼ�
  *
  *	@author ��â��
  */
void waitChildProcess()
{
	int status;

	while (1) {
		// �ڽ� ���μ��� ���� ������ �����Ѵ�.
		if (fork() == 0) 
			break;

		wait(&status);				// �ڽ� ���μ����� ����Ǳ⸦ ��ٸ���, exit�Լ� ȣ��� ���ڰ��� status�� �������.
		dbg(9, "PARENT: FATAL_ERR: CHILD Deleted. status(0X%x)(%d)", status, status);

		if(status == 0x900) {		// �ڽ� ���μ����� exit�Լ� ���ڿ� 9�� ��Ƽ� ������ ��� ex) exit(9);
			dbg(9, "ERR: CHILD REQUEST PARENT PGM Exit. EXIT PARENT");
			exit(9);
		}
	}
}

/**
  * 1. �ڽ� ���μ����� ���·� ���� ����ȭ
  * 
  * History:
  *		��â��, 1.0, 2016.02.16 �ʱ��ۼ�
  *
  *	@author ��â��
  */
void makeParentProcIndependent()
{
	setpgrp();						// �ڽ��� ���μ��� �׷��� �ڽ��� ���μ��� ���̵�� �ٲ�
									// �̷��� �ϸ� �α׾ƿ��� �ص�, SIGHUP(HangUp Signal)�� ���� �ʰ� ��
	signal(SIGCLD, SIG_IGN);		// SIGCLD(Child Singnal) �ڽ� ���μ����� �� �ϳ��� ����/���� �߻��� �θ𿡰� ������
									// �׸��� �θ� ���μ����� ���� ��ų �� ������ SIG_IGN(Ignore Signal)�� �����Ͽ� �� ��ȣ�� ������
}
