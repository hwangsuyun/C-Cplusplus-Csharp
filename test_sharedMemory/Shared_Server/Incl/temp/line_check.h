
int setErrorShmLineInfo( unsigned char **shm, int server_no );
int setOkShmLineInfo( unsigned char **shm, int server_no );
int getErrorServer( unsigned char **shm, int server_cnt, int server_no, int *server_index );
int getNormalServer( unsigned char **shm, int server_cnt); 

int getServerInfo( char *ini_file, char *site_code ); 

int incRunCnt( unsigned char **shm, int server_cnt );
int decRunCnt( unsigned char **shm, int server_cnt );
int setRunCnt( unsigned char **shm, int server_cnt, int run_cnt);
int getRunCnt( unsigned char **shm, int server_cnt );
