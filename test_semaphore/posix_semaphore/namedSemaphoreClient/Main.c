#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include "cJSON.h"
#include "SemClient.h"

int main(int argc, char **argv)                                                                                                                             
{
	char srcIp[20], dstIp[20];
	int dstPort;
	int sLen = 0;

	memset( srcIp, 0x00, sizeof(srcIp) );  
	memset( dstIp, 0x00, sizeof(dstIp) );  

	sprintf( srcIp, "%s", "133.9.100.5" );
	sprintf( dstIp, "%s", "133.9.100.5" );
	dstPort = 20040;

	if (strcmp(argv[3],"LOCK") == 0)        lockSem(argv[1], argv[2], srcIp, dstIP, dstPort);
	//else if (strcmp(argv[2],"UNLOCK") == 0)   unlockSem(argv[0], argv[1], ip, port);

	printf("Bye...\n");

	return 0;
}

