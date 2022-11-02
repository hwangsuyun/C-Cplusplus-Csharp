#include		<stdio.h>
#include		<stdlib.h>
#include		<string.h>
#include		<signal.h>
#include		<fcntl.h>
#include		<sys/socket.h>
#include		<sys/shm.h>
#include		<sys/types.h>
#include		<sys/wait.h>
#include		<netinet/in.h>
#include		<errno.h>
#include		<fcntl.h>
#include		<sys/time.h>
#include		<sys/stat.h>
#include		<pthread.h>
#include		<unistd.h>

#include		"myipc.h"
#include		"utillib.h"

#define	UPDOWN_PORT		50000
#define	KILL_ALL_RETURN_VALUE	9

#define PROGRAM_NAME "PCC"
#define INIFILE_NAME "process_info"

VANGRP				*vangrp;
PROCESSPOOL			*processpool;

int				listenfd;
int				tc_timeout = 5;

long int ServerPort = UPDOWN_PORT;

int ProcessPool_ShmKey = (int)PROCESSPOOL_SHMKEY;
int Pool_SemKey = (int)POOL_SEMKEY;
int Pcc_SemKey = (int)PCC_SEMKEY;

void sig_chld();

int checkBlackList(char *cli_ip)
{
	int     i, arr_cnt =0;
        char    arr_temp[MAX_ARG_COUNT][MAX_ARGS_SIZE];
        char    temp[1024 + 1];

        memset(temp, 0x00, sizeof(temp));
        memset(arr_temp, 0x00, sizeof(arr_temp));
        ReadString(PROGRAM_NAME, "BlackList", "", temp, sizeof(temp) - 1, INIFILE_NAME);
        if (strlen(temp) == 0 ) return(-1);
 
	arr_cnt = buff_array(temp, ',', arr_temp);
	if (arr_cnt <= 0) return(-1);
	
	for(i = 0; i < arr_cnt; i++) {
		if(strcmp(cli_ip, arr_temp[i]) ==0) return(0);
	}
	return(1);
}

void Init_VANGRP()
{
	int 	i, rc;
	
	for( i = 0 ; i < VANGRPPOOL_CNT ; i++)
	{
		vangrp->Id[i] = i;
		memset(vangrp->IP[i], 0x00, sizeof(vangrp->IP[i]));
		vangrp->Port[i] 		= 0;
		vangrp->Status[i]		= ESTABLISH;
		vangrp->DisConnectCnt[i]= 0;
		vangrp->Current[i] 		= 0;
		vangrp->Cnt[i]			= 0;
		vangrp->Seqno[i]		= 0;
	}
}

void Init_PROCESSPOOL()
{
	int		i;
	
	for(i = 0; i < MAX_PROCESSPOOL_CNT ; i++)
	{
		processpool->Id[i] = i;
		memset(processpool->Name[i], 0x00, sizeof(processpool->Name[i]));
		processpool->Pid[i]    = -1;
/*		processpool->VanGrp[i] = -1; */
		processpool->Status[i] = UN_INITIAL;
		processpool->UpDown[i] = UP;
		if ( i < MAX_TLFLOGPOOL_CNT ){
			processpool->tlflog_Seqno[i] = -1;
		}
	}
}

int Init_Shm()
{
	char	*func = "Init_Shm";
	char	fifo_name[20];
	
	int i, rc, ret;

	s.val = 1;
	pool_semid = semget(Pool_SemKey, 1, QPERM | IPC_CREAT | IPC_EXCL);
	if ( pool_semid < 0){
		LOGDEBUG(0, "[%s] ERROR semget Pool_SemKey=%d, errno=%d, err=%s", func, Pool_SemKey, errno, strerror(errno));
		pool_semid = semget(Pool_SemKey, 1, 0);
		if (pool_semid < 0) {
			LOGDEBUG(0, "[%s] ERROR semget2 Pool_SemKey=%d, errno=%d, err=%s", func, Pool_SemKey, errno, strerror(errno));
			/* return -1; */
		}
	}
	else{
		rc = semctl( pool_semid, 0, SETVAL, s);
		if ( rc < 0) {
			LOGDEBUG(0, "[%s] ERROR semctl Pool_SemKey=%d, errno=%d, err=%s", func, Pool_SemKey, errno, strerror(errno));
			return -1;
		}
	}
	
	processpool_shmid = shmget( ProcessPool_ShmKey, sizeof( PROCESSPOOL), QPERM);
	if (processpool_shmid < 0){
		LOGDEBUG(0, "[%s] ERROR shmget ProcessPool_ShmKey=%d ", func, ProcessPool_ShmKey);
		processpool_shmid = shmget( ProcessPool_ShmKey, sizeof( PROCESSPOOL), IPC_CREAT | QPERM);
		if (processpool_shmid < 0){
			LOGDEBUG(0, "[%s] ERROR shmget ProcessPool_ShmKey=%d ", func, ProcessPool_ShmKey);
			return -1;
		}

		processpool = (PROCESSPOOL*) shmat (processpool_shmid, (void*)0, 0);
		if (processpool == (PROCESSPOOL*)-1) {
			LOGDEBUG(0, "[%s] ERROR shmat ProcessPool_ShmKey=%d", func, ProcessPool_ShmKey);
			return -1;
		}

		Init_PROCESSPOOL();
	}
	else{
		processpool = (PROCESSPOOL*) shmat (processpool_shmid, (void*)0, 0);
	}	

	return 0;
}

int RUN_PROCESS(char *p_name, int cnt, int p_gubun)
{
	pid_t		pid;
	char		tmpStr[52];
	char		*func = "RUN_PROCESS";
	int 		i;
	int 		*pool_cnt;
	int			ret;

	LOGDEBUG( 5 , "[%s] CNT[%d] NAME[%s]" , func , cnt , p_name );
	for(i = 0; i < cnt ; i++)
	{
		pid = fork();
		if (pid == 0){
			sprintf(tmpStr, "%s%s", "./", p_name);
			LOGDEBUG( 5 , "[%s] NAME[%s]" , func , tmpStr );
			if (!FileExists(tmpStr)) {
				printf("process name [%s] is not exist\n ", tmpStr );
				exit(0);
			}
			ret		= execl(tmpStr, p_name, NULL);
			if (ret < 0){
				LOGDEBUG(0, "[%s] ERROR execl run failed (%s) (%d) (%s)", func, p_name , errno , strerror(errno ) );
			}
			exit(0);
		}
	}
		
	return 0;
}

int RESTART_PROCESS(void)
{
	int 	i, pid, rc;
	int		result = 0;
	char	*func = "RESTART_PROCESS";

	for(i = 0 ; i < MAX_PROCESSPOOL_CNT; i++)
	{
		pid = processpool->Pid[i];
		if (pid < 0)
			continue;

		processpool->UpDown[i] = DOWN;
		rc = kill(pid, SIGTERM);
		if (rc == -1){
			LOGDEBUG(0, "[%s] ERROR process kill failed (%d)(%s)", func, pid, processpool->Name[i]);
			result = -1;
		}
		LOGDEBUG(3, "[%s] process kill (%d)(%s)", func, pid, processpool->Name[i]);
	}

	Init_VANGRP();
	Init_PROCESSPOOL();
	Read_Process_Info();

	return result;
}


int KILL_PROCESS(char *process_name)
{
	int 	i, pid, rc;
	int 	result = 0;
	char	*func = "KILL_PROCESS";

	for(i = 0 ; i < MAX_PROCESSPOOL_CNT; i++)
	{
		if (strcmp(processpool->Name[i], process_name) != 0)
			continue;

		pid = processpool->Pid[i];
		processpool->UpDown[i] = DOWN;
		rc = kill(pid, SIGTERM);
		if(rc == -1){
			LOGDEBUG(0, "[%s] process kill failed(%d)(%s)", func, pid, process_name);
			result = -1;
		}
		LOGDEBUG(3, "[%s] process kill (%d)(%s)", func, pid, process_name);
	}

	return result;
}

int KILL_ALL(void)
{
	int		i, pid, rc;
	int		result = 0;
	char	*func = "KILL_ALL";

	for(i = 0; i < MAX_PROCESSPOOL_CNT; i++)
	{
		pid = processpool->Pid[i];
		if (pid < 0)
			continue;
		
		processpool->UpDown[i] = DOWN;
		rc = kill(pid, SIGTERM);
		if (rc == -1){
			LOGDEBUG(0, "[%s] process kill failed i(%d)(%d)(%s)", func, i, pid, processpool->Name[i]);
			result = -1;
		}
		LOGDEBUG(3, "[%s] process kill i(%d)(%d)(%s)", func, i, pid, processpool->Name[i]);
	}
	
	rc = shmctl(processpool_shmid, IPC_RMID, (struct shmid_ds*) 0);
	if ( rc < 0){
		LOGDEBUG(0, "[%s] shmctl failed processpool_shmid", func);
			result = -1;
	}

	rc = semctl(pool_semid, 0, IPC_RMID, 0);
	if( rc < 0){
		LOGDEBUG(0, "[%s] semctl failed pool_semid", func);
		result = -1;
	}
	
	rc = semctl(pcc_semid, 0, IPC_RMID, 0);
	if( rc < 0){
		LOGDEBUG(0, "[%s] semctl failed pcc_semid", func);
		result = -1;
	}

	return result;

}

int Read_Process_Info()
{
	char	process_name[50];
	char	ip[20];
	int		port;
	int		cnt;
	FILE	*fp;
	int 	rc;
	char	buf[1024];
	int		flag = 0;
	int		len;
	int 	p_gubun;
	int		pcc_flag = 0;
	char	*func = "Read_Process_Info";

	fp = fopen("./process_info", "r");
	if(fp == NULL){
		return -2;
	}
	while(!feof(fp))
	{
		memset(buf, 0x00, sizeof(buf));
		if (!fgets(buf, 1024, fp)){
			break;
		}

		if (!flag){
			if(buf[0] != '['){
				continue;
			}
			
			len = strlen(buf+1);
			memset(process_name, 0x00, sizeof(process_name));
			strncpy(process_name, buf+1, len - 2 );
			if (strncmp( process_name, "PCC", 3 ) == 0 ){
				pcc_flag++;
			}
			flag++;
			LOGDEBUG(3, "[%s] process_name (%s)",func, process_name);
			continue;
		}
		else{
			if(buf[0] == '['){
				LOGDEBUG(1,"[%s] P_GUBUN read failed", func);
				return -1;
			}

			rc = strncmp(buf, "CNT", 3);
			if (rc == 0){
				cnt = atoi(buf + 4);
				flag = 1;
			}

			rc = strncmp(buf, "TC_TIMEOUT", 10);
			if( rc == 0 && pcc_flag){
				tc_timeout = atoi(buf + 11 );
				pcc_flag = 0;
				flag = 1;
			}

			rc = strncmp(buf, "P_GUBUN", 7);
			if(rc == 0){
				p_gubun = atoi(buf + 8);
				LOGDEBUG( 5 , "[%s] P_GUBUN[%d] NAME[%s]" , func , p_gubun , process_name );

				switch(p_gubun)
				{
					case 	0:
						rc = RUN_PROCESS(process_name, 1, p_gubun);
						if ( rc < 0){
							LOGDEBUG(0, "[%s] run process failed name(%s)",func, process_name);
						}
						break;
					case 	1:
						rc = RUN_PROCESS(process_name, cnt, p_gubun);
						if (rc < 0){
							LOGDEBUG(0, "[%s] run process failed name(%s)",func, process_name);
						}
						break;
					case	2:
						rc = RUN_PROCESS(process_name, cnt, p_gubun);
						if(rc < 0){
							LOGDEBUG(0, "[%s] run process failed name(%s)", func, process_name);
						}
						break;
					default:
						LOGDEBUG(0, "[%s] is not p_gubun(%d)", func, p_gubun);
						break;
				}
				flag = 0;
			}
		}
	}
	fclose(fp);
	return 0;
}

void Process_StartDown(char option, char *process_name)
{
	char	r_buff[200], s_buff[200];
	int 	sockfd, ret, rc;
	char	*func = "Process_StartDown";
	char	tmpStr[200];
	fd_set	rset, allset;
	struct	timeval	tm;
	
	struct sockaddr_in	sin;

	memset(&sin, 0, sizeof(sin));

	sin.sin_family		= AF_INET;
	sin.sin_addr.s_addr	= inet_addr("127.0.0.1");
	sin.sin_port		= htons( ServerPort);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){
		LOGDEBUG(0, "[%s] sockfd create failed(%s)", func, process_name);
		printf("process run failed\n");
		return;
	}
	
	ret = connect (sockfd, (struct sockaddr*) &sin, sizeof(sin));
	if ( ret < 0){
		LOGDEBUG(0, "[%s] PCC가 실행되어있지 않습니다.", func);
		printf("프로세스 제어 프로그램이 실행되어있지 않습니다.\n");
		printf("프로세스 제어 프로그램을 먼저 실행하세요.\n");
		return;
	}
	LOGDEBUG( 3 , "[%s] CONNECT OK [%d]" , func , ServerPort  );
	
	memset(r_buff, 0x00, sizeof(r_buff));
	memset(s_buff, 0x00, sizeof(s_buff));

	sprintf(s_buff, "FSTX%c%02d%s", option , strlen(process_name), process_name);
	ret = send (sockfd, s_buff, sizeof(s_buff), 0);
	if ( ret < 0){
		LOGDEBUG(0, "[%s] send failed(%s)", func, process_name);
		printf("process run failed\n", process_name);
		close(sockfd);
		return;
	}
	LOGDEBUG( 3 , "[%s] send DATA[%s]" , func , s_buff );

	FD_ZERO(&allset);
	FD_SET(sockfd, &allset);
	rset = allset;
	tm.tv_sec = 10;
	tm.tv_usec = 0;
	
	rc = select(sockfd +1 , &rset, NULL, NULL, &tm);
	if (rc < 0){
		LOGDEBUG(0, "[%s] select faied", func);
		close(sockfd);
		return;
	}
	else if( rc == 0){
		LOGDEBUG(3, "[%s] timeout!!", func);
		printf("timeout!!\n");
		close(sockfd);
		return;
	}
	
	ret = recv(sockfd, r_buff, sizeof(r_buff), 0);
	if( ret < 0){
		LOGDEBUG(0, "[%s] recv failed(%s)", func, process_name);
		printf("process run failed\n", process_name);
		close(sockfd);
		return;
	}
LOGDEBUG(3, "[%s] recv_OK(%s)", func, r_buff);		
	ret = strncmp( r_buff + 4, "0", 1);
	if (ret == 0){
		LOGDEBUG(1, "[%s] process run ok(%s)", func, process_name);
		printf("process run ok\n");
		close(sockfd);
		return;
	}

	ret = strncmp( r_buff + 4, "9", 1);
	if (ret == 0){
		memset(tmpStr, 0x00, sizeof(tmpStr));
		strcpy(tmpStr, r_buff+ 5) ;
		LOGDEBUG(0, "[%s] process run failed(%s)errmsg(%s)",func, process_name, tmpStr);
		printf("process failed (%s)\n", tmpStr);
		close(sockfd);
		return;
	}

	close(sockfd);
}

int		Open_Server_Socket(int port)
{
	int 	sockfd, rc;
	int 	reuse = 1;
	char	*func = "Open_Server_Socket";

	struct sockaddr_in	server;

	server.sin_family		= AF_INET;
	server.sin_addr.s_addr	= INADDR_ANY;
	server.sin_port			= htons(port);

	if( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
	{
		LOGDEBUG(0, "[%s] socket create failed", func);
		return -1;
	}

	for(;;){
		rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
		if ( rc == - 1){
			LOGDEBUG(0, "[%s] setsockopt failed", func);
		}
		rc = bind(sockfd, (struct sockaddr*)&server , sizeof(server));
		if( rc == -1)
		{
			LOGDEBUG(0, "[%s] bind create failed", func);
			sleep(1);
			continue;
		}
		break;
	}

	if(listen(sockfd, 5) == -1)
	{
		LOGDEBUG(0, "[%s] listen create failed", func);
		return -3;
	}

	return sockfd;
}

int		IsExist_PCC(void)
{
	int		rc;
	char	*func = "IsExist_PCC";

	s.val = 1;
	pcc_semid = semget(Pcc_SemKey, 1, QPERM | IPC_CREAT | IPC_EXCL);
	if ( pcc_semid < 0){
		pcc_semid = semget(Pcc_SemKey, 1, 0);
	}
	else{
		rc = semctl( pcc_semid, 0, SETVAL, s);
		if( rc < 0 ){
			LOGDEBUG(0, "[%s] semctl failed", func);
		}
	}

	rc = sem_p(pcc_semid, 0);
	if ( rc == -2 ){
		return -1;
	}

	return 0;
}
int	main(int argc, char *argv[])
{
	int 	rc, i;
	char	*func = "pcc_main";
	pid_t	pid;
	char	option;
	char	process_name[20];
	char    nice_file[1024];
	
 	if (access(INIFILE_NAME, R_OK)) {
 		printf(" %s access fail\n", INIFILE_NAME);
 		exit(-1);
 	}	
	
 	SetLogFromIniFile(INIFILE_NAME, PROGRAM_NAME);
 	GetLogInfo();
	
	ServerPort = ReadInteger(PROGRAM_NAME, "ServerPort", ServerPort, INIFILE_NAME);

	ProcessPool_ShmKey = (key_t) ReadInteger(PROGRAM_NAME, "ProcessPool_ShmKey", (int)ProcessPool_ShmKey, INIFILE_NAME);
	Pool_SemKey        = (key_t) ReadInteger(PROGRAM_NAME, "Pool_SemKey", (int)Pool_SemKey, INIFILE_NAME);
	Pcc_SemKey        = (key_t) ReadInteger(PROGRAM_NAME, "Pcc_SemKey", (int)Pcc_SemKey, INIFILE_NAME);
	
	if (argc > 1)
	{
		//rc = Init_Shm();
		
		option = toupper(*(argv[1] + 1));
		
		if (option == 'A' || option == 'R')
			Process_StartDown(option, "");
		else if (option == 'S' || option == 'D')
		{
			for (i = 0 ; i < argc -2; i++)
			{
				memset(process_name, 0x00, sizeof(process_name));
				strcpy(process_name, argv[i + 2]);
				Process_StartDown(option, process_name);
			}
		}
		
		exit(1);
	}
	
	
	pid = fork();
	if (pid > 0) exit(1);	
	
	
	rc = IsExist_PCC();
	if ( rc < 0 ){
		printf("이미 실행중입니다!! \n");
		exit(1);
	}
	
	LOGDEBUG(1, "[%s] start pcc", func);

	rc = Init_Shm();
	if ( rc < 0 ){
		printf("초기화를 실패하였습니다!! \n");
		exit(1);
	}
	memset(nice_file, 0x00, sizeof(nice_file));
	sprintf(nice_file, "%s", "van.nice");
	unlink(nice_file);
	
	rc = Read_Process_Info();
	LOGDEBUG(5, "[%s] READY PROCESS", func);
	
	printf("PCC 실행완료!! \n\n");
	
	signal(SIGCHLD, sig_chld);	
	
	listenfd = Open_Server_Socket(ServerPort);
	LOGDEBUG(1, "[%s] Open_Server_Socket Port=%ld", func, ServerPort);
	if (listenfd < 0){
		LOGDEBUG(0, "[%s] Open_Server_Socket failed", func);
		return -1;
	}
	LOGDEBUG(3, "[%s] open socket(%d)", func, listenfd);	
	rc = loop(listenfd);
	if ( rc < 0){
		LOGDEBUG(0, "[%s] loop failed", func);
		close(listenfd);
		return -1;
	}
	if ( rc == 1){
		LOGDEBUG(1, "[%s] I die!!", func);
		close(listenfd);
		exit(0);
	}
}

int	Run_UpDown_Process(char *r_buff, char *s_buff)
{
	char	option;
	char	process_name[20];
	int		rc, len;
	char	lenStr[3];
	
	char	*func = "Run_UpDown_Process";

	rc = strncmp(r_buff, "FSTX", 4);
	if (rc != 0){
		LOGDEBUG(0, "[%s] r_buff FSTX failed(%s)", func, r_buff);
		strcpy(s_buff, "FSTX9FSTX isnot start");
		return -1;
	}
	
	option = *(r_buff + 4);
	memset(lenStr, 0x00, sizeof(lenStr));
	strncpy(lenStr, r_buff + 5, 2);
	len = atoi(lenStr);
	strncpy(process_name, r_buff + 7, len);
	process_name[len] = 0x0;
	LOGDEBUG(3, "[%s] process option(%c) name(%s) len(%d)", func, option, process_name, len);

	if (option == 'S' ){
		rc = RUN_PROCESS(process_name, 1, 0);
		if ( rc < 0){
			LOGDEBUG(0, "[%s] run process failed(%s)", func, process_name);
			sprintf(s_buff, "FSTX%01d%s(%s)", 9, "run process failed", process_name);
			return -1;
		}
		LOGDEBUG(3, "[%s] run process ok(%s)", func, process_name);
		sprintf(s_buff, "FSTX%01d%s(%s)", 0, "run process ok", process_name);
		return 0;
	}

	if (option == 'D' ){
		rc = KILL_PROCESS(process_name);
		if ( rc < 0){
			LOGDEBUG(0, "[%s] kill process failed(%s)", func, process_name);
			sprintf(s_buff, "FSTX%01d%s(%s)", 9, "kill process failed", process_name);
			return -1;
		}
		LOGDEBUG(3, "[%s] kill process ok(%s)", func, process_name);
		sprintf(s_buff, "FSTX%01d%s(%s)", 0, "kill process ok", process_name);
		return 0;
	}
	
	if (option == 'A' ){
		rc = KILL_ALL();
		if ( rc < 0){
			LOGDEBUG(0, "[%s] kill all failed", func);
			sprintf(s_buff, "FSTX%01d%s", 9, "kill all failed");
			return KILL_ALL_RETURN_VALUE;
		}
		LOGDEBUG(3, "[%s] kill all ok", func);
		sprintf(s_buff, "FSTX%01d%s", 0, "kill all ok");
		return KILL_ALL_RETURN_VALUE;
	}
	
	if (option == 'R' ){
		rc = RESTART_PROCESS();
		if ( rc < 0){
			LOGDEBUG(0, "[%s] restart process failed", func);
			sprintf(s_buff, "FSTX%01d%s(%s)", 9, "restart process failed");
			return -1;
		}
		LOGDEBUG(3, "[%s] restart process ok", func);
		sprintf(s_buff, "FSTX%01d%s", 0, "restart process ok");
		return 0;
	}

	LOGDEBUG(0, "[%s] not found option(%c)(%s)", func, option, process_name);
	sprintf(s_buff, "FSTX%01d%s(%c)", 9, "not found option", option);
	return -1;
}

void 	ProcessTimer()
{
	char	*func = "ProcessTimer";

	int 	i, rc;
	struct	q_entry		t_entry;
/*	INNERJUNMUNDATA		injunmun;
	/*
	int		vanpool_id;
	*/
/*	
	for( i = 0 ; i < MAX_VANGRP_CNT ; i++)
	{
		if( vangrp->Status[i]	==  DISCONNECT ){
		/*	case of signal */
		/*	vanpool_id = vangrp->Id[i] * VANPOOL_LIMIT + MAX_PROSINGLE_CNT;
/*LOGDEBUG(0, "[%s] vanpool_id(%d)", func, vanpool_id);
/*			rc = kill(processpool->Pid[vanpool_id], SIGUSR2);
/*			if ( rc == -1 ){
/*				LOGDEBUG(1,"[%s] signal testcall failed", func);
/*				processpool->Status[vanpool_id] = MARK_DOWN;
/*			}
/* 
			
LOGDEBUG(0, "[%s] vangrp_id(%d)", func, vangrp->Id[i]);
			memset ( &t_entry, 0x00, sizeof(struct q_entry) );
			t_entry.pid = 999999;
			t_entry.vangrp_id = vangrp->Id[i];
			t_entry.pool_id = 0;
			t_entry.seq_no  = 0;
			t_entry.pipeuse_yn = 'N';
			memset( &injunmun, 0x00, sizeof(INNERJUNMUNDATA) );
			memcpy(injunmun.req_gubun, "99" , 2 );

			NulltoSpace( (char*)&injunmun, sizeof(injunmun) - 1 );

			strncpy(t_entry.mtext, (char*)&injunmun, sizeof(injunmun) );

			rc = msgsnd(req_qid, &t_entry, sizeof(struct q_entry), IPC_NOWAIT );
			if ( rc == -1 ){
				LOGDEBUG(3, "[%s] msgsnd failed to REQ_Q for TESTCALL", func);
			}
				LOGDEBUG(3, "[%s] msgsnd OK to REQ_Q for TESTCALL", func);
			
		}
		
	}

	if ( vangrp->Status[ MAX_VANGRP_CNT ] != CONNECT )
	{
		memset( &injunmun, 0x00, sizeof(INNERJUNMUNDATA) );
		memcpy(injunmun.req_gubun, "98" , 2 );

		NulltoSpace( (char*)&injunmun, sizeof(injunmun) - 1 );
		
		rc = msgsnd(tlfbroker_qid, (char*)&injunmun, sizeof(INNERJUNMUNDATA), IPC_NOWAIT );
		if ( rc == -1 ){
			LOGDEBUG(0, "[%s] msgsnd failed to TLFBROKER_Q for RECONNECT", func );
		}
		LOGDEBUG(0, "[%s] msgsnd OK to TLFBROKER_Q for RECONNECT", func );
	}
*/	
}

int	loop(int listenfd)
{
	char	*func = "loop";
	fd_set	rset, allset;
	int		nread, rc;
	int		clifd;
	char	cli_ip[100];
	char	r_buff[MAX_LEN];
	char	s_buff[MAX_LEN];
	int		flag = 0;
	struct	timeval		tv;

	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	for(;;)
	{
	/*	FD_ZERO(&allset);
		FD_SET(listenfd, &allset);*/

		tv.tv_sec 	= tc_timeout;
		tv.tv_usec 	= 0;
		
		rset = allset;
		rc = select(listenfd + 1, &rset, NULL, NULL, &tv);
		if (rc < 0){
			if (errno == EINTR){
LOGDEBUG(1, "[%s]", func);
				continue;
			}
			LOGDEBUG(0, "[%s] select error(%s)(%d)", func, strerror(errno), errno);
			return -1;
		}

		if( rc == 0){
			LOGDEBUG(7,"[%s] select timeout", func);
			ProcessTimer();

			continue;
		}

		if (FD_ISSET(listenfd, &rset)){
			memset(cli_ip, 0x00, sizeof(cli_ip));
			clifd = net_open_cli( listenfd, cli_ip);
			if(clifd == -1){
				continue;
			}
			if (checkBlackList(cli_ip) == 0) {
				close(clifd);
				continue;
			}
			
			memset( r_buff, 0x00, sizeof(r_buff));
			nread = recv(clifd, r_buff, MAX_LEN, 0);
			if (nread <= 0){
				LOGDEBUG(0, "[%s] read failed(%d)", func, clifd);
				//FD_CLR(clifd, &allset);
				strcpy(s_buff, "FSTX9recv failed");
			}
			else{
				LOGDEBUG(3, "[%s] RecvData(%s)", func, r_buff);
				
				rc = Run_UpDown_Process(r_buff, s_buff);
				if( rc < 0){
					LOGDEBUG(0, "[%s] Run_Process falied", func);
				}
				if ( rc == KILL_ALL_RETURN_VALUE ) {
					LOGDEBUG( 5 , "[%s] ALL Process Down" , func );
					flag = 1;
				}
			}

			rc = send(clifd, s_buff, strlen(s_buff), 0);
			if ( rc < 0){
				LOGDEBUG(0, "[%s] send failed (%d)(%s)", func, clifd, s_buff);
				close(clifd);
				continue;
			}
			LOGDEBUG(3, "[%s] send ok (%d)(%s)", func, clifd, s_buff);
			close(clifd);
			if ( flag == 1){
				close(listenfd);
				return 1;
			}
		}
	}
}

int AGAIN_RUN_PROCESS( int p_pool_id )
{
	int 		i;
	pid_t		pid;
	char		*func = "AGAIN_RUN_PROCESS";
	char		tmpStr[52];
	char		processname[20];

	strcpy(processname, processpool->Name[p_pool_id]);
	
	processpool->Pid[p_pool_id] = -1;
	memset(processpool->Name[p_pool_id], 0x00, sizeof(processpool->Name[p_pool_id]));
	
	if(processpool->UpDown[p_pool_id] == UP){
		pid = fork();
		if (pid == 0){
				sprintf(tmpStr, "%s%s", "./", processname);
				execl(tmpStr, processname, NULL);
				LOGDEBUG(0, "[%s] execl run failed (%s)", func, processname);
				exit (0);
		}
		LOGDEBUG(3, "[%s] again execl process run ok ", func);

	}
	
	/*
	memset(processpool->Name[p_pool_id], 0x00, sizeof(processpool->Name[p_pool_id]));
	*/

	return 0;
}

int AGAIN_RUN_VAN(int p_pool_id)
{
	int		i;
	pid_t		pid;
	char	*func = "AGAIN_RUN_VAN";
	char	processname[20];
	char	tmpStr[52];

	strcpy(processname, processpool->Name[p_pool_id]);
	
	processpool->Status[p_pool_id] = MARK_DOWN;
	processpool->Pid[p_pool_id] = -1;
	memset(processpool->Name[p_pool_id], 0x00, sizeof(processpool->Name[p_pool_id]));
	
	if (processpool->UpDown[p_pool_id] == UP){
		pid = fork();
		if (pid == 0){
				sprintf(tmpStr, "%s%s", "./", processname);
				execl(tmpStr, processname, NULL);
				LOGDEBUG(0, "[%s] execl run failed (%s)", func, processname);
				exit(0);
		}
		LOGDEBUG(3, "[%s] again execl van run ok (%d)", func, p_pool_id);
	}

	
	return 0;
}

void sig_chld(int signo)
{
	pid_t		pid;
	int 		rc, stat;
	char		*func = "sig_chld";
	int 		pcc_pool_id, i;

	LOGDEBUG(3, "[%s] catch SIGCHLD siganl", func);

	while(( pid = waitpid(-1, &stat, WNOHANG)) > 0){
		LOGDEBUG( 4 , "[%s] CHILD PROCESS DOWN(%d)" , func, pid );
		for(i = 0 ; i < MAX_PROCESSPOOL_CNT ; i++){
			if (processpool->Pid[i] == pid){
				pcc_pool_id = i;
				break;
			}
		}

		if( i == MAX_PROCESSPOOL_CNT ) {
			signal(SIGCHLD, sig_chld);
			continue;
		}

		LOGDEBUG( 4 , "[%s] CHILD ID[%d] PID[%d] NAME [%s] " , 
			func ,  pcc_pool_id ,  processpool->Pid[pcc_pool_id] ,processpool->Name[pcc_pool_id]);
		
		if (pcc_pool_id < MAX_PROSINGLE_CNT){
			rc = AGAIN_RUN_PROCESS( pcc_pool_id );
			if ( rc < 0){
				LOGDEBUG(0, "[%s] run process failed name(%d)",func, pid);
			}
		}/* 단독으로 실행 */
		else if (pcc_pool_id >= MAX_PROSINGLE_CNT && pcc_pool_id < MAX_PROCESSPOOL_CNT) {
			rc = AGAIN_RUN_VAN( pcc_pool_id );
			if ( rc < 0){
				LOGDEBUG(0, "[%s] run van failed name(%d)",func, pid);
			}/* Pool에서 찾아서 실행 */
		}
	}
	
	signal(SIGCHLD, sig_chld);
}
			
			
