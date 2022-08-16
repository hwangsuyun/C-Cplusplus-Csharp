int sock_accept(int sockfd, char *cliaddr_buff);
int net_connect_block(char	*serv_h_addr, int	 portno);
int		readSocket( int pipe_fd , char *repStr , int size , int timeout );
int  net_connect( char *host , int port );
int  net_connect_nonb( char *host , int port, int nsec );
int  tcp_client( char *host , int port );
int  tcp_client_nonb( char *host , int port, int nsec );

int Open_Server_Socket(int port);
void nas_usleep(int micro_sec);
void nas_sleep(int w_sec);
