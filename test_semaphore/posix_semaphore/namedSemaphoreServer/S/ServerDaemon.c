#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int startServer(int port)
{
	char *__fn = "startServer";
	int listenFd;
	int nsockFd;
	char buff[100];
	
	// Open_Server_Socket(port) 에서 하는 일
	// 1. 소켓을 TCP/IP 모드로 생성한다.
	// 2. 프로세스 비정상 종료시에, 바로 다시 bind이 가능하도록 설정한다.
	// 3. 소켓을 커널에 등록하여, 외부와 송수신이 가능한 상태로 만든다.(여기서 port가 설정된 구조체를 할당한다.)
	// 4. 클라이언트 접속 요청을 확인한다.(클라이언트의 연결을 기다린다.)
	listenFd = Open_Server_Socket(port);
	if(listenFd < 0) {
		dbg(9, "ERR: Open_Server_Socket failed. port(%d) <%s>", port, __fn);
		exit(9);							// 이 경우 wait 하고 있는 부모 프로세스에 9를 전달한다.
	}
	dbg(6, "OK: Open_Server_Socket. port(%d). <%s>", port, __fn);

	while(1) {
		memset(buff, 0x00, sizeof(buff));

		dbg(2, "CALL: sock_accept. <%s>", __fn);
		// 클라이언트 접속 요청에 따라 접속을 허락한다.
		nsockFd = sock_accept(listenFd, buff);
		if (nsockFd < 0 ) {
			dbg(9, "ERR: Socket Accept Fail(%d) <%s>", nsockFd, __fn);
			continue;
		}

		// 자식은 listenFd를 닫고 다음 로직을 진행한다.
		if (fork() == 0) {
			dbg(9, "Client %s CONNECTED. <%s>", buff, __fn);
			close(listenFd);
			break;
		}	
		// 부모의 소켓을 닫기
		close(nsockFd);
	}

	serverMain(nsockFd);

	// 자식의 소켓을 닫기
	close(nsockFd);
	exit(0);
}
