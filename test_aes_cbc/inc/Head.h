#ifndef _HEAD_H_
#define _HEAD_H_

#define		VERSION5			5
#define 	VERSION7			7
#define		INI_FILENAME		"CreditCard32.ini"
#define		INI_NULL			-9999
#define		DLL_ERR				-1
#define		DLL_OK				0

#define		_MAX_SERVER_CNT_	10
#define		_MAX_IP_LEN_		30
#define		_SITE_CD_LEN_		10
#define		_MASK_FLAG_LEN_		4
#define		MAX_BUFF			1024
#define		MAX_FNAME_LEN		256

#define		SOCKET_ERROR		(-1)
#define		DATA_LEN			4096

#define		TERMINAL_ID_LEN		10

///////////////////////////////////////////////////////////////////
//압축 관련 
#define COMPRESS			1
#define UNCOMPRESS			2
#define COMPRESS_BUFF_SIZE	(uLongf)( 12 + ( (1024*4) * 1.001 ) + 1 )
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//암호화 관련
enum { 
	AES_ECB=0, 
	AES_CBC=1, 
	AES_CFB=2
};
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//7차용 헤더 관련 
#define UID_LEN 4
#define VER_LEN 4
#define ENC_LEN 1
#define COMP_LEN 1
#define CHAR_LEN 6
#define JSON_LEN 8
#define HEAD_LEN UID_LEN + VER_LEN + ENC_LEN + COMP_LEN + CHAR_LEN + JSON_LEN

typedef struct HEAD_V7 {
	char	uid[ UID_LEN + 1 ];
	char	version[ VER_LEN + 1 ];
	char	enc[ ENC_LEN + 1 ];
	char	compress[ COMP_LEN + 1 ];
	char	charset[ CHAR_LEN + 1 ];
	char	length[ JSON_LEN + 1 ];
} HEAD_V7;
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//서버 정보 관리 관련
typedef struct  
{
	char			svr_ip[ _MAX_IP_LEN_ + 1 ];
	unsigned short	v7_port;
	int				err_cnt;
} SERVER_INFO;

typedef struct  
{
	SERVER_INFO server_info[ _MAX_SERVER_CNT_ ];
	int			trans_idx;
} TRAN_INFO;
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//INI 설정 정보 관리
int			DBG_LEVEL;
int			MAX_SERVER;
int			MAIN_CHECK;
int			REPLY_TIMEOUT;
int			CONNECT_TIMEOUT;
int			RECV_TIMEOUT;
int			ENCRYPT_VERSION;
TRAN_INFO	tran_info;
char		SITE_CD[ _SITE_CD_LEN_+1 ];
//char		MASK_FLAG[_MASK_FLAG_LEN_+1];
int			MASK_FLAG;
int			COMPRESSION;
char TERMINAL_ID[TERMINAL_ID_LEN+1];

#endif
