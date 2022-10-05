#define		__TF	"debug.c"

#include			<stdarg.h>
#include			<unistd.h>

#include	"lib_fn.h"

int		__daemon_dbg_flag=0;
int		__log_debug_level; 
char		__log_file_name[1024];
int		__dbg_chg_flag = 0, __dbg_old_level;
int		dbg_fork_cnt = 0;
char		__process[100];

void	Daemon_Dbg_Init( char *name , int level, int new_flag, char *process)
{
static char *func = "debug_init";

	__daemon_dbg_flag = 1;

	strcpy( __log_file_name , name );
	__log_debug_level			= level;
	strcpy(__process, process);

	if(new_flag == 1) {
		rm_dbg_file();
	}
}

int rm_dbg_file()
{
	char 			tmp[100];
	char			buf[100];

	if(__daemon_dbg_flag) {
		sprintf(tmp, "%s/%s", __log_file_name, get_date(buf, T_YYYYMMDD));
	}
	else {
		strcpy(tmp, __log_file_name);
	}
	unlink(tmp);
	return 0;
}

void dbg( int level, char *format, ...)
{
	static char		buf[8000];
	char 			tmp[100];
	va_list			ap;

	if( __log_debug_level > level ) {
		return;
	}

	memset( buf, 0x00, sizeof(buf) );

	/* --- Make message --- */
	va_start(ap, format);
	vsprintf( buf, format, ap);
	va_end(ap);

	buf[4000]='$';
	buf[4001]='$';
	buf[4002]=0x0;
	dbg_write(level, __log_file_name, buf);
}

void Err_Log(char *err_dir, char *format, ...)
{
	static char		buf[8000];
	char 			tmp[100];
	va_list			ap;

	memset( buf, 0x00, sizeof(buf) );

	/* --- Make message --- */
	va_start(ap, format);
	vsprintf( buf, format, ap);
	va_end(ap);

	buf[7999]=0x0;
	err_write(err_dir, buf);
}

int err_write(char *f_name, char *dbg_data)
{
	char		date_buf[20];
	char		time_buf[20];
	char		file_name[100];

	char 			tmp[100];
	int				ret;
	va_list			ap;
	FILE			*log;
	char			logFile[101];

	sprintf(logFile, "%s/error_%s.error", f_name, get_date(date_buf, T_YYYYMMDD));

	ret	= access(f_name, F_OK );
	if( ret != 0 ) {
		ret = Create_Dir_Path(logFile);
		if(ret < 0) {
			ret = access(f_name, F_OK);
			if(ret != 0) {
				return 0;
			}
		}
	} 

	log = fopen( logFile , "a" );
	if( log == NULL ) {
		return 0;
	}

	fprintf(log,"[%d][%s] %s\n", dbg_fork_cnt, get_time(tmp, T_HHMMSS), dbg_data);

	fclose( log );
	return 0;
}

int dbg_write(int d_level, char *f_name, char *dbg_data)
{
	char		date_buf[20];
	char		time_buf[20];
	char		file_name[100];

	char 			tmp[100];
	int				ret;
	va_list			ap;
	FILE			*log;
	char			logFile[101];

	if(__daemon_dbg_flag) {
		if(f_name[0] == 0x0) {
			sprintf(logFile, "./UnSetDaemonDbg/%s", get_date(date_buf, T_YYYYMMDD));
		}
		else {
			sprintf(logFile, "%s/%s.%s", f_name, get_date(date_buf, T_YYYYMMDD), __process);
		}
	}
	else {
		strcpy(logFile, f_name);
	}

	ret	= access(f_name, F_OK );
	if( ret != 0 ) {
		ret = Create_Dir_Path(logFile);
		if(ret < 0) {
			ret = access(f_name, F_OK);
			if(ret != 0) {
				return 0;
			}
		}
	} 

	log = fopen( logFile , "a" );
	if( log == NULL ) {
		return 0;
	}

	fprintf(log,"[%d][%s] %s\n", dbg_fork_cnt, get_time(tmp, T_HHMMSS), dbg_data);

	fclose( log );
	return 0;
}
