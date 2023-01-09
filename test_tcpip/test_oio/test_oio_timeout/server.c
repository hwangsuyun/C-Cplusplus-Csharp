#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/types.h>

// blocking server that select function using for timeout 
int main(int argc, char **argv)
{
	int server_sockfd, client_sockfd;
	int client_len, n;
	int state = 0;
	char buf[80];
	struct sockaddr_in clientaddr, serveraddr;
	
	// select time out ������ ���� timeval ����ü 
	struct timeval tv;
	fd_set readfds;

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

		printf("client(%d)�� �����մϴ�.\n", i);
		i++;

		// client_sockfd �� �Է°˻縦 ���ؼ� fd_set �� ����Ѵ�. 
        FD_ZERO(&readfds);
        FD_SET(client_sockfd, &readfds);
        // �� 5�ʰ� ��ٸ���. 
        tv.tv_sec = 5;
        tv.tv_usec = 10;

		// �Է��� �ִ��� ��ٸ���. 
		state = select(client_sockfd+1, &readfds, (fd_set *)0, (fd_set *)0, &tv);
		switch (state)
		{
			case -1:
				perror("select error :");
                exit(0);
            // ���� 5�ʾȿ� �ƹ��� �Է��� �����ٸ� Time out �߻���Ȳ�̴�. 
            case 0:
                printf("Time out error\n");
                break;
			// 5�ʾȿ� �Է��� ��������� ó���Ѵ�. 
			default:
				if ((n = read(client_sockfd, buf, 80)) <= 0)
				{
					perror("read error : ");
					usleep(10);
					break;
				}
				if (write(client_sockfd, buf, 80) <=0)
				{
					perror("write error : ");
					break;
				}
				break;
		}
		close(client_sockfd);
	}
}

