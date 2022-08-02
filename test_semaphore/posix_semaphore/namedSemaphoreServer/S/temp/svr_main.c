#define	__TF	"svr_main.c"

#include	<stdio.h>
#include	"extern.h"


extern int		tmout;
extern int		svr_port[100];
extern char		svr_ip[100][20];
extern int		max_server_cnt;

int	Server_Main(int fork_cnt, int sockfd)
{
static	char	*__fn = "svr_main";
	char	recv_buff[4096], send_buff[4096];
	int	rcv_len, i, r;
	int		conn_idx;

	memset(recv_buff, 0x00, sizeof(recv_buff));
	memset(send_buff, 0x00, sizeof(send_buff));
	
	rcv_len = readSocket(sockfd, recv_buff, sizeof(recv_buff), tmout);
	if( rcv_len == 0 ) {
		dbg(9, "ERR: Client Closed .. <%s:%s>", __fn, __TF);
		return -1;
	}
	else if( rcv_len == -1 ) {
		dbg(9, "ERR: Client Timeout <%s:%s>", __fn, __TF);
		return -1;
	}
	else if( rcv_len == -2 ) {
		dbg(9, "ERR: Cliser Recv Err recv_buff(%s) <%s:%s>", recv_buff, __fn, __TF);
/*****************************
  err reply msg send 
******************************/
		return -1;
	}
	dbg(6, "Recv from Client [%d][%d][%s] <%s:%s>", sockfd, rcv_len, recv_buff, __fn, __TF);

	i=0;
	while(i <= max_server_cnt){
		conn_idx = (fork_cnt+i) % max_server_cnt;
		r = TCP_Svr_Comm(0, svr_ip[conn_idx], svr_port[conn_idx], recv_buff, rcv_len, send_buff, sizeof(send_buff)-1, tmout);
		if( r < 0 ){
			if( r == -100 ){
				dbg(9, "ERR: Connect Fail Ip(%s) Port(%d)",
				svr_ip[conn_idx], svr_port[conn_idx]);
				i++;
				continue;
			}
			dbg(9, "ERR: TCP_Svr_Comm. r(%d). svr_ip(%s). svr_port(%d). <%s:%s>", r, svr_ip[conn_idx], svr_port[conn_idx], __fn, __TF);
/****************************** 
  err reply msg create 
******************************/
		}
		dbg(6, "Send to Server [%d][%s] <%s:%s>", rcv_len, recv_buff, __fn, __TF);
		dbg(6, "Recv from Server [%d][%s] <%s:%s>", r, send_buff, __fn, __TF);
		break;
	}

	r = send( sockfd, send_buff, r, 0 );
	dbg(6, "Send to Client [%d][%d][%s] <%s:%s>", sockfd, r, send_buff, __fn, __TF);

	return 0;
}
