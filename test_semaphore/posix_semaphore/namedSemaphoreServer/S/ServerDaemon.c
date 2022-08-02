#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int startServer(int port)
{
	char *__fn = "startServer";
	int listenFd;
	int nsockFd;
	char buff[100];
	
	// Open_Server_Socket(port) ���� �ϴ� ��
	// 1. ������ TCP/IP ���� �����Ѵ�.
	// 2. ���μ��� ������ ����ÿ�, �ٷ� �ٽ� bind�� �����ϵ��� �����Ѵ�.
	// 3. ������ Ŀ�ο� ����Ͽ�, �ܺο� �ۼ����� ������ ���·� �����.(���⼭ port�� ������ ����ü�� �Ҵ��Ѵ�.)
	// 4. Ŭ���̾�Ʈ ���� ��û�� Ȯ���Ѵ�.(Ŭ���̾�Ʈ�� ������ ��ٸ���.)
	listenFd = Open_Server_Socket(port);
	if(listenFd < 0) {
		dbg(9, "ERR: Open_Server_Socket failed. port(%d) <%s>", port, __fn);
		exit(9);							// �� ��� wait �ϰ� �ִ� �θ� ���μ����� 9�� �����Ѵ�.
	}
	dbg(6, "OK: Open_Server_Socket. port(%d). <%s>", port, __fn);

	while(1) {
		memset(buff, 0x00, sizeof(buff));

		dbg(2, "CALL: sock_accept. <%s>", __fn);
		// Ŭ���̾�Ʈ ���� ��û�� ���� ������ ����Ѵ�.
		nsockFd = sock_accept(listenFd, buff);
		if (nsockFd < 0 ) {
			dbg(9, "ERR: Socket Accept Fail(%d) <%s>", nsockFd, __fn);
			continue;
		}

		// �ڽ��� listenFd�� �ݰ� ���� ������ �����Ѵ�.
		if (fork() == 0) {
			dbg(9, "Client %s CONNECTED. <%s>", buff, __fn);
			close(listenFd);
			break;
		}	
		// �θ��� ������ �ݱ�
		close(nsockFd);
	}

	serverMain(nsockFd);

	// �ڽ��� ������ �ݱ�
	close(nsockFd);
	exit(0);
}
