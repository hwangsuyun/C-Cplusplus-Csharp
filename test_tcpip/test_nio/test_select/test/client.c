#include <sys/stat.h> 
#include <arpa/inet.h> 
#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <errno.h> 

int ConnectWait(int sockfd, struct sockaddr *saddr, int addrsize, int sec) 
{ 
	int newSockStat; 
	int orgSockStat; 
	int res = 0, n; 
	fd_set  rset, wset; 
	struct timeval tval; 

	int error = 0; 
	int esize; 

	if ( (newSockStat = fcntl(sockfd, F_GETFL, NULL)) < 0 )  
	{ 
		perror("F_GETFL error"); 
		return -1; 
	} 

	orgSockStat = newSockStat; 
	newSockStat |= O_NONBLOCK; 

	// Non blocking ���·� �����.  
	if(fcntl(sockfd, F_SETFL, newSockStat) < 0) 
	{ 
		perror("F_SETLF error"); 
		return -1; 
	} 

	// ������ ��ٸ���. 
	// Non blocking �����̹Ƿ� �ٷ� �����Ѵ�. 
	if((res = connect(sockfd, saddr, addrsize)) < 0) 
	{ 
		if (errno != EINPROGRESS) 
			return -1; 
	} 

	printf("RES : %d\n", res); 
	// ��� ������ �������� ��� ������ ���� ���·� �ǵ����� �����Ѵ�.  
	if (res == 0) 
	{ 
		printf("Connect immediately Success\n"); 
		fcntl(sockfd, F_SETFL, orgSockStat); 
		return 1; 
	} 

	FD_ZERO(&rset); 
	FD_SET(sockfd, &rset); 
	wset = rset; 

	tval.tv_sec        = sec;     
	tval.tv_usec    = 0; 

	if ( (n = select(sockfd+1, &rset, &wset, NULL, NULL)) == 0) 
	{ 
		// timeout 
		errno = ETIMEDOUT;     
		return -1; 
	} 

	// �аų� �� �����Ͱ� �ִ��� �˻��Ѵ�.  
	if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset) ) 
	{ 
		printf("Read data\n"); 
		esize = sizeof(int); 
		if ((n = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&esize)) < 0) 
			return -1; 
	} 
	else 
	{ 
		perror("Socket Not Set"); 
		return -1; 
	} 


	fcntl(sockfd, F_SETFL, orgSockStat); 
	if(error) 
	{ 
		errno = error; 
		perror("Socket"); 
		return -1; 
	} 

	return 1; 
} 

int main(int argc, char **argv) 
{ 
	struct sockaddr_in serveraddr; 
	int sockfd; 
	int len; 

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) 
	{ 
		perror("error"); 
		return 1; 
	} 
	serveraddr.sin_family = AF_INET; 
	serveraddr.sin_family = AF_INET; 
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	serveraddr.sin_port = htons(atoi("21000")); 

	len = sizeof(serveraddr); 

	if (ConnectWait(sockfd, (struct sockaddr *)&serveraddr, len, 5) < 0) 
	{ 
		perror("error"); 
	} 
	else 
	{ 
		printf("Connect Success\n"); 
	} 
	while(1) 
	{ 
		sleep(1); 
		//write(sockfd, "hi", 2);
	} 
	close(sockfd); 
} 
