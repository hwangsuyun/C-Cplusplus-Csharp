#define	__TF	"inifile_lib.c"

#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	<sys/stat.h>
#include	<stdarg.h>
#include	<unistd.h>

#include	"lib_fn.h"

int File_Exist(char *f_name)
{
static char *__fn = "File_Exist";

	if(access(f_name, R_OK) < 0) {
		dbg(2, "ERR: File_Exist. (%s) is not exist. <%s:%s>", f_name, __fn, __TF);
		return(-1);
	}
	return(0);
}

int Chk_DigitStr(char *str)
{
static char *__fn = "Chk_DigitStr";

	while(*str) {
		if((*str < '0') || (*str > '9')) {
			return(-1);
		}
		str++;
	}
	return(0);
}

int ini_readstr_buff(char *ini_file, char *block, char *title, char *arr_buff, int record_sz, int max_cnt)
{
static char *__fn = "ini_readstr_buff";

	int     r, i, cnt=0;
	char    tmp_buff[40000];
	char	*tptr, *tptr1, *arr_ptr;

	dbg(1, "START: ini_file(%s). block(%s). title(%s). record_sz(%d). max_cnt(%d). <%s:%s>", 
			ini_file, block, title, record_sz, max_cnt, __fn, __TF);

	r = ini_readstring(ini_file, block, title, tmp_buff, " ");
	if(r < 0) {
		dbg(2, "ERR: %s. ini_readstring(%s, %s, %s) ERROR <%s:%s>", __fn, ini_file, block, title, __fn, __TF);
		return(-1);
	}
	dbg(2, "OK: %s. ini_readstring(%s, %s, %s). tmp_buff(%s) <%s:%s>", __fn, ini_file, block, title, tmp_buff, __fn, __TF);

	cnt = FSstr_to_arr(tmp_buff, ';', arr_buff, record_sz, max_cnt);
	if(cnt < 0) {
		dbg(2, "ERR: FSstr_to_arr Error. (%s). fs(%c). <%s:%s>",
			tmp_buff, ';', __fn, __TF);
		return(-1);
	}
	return(cnt);
}


int ini_readinteger(char *ini_file, char *block, char *title, int defval)
{
static char *__fn = "ini_readinteger";

	int	r;
	char	inival[1024];

	dbg(2, "START: ini_readinteger(%s, %s, %s, %d) <%s:%s>", ini_file, block, title, defval, __fn, __TF);

	memset(inival, 0x0, sizeof(inival));
	r = ini_readstring(ini_file, block, title, inival, " ");
	if(r < 0) {
		dbg(2, "ERR: ini_readstring <%s:%s>", __fn, __TF);
		return(defval);
	}
	dbg(1, "OK: ini_readstring(%s) <%s:%s>", inival, __fn, __TF);

	trim_string(inival);

	dbg(1, "inival(%s) <%s:%s>", inival, __fn, __TF);

	if(Chk_DigitStr(inival) < 0) {
		dbg(2, "ERR: not integer string(%s) <%s:%s>", inival, __fn, __TF);
		return(defval);
	}

	r = atoi(inival);
	dbg(2, "OK: ini_readinteger. integer value(%d). OK <%s:%s>", r, __fn, __TF);
	return(r);
}

int ini_readstring(char *ini_file, char *block, char *title, char *inival, char *defval)
{
static char *__fn = "ini_readstring";

	int r;
	FILE *fp;
	char	*t_ptr, buf[50000];
	dbg(1, "START: ini_readstring(%s, %s, %s). defval(%s) <%s:%s>", ini_file, block, title, defval, __fn, __TF);

	fp = fopen (ini_file , "r");
	if (fp == NULL){
		dbg(2, "ERR: ini_file(%s) Open Error <%s:%s>", ini_file, __fn, __TF);
		strcpy(inival, defval);
		return -2;
	}

	if(found_ini_block(fp, block) < 0) {
		dbg(2, "ERR: found_ini_block(%s) ERROR <%s:%s>", block, __fn, __TF);
		strcpy(inival, defval);
		fclose(fp);
		return(-1);
	}
	dbg(1, "block found(%s) <%s:%s>", block, __fn, __TF);

	while(!feof(fp)) {
		memset(buf, 0x00, sizeof(buf));
		if( !fgets(buf, sizeof(buf), fp)){
			dbg(2, "ERR: File END. block(%s). title(%s) not FOUND <%s:%s>", block, title, __fn, __TF);
			strcpy(inival, defval);
			fclose(fp);
			return(-1);
		}

		t_ptr = strchr(inival, '\n');
		if(t_ptr != NULL) {
			*t_ptr = 0x0;
		}
		if(strlen(buf) < 3) {
			continue;
		}

		if(buf[0] == '[') {
			dbg(2, "ERR: Next Block(%s) found <%s:%s>", buf, __fn, __TF);
			fclose(fp);
			return(-1);
		}

		if(get_title_str(buf, title, inival) < 0) {
			continue;
		}

		trim_string(inival);

		dbg(2, "OK: ini_readstring. inival(%s), title(%s) <%s:%s>", 
				inival, title, __fn, __TF);
		fclose(fp);
		return(0);
	}
	fclose(fp);
	dbg(2, "ERR: ini_readstring. file end. default val return(%s) <%s:%s>", inival, __fn, __TF);
	strcpy(inival, defval);
	return(-1);
}

int get_title_str(char *ini_data, char *title, char *inival)
{
static char *__fn = "get_title_str";

	int	i, s_len;
	char	*fp;
	char	*t_ptr, token[500];

	dbg(1, "Ini_data=%s <%s:%s>", ini_data, __fn, __TF);

	memset(token, 0x0, sizeof(token));
	t_ptr = Get_Token_Ch(ini_data, token, '=');
	if(t_ptr == NULL) {
		dbg(1, "not title token(%s) <%s:%s>", ini_data, __fn, __TF);
		return(-1);
	}

	if(strcmp(title, token) != 0) {
		dbg(1, "not title(%s). token(%s) <%s:%s>", title, token, __fn, __TF);
		return(-1);
	}

	strcpy(inival, t_ptr);
	dbg(1, "OK: get_ini_str(%s) <%s:%s>", inival, __fn, __TF);
	return(strlen(inival));
}

int found_ini_block(FILE *fp, char *block)
{
static char *__fn = "found_ini_block";

	int	r;
	char	buf[1024];
	int	start_flag = 1;


	while(!feof(fp)) {
		memset(buf, 0x00, sizeof(buf));
		if( !fgets(buf, 1024, fp)){
			if(start_flag) {
				dbg(2, "ERR: file read(fgets) error. <%s:%s>", __fn, __TF);
			}
			else {
				dbg(2, "ERR: FILE END. block(%s) Not Found <%s:%s>", block, __fn, __TF);
			}
			return(-1);
		}
		if(start_flag) {
			start_flag = 0;
		}

		if(buf[0] != '[' || buf[strlen(block) +1 ] != ']') {
			continue;
		}

		r = strncmp(buf+1, block, strlen(block));
		if (r == 0){
			dbg(1, "OK: block(%s) found <%s:%s>", buf, __fn, __TF);
			return(0);
		}
	}
	dbg(2, "ERR: file end. INI block(%s) not found. <%s:%s>", block, __fn, __TF);
	return(-1);
}

void l_trim_string(char *str_val)
{
static char *__fn = "l_trim_string";

	char	*s_ptr, *t0_ptr, *t_ptr;

	dbg(1, "START: l_trim_string(%s) <%s:%s>", str_val, __fn, __TF);

	t0_ptr = (char *)malloc(strlen(str_val)+1);

	t_ptr = t0_ptr;
	s_ptr = str_val;
	while(*s_ptr) {
		switch(*s_ptr) {
			case	' ' :
			case	'\t' :
				s_ptr++;
				break;
			
			default :
				while(*s_ptr) {
					*t_ptr = *s_ptr;
					t_ptr++;
					s_ptr++;
				}
				*t_ptr = 0x0;
		}
	}

	strcpy(str_val, t0_ptr);
	dbg(1, "END: l_trim_string(%s) <%s:%s>", str_val, __fn, __TF);
}

void r_trim_string(char *str_val)
{
static char *__fn = "r_trim_string";

	char	*s_ptr;
	int	i, s_len;

	dbg(1, "START: r_trim_string(%s) <%s:%s>", str_val, __fn, __TF);

	s_len = strlen(str_val);

	for(i=s_len; i>=0; i--) {
		switch(*(str_val+i)) {
			case	0x0 :
			case	' ' :
			case	'\t' :
			case	'\n' :
				*(str_val+i) = 0x0;
				break;
			default :
				dbg(1, "END: r_trim_string(%s) <%s:%s>", str_val, __fn, __TF);
				return;
		}
	}

	dbg(1, "END:22 r_trim_string(%s) <%s:%s>", str_val, __fn, __TF);
}

void trim_string(char *str_val)
{
static char *__fn = "trim_string";

	dbg(1, "START: trim_string(%s) <%s:%s>", str_val, __fn, __TF);

	l_trim_string(str_val);
	r_trim_string(str_val);

	dbg(1, "END: trim_string(%s) <%s:%s>", str_val, __fn, __TF);
}

char *Get_Token_Ch(char *s_ptr, char *ret_str, char fs_ch)
{
static char *__fn = "Get_Token_Ch";

	char	*t_ptr;
	int	len;

	dbg(1, "START: s_ptr(%s). fs_ch(%c) <%s:%s>", s_ptr, fs_ch, __fn, __TF);

	t_ptr = strchr(s_ptr, fs_ch);
	if(t_ptr == NULL) {
		dbg(1, "ERR: strchr('%s', '%c') <%s:%s>", s_ptr, fs_ch, __fn, __TF);
		return(NULL);
	}
	len = t_ptr - s_ptr;
	strncpy(ret_str, s_ptr, len);
	ret_str[len] = 0x0;
	dbg(1, "OK: ret_str(%s). t_ptr(%s) <%s:%s>", ret_str, t_ptr, __fn, __TF);
	

	trim_string(ret_str);
	dbg(1, "END: ret_str(%s). t_ptr(%s) <%s:%s>", ret_str, t_ptr, __fn, __TF);
	return(t_ptr+1);
}

int FSstr_to_arr(char *in_str, char fs, char *arr_buff, int record_sz, int max_cnt)
{
static char *__fn = "FSstr_to_arr";

	int     r, i, cnt=0;
	char    tmp_buff[2048];
	char	*tptr, *tptr1, *arr_ptr;

	dbg(1, "START: in_str(%s). fs(%c). record_sz(%d), max_cnt(%d). <%s:%s>", 
			in_str, fs, record_sz, max_cnt, __fn, __TF);

	arr_ptr = arr_buff;
	tptr = in_str;
	while(*tptr) {
		dbg(1, "CALL: Get_Token_Ch. tptr(%s), arr_ptr(%s), fs_ch(';') <%s:%s>", tptr, arr_ptr, __fn, __TF);
		tptr1 = Get_Token_Ch(tptr, arr_ptr, fs);
		if(tptr1 == NULL) {
			dbg(2, "WORN: %s. Get_Token_Ch(%s, %s, ';') Error <%s:%s>", __fn, tptr, arr_ptr, __fn, __TF);
			break;
		}
		dbg(1, "OK: Get_Token_Ch. token(%s). next_ptr(%s) <%s:%s>", arr_ptr, tptr1, __fn, __TF);

		tptr = tptr1;
		arr_ptr += record_sz;
		cnt++;

		for(i=0; i<cnt; i++) {
			dbg(1, "arr[%d]=%s. <%s:%s>", i, arr_buff+(i*record_sz), __fn, __TF);
		}

		if(cnt >= max_cnt) {
			dbg(9, "WORN: cnt(%d) over max_cnt(%d). <%s:%s>", cnt, max_cnt, __fn, __TF);
			break;
		}

		dbg(1, "while loop continue. tptr(%s). strlen(tptr)=%d. arr_ptr(%s). cnt(%d)", tptr, strlen(tptr), arr_ptr, cnt);
		if(!*tptr) {
			break;
		}
		if(strlen(tptr) == 0) {
			break;
		}
	}

	for(i=0; i<cnt; i++) {
		dbg(1, "arr[%d]=%s. <%s:%s>", i, arr_buff+(i*record_sz), __fn, __TF);
	}
	dbg(2, "OK: return(%d). <%s:%s>", cnt, __fn, __TF);
	return(cnt);
}
