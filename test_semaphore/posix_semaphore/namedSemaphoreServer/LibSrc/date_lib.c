#define	__TF	"date_lib.c"

#include <sys/types.h>

#include "lib_fn.h"

static char	*month_str[] = {
	"JAN", "FEB", "MAR", "APR", "MAY", "JUN", 
	"JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

/***********************************************************************
 함 수 명 : is_same_date
 기    능 : 지정한 날짜가 기준날자와 동일한지 검사

 파라메터 : s - 검사할 날짜 (형식: "YYYY-MM-DD") (입력)
		   t - struct tm 형식의 날짜 (입력)

 리 턴 값 : TRUE    - 날짜 동일
			FALSE   - 날짜 다름

 비    고 : YYYY-MM-DD 까지만 비교
***********************************************************************/
int
is_same_date(char *s, struct tm *t)
{
	int year, month, day;

	sscanf(s, "%d-%d-%d", &year, &month, &day);

	if ((year == (t->tm_year + 1900)) &&	
		(month == (t->tm_mon + 1)) &&	
		(day == (t->tm_mday)))
		return 1;
	else
		return 0;
}
int get_hour(void)
{
        long t;
    struct tm *tp, *localtime();
    int    ho;

    time((time_t *)&t);
    tp=localtime((time_t *)&t);

        ho  = tp->tm_hour;

        return ho;
}


/***********************************************************************
 함 수 명 : get_sysdate
 기    능 : 지정한 형식 날짜의 돌아오는 sysdate을 구한다

 파라메터 :	szvalue	- 받을 날짜(INOUT)
			ntype   - 날짜 형식(IN : date.h 참조)

 리 턴 값 : sysdate(문자열)
***********************************************************************/
char*
get_sysdate(char* szvalue, int ntype)
{
	long t;
	struct tm *tp, *localtime();
	int    yy, mm, dd, ho, mi, ss;

	time((time_t *)&t);
	tp=	localtime((time_t *)&t);

	yy  = tp->tm_year;
	mm  = tp->tm_mon + 1;
	dd  = tp->tm_mday;
	ho  = tp->tm_hour;
	mi  = tp->tm_min;
	ss  = tp->tm_sec;

    if( tp->tm_year > 100 )
       yy = 2000 + tp->tm_year - 100;
	else
       yy = 1900 + tp->tm_year;

	switch( ntype ) {
	case	T_YYYYMMDDHHMMSS	:
	        sprintf(szvalue,"%04d%02d%02d%02d%02d%02d",
		            yy,mm,dd,ho,mi,ss);
		    break;
    case	T_YYYYMMDDHH_MM_SS	:
            sprintf(szvalue,"%04d%02d%02d%02d:%02d:%02d",
		            yy,mm,dd,ho,mi,ss);
		    break;
    case    T_YYYY_MM_DD_HH_MM_SS  :
		    sprintf(szvalue,"%04d-%02d-%02d %02d:%02d:%02d",
		            yy,mm,dd,ho,mi,ss);
		    break;
    case	T_YYMMDDHHMMSS		:
            sprintf(szvalue,"%02d%02d%02d%02d%02d%02d",
			        yy%100,mm,dd,ho,mi,ss);
            break;
    case	T_YYMMDDHH_MM_SS	: 
		    sprintf(szvalue,"%02d%02d%02d%02d:%02d:%02d",
		            yy%100,mm,dd,ho,mi,ss);
		    break;
	case	T_MMDDHHMMSS	:
			sprintf(szvalue,"%02d%02d%02d%02d%02d",
					mm,dd,ho,mi,ss);
			break;
    default:
            sprintf(szvalue,"%04d%02d%02d%02d%02d%02d",
		            yy,mm,dd,ho,mi,ss);
		    break;
    }
   
	return szvalue;
}

int	getday()
{
	long t;
	struct tm *tp, *localtime();
	int    yy, mm, dd, ho, mi, ss;

	time((time_t *)&t);
	tp=	localtime((time_t *)&t);

	return tp->tm_wday;
}

void get_hhmmss(int *hh, int *mm, int *ss)
{
	long t;
	struct tm *tp, *localtime();

	time((time_t *)&t);
	tp=	localtime((time_t *)&t);

	*hh  = tp->tm_hour;
	*mm  = tp->tm_min;
	*ss  = tp->tm_sec;
}

void get_syshandate(char * szvalue, int ntype)
{
	long t;
	struct tm *tp, *localtime();
	int    yy, mm, dd, ho, mi, ss;

	time((time_t *)&t);
	tp=	localtime((time_t *)&t);

	yy  = tp->tm_year;
	mm  = tp->tm_mon + 1;
	dd  = tp->tm_mday;
	ho  = tp->tm_hour;
	mi  = tp->tm_min;
	ss  = tp->tm_sec;

    if( tp->tm_year > 100 )
       yy = 2000 + tp->tm_year - 100;
	else
       yy = 1900 + tp->tm_year;

	        sprintf(szvalue,"%04d년%02d월%02d일%02d시%02d분%02d초",
		            yy,mm,dd,ho,mi,ss);
}

/***********************************************************************
 함 수 명 : get_time
 기    능 : 지정한 형식 시간의 돌아오는 system time을 구한다

 파라메터 :	szvalue	- 받을 시간(INOUT)
			ntype   - 시간 형식(IN : date.h 참조)

 리 턴 값 : system time(문자열)
***********************************************************************/
char* 
get_time(char* szvalue,int ntype)
{
	long t;
    struct tm *tp, *localtime();
    int    yy, mm, dd, ho, mi, ss;

    time((time_t *)&t);
    tp=localtime((time_t *)&t);

	yy  = tp->tm_year;
	mm  = tp->tm_mon + 1;
	dd  = tp->tm_mday;
	ho  = tp->tm_hour;
	mi  = tp->tm_min;
	ss  = tp->tm_sec;

	if( tp->tm_year > 100 )
		yy = 2000 + tp->tm_year - 100;
	else
		yy = 1900 + tp->tm_year;

	switch( ntype ) {
	case	T_HHMMSS			:
			sprintf(szvalue,"%02d%02d%02d",ho,mi,ss);
			break;
	case	T_HH_MM_SS			:
			sprintf(szvalue,"%02d:%02d:%02d",ho,mi,ss);
			break;
	case	T_MMSS				:
			sprintf(szvalue,"%02d%02d",mi,ss);
			break;
	case	T_MM_SS				:
			sprintf(szvalue,"%02d:%02d",mi,ss);
			break;
	default						:
			sprintf(szvalue,"%02d%02d%02d",ho,mi,ss);
			break;
	}

	return szvalue;
}

/***********************************************************************
 함 수 명 : get_date
 기    능 : 지정한 형식 시간의 돌아오는 system date를 구한다

 파라메터 :	szvalue	- 받을 날짜(INOUT)
			ntype   - 날짜 형식(IN : date.h 참조)

 리 턴 값 : system date(문자열)
***********************************************************************/
char* 
get_date(char* szvalue,int ntype)
{
	long t;
    struct tm *tp, *localtime();
    int    yy, mm, dd, ho, mi, ss;

    time((time_t *)&t);
    tp=localtime((time_t *)&t);

	yy  = tp->tm_year;
	mm  = tp->tm_mon + 1;
	dd  = tp->tm_mday;
	ho  = tp->tm_hour;
	mi  = tp->tm_min;
	ss  = tp->tm_sec;

	if( tp->tm_year > 100 )
		yy = 2000 + tp->tm_year - 100;
	else
		yy = 1900 + tp->tm_year;

	switch( ntype ) {
	case	T_YYYYMMDD		:
			sprintf(szvalue,"%04d%02d%02d",
					yy,mm,dd);
			break;
	case	T_YYMMDD		:
			sprintf(szvalue,"%02d%02d%02d",
					yy%100,mm,dd);
			break;
	case	T_MMDD			:
			sprintf(szvalue,"%02d%02d",
					mm,dd);
			break;
	default:
			sprintf(szvalue,"%04d%02d%02d",
					yy,mm,dd);
			break;
	}

	return szvalue;
}

int get_time_parse( int ntype )
{
	long t;
    struct tm *tp, *localtime();
    int    ho, mi, ss;
	int    retVal = 0;

    time((time_t *)&t);
    tp=localtime((time_t *)&t);

	ho  = tp->tm_hour;
	mi  = tp->tm_min;
	ss  = tp->tm_sec;

	switch( ntype ) {
		case T_HOUR :
			retVal  = ho;
			break;
		case T_MINUTE :
			retVal  = mi;
			break;
		case T_SECOND :
			retVal  = ss;
			break;
	}

	return retVal;
}

time_t get_time_t()
{
	long	 t;

    return time((time_t *)&t);
}

int diff_time( time_t prevTime )
{
	long t;

    return difftime( time((time_t *)&t) , prevTime );
}

char *get_date_buff(char *dt_buff)
{
        long t;
        struct tm *tp, *localtime();
        int    yy, mm, dd, ho, mi, ss;

        time((time_t *)&t);
        tp=localtime((time_t *)&t);

        yy  = tp->tm_year;
        mm  = tp->tm_mon + 1;
        dd  = tp->tm_mday;

        if( tp->tm_year > 100 ) {
                yy = 2000 + tp->tm_year - 100;
        }
        else {
                yy = 1900 + tp->tm_year;
        }

        sprintf(dt_buff,"%04d%02d%02d", yy,mm,dd);
        return(dt_buff);
}

char *get_datetime_buff(char *dt_buff)
{
        long t;
        struct tm *tp, *localtime();
        int    yy, mm, dd, ho, mi, ss;

        time((time_t *)&t);
        tp=localtime((time_t *)&t);

        yy  = tp->tm_year;
        mm  = tp->tm_mon + 1;
        dd  = tp->tm_mday;
        ho  = tp->tm_hour;
        mi  = tp->tm_min;
        ss  = tp->tm_sec;

        if( tp->tm_year > 100 ) {
                yy = 2000 + tp->tm_year - 100;
        }
        else {
                yy = 1900 + tp->tm_year;
        }

        sprintf(dt_buff,"%04d%02d%02d%02d%02d%02d",yy,mm,dd,ho,mi,ss);
        return(dt_buff);
}

int Get_BeforeDate(char *date_buff, int day)
{
        long t;
        struct tm *tp, *localtime();
        int    yy, mm, dd, ho, mi, ss;

        time((time_t *)&t);

        if(day != 0){
                t = t - (86400 * day);
        }

        tp=     localtime((time_t *)&t);

        yy  = tp->tm_year;
        mm  = tp->tm_mon + 1;
        dd  = tp->tm_mday;
        ho  = tp->tm_hour;
        mi  = tp->tm_min;
        ss  = tp->tm_sec;

        if( tp->tm_year > 100 )
            yy = 2000 + tp->tm_year - 100;
        else
            yy = 1900 + tp->tm_year;

        sprintf(date_buff,"%04d%02d%02d", yy,mm,dd);

        return 0;
}

/* END OF FILE */
