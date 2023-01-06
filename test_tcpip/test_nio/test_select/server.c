#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#define MAXLINE 1024 
#define PORTNUM 21000
#define SOCK_SETSIZE 1021

void nonblock(int sockfd) 
{ 
	int opts; 
	opts = fcntl(sockfd, F_GETFL); 
	if(opts < 0) 
	{ 
		perror("fcntl(F_GETFL)\n"); 
		exit(1); 
	} 
	opts = (opts | O_NONBLOCK); 
	if(fcntl(sockfd, F_SETFL, opts) < 0) 
	{ 
		perror("fcntl(F_SETFL)\n"); 
		exit(1); 
	} 
}

// non blocking server that select function using
int main(int argc, char **argv)
{
	int listen_fd, client_fd;
	socklen_t addrlen;
	int fd_num;
	int maxfd = 0;
	int sockfd;
	int readn;
	int i = 0;
	int flag = 0;
	char buf[MAXLINE];
	fd_set readfds, allfds;
	struct sockaddr_in server_addr, client_addr;

	if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket error");
		return 1;
	}   
	memset((void *)&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORTNUM);
	if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("bind error");
		return 1;
	}   
	if(listen(listen_fd, 5) == -1)
	{
		perror("listen error");
		return 1;
	}  

	// SELECT set ���
	FD_ZERO(&readfds);
	FD_SET(listen_fd, &readfds);
	maxfd = listen_fd;
	while(1)
	{
		// select �ǰ� ���� FD_ZERO, FD_SET �� �ٽ� �������� �ؼ� �̷��� �ٸ� fd ���
		allfds = readfds;
		printf("Select Wait %d\n", maxfd);

		// select ����(blocking state)
		fd_num = select(maxfd + 1 , &allfds, (fd_set *)0,
				(fd_set *)0, NULL);
		if (fd_num < 0) continue;

		// select �б� set�� �����Ǹ�
		if (fd_num > 0 && FD_ISSET(listen_fd, &allfds))
		{
			addrlen = sizeof(client_addr);
			client_fd = accept(listen_fd,
					(struct sockaddr *)&client_addr, &addrlen);

			// ���ο� set ���
			FD_SET(client_fd, &readfds);

			// accept�� ������ client_fd�� ������ ���� (read �� ���ŷ �Ǵ°� ����)
			nonblock(client_fd);

			// select �б� set�� ���������Ƿ� accept �Լ� ȣ��
			if (client_fd > maxfd)
				maxfd = client_fd;
			printf("Accept OK\n");
			continue;
		}

		for (i = 0; i <= maxfd; i++)
		{
			sockfd = i;
			if (FD_ISSET(sockfd, &allfds))
			{
				/*
				if( (readn = read(sockfd, buf, MAXLINE-1)) == 0)
				{
					printf("close\n");
					close(sockfd);
					FD_CLR(sockfd, &readfds);
				}
				else
				{
					buf[readn] = '\0';
					write(sockfd, buf, strlen(buf));
				}
				*/
				while( (readn = read(sockfd, buf, MAXLINE - 1)) > 0 )
				{
					printf("buf(%s)\n", buf);
					write(sockfd, buf, readn);
				}
				if (readn == -1)
				{
					if (errno != EAGAIN)
					{
						close(sockfd);
						FD_CLR(sockfd, &readfds);
					}
				}
				if (--fd_num <= 0) 
				{
					break;
				}
				// accept ���� ���� �� �����Ͱ� ���� ���
				// (Ŭ���̾�Ʈ�� ���� �� ������ ���� ���� ���� ����� ���)
				if (fd_num > 0 && readn == 0) 
				{
					close(sockfd);
					FD_CLR(sockfd, &readfds);
				}
			}
		}
	}
}
