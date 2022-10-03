typedef struct  
{        
	char pgm[30 + 1];
	char site_cd[10 + 1];
	char ip[15 + 1];
	char flag[3 + 1];
	char semKey[3 + 1];
	char shmKey[3 + 1];
} PROTOCOL; 

int parseRecvData(char *buff, PROTOCOL *data);

