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
#include <fcntl.h>

int main(int argc, char **argv)
{
	int server_sockfd, client_sockfd;
	int newSockStat;
	int orgSockStat;
	int client_len, n;
	int state = 0;
	int ret = 0;
	char buf[80];
	struct sockaddr_in clientaddr, serveraddr;
	
	// select time out 설저을 위한 timeval 구조체 
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

	if ( (newSockStat = fcntl(server_sockfd, F_GETFL, NULL)) < 0 )
	{
		perror("F_GETFL error");
		return -1;
	}

	orgSockStat = newSockStat;
	newSockStat |= O_NONBLOCK;

	// Non blocking 상태로 만든다.  
	if(fcntl(server_sockfd, F_SETFL, newSockStat) < 0)
	{
	    perror("F_SETLF error");
	    return -1;
	}
	
	// server_sockfd 의 입력검사를 위해서 fd_set 에 등록한다. 
//	FD_ZERO(&readfds);
//	FD_SET(server_sockfd, &readfds);
	// 약 5초간 기다린다.
//	tv.tv_sec = 5;
//	tv.tv_usec = 10;

	int i = 1;
	while(1)
	{
		memset(buf, 0x00, 80);
		
		// 비봉쇄 소켓을 변경했기 때문에 바로 리턴함
		// 바로 리턴하기 때문에 client_sockfd 는 -1이다.
		// 그래서 client_sockfd 를 FD에 등록하면 에러가 발생
		client_sockfd = accept(server_sockfd, (struct sockaddr *)&clientaddr,
				&client_len);
		if (client_sockfd < 0) continue;

		printf("client(%d)가 입장합니다.\n", i);
		i++;

		// 연결이 있으면 아래 로직 
		// client_sockfd 의 입력검사를 위해서 fd_set 에 등록한다. 
		FD_ZERO(&readfds);
		FD_SET(client_sockfd, &readfds);
		// 약 5초간 기다린다.
		tv.tv_sec = 0;
		tv.tv_usec = 10;

		// select() 함수는 소켓 set에 포함된 
		// 소켓이 입력을 위한 준비가 될 때가지 대기한다.
		// timeout 시간만큼
		// select 되고 나면 FD_ZERO, FD_SET 을 다시 등록해줘야 함
		ret = select(client_sockfd+1, &readfds, (fd_set *)0, (fd_set *)0, &tv);
		//ret = select(server_sockfd+1, &readfds, (fd_set *)0, (fd_set *)0, NULL); //무한정 대기
		if (ret < 0)
		{
			return -1;
		}
		// 읽은 데이터가 있는지 검사한다. 
		ret = FD_ISSET(client_sockfd, &readfds);
		if (ret < 0)
		{
			return -1;
		}
		
		state = ret;
		switch (state)
		{
			case -1:
				perror("select error :");
                exit(0);
            // 만약 5초안에 아무런 입력이 없었다면 Time out 발생상황이다. 
            case 0:
                printf("Time out error\n");
                break;
			// 5초안에 입력이 들어왔을경우 처리한다. 
			default:
				if ((n = read(client_sockfd, buf, 80)) <= 0)
				{
					perror("read error : ");
					usleep(10);
					break;
				}
				printf("read(%s)\n", buf);
				if (write(client_sockfd, buf, 80) <=0)
				{
					perror("write error : ");
					break;
				}
				break;
		}
		close(client_sockfd);
	}
	close(server_sockfd);
}

