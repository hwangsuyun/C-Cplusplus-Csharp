/* strchr example */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "ipv6_util.h"


int main(int argc, char* argv[])
{

	IPV6_UTIL_ST inst;
	int ret;

	if(init_ipv6_util(&inst) < 0){
		printf("init_ipv6_util is failed\n");
		return 0;
	}

	inst.set_ip_address(&inst,argv[1]);
	ret = inst.convert(&inst);
	printf("type: %d, compress_type: %d\n",inst.type, inst.compress_type);
	printf("src_ip: %s\n",argv[1]);
	printf("convert ip: %s\n",inst.get_convert_ip_address(&inst));

	printf("get_ipv4_address: %s\n",inst.get_ipv4_address(&inst));
	printf("get_ipv4_mapped_address: %s\n",inst.get_ipv4_mapped_address(&inst));
	printf("get_ipv6_compress_address: %s\n",inst.get_ipv6_compress_address(&inst));
	printf("get_ipv6_uncompress_address: %s\n",inst.get_ipv6_uncompress_address(&inst));
#if 0
	ret = inst.get_ip_address_version(argv[1]);
	printf("type: %d\n",ret);
	ret =inst.get_ipv6_address_type(argv[1]);
	printf("compress_type: %d\n",ret);

	inst.set_ip_address(&inst,argv[1]);
	ret = inst.convert(&inst);
	if(ret < 0) {
		printf("err: convert failed(%d)\n",ret);
		return 0;
	}
	printf("type: %d, compress_type: %d\n",inst.type, inst.compress_type);
	printf("src_ip: %s\n",argv[1]);
	printf("convert ip: %s\n",inst.get_convert_ip_address(&inst));
#endif
	return 0;
}
