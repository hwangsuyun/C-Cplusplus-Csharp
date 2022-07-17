#pragma once

#define STX     0x02
#define ETX     0x03
#define FS      0x1c

#define TRUE    1
#define FALSE   0

typedef struct {
	char pgm[30 + 1]; 
	char siteCd[10 + 1]; 
	char ip[15 + 1]; 
	char flag[10 + 1]; 
	char key[50 + 1];
} SendMsg;

typedef struct {
	char msg[50 + 1];
	char msgCd[10 + 1];
	char key[50 + 1];
} RecvMsg;

int lockSem(char *pgm, char *siteCd, char *dstIp, int dstIort);
int unlockSem(char *pgm, char *siteCd, char *dstIp, int dstPort, char *key);
int sendRecvMsg(char *ip, int port, char *sendMessage, int sLen, char *key);
void makeJsonData4Req(char *, char *, char *, char *, char *, char *); 
int ConnectSemServer(char *IpAddr, int Port);
void parseRecvData(char *in, RecvMsg *out);
int createKeyFile(char *key, char *currProcName);
