#pragma once

typedef struct  
{        
	char pgm[30 + 1];
	char siteCd[10 + 1];
	char ip[15 + 1];
	char flag[10 + 1];
	char key[50 + 1];
} RecvMsg; 

int parseRecvData(char *buff, RecvMsg *data);

