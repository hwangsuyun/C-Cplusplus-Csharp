#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char **argv)
{
	struct sockaddr_in6 sin6, clisin6;
	int sockfd = 0;
	int clisockfd = 0;
	int clilen = sizeof(clisin6);
	char buf[256];

	if (argc < 2)
	{
		printf("argv[1] is port number\n");
		return -1;
	}

	sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("socket create error\n");
		return -1;
	}

	// bind 를 위해서 소켓특성을 묶어준다. 
	// IPv6 버전으로 맞춘다.
	sin6.sin6_family = AF_INET6;
	sin6.sin6_flowinfo = 0;
	sin6.sin6_port = htons(atoi(argv[1]));
	// in6addr_any 는 *:를 나타낸다.
	// ipv4 에서의 htonl(INADDR_ANY)와 동일하다고 볼 수 있다.
	sin6.sin6_addr = in6addr_any;

	if (bind(sockfd, (struct sockaddr *)&sin6, sizeof(sin6)) == -1)
	{
		printf("Bind error\n");
		return -1;
	}

	if (listen(sockfd, 5) == -1)
	{
		printf("Listen error\n");
		return -1;
	}

	while (1)
	{
		clisockfd = accept(sockfd, (struct sockaddr *)&clisin6, (socklen_t *)&clilen);
		memset(buf, 0x00, sizeof(buf));
		read(clisockfd, buf, sizeof(buf));
		printf("read : (%s)\n", buf);
		write(clisockfd, buf, sizeof(buf));

		close(clisockfd);
	}

	return 0;
}
