#ifndef _UTILLIB_H_
#define _UTILLIB_H_

void	DelChar(char *src, int src_len, char ch, char *dst, int dst_len);

void	LPAD( char *src );
void	RPAD( char *src );
void	PAD( char *src );

char*	get_time(char* szvalue);
char*	get_datetime(char* szvalue);
char*	get_date(char* szvalue);
int		get_before_date( char *date_buff, int day );
int		hex2str(unsigned char *r_buff, char *s_buff, int s_len);
int		hex2int(char *hexa, int *ival);
int		GetField( char *source, char ch, char data_arr[][50] );
void	NasEncrypt(char* src, int len);
void	NasDecrypt(char* src, int len);
void	NulltoSpace(char *buf, int size);

int		nasAesCrypt(char *type, char const* key, char const* iv, int cryptByte, char const* in, char* result, size_t len, int iMode);

void	Base64Enc( char *input, char *output, int len );
int		Base64Dec( char *input, char *output );

void	Char2Hex(unsigned char ch, char *szHex);
unsigned char	Hex2Char(char const *szHex);
void	CharStr2HexStr(unsigned char const *pucCharStr, char *pszHexStr, int iSize);
void	HexStr2CharStr(char const *pszHexStr, unsigned char *pucCharStr, int iSize);

int		Compression(int flag, char *source, int sourceLen, char *dest, int *destLen );	

//Van 코드변경(v5 <-> v7)
int		ChangeVanCode( int rVer, char *rOrgCd, char *rResCd );

void	Symbol2String( char *dst, int len, char symbol );
void	L_PAD( char *src, char *dst, int len, char symbol );
void	R_PAD( char *src, char *dst, int len, char symbol );

#ifdef WIN32
	int		Recyclebin( char *file_path, int delete_flag );
	void	GetPrgName( char *rName );
	void	GetDLLName( char *rName );
#else
	int		Create_Dir_PathFile(char *dirfile_name);
	int		Create_Dir_Path(char *d_name);
	int		__create_dir_path(char *d_name);
#endif
	
#endif 
