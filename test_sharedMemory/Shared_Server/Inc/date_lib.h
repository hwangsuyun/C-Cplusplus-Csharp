#define	_DATE_H

#include <time.h>

#define T_YYYYMMDD          	1
#define T_YYMMDD            	2
#define T_MMDD              	3
#define T_HHMMSS            	4
#define T_HH_MM_SS          	5
#define T_MMSS              	6
#define T_MM_SS             	7
#define T_YYYYMMDDHHMMSS    	8
#define T_YYYYMMDDHH_MM_SS  	9
#define T_YYMMDDHHMMSS      	10
#define T_YYMMDDHH_MM_SS    	11
#define T_YYYY_MM_DD_HH_MM_SS   12
#define	T_MMDDHHMMSS			13

#define	T_HOUR					100
#define	T_MINUTE				101
#define	T_SECOND				102

/*
int is_same_date(char *s, struct tm *t);
char* get_sysdate(char* szvalue, int ntype);
char* get_time(char* szvalue,int ntype);
char* get_date(char* szvalue,int ntype);
int get_time_parse( int ntype );
int diff_time( time_t prevTime );
char *get_date_buff(char *dt_buff);
char *get_datetime_buff(char *dt_buff);
*/

int is_same_date(char *s, struct tm *t);
int get_hour(void);
char* get_sysdate(char* szvalue, int ntype);
int	getday();
void get_hhmmss(int *hh, int *mm, int *ss);
void get_syshandate(char * szvalue, int ntype);
char*  get_time(char* szvalue,int ntype);
char*  get_date(char* szvalue,int ntype);
int get_time_parse( int ntype );
time_t get_time_t();
int diff_time( time_t prevTime );
char *get_date_buff(char *dt_buff);
char *get_datetime_buff(char *dt_buff);
int Get_BeforeDate(char *date_buff, int day);
