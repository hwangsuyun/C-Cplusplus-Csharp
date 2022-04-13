#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>

void getSelfIP(char **inputIP);
//void getSelfIP(char *inputIP);

int main()
{
	char *ip[32];
	//char ip[32];
	getSelfIP(ip);
	printf("ip(%s)\n", ip);
	
	return 0;
}

void getSelfIP(char **inputIP)
{
	int fd; 
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	// I want to get an IPv4 IP address
	ifr.ifr_addr.sa_family = AF_INET;

	// I want IP address attached to "eth0"
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);

	close(fd);

	sprintf((char *)inputIP, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}

/*
void getSelfIP(char *inputIP)
{
	int fd; 
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	// I want to get an IPv4 IP address 
	ifr.ifr_addr.sa_family = AF_INET;

	// I want IP address attached to "eth0" 
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);

	close(fd);

	sprintf(inputIP, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}
*/
