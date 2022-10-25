
#include <sys/types.h>
#ifndef WIN
#include <ifaddrs.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ipv6_util.h"

static int verify_ipv6_address_format(char *ip)
{
	char *temp;
	int len = IPV6_UTIL_INIT_VALUE;
	int ip_len = IPV6_UTIL_INIT_VALUE;
	int total_colon_count = IPV6_UTIL_INIT_VALUE;
	int max_colon_count = IPV6_UTIL_MAX_COLON_COUNT;
	int field_count = IPV6_UTIL_INIT_VALUE;
	int field_compress_count = IPV6_UTIL_INIT_VALUE;
	int field_length = IPV6_UTIL_INIT_VALUE;
	int colon_flag = IPV6_UTIL_INIT_COLON_FLAG;
	int compress_flag = IPV6_UTIL_INIT_COMPRESS_COLON_FLAG;
	int compress_type = IPV6_UTIL_INIT_VALUE;

	if(!ip) return IPV6_UTIL_ERROR_INPUT_DATA_NULL;

	ip_len = strlen(ip);
	if(ip_len > IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH || ip_len < IPV6_UTIL_MIN_IPV6_ADDRESS_LENGTH) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS_LENGTH;
	temp = ip;
	
	while( *temp && ( (*temp == 0x3A ) || (*temp >= 0x30 && *temp <= 0x39) || (*temp >= 0x41 && *temp <= 0x46)
                || (*temp >= 0x61 && *temp <= 0x66) ) ){ 
		if(*temp == IPV6_UTIL_COLON_CODE){
		#ifdef _IPV6_UTIL_DEBUG
			printf("verify_ipv6_address_format: temp(%x), len(%d)\n",*temp,len);
		#endif
			if(colon_flag == IPV6_UTIL_INIT_COLON_FLAG) colon_flag = IPV6_UTIL_SINGLE_COLON_FLAG;
			else if(colon_flag == IPV6_UTIL_SINGLE_COLON_FLAG){ 
		#ifdef _IPV6_UTIL_DEBUG
				printf("=======verify_ipv6_address_format: colon_flag(%d),compress_type(%d)\n",colon_flag,compress_type);
		#endif
				colon_flag = IPV6_UTIL_DOUBLE_COLON_FLAG;
				if( (len+1) == IPV6_UTIL_DOUBLE_COLON_LENGTH) compress_type = IPV6_UTIL_IPV6_ADDRESS_COMPRESS_BEGIN_TYPE;
				else if((ip_len-1) == len) compress_type = IPV6_UTIL_IPV6_ADDRESS_COMPRESS_END_TYPE;
				else compress_type = IPV6_UTIL_IPV6_ADDRESS_COMPRESS_MIDDLE_TYPE; 
		#ifdef _IPV6_UTIL_DEBUG
				printf("=======>>>>verify_ipv6_address_format: colon_flag(%d),compress_type(%d)\n",colon_flag,compress_type);
		#endif
			} 
			else return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS;
			
			if(colon_flag == IPV6_UTIL_DOUBLE_COLON_FLAG){
				if(compress_flag != IPV6_UTIL_COMPRESS_COLON_FLAG) compress_flag = IPV6_UTIL_COMPRESS_COLON_FLAG;
				else return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS;
			}
				
			field_length = IPV6_UTIL_INIT_VALUE;
			total_colon_count++;
		}
		else{
		#ifdef _IPV6_UTIL_DEBUG
			printf("verify_ipv6_address_format: temp(%x), len(%d)\n",*temp,len);
		#endif
			if(field_length == IPV6_UTIL_INIT_VALUE) field_count++; 
			field_length++;
			if(colon_flag != IPV6_UTIL_INIT_COLON_FLAG) colon_flag = IPV6_UTIL_INIT_COLON_FLAG;
			if(field_length > IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_LENGTH) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS;
		}
		temp++; len++;
	}
#ifdef _IPV6_UTIL_DEBUG
	printf("verify_ipv6_address_format: compress_flag(%d), field_count(%d), total_colon_count(%d)\n",compress_flag,field_count,total_colon_count);
#endif
	if(ip_len != len) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS;
	if( compress_flag == IPV6_UTIL_COMPRESS_COLON_FLAG && compress_type != IPV6_UTIL_IPV6_ADDRESS_COMPRESS_MIDDLE_TYPE) 
		max_colon_count = IPV6_UTIL_COMPRESS_BEGIN_TYPE_MAX_COLON_COUNT;	
#ifdef _IPV6_UTIL_DEBUG
	printf("verify_ipv6_address_format: compress_type(%d), max_colon_count(%d)\n",compress_type,max_colon_count);
#endif
	if(total_colon_count > max_colon_count) return IPV6_UTIL_ERROR_IPV6_ADDRESS_COLON_MAX_COUNT_OVER;
	if(compress_flag == IPV6_UTIL_COMPRESS_COLON_FLAG && field_count > IPV6_UTIL_MAX_IPV6_ADDRESS_COMPRESS_TYPE_FIELD_COUNT)
		return IPV6_UTIL_ERROR_IPV6_ADDRESS_COMPRESS_TYPE_FIELD_MAX_COUNT_OVER;
	return IPV6_UTIL_SUCCESS;
}

static int get_ipv6_address_colon_count(char *ip)
{
	char *temp;
	int count= 0;

	if(!ip) return IPV6_UTIL_ERROR_INPUT_DATA_NULL;

	temp = ip;
	GET_IPV6_ADDRESS_COLON_COUNT(temp,count);

	return count;
}
static int get_ipv6_address_type(char *ip)
{
	int len;

	if(!ip) return IPV6_UTIL_ERROR_INPUT_DATA_NULL;	
	if(strstr(ip,IPV6_UTIL_DOUBLE_COLON_STRING) != NULL) return IPV6_UTIL_IPV6_ADDRESS_COMPRESS_TYPE;
	len = strlen(ip);

	if(len == IPV6_UTIL_NORMAL_IPV6_ADDRESS_LENGTH) return IPV6_UTIL_IPV6_ADDRESS_NORMAL_TYPE;
	if(len < IPV6_UTIL_NORMAL_IPV6_ADDRESS_LENGTH) return IPV6_UTIL_IPV6_ADDRESS_OMIT_LEADING_ZEROS_TYPE;	

	return IPV6_UTIL_ERROR_IPV6_ADDRESS_UNKNOWN_TYPE;
}

static int get_ipv6_address_compress_type(char *ip)
{
	char *temp;
	char *colon;
	int len;
	int type;
	
	if(!ip) return IPV6_UTIL_ERROR_INPUT_DATA_NULL;

	temp = ip;
	len = strlen(ip);
	colon = strstr(temp,IPV6_UTIL_DOUBLE_COLON_STRING);
	if(!colon) return IPV6_UTIL_ERROR_IPV6_ADDRESS_NOT_COMPRESS_TYPE;

	if(colon == temp) type = IPV6_UTIL_IPV6_ADDRESS_COMPRESS_BEGIN_TYPE;
	else if(colon == (temp+(len-IPV6_UTIL_DOUBLE_COLON_LENGTH))) type = IPV6_UTIL_IPV6_ADDRESS_COMPRESS_END_TYPE;
	else type = IPV6_UTIL_IPV6_ADDRESS_COMPRESS_MIDDLE_TYPE;
	
	return type;
}

static int get_ip_address_version_type(char *ip)
{
    char *temp;

	if(!ip) return IPV6_UTIL_ERROR_INPUT_DATA_NULL;

	temp = strchr(ip,IPV6_UTIL_PERIOD_CODE);
	if(!temp) return IPV6_UTIL_IPV6_ADDRESS_TYPE;

	temp = strchr(ip,IPV6_UTIL_COLON_CODE);
	if(!temp) return IPV6_UTIL_IPV4_ADDRESS_TYPE;
	return IPV6_UTIL_IPV4_MAPPED_IPV6_ADDRESS_TYPE;
}

static int convert_ipv4_mapped_ipv6_to_ipv4(char *in, char *out, int max_len)
{
	char *temp;
	int len;

	if(!in) return IPV6_UTIL_ERROR_INPUT_DATA_NULL;

	temp = in;
	TO_LOWER_CASE(temp);

	temp = strstr(in,IPV6_UTIL_IPV4_MAPPED_PREFIX_COMPRESS);
	if(!temp){
		temp = strstr(in,IPV6_UTIL_IPV4_MAPPED_PREFIX_OMIT_LEADING_ZEROS);	
		if(!temp){
			temp = strstr(in,IPV6_UTIL_IPV4_MAPPED_PREFIX_NORMAL);
			if(!temp) return IPV6_UTIL_ERROR_NOT_IPV4_MAPPED_ADDRESS;
			len = IPV6_UTIL_IPV4_MAPPED_PREFIX_NORMAL_LENGTH;		
		}else{
			len = IPV6_UTIL_IPV4_MAPPED_PREFIX_OMIT_LEADING_ZEROS_LENGTH;
		}
	}else{
		len = IPV6_UTIL_IPV4_MAPPED_PREFIX_COMPRESS_LENGTH;
	}
	if(out && max_len > 0) ipv6_util_string_copy(out,max_len,"%s",temp + len);		

	return IPV6_UTIL_SUCCESS;
}

static int set_uncompress_ipv6_address_field_value(char *in, char *out,int type)
{
	int ret;
	if(!in) return IPV6_UTIL_ERROR_INPUT_DATA_NULL;

	ret = IPV6_UTIL_SUCCESS;
	switch(type)
	{	
		case IPV6_UTIL_UNCOMPRESS_OMIT_LEADING_ZEROS_ONE_TYPE: 
			if(out) ipv6_util_string_copy(out,IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_LENGTH+1,IPV6_UTIL_UNCOMPRESS_OMIT_LEADING_ZEROS_ONE_FORMAT,in); break;
		case IPV6_UTIL_UNCOMPRESS_OMIT_LEADING_ZEROS_TWO_TYPE: 
			if(out) ipv6_util_string_copy(out,IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_LENGTH+1,IPV6_UTIL_UNCOMPRESS_OMIT_LEADING_ZEROS_TWO_FORMAT,in); break;
		case IPV6_UTIL_UNCOMPRESS_OMIT_LEADING_ZEROS_THREE_TYPE: 
			if(out) ipv6_util_string_copy(out,IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_LENGTH+1,IPV6_UTIL_UNCOMPRESS_OMIT_LEADING_ZEROS_THREE_FORMAT,in); break;
		case IPV6_UTIL_UNCOMPRESS_OMIT_LEADING_ZEROS_FOUR_TYPE: 
			if(out) ipv6_util_string_copy(out,IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_LENGTH+1,IPV6_UTIL_UNCOMPRESS_OMIT_LEADING_ZEROS_FOUR_FORMAT,in); break;
		default: ret = IPV6_UTIL_ERROR; break;

	}
	return ret;
}

static int uncompress_ipv6_address_omit_leading_zeros(char *in, char *out, int max_len)
{
	IPV6_UTIL_V6ADDR address;
	IPV6_UTIL_V6ADDR_FIELD *field;

	char *temp;
	char *split;
	
	int field_len;
	int i;

	if(!in) return IPV6_UTIL_ERROR_INPUT_DATA_NULL;
	if(max_len <= IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS_LENGTH;

	memset(&address,0x00,sizeof(address));

	temp = in;

#ifdef _IPV6_UTIL_DEBUG
    printf("uncompress_ipv6_address_omit_leading_zeros: in(%s)\n",in);
#endif
	for(i = 0 ; i< IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT; i++){
        field = &address.fields[i];
		split = strchr(temp,IPV6_UTIL_COLON_CODE);
		if(!split){
			if(i != IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT-1) break;
			field_len = strlen(temp);
			if(set_uncompress_ipv6_address_field_value(temp,field->data,field_len) < 0) break;
		}
		else{
			field_len = split - temp;
			if(set_uncompress_ipv6_address_field_value(temp,field->data,field_len) < 0) break;
		}
		temp = split + 1;
#ifdef _IPV6_UTIL_DEBUG
		printf("%d:[%s]\n",i,field->data);
#endif
	}
		
	if(i != IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS;

	max_len = IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH + 1;
	if(out){
		field_len = ipv6_util_string_copy(out,max_len,IPV6_UTIL_UNCOMPRESS_IPV6_ADDRESS_FORMAT, address.fields[0].data
																			  , address.fields[1].data
																			  , address.fields[2].data
																			  , address.fields[3].data
																			  , address.fields[4].data
																			  , address.fields[5].data
																			  , address.fields[6].data
																			  , address.fields[7].data);
		if(field_len != IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS_LENGTH;

		TO_LOWER_CASE(out);
	}
	return IPV6_UTIL_SUCCESS;
}




static int uncompress_ipv6_address_compress_begin(char *in, char *out, int max_len)
{
	IPV6_UTIL_V6ADDR address;
	IPV6_UTIL_V6ADDR_FIELD *field;

	char *temp;
	char *split;
	
	int total_colon_count;
	int compress_colon_count;
	int field_len;
	int i;

	if(!in) return IPV6_UTIL_ERROR_INPUT_DATA_NULL;
	if(max_len <= IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS_LENGTH;

	memset(&address,0x00,sizeof(address));

	if(strlen(in) == IPV6_UTIL_DOUBLE_COLON_LENGTH)	compress_colon_count = IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT;
	else{
		total_colon_count = get_ipv6_address_colon_count(in);
		if(total_colon_count <= 0) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS;
		compress_colon_count = IPV6_UTIL_MAX_COLON_COUNT - total_colon_count + IPV6_UTIL_OMIT_COMPRESS_BEGIN_TYPE_COLON_COUNT;
	}

	temp = in + IPV6_UTIL_DOUBLE_COLON_LENGTH;

	for(i = 0; i< compress_colon_count; i++){
		field = &address.fields[i];
		strncpy(field->data,IPV6_UTIL_OMIT_LEADING_ZEROS_FOUR,IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_LENGTH);
#ifdef _IPV6_UTIL_DEBUG
		printf("%d:[%s]\n",i,field->data);
#endif
	}
	for(i ; i< IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT; i++){
        field = &address.fields[i];
		split = strchr(temp,IPV6_UTIL_COLON_CODE);
		if(!split){
			if(i != IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT-1) break;
			field_len = strlen(temp);
			if(set_uncompress_ipv6_address_field_value(temp,field->data,field_len) < 0) break;
		}
		else{
			field_len = split - temp;
			if(set_uncompress_ipv6_address_field_value(temp,field->data,field_len) < 0) break;
		}
		temp = split + 1;
#ifdef _IPV6_UTIL_DEBUG
		printf("%d:[%s]\n",i,field->data);
#endif
	}
		
	if(i != IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS;

	max_len = IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH + 1;
	if(out){
		field_len = ipv6_util_string_copy(out,max_len,IPV6_UTIL_UNCOMPRESS_IPV6_ADDRESS_FORMAT, address.fields[0].data
																			  , address.fields[1].data
																			  , address.fields[2].data
																			  , address.fields[3].data
																			  , address.fields[4].data
																			  , address.fields[5].data
																			  , address.fields[6].data
																			  , address.fields[7].data);
		if(field_len != IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS_LENGTH;

		TO_LOWER_CASE(out);
	}
	return IPV6_UTIL_SUCCESS;
}

static int uncompress_ipv6_address_compress_middle(char *in, char *out, int max_len)
{
    IPV6_UTIL_V6ADDR address;
    IPV6_UTIL_V6ADDR_FIELD *field;

	char begin[IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH+1];
	char end[IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH+1];
    char *temp;
    char *split;

    int being_colon_count;
    int end_colon_count;
    int compress_colon_count;
    int field_len;
    int len;
    int i;

	if(!in) return IPV6_UTIL_ERROR_INPUT_DATA_NULL;
    if(max_len <= IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS_LENGTH;

    memset(&address,0x00,sizeof(address));

	temp = in;

	split = strstr(temp,IPV6_UTIL_DOUBLE_COLON_STRING);
	if(!split) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS;
	
	len = split - temp;

	ipv6_util_string_copy(begin,len+1,"%s",temp);
	ipv6_util_string_copy(end,sizeof(end),"%s",split+IPV6_UTIL_DOUBLE_COLON_LENGTH);
	
	being_colon_count = get_ipv6_address_colon_count(begin);
	being_colon_count++;
	end_colon_count = get_ipv6_address_colon_count(end);
	end_colon_count++;
	compress_colon_count = IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT - end_colon_count;

	temp = begin;
	for(i = 0 ; i < being_colon_count; i++){
        field = &address.fields[i];
        split = strchr(temp,IPV6_UTIL_COLON_CODE);
        if(!split){
            field_len = strlen(temp);
			if(set_uncompress_ipv6_address_field_value(temp,field->data,field_len) < 0) break;
        }
        else{
            field_len = split - temp;
			if(set_uncompress_ipv6_address_field_value(temp,field->data,field_len) < 0) break;
        }
        temp = split + 1;
#ifdef _IPV6_UTIL_DEBUG
        printf("%d:[%s]\n",i,field->data);
#endif
    }

	for(i  ; i < compress_colon_count; i++){
		field = &address.fields[i];
        strncpy(field->data,IPV6_UTIL_OMIT_LEADING_ZEROS_FOUR,IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_LENGTH);
#ifdef _IPV6_UTIL_DEBUG
        printf("%d:[%s]\n",i,field->data);
#endif
	}

	temp = end;
	for(i  ; i < IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT; i++){
        field = &address.fields[i];
        split = strchr(temp,IPV6_UTIL_COLON_CODE);
        if(!split){
			if(i != IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT-1) break;
            field_len = strlen(temp);
			if(set_uncompress_ipv6_address_field_value(temp,field->data,field_len) < 0) break;
        }
        else{
            field_len = split - temp;
			if(set_uncompress_ipv6_address_field_value(temp,field->data,field_len) < 0) break;
        }
        temp = split + 1;
#ifdef _IPV6_UTIL_DEBUG
        printf("%d:[%s]\n",i,field->data);
#endif
    }

	if(i != IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS;

    max_len = IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH + 1;
    if(out){
        field_len = ipv6_util_string_copy(out,max_len,IPV6_UTIL_UNCOMPRESS_IPV6_ADDRESS_FORMAT, address.fields[0].data
                                                                              , address.fields[1].data
                                                                              , address.fields[2].data
                                                                              , address.fields[3].data
                                                                              , address.fields[4].data
                                                                              , address.fields[5].data
                                                                              , address.fields[6].data
                                                                              , address.fields[7].data);
        if(field_len != IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS_LENGTH;

        TO_LOWER_CASE(out);
    }
    return IPV6_UTIL_SUCCESS;	
}

static int uncompress_ipv6_address_compress_end(char *in, char *out, int max_len)
{
	IPV6_UTIL_V6ADDR address;
	IPV6_UTIL_V6ADDR_FIELD *field;

	char *temp;
	char *split;
	
	int total_colon_count;
	int compress_colon_count;
	int field_len;
	int i;

	if(!in) return IPV6_UTIL_ERROR_INPUT_DATA_NULL;
	if(max_len <= IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS_LENGTH;

	memset(&address,0x00,sizeof(address));

	total_colon_count = get_ipv6_address_colon_count(in);
	if(total_colon_count <= 0) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS;

	compress_colon_count = IPV6_UTIL_MAX_COLON_COUNT - total_colon_count + IPV6_UTIL_OMIT_COMPRESS_BEGIN_TYPE_COLON_COUNT;

	temp = in;

	for(i = 0; i < IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT - compress_colon_count; i++){
	    field = &address.fields[i];
		split = strchr(temp,IPV6_UTIL_COLON_CODE);
		if(!split) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS;

		field_len = split - temp;
		if(set_uncompress_ipv6_address_field_value(temp,field->data,field_len) < 0) break;
		temp = split + 1;
#ifdef _IPV6_UTIL_DEBUG
		printf("%d:[%s]\n",i,field->data);
#endif
	}

	for(i ; i< IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT; i++){
		field = &address.fields[i];
		strncpy(field->data,IPV6_UTIL_OMIT_LEADING_ZEROS_FOUR,IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_LENGTH);
#ifdef _IPV6_UTIL_DEBUG
		printf("%d:[%s]\n",i,field->data);
#endif
	}
		
	if(i != IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS;

	max_len = IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH + 1;
	if(out){
		field_len = ipv6_util_string_copy(out,max_len,IPV6_UTIL_UNCOMPRESS_IPV6_ADDRESS_FORMAT, address.fields[0].data
																			  , address.fields[1].data
																			  , address.fields[2].data
																			  , address.fields[3].data
																			  , address.fields[4].data
																			  , address.fields[5].data
																			  , address.fields[6].data
																			  , address.fields[7].data);
		if(field_len != IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS_LENGTH;

		TO_LOWER_CASE(out);
	}
	return IPV6_UTIL_SUCCESS;
}

static int compress_ipv6_address(char *in, char *out, int max_len)
{
    IPV6_UTIL_V6ADDR address;
    IPV6_UTIL_V6ADDR_FIELD *field;

    char *temp;
    char *split;

	int omit_flag = IPV6_UTIL_OMIT_LEADING_ZEROS_START_FLAG;
	int compress_flag = IPV6_UTIL_INIT_VALUE;
    int field_len = IPV6_UTIL_INIT_VALUE;
	int compress_count = IPV6_UTIL_INIT_VALUE;
    int i,j;

	if(!in) return IPV6_UTIL_ERROR_INPUT_DATA_NULL;
    if(max_len <= IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH) return IPV6_UTIL_ERROR_INVALID_IPV6_ADDRESS_LENGTH;

    memset(&address,0x00,sizeof(address));

	temp = in;
#ifdef _IPV6_UTIL_DEBUG
    printf("compress_ipv6_address: in(%s)\n",in);
#endif
	for(i = 0; i < IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT; i++){
		field = &address.fields[i];
		while(*temp != IPV6_UTIL_COLON_CODE && *temp){
#ifdef _IPV6_UTIL_DEBUG
			printf("compress_ipv6_address: in(0x0%x)\n",*temp);
#endif
			if(*temp != 0x30 || ( *temp == 0x30 && omit_flag == IPV6_UTIL_OMIT_LEADING_ZEROS_END_FLAG)){
				omit_flag = IPV6_UTIL_OMIT_LEADING_ZEROS_END_FLAG;
				field->data[field_len++] = *temp++;
			}else{
				if(*temp == 0x30 && field_len == IPV6_UTIL_INIT_VALUE) field->compress_type = IPV6_UTIL_IPV6_ADDRESS_OMIT_LEADING_ZEROS_TYPE;
				temp++;
			}
		}
		if(field_len == IPV6_UTIL_INIT_VALUE){
			field->compress_type = IPV6_UTIL_IPV6_ADDRESS_COMPRESS_TARGET_OMIT_LEADING_ZEROS_TYPE;
			strcpy(field->data,IPV6_UTIL_OMIT_LEADING_ZEROS_ONE);
		}
		temp++;
		field_len = IPV6_UTIL_INIT_VALUE;
		omit_flag = IPV6_UTIL_OMIT_LEADING_ZEROS_START_FLAG;
#ifdef _IPV6_UTIL_DEBUG
		printf("%d: type(%d), data(%s)\n",i,field->compress_type,field->data);	
#endif
	}

	for(i = 0,j=IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT-1; i < IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT; i++,j--){
		field = &address.fields[j];
		if(field->compress_type == IPV6_UTIL_IPV6_ADDRESS_COMPRESS_TARGET_OMIT_LEADING_ZEROS_TYPE){
#ifdef _IPV6_UTIL_DEBUG
			printf("1------%d: type(%d), data(%s)\n",j,field->compress_type, field->data);	
#endif
			if(compress_flag == IPV6_UTIL_INIT_VALUE){
				field->compress_type = IPV6_UTIL_IPV6_ADDRESS_COMPRESS_TYPE;
				compress_flag = IPV6_UTIL_COMPRESS_START_FLAG;
				compress_count++;
			}
			else{ 
				field->compress_type = IPV6_UTIL_IPV6_ADDRESS_COMPRESS_TYPE;
				compress_count++;
			}
#ifdef _IPV6_UTIL_DEBUG
			printf("1------%d: type(%d), data(%s)\n",j,field->compress_type, field->data);	
#endif
		}
		else{
#ifdef _IPV6_UTIL_DEBUG
			printf("2------%d: type(%d), data(%s)\n",j,field->compress_type, field->data);	
#endif
			if(compress_flag == IPV6_UTIL_COMPRESS_START_FLAG &&  compress_count > IPV6_UTIL_INIT_VALUE){ 
				compress_flag = IPV6_UTIL_COMPRESS_END_FLAG; 
				break;
			}
#ifdef _IPV6_UTIL_DEBUG
			printf("2------%d: type(%d), data(%s)\n",j,field->compress_type, field->data);	
#endif
		}
	}

	field_len = IPV6_UTIL_INIT_VALUE;
	compress_flag = IPV6_UTIL_INIT_VALUE;

	for(i = 0; i < IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT; i++){
        field = &address.fields[i];
		if(field->compress_type != IPV6_UTIL_IPV6_ADDRESS_COMPRESS_TYPE){
			if(compress_flag == IPV6_UTIL_COMPRESS_START_FLAG){
				if(i == IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT -1) field_len += ipv6_util_string_copy(out+field_len,max_len - field_len,":%s",field->data);
				else field_len += ipv6_util_string_copy(out+field_len,max_len - field_len,":%s:",field->data);
				compress_flag = IPV6_UTIL_COMPRESS_END_FLAG;		
			}
			else{ 
				if(i == IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT -1) field_len += ipv6_util_string_copy(out+field_len,max_len - field_len,"%s",field->data);
				else field_len += ipv6_util_string_copy(out+field_len,max_len - field_len,"%s:",field->data);
			}
		}
		else{
			if(compress_flag == IPV6_UTIL_INIT_VALUE) compress_flag = IPV6_UTIL_COMPRESS_START_FLAG;
			if(i == IPV6_UTIL_MAX_IPV6_ADDRESS_FIELD_COUNT -1 || field_len == IPV6_UTIL_INIT_VALUE) 
				field_len += ipv6_util_string_copy(out+field_len,max_len - field_len,"%c",IPV6_UTIL_COLON_CODE);
		}
	}	

	return IPV6_UTIL_SUCCESS;
}

static int uncompress_ipv6_address_compress(char *in, char *out, int max_len)
{
	char *temp;	
	int ret;

	if(!in) return IPV6_UTIL_ERROR_INPUT_DATA_NULL;

	temp = in;
	ret = get_ipv6_address_compress_type(temp);
	if(ret < 0) return ret;

	switch(ret)
	{
		case IPV6_UTIL_IPV6_ADDRESS_COMPRESS_BEGIN_TYPE: ret = uncompress_ipv6_address_compress_begin(in, out, max_len); break; 
		case IPV6_UTIL_IPV6_ADDRESS_COMPRESS_MIDDLE_TYPE: ret = uncompress_ipv6_address_compress_middle(in, out, max_len); break;
		case IPV6_UTIL_IPV6_ADDRESS_COMPRESS_END_TYPE: ret = uncompress_ipv6_address_compress_end(in, out, max_len); break;
		default: ret = IPV6_UTIL_ERROR_IPV6_ADDRESS_NOT_COMPRESS_TYPE; break;

	}	

	return ret;
}

static int get_compress_ipv6_address(char *in, char *out, int max_len)
{
	int ret;
    int type;
    ret = verify_ipv6_address_format(in);
    if(ret < 0) return ret;

    type = get_ipv6_address_type(in);
    if(type < 0) return ret;

    switch(type)
    {
		case IPV6_UTIL_IPV6_ADDRESS_NORMAL_TYPE:ret = compress_ipv6_address(in, out, max_len); break;
        default: ret = type; if(out) ipv6_util_string_copy(out,max_len,"%s",in); break;
    }
    return ret;
}

static int get_uncompress_ipv6_address(char *in, char *out, int max_len)
{
	int ret;
	int type;
	ret = verify_ipv6_address_format(in);
    if(ret < 0) return ret;

	type = get_ipv6_address_type(in);
	if(type < 0) return ret;

	switch(type)
	{
		case IPV6_UTIL_IPV6_ADDRESS_OMIT_LEADING_ZEROS_TYPE: ret = uncompress_ipv6_address_omit_leading_zeros(in, out, max_len); break;
		case IPV6_UTIL_IPV6_ADDRESS_COMPRESS_TYPE: ret = uncompress_ipv6_address_compress(in, out, max_len); break;
		default: ret = IPV6_UTIL_IPV6_ADDRESS_NORMAL_TYPE; if(out) ipv6_util_string_copy(out,max_len,"%s",in); break;
	}
	return ret;
}

static int convert_ipv6_address(IPV6_UTIL_ST* inst)
{
	int ret;
	int type;

	if(!inst) return IPV6_UTIL_ERROR_INPUT_DATA_NULL;
	
#ifdef _IPV6_UTIL_DEBUG	
	printf("ip : %s\n",inst->src_ip);
#endif

	ret = get_ip_address_version_type(inst->src_ip);
	if(ret != IPV6_UTIL_IPV6_ADDRESS_TYPE){
#ifdef _IPV6_UTIL_DEBUG
		printf("get_ip_address_version_type: ret(%d)\n",ret);
#endif
		inst->type = ret;
		if(ret == IPV6_UTIL_IPV4_MAPPED_IPV6_ADDRESS_TYPE){
			ipv6_util_string_copy(inst->ipv4_mapped_ipv6,sizeof(inst->ipv4_mapped_ipv6),"%s",inst->src_ip);
			ret = convert_ipv4_mapped_ipv6_to_ipv4(inst->src_ip,inst->convert_ipv4,sizeof(inst->convert_ipv4));	
			if(ret < 0){ 
#ifdef _IPV6_UTIL_DEBUG
				printf("convert_ipv4_mapped_ipv6_to_ipv4: ret(%d)\n",ret); 
#endif
			return ret;
			}	
#ifdef _IPV6_UTIL_DEBUG
			printf("convert_ipv4_mapped_ipv6_to_ipv4: out(%s)\n",inst->convert_ipv4);
#endif
		}else{
			ipv6_util_string_copy(inst->convert_ipv4,sizeof(inst->convert_ipv4),"%s",inst->src_ip);
		}
		return ret;
	}

	inst->type = ret;

	type = get_ipv6_address_type(inst->src_ip);
    if(type < 0) return type;
	
#ifdef _IPV6_UTIL_DEBUG
	printf("ip_address_type: %d\n",type);
#endif
	inst->compress_type = type;

	if(type == IPV6_UTIL_IPV6_ADDRESS_NORMAL_TYPE){ 
		ipv6_util_string_copy(inst->uncompress_ip,sizeof(inst->uncompress_ip),"%s",inst->src_ip);
		ret = get_compress_ipv6_address(inst->src_ip, inst->compress_ip, sizeof(inst->compress_ip));
	}
	else{
		ipv6_util_string_copy(inst->compress_ip,sizeof(inst->compress_ip),"%s",inst->src_ip);
	   	ret = get_uncompress_ipv6_address(inst->src_ip, inst->uncompress_ip, sizeof(inst->uncompress_ip));
	}
  return ret;
}
static void set_ipv6_util_ip_address(IPV6_UTIL_ST* inst, char* ip)
{
	if(!inst) return ;

	inst->type = IPV6_UTIL_INIT_VALUE;
	inst->compress_type = IPV6_UTIL_INIT_VALUE;
	memset(inst->compress_ip,0x00,IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH+1);
	memset(inst->uncompress_ip,0x00,IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH+1);
	memset(inst->convert_ipv4,0x00,IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH+1);
	memset(inst->ipv4_mapped_ipv6,0x00,IPV6_UTIL_MAX_IPV6_ADDRESS_LENGTH+1);

	ipv6_util_string_copy(inst->src_ip,IPV6_UTIL_MAX_IPV4_MAPPED_IPV6_ADDRESS_LENGTH+1,"%s",ip);
}

static char* get_ipv6_util_ipv4_address(IPV6_UTIL_ST* inst)
{
	if(!inst) return NULL;

	if(strlen(inst->convert_ipv4) > 0) return (char*)inst->convert_ipv4;
	return NULL;
}
static char* get_ipv6_util_ipv4_mapped_address(IPV6_UTIL_ST* inst)
{
	if(!inst) return NULL;

	if(strlen(inst->ipv4_mapped_ipv6) > 0) return (char*)inst->ipv4_mapped_ipv6;
	return NULL;
}

static char* get_ipv6_util_ipv6_compress_address(IPV6_UTIL_ST* inst)
{
	if(!inst) return NULL;

	if(strlen(inst->compress_ip) > 0) return (char*)inst->compress_ip;
	return NULL;
}

static char* get_ipv6_util_ipv6_uncompress_address(IPV6_UTIL_ST* inst)
{
	if(!inst) return NULL;

	if(strlen(inst->uncompress_ip) > 0) return (char*)inst->uncompress_ip;
	return NULL;
}


static char* get_ipv6_util_convert_ip(IPV6_UTIL_ST* inst)
{
	if(!inst) return NULL;
	
	char* ip;

	if(inst->type == IPV6_UTIL_INIT_VALUE) return NULL;

	if(inst->type != IPV6_UTIL_IPV6_ADDRESS_TYPE){
		if(inst->type == IPV6_UTIL_IPV4_MAPPED_IPV6_ADDRESS_TYPE) ip = (char*)inst->convert_ipv4;
		else ip = (char*)inst->src_ip;
	}
	else{
		if(inst->compress_type == IPV6_UTIL_IPV6_ADDRESS_NORMAL_TYPE) ip = (char*)inst->compress_ip;
		else ip = (char*)inst->uncompress_ip;
	}
	return ip;
}

int init_ipv6_util(IPV6_UTIL_ST* inst)
{
	if(!inst) return IPV6_UTIL_ERROR_INPUT_DATA_NULL;

	memset(inst,0x00,sizeof(IPV6_UTIL_ST));

	inst->set_ip_address = set_ipv6_util_ip_address;
	inst->get_ip_address_version = get_ip_address_version_type;
	inst->get_ipv6_address_type = get_ipv6_address_type;
	inst->get_convert_ip_address = get_ipv6_util_convert_ip;
	inst->get_ipv4_address = get_ipv6_util_ipv4_address;
	inst->get_ipv4_mapped_address = get_ipv6_util_ipv4_mapped_address;
	inst->get_ipv6_compress_address = get_ipv6_util_ipv6_compress_address;
	inst->get_ipv6_uncompress_address = get_ipv6_util_ipv6_uncompress_address;
	inst->convert = convert_ipv6_address;
	inst->verify_ipv6_address = verify_ipv6_address_format;
	return IPV6_UTIL_SUCCESS;
}

