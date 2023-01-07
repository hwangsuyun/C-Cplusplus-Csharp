#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>

// blocking server
int main(int argc, char **argv)
{
	int server_sockfd, client_sockfd;
	int client_len, n;
	char buf[80];
	struct sockaddr_in clientaddr, serveraddr;

	client_len = sizeof(clientaddr);

	if ((server_sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error : ");
		exit(0);
	}
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(atoi("21000"));

	bind (server_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	listen(server_sockfd, 5);

	int i = 1;
	while(1)
	{
		memset(buf, 0x00, 80);
		client_sockfd = accept(server_sockfd, (struct sockaddr *)&clientaddr,
				&client_len);

		printf("client(%d)가 입장합니다.\n", i);
		i++;

		if ((n = read(client_sockfd, buf, 80)) <= 0)
		{
			close(client_sockfd);
			continue;
		}
		if (write(client_sockfd, buf, 80) <=0)
		{
			perror("write error : ");
			close(client_sockfd);
		}
		close(client_sockfd);
	}
}

