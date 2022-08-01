#define	__TF	"tcp_lib.c"

#include		<stdio.h>
#include		<sys/socket.h>
#include		<netinet/in.h>
#include		<arpa/inet.h>
#include		<netdb.h>
#include		<errno.h>
#include		<fcntl.h>
#include		<errno.h>
#include		<signal.h>
#include		<string.h>
#include		<sys/types.h>
#include		<unistd.h>

#include	"lib_fn.h"

int sock_accept(int sockfd, char *cliaddr_buff)
{	   
static char *__fn = "sock_accept";

	struct  sockaddr_in	 cli_addr;
	int	 clilen, clifd;
		
	// accept()로 접속 요청을 허락하게 되면 클라이언트와 통신을 하기 위해서 커널이 자동으로 소켓을 생성
	// 이 소켓을 clifd로 세팅하는 과정임
	clilen = sizeof(cli_addr);
	clifd = accept (sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (clifd < 0 ) {
		return(-1);
	}

	strcpy(cliaddr_buff, inet_ntoa(cli_addr.sin_addr));

	dbg(2, "accept OK. cli_addr(%s). <%s:%s>", cliaddr_buff, __fn, __TF);
	return(clifd);
}

int net_connect_block(char	*serv_h_addr, int	 portno)
{
char	*__fn = "(LIB) net_connect_block";
	int		 sockfd, r;
	struct  sockaddr_in cli_addr;
	unsigned long	   inaddr;
	struct  servent	 *sp;
	struct  hostent	 *hp;


	dbg(1, "START: <%s:%s>", __fn, __TF);

	memset((char *)&cli_addr, 0x0, sizeof(cli_addr));
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_port = htons(portno);
	cli_addr.sin_addr.s_addr = inet_addr(serv_h_addr);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		return(-1);
	}

	r = connect(sockfd, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
	if(r < 0) {
		/*  errno = ?*/
		dbg(2, "ERR: connect(%d). <%s:%s>", r, __fn, __TF);
		return(-1);
	}
	dbg(1, "OK: <%s:%s>", __fn, __TF);

	return(sockfd);
}

int		readSocket( int pipe_fd , char *repStr , int size , int timeout )
{
static char *__fn = "readSocket";

	int					ret;
	fd_set				fd;
	struct	timeval		tv;
	tv.tv_sec		= timeout;
	tv.tv_usec		= 0;

	FD_ZERO( &fd );				// fd의 모든 비트를 지운다.
	FD_SET( pipe_fd , &fd );	// fd중 소켓 pipe_fd에 해당하는 비트를 1로 한다 

	// int select(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
	// 리턴값 : 성공시 0, 이상오류 발생시 -1 리턴
	//          0을 리턴하는 경우에는 타임아웃에 의해 리턴되었음을 의미
	//          0보다 큰 경우는 변경된 파일 디스크립터의 수를 의미
	// 인자값 분석
	// n        : 검색대상이 되는 파일 디스크립터의 수
	// readfds  : "입력스트림에 변화가 발생했는지" 확인하고자 하는 소켓들의 정보를 전달합니다.
	//           여기서 입력 스트림에 변화가 발생했다는 것은 수신할 데이터가 있다는 뜻
	// writefds : "데이터 전송 시, 블로킹되지 않고 바로 전송이 가능한지" 확인하고자 하는 소켓들의 정보를 전달
	// excepfds : "예외가 발생했는지" 확인하고자하는 소켓들의 정보를 전달
	// timeout : 함수 호출 후, 무한 대기 상태에 빠지지 않도록 타임-아웃(time-out)을 설정
	// ex) select(1, &reads, 0, 0, 5); ---> <감지> 5초동안 감지 수신에 변화가 없는지 감지하고 나와라 - 리턴값 = 1 -
	// 검사 디스크립트 수는 1개
	ret		= select( pipe_fd + 1 , &fd , 0 , 0 , &tv );
	if( ret < 0 ) {
		dbg( 9 , "ERR: select error [%s,%d]. <%s:%s>" , strerror(errno) , errno, __fn, __TF);
		return -1;
	}

	if( ret == 0 ) {
		dbg( 9 , "ERR: select timeout. pipe_fd(%d). <%s:%s>" , pipe_fd, __fn, __TF );
		return -1;
	}
	dbg(1, "OK: select OK. pipe_fd(%d). <%s:%s>" , pipe_fd, __fn, __TF);

	// fd중 소켓 pipe_fd에 해당하는 비트가 1인지 확인
	if( FD_ISSET( pipe_fd , &fd ) ) {
		memset( repStr , 0x00 , size );
		ret	= recv( pipe_fd , repStr , size , 0 ); 		// 해당 소켓을 이용하여 클라이언트 값 읽기
		if( ret < 0 ) {
			dbg( 9 , "ERR: read error [%s %d]. <%s:%s>" , strerror(errno) , errno , __fn, __TF);
			return -1;
		}
		dbg(2, "OK: ret(%d). repStr(%s). <%s:%s>", ret, repStr, __fn, __TF);
		return(ret);
	} 
	dbg( 2 , "ERR: FD_ISSET error [%s %d]. <%s:%s>" , strerror(errno) , errno , __fn, __TF);
	return(-2);
}

int  net_connect( char *host , int port )
{
static char *__fn = "net_connect";
	int	sockfd;
	
	dbg(2, "START: CALL tcp_client. ip(%s). port(%d). <%s:%s>", host, port, __fn, __TF);
	sockfd = tcp_client(host, port);
	if(sockfd < 0) {
		dbg(2, "ERR: tcp_client. ip(%s). port(%d). <%s:%s>", host, port, __fn, __TF);
		return(-1);
	}
	dbg(2, "OK: tcp_client. ip(%s). port(%d). <%s:%s>", host, port, __fn, __TF);
	return(sockfd);
}

int  tcp_client( char *host , int port )
{
static  char *__fn = "tcp_client";
	int			 s;
	int			 ret;
	int			 flags;

	struct sockaddr_in sin;

	memset (&sin, 0, sizeof (sin));

	sin.sin_family	  = AF_INET;
	sin.sin_addr.s_addr = inet_addr( host );
	sin.sin_port		= htons( port );

	/* Opens the socket */
	s = socket (AF_INET, SOCK_STREAM , 0 );
	if (s < 0) {
		return -1;
	}
	/*
	ret = fcntl( s, F_SETFL, O_NONBLOCK );
	if ( ret == -1 ){
		return -1;
	}
	*/
	ret	 = connect( s , (struct sockaddr *)&sin, sizeof( sin ) );
	if( ret < 0 ) {
   
		return -1;
	}
	dbg(2, "OK: return sockfd(%d) <%s:%s>", s, __fn, __TF);
	return (s);
} /* end of tcp_server () */

int  net_connect_nonb( char *host , int port, int nsec )
{
static char *__fn = "net_connect_nonb";
	int	sockfd;
	
	dbg(2, "START: CALL tcp_client. ip(%s). port(%d). <%s:%s>", host, port, __fn, __TF);
	sockfd = tcp_client_nonb(host, port, nsec);
	if(sockfd < 0) {
		dbg(2, "ERR: tcp_client. ip(%s). port(%d). <%s:%s>", host, port, __fn, __TF);
		return(-1);
	}
	dbg(2, "OK: tcp_client. ip(%s). port(%d). <%s:%s>", host, port, __fn, __TF);
	return(sockfd);
}

int  tcp_client_nonb( char *host , int port, int nsec )
{
	int			 s, n, error;
	int		ret;
	int		flags;
	socklen_t	len;

	fd_set		rset;
	fd_set		wset;

	struct timeval	tv;

	struct sockaddr_in sin;
	static	char *__fn = "tcp_client_nonb";

	memset (&sin, 0, sizeof (sin));

	sin.sin_family		= AF_INET;
	sin.sin_addr.s_addr	= inet_addr( host );
	sin.sin_port		= htons( port );

	s = socket (AF_INET, SOCK_STREAM , 0 ); 
	if (s < 0) {
		dbg(9, "ERR: can't create socket: %s. <%s:%s>",	strerror(errno), __fn, __TF);
		return -1;
	}
	
	flags = fcntl( s, F_GETFL, 0 );
	if ( flags == -1 ){
		dbg(9, "ERR: fcntl F_GETFL. <%s:%s>", __fn, __TF);
		return -1;
	}

	ret = fcntl( s, F_SETFL, flags | O_NONBLOCK );
	if ( ret == -1 ){
		dbg(9, "ERR: ERR: can't fcntl :%s. <%s:%s>", strerror(errno), __fn, __TF);
		return -1;
	}
			
	ret		= connect( s , (struct sockaddr *)&sin, sizeof( sin ) );
	if(ret < 0) {
		if(errno != EINPROGRESS) {
			dbg(8, "ERR: [%s] : can't connect socket: %d %s",	__fn, errno, strerror(errno));
			close(s);
			return(-1);
		}
	}

	if( ret == 0 )
	{
		fcntl( s, F_SETFL, flags );
		return (s);
	}

	FD_ZERO( &rset );
	FD_SET(s, &rset );
	wset = rset;
	tv.tv_sec = nsec;
	tv.tv_usec = 0;

	ret = select ( s + 1, &rset, &wset, NULL, &tv );
	if ( ret < 0 ){
		dbg(9, "[%s] ERR: select (%d) (%s)", __fn, errno, strerror(errno) );
		close ( s );
		return -1;
	}
	
	if ( ret == 0 ){
		dbg(9, "[%s] ERR: select timout", __fn );
		close( s );
		return -1;
	}

	if ( FD_ISSET(s, &rset ) || FD_ISSET( s, &wset ) ){
		len = sizeof(error);
		ret = getsockopt(s, SOL_SOCKET, SO_ERROR, &error, &len);
		if (ret < 0) {
			dbg(9, "ERR: getsockopt(%d). error(%d)", ret, error);
			return(-1);
		}
	}
	else
	{
		dbg(9, "ERR: select error : s not set" );
		return (-1);
	}

	if ( fcntl( s, F_SETFL, flags ) == -1 ){
		dbg(9, "[%s] ERR: fcntl F_SETFL", __fn );
		close( s );
		return -1;
	}
	
	if( error )
	{
		close( s );
		errno = error;
		return (-1);
	}
	
	return (s);
}


int Open_Server_Socket(int port)
{
	static char	*__fn = "Open_Server_Socket";
	int sockfd, rc, try_cnt=0;
	int		reuse = 1;
	struct sockaddr_in server;

	dbg(1, "START: port(%d). <%s:%s>", port, __fn, __TF);

	server.sin_family 	= AF_INET;					// AF_INET 은 Address Family 
													// 즉, 인터넷 주소를 뜻한다.
	server.sin_addr.s_addr	= INADDR_ANY;			// 서버에 연결된 네트워크 인터페이스를 목적지로 해서
													// 들어오는 모든 자료를 수신 가능
	server.sin_port			= htons(port);			// htons : 호스트로부터 빅 엔디안인 TCP/IP 네트워크 바이스 순서로
													// u_short 타입의 값을 변환한다.

	// 첫번째 인자 : 통신 프로토콜의 설정 ex) AF_INET, PF_INET6
	// 두번째 인자 : SOCK_STREAM(TCP/IP), SOCK_DGRAM(UDP/IP)
	// 세번째 인자 : 통신에 있어, 특정 프로토콜 사용을 지정하기 위한 변수, 보통 0을 사용
	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
	{
		dbg(9, "ERR: socket create failed. <%s:%s>",  __fn, __TF);
		return -1;
	}

	// int setsockopt(int s, int  level,  int  optname,  const  void  *optval, socklen_t optlen);
	// setsockopt를 사용하는 목적 : 소켓 옵션 조작을 통하여 세부사항을 조절할 수 있음
	// s : 소켓지정번호
	// level : 소켓의 어떤 레벨로 소켓정보를 가져올 것인지에 대한 정보 명시  ex) SOL_SOCKET, IPPROTO_TCP
	// optname : 설정을 위한 소켓 옵션의 번호 ex) SO_REUSEADDR(이미 사용된 주소를 재사용(bind) 하도록 한다.
	// optval : 설정값을 저장하기 위한 버퍼의 포인터
	// optlen : optval 버퍼의 크기

	// 아래 설정은 다음과 같은 기능을 한다 
	// 소켓을 이용한 서버 프로그램을 운용하다 보면 강제종료되거나 비정상 종료되는 경우가 발생한다.
	// 테스트를 목적으로 할 경우에는 특히 강제종료 시켜야 하는 경우가 자주 발생하는데,
	// 강제종료 시키고 프로그램을 다시 실행시킬 경우 다음과 같은 메시지를 종종 보게 된다.
	///////////////////////////////////////////////
	// bind error : Address already in use
	///////////////////////////////////////////////
	// 이는 기존 프로그램이 종료되었지만, 비정상종료된 상태로 아직 커널이 bind정보를 유지하고 있음으로 발생하는 문제다.
	// 보통 1-2분 정보 지나면 커널이 알아서 정리를 하긴 하지만, 그 시간동안 기달려야 한다는 것은 상당히 번거로운 일이다.
	// 이 경우 다음과 같은 코드를 삽입함으로써 문제를 해결할 수 있다.
	// int sock = sock(....);
	// setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&bf, (int)sizeof(bf));
	// 위와 같이 하면 커널은 기존에 bind로 할당된 소켓자원을 프로세스가 재사용할 수 있도록 허락한다.
	rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
	if ( rc == - 1) {
		dbg(9, "ERR: setsockopt failed. <%s:%s>",  __fn, __TF);
		return(-1);
	}
	
	for(;;){
		// bind() 함수를 이용하여 socket에 server socket에 필요한 정보를 할당하고 커널에 등록
		// 1. 만들어진 sockfd는 socket 디스크립터
		// 2. 커널에 등록해야 다른 시스템과 통신할 수 있는 상태가 됨
		// 3. 커널에 등록해야 다른 시스템과 통신할 수 있는 상태가 됨
		// 4. 더 정확히 이야기하면 커널이 socket를 이용하여 외부로부터의 자료를 수신할 수 있게 됨
		// 5. socket에 주소와 port를 할당하기 위해 sockaddr_in 구조체를 이용함
		rc = bind(sockfd, (struct sockaddr*)&server, sizeof(server));
		if(rc == -1) {
			dbg(9, "ERR: bind create failed. port(%d) [%s]. <%s:%s>",  port, strerror(errno), __fn, __TF);
			// 10번 시도하여 bind되지 않으면 실패
			if(try_cnt++ > 10) {
				dbg(9, "ERR: bind error. max Try(%d). <%s:%s>", try_cnt, __fn, __TF);
				return(-1);
			}
			sleep(1);
			continue;
		}
		break;
	}

	// int listen(int s, int backlog);
	// s : 소켓 디스크립터
	// backlog : 대기 메시지 큐의 개수
	// listen() 함수로 클라이언트 접속 요청을 확인한다.
	if (listen(sockfd, 5) == -1) {
		dbg(9, "ERR: listen create failed. <%s:%s>",  __fn, __TF);
		return -3;
	}

	return sockfd;
}

void nas_usleep(int micro_sec)
{
	static char	*__fn	= "nas_usleep";
	int		ret;
	int		dly_pipefd[2];
	fd_set		fd;
	struct	timeval	tv;

	tv.tv_sec	= 0;
	tv.tv_usec	= micro_sec;

dbg(3, "START: usleep(%ld). <%s:%s>", micro_sec, __fn, __TF);

	pipe(dly_pipefd);

	while(1) {
		FD_ZERO( &fd );
		FD_SET( dly_pipefd[0] , &fd );
		ret		= select( dly_pipefd[0] + 1 , &fd , 0 , 0 , &tv );
		break;
	}

	close(dly_pipefd[0]);
	close(dly_pipefd[1]);
dbg(3, "END: usleep(%ld). <%s:%s>", micro_sec, __fn, __TF);
}

void nas_sleep(int w_sec)
{
static char	*__fn	= "nas_sleep";
	int		ret;
	int		dly_pipefd[2];
	fd_set		fd;
	long 		start_tm, cur_tm;

	struct	timeval		tv;

	time((time_t *)&start_tm);
	cur_tm = start_tm;

	pipe(dly_pipefd);

dbg(2, "START: nas_sleep(%d). <%s:%s>", w_sec, __fn, __TF);

	while((cur_tm - start_tm) < w_sec) {
		tv.tv_sec		= w_sec - (cur_tm - start_tm);
		tv.tv_usec		= 0;

		FD_ZERO( &fd );
		FD_SET( dly_pipefd[0] , &fd );
		ret		= select( dly_pipefd[0] + 1 , &fd , 0 , 0 , &tv );

		time((time_t *)&cur_tm);
	}

	close(dly_pipefd[0]);
	close(dly_pipefd[1]);
dbg(2, "END: nas_sleep(%d). <%s:%s>", w_sec, __fn, __TF);
}
