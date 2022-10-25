#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP_SIZE 64

// in : ip out : ip
// in : hostname out : ip (ipv4)
int sp_change_iaddr(char *out, int outlen, char *inhost)
{
	struct hostent   hostaddr;
	struct hostent   *t;
	struct in_addr   iaddr;
	char   buf[4096];
	int    l;
	int    ret;

	memset(&hostaddr, 0x00, sizeof(struct hostent));
	memset(buf,0x00,sizeof(buf));
	memset(&iaddr,0x00,sizeof(struct in_addr));
	l = 0;

	strncpy(out, inhost, outlen);
	if ( inhost[0] == '\0' )
	{
		return -1;
	}
	ret = gethostbyname_r(inhost,&hostaddr,buf,4095,&t,&l);
	if (ret != 0)
	{
		return -1;
	}

	if (t == NULL)
	{
		return -1;
	}

	memcpy ( &iaddr, *t->h_addr_list, 4 );

	snprintf( out, 16,"%s", (char *)inet_ntoa( iaddr ));

	return 0;
}

// in : ip out : ip
// in : hostname out : ip (ipv4 and ipv6)
int sp_change_iaddr6(char *out, int outlen, char *inhost)
{
	struct addrinfo hints, *rp, *result;
	struct sockaddr_in *addr_in;
	struct sockaddr_in6 *addr_in6;
	int    ret;

	memset(&hints, 0x00, sizeof(struct addrinfo));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	strncpy(out, inhost, outlen);
	if ( inhost[0] == '\0' )
	{
		return -1;
	}
	ret = getaddrinfo(inhost, NULL, &hints, &result);
	if (ret != 0)
	{
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		addr_in = (struct sockaddr_in *)rp->ai_addr;
		addr_in6 = (struct sockaddr_in6 *)rp->ai_addr;

		switch (rp->ai_family)
		{
			case AF_INET:
				inet_ntop(AF_INET, &addr_in->sin_addr, out, outlen);
				printf("IPV4\n");
				break;
			case AF_INET6:
				inet_ntop(AF_INET6, &addr_in6->sin6_addr, out, outlen);
				printf("IPV6\n");
				break;
		}
	}

	return 0;
}

int main()
{
	char inhost[100];
	char out[100];
	int ret = 0;

	memset(inhost, 0x00, sizeof(inhost));
	memset(out, 0x00, sizeof(out));
	memcpy(inhost, "spmaster", sizeof(inhost));

	ret = sp_change_iaddr(out, sizeof(out), inhost);

	printf("ret : (%d)\n", ret);
	printf("out : (%s)\n", out);

	ret = sp_change_iaddr6(out, sizeof(out), inhost);
	printf("ret : (%d)\n", ret);
	printf("out : (%s)\n", out);

	return 0;
}
