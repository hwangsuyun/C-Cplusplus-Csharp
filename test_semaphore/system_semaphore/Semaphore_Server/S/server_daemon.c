#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int server_daemon(int port)
{
	char *__fn = "server_daemon";
	int listenFd;
	int nsockFd;
	char buff[100];
	
	listenFd = Open_Server_Socket(port);
	if(listenFd < 0)
	{
		dbg(9, "ERR: Open_Server_Socket failed. port(%d) <%s>", port, __fn);
		exit(9);
	}
	dbg(6, "OK: Open_Server_Socket. port(%d). <%s>", port, __fn);

	while(1)
	{
		memset(buff, 0x00, sizeof(buff));

		dbg(2, "CALL: sock_accept. <%s>", __fn);
		nsockFd = sock_accept(listenFd, buff);
		if (nsockFd < 0 ) {
			dbg(9, "ERR: Socket Accept Fail(%d) <%s>", nsockFd, __fn);
			continue;
		}

		if (fork() == 0) 
		{
			dbg(9, "Client %s CONNECTED. <%s>", buff, __fn);
			close(listenFd);
			break;
		}	
		close(nsockFd);
	}

	server_main(nsockFd);

	close(nsockFd);
	exit(0);
}
