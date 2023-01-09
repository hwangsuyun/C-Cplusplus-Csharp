#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	struct sockaddr_in serveraddr;
	int server_sockfd;
	int client_len;
	char buf[80];
	char rbuf[80];


	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("error :");
		exit(0);
	}

	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveraddr.sin_port = htons(atoi("21000"));

	client_len = sizeof(serveraddr);

	if (connect(server_sockfd, (struct sockaddr *)&serveraddr, client_len) < 0)
	{
		perror("connect error :");
		exit(0);
	}

	memset(buf, 0x00, 80);
	read(0, buf, 80);
	if (write(server_sockfd, buf, 80) <= 0)
	{
		perror("write error : ");
		exit(0);
	}
	memset(buf, 0x00, 80);
	if (read(server_sockfd, buf, 80) <= 0)
	{
		perror("read error : ");
		exit(0);
	}
	close(server_sockfd);
	printf("read : %s", buf);
}

