/* strchr example */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ipv6_util.h"

#define len 46

int xgetIPv6FullAddress(char *in, char *out)
{
	IPV6_UTIL_ST inst;
	char *ipv6 = NULL;

	if (out == NULL)
	{
		printf("out is NULL\n");
	    return -1;
	}

	if(init_ipv6_util(&inst) < 0){
		//printf("init_ipv6_util is failed\n");
		return -1;
	}

	inst.set_ip_address(&inst, in);
	if (inst.convert(&inst) != 0)
	{
		//printf("err: convert failed(%d)\n", ret);
		printf("err: convert failed\n");
		return -1;
	}
#if 0
	printf("type: %d, compress_type: %d\n",inst.type, inst.compress_type);
	printf("src_ip: %s\n", in);
	printf("convert ip: %s\n",inst.get_convert_ip_address(&inst));
	
	printf("get_ipv4_address: %s\n",inst.get_ipv4_address(&inst));
	printf("get_ipv4_mapped_address: %s\n",inst.get_ipv4_mapped_address(&inst));
	printf("get_ipv6_compress_address: %s\n",inst.get_ipv6_compress_address(&inst));
	printf("get_ipv6_uncompress_address: %s\n",inst.get_ipv6_uncompress_address(&inst));
#endif
	memset(out, 0x00, sizeof(out));
	ipv6 = inst.get_ipv6_uncompress_address(&inst);
	if (ipv6 == NULL)
	{
		printf("err: out is (null)\n");
		return -1;
	}
	snprintf(out, len, "%s", ipv6);

	return 0;
}

int main(int argc, char* argv[])
{
	int ret = 0;
	char out[64];

	memset(out, 0x00, sizeof(out));
	ret = xgetIPv6FullAddress(argv[1], out);
	
	if (ret < 0)
	{
		printf("failed, ret : (%d), out : (%s)\n", ret, out);
	}
	else
	{
		printf("succeed, ret : (%d), out : (%s)\n", ret, out);
	}

	return 0;
}
