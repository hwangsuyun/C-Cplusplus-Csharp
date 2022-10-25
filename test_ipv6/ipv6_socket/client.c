#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
	struct sockaddr_in6 svrsin6;
	struct hostent *hp;
	char ipv6_addr[16];
	char addr6_str[40];
	char buf[256];

	int sockfd = 0;
	int clilen = 0;

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

	svrsin6.sin6_family = AF_INET6;
	svrsin6.sin6_flowinfo = 0;
	svrsin6.sin6_port = htons(atoi(argv[1]));

	//inet_pton(AF_INET6, "fe80::f84d:caff:fe51:12e9/64", (void *)&ipv6_addr);
	inet_pton(AF_INET6, "::1", (void *)&ipv6_addr);
	// in6addr_loopback 는 loopback 주소로 연결하기 위해서 사용한다.
	// ipv6에서의 loopback 주소는 "::1" 이다.
	// 루프백주소로 연결하길 원한다면
	// svrsin6.sin6_addr = in6addr_loopback;
	// 혹은
	// inet_pton(AF_INET6, "::1", (void *)&ipv6_addr);
	// 하면 된다.
	memcpy((void *)&svrsin6.sin6_addr, (void *)&ipv6_addr, 16);
	inet_ntop(AF_INET6, (void *)&svrsin6.sin6_addr, addr6_str, 40);

	printf("%s\n", addr6_str);
	printf("connect...\n");

	clilen = sizeof(svrsin6);
	if (connect(sockfd, (struct sockaddr *)&svrsin6, clilen) < 0)
	{
		printf("connect error\n");
		return -1;
	}

	memset(buf, 0x00, sizeof(buf));
	read(0, buf, sizeof(buf));
	write(sockfd, buf, sizeof(buf));
	read(sockfd, buf, sizeof(buf));
	printf("-->%s\n", buf);

	printf("Connect Success\n");
	close(sockfd);

	return 0;
}
