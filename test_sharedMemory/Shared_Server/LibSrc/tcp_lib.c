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

	FD_ZERO( &fd );
	FD_SET( pipe_fd , &fd );
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

	if( FD_ISSET( pipe_fd , &fd ) ) {
		memset( repStr , 0x00 , size );
		ret	= recv( pipe_fd , repStr , size , 0 );
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

	server.sin_family 	= AF_INET;
	server.sin_addr.s_addr	= INADDR_ANY;
	server.sin_port			= htons(port);

	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
	{
		dbg(9, "ERR: socket create failed. <%s:%s>",  __fn, __TF);
		return -1;
	}

	rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
	if ( rc == - 1){
		dbg(9, "ERR: setsockopt failed. <%s:%s>",  __fn, __TF);
		return(-1);
	}
	
	for(;;){
		rc = bind(sockfd, (struct sockaddr*)&server, sizeof(server));
		if(rc == -1)
		{
			dbg(9, "ERR: bind create failed. port(%d) [%s]. <%s:%s>",  port, strerror(errno), __fn, __TF);
			if(try_cnt++ > 10) {
				dbg(9, "ERR: bind error. max Try(%d). <%s:%s>", try_cnt, __fn, __TF);
				return(-1);
			}
			sleep(1);
			continue;
		}
		break;
	}

	if (listen(sockfd, 5) == -1)
	{
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
