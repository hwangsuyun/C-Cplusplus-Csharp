#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/wait.h> 
#include <stdio.h>

#include "common.h"
#include "auto_recv_nas_sftp.h"

char fileName[100][1024];
int cnt;

int sub_main( char *sIniPath )
{
	int	ret;
	int	i;
	int	fd;
	int	loop_cnt;
	char	site_list[ MAXSIZ ];
	char	section[ BUFSIZ ];
	char 	site_cd[ SITE_CD_LEN + 1 ];
	char 	file_fg[ FILE_FG_LEN + 1 ];
	char	error_path[ PATH_LEN + 1 ];
	char url[1024];
	char path[100];
	char backPath[100];
	char targetPath[100];
	SITE_INFO site_info;

	memset( site_list, 0x00, sizeof( site_list ) );
	ret = ReadString( "RECV", "LIST", "", site_list, sizeof( site_list ) - 1, sIniPath );
	LOG( 1, _DEBUG_, "site_list (%s)(%d)", site_list, ret ); 	

	loop_cnt = get_filed_count( site_list, ';' );
	if( loop_cnt <= 0 ) {
		return info.sleep_time;
	}
	LOG( 1, _DEBUG_, "get_filed_count(%d)", loop_cnt ); 	

	for( i=0; i<loop_cnt; i++ ) {
		memset( section, 0x00, sizeof( section ) );
		get_filed( site_list, ';', i, section );
		PAD( section );

		memset( site_cd, 0x00, sizeof( site_cd ) );
		get_filed( section, ',', 0, site_cd );
		PAD( site_cd );

		memset( file_fg, 0x00, sizeof( file_fg ) );
		get_filed( section, ',', 1, file_fg );
		PAD( file_fg );

		memset( (char *)&site_info, 0x00, sizeof( site_info ) );
		ReadString( section, "IP", "", site_info.ip, sizeof( site_info.ip ) - 1, sIniPath );
		ReadString( section, "ID", "", site_info.id, sizeof( site_info.id ) - 1, sIniPath );
		site_info.port = ReadInteger( section, "PORT", 0, sIniPath );
		ReadString( section, "PASSWD", "", site_info.passwd, sizeof( site_info.passwd ) - 1, sIniPath );
		ReadString( section, "SOURCE", "", site_info.source, sizeof( site_info.source ) - 1, sIniPath );
		ReadString( section, "BACKUP", "", site_info.backup, sizeof( site_info.backup ) - 1, sIniPath );
		ReadString( section, "TARGET", "", site_info.target, sizeof( site_info.target ) - 1, sIniPath );
		ReadString( section, "ERROR", "", site_info.error, sizeof( site_info.error ) - 1, sIniPath );
		ReadString( section, "PROCESS", "", site_info.process, sizeof( site_info.process ) - 1, sIniPath );

		LOG( 5, _OK_, "========================= [%s] =========================", section );
		LOG( 5, _OK_, "## SITE_CD   (%s)", site_cd );
		LOG( 5, _OK_, "## FILE_FG   (%s)", file_fg );
		LOG( 5, _OK_, "## IP        (%s)", site_info.ip );
		LOG( 5, _OK_, "## PORT      (%d)", site_info.port );
		LOG( 5, _OK_, "## ID        (%s)", site_info.id );
		LOG( 5, _OK_, "## PASSWD    (%s)", site_info.passwd );
		LOG( 5, _OK_, "## SOURCE    (%s)", site_info.source );
		LOG( 5, _OK_, "## BACKUP    (%s)", site_info.backup );
		LOG( 5, _OK_, "## TARGET    (%s)", site_info.target );
		LOG( 5, _OK_, "## ERROR     (%s)", site_info.error );

		memset(url, 0x00, sizeof(url));
		sprintf(url, "%s%s%s%s%s%s%s%d%s%s", "sftp://", site_info.id, ":", site_info.passwd, "@", site_info.ip, ":", site_info.port, site_info.source, "/");
		LOG( 5, _OK_, "url(%s)", url );
		
		ret = checkEndFile(url);
		if( ret != 0 ) {
			LOG( 9, _ERROR_, "failed checkEndFile(%d)(%s)", ret, url);
			continue; 
		}

		for (i = 0; i < cnt; i++)
		{
			LOG( 5, _OK_, "##### 수신 파일명 (%s)", fileName[i] );

			memset(path, 0x00, sizeof(path));
			memset(backPath, 0x00, sizeof(backPath));
			sprintf(path, "%s%s%s", site_info.source, "/", fileName[i]);
			sprintf(backPath, "%s%s%s", site_info.backup, "/", fileName[i]);
			sprintf(targetPath, "%s%s%s", site_info.target, "/", fileName[i]);
		
			ret = doWork( site_cd, file_fg, url, path, backPath, targetPath, fileName[i]);
			if( ret < 0 ) {
				LOG( 9, _ERROR_, "failed doWork(%d)(%s)(%s)(%s)", ret, site_cd, file_fg, fileName);
			}
		}	
	}

	return info.sleep_time;
}

int doWork( char *site_cd, char *file_fg, char *url, char *path, char *backPath, char *targetPath, char *fileName)
{
	int ret;
	int pid;
	int status;
	char fullUrl[1024];

	memset(fullUrl, 0x00, sizeof(fullUrl));
	sprintf(fullUrl, "%s%s", url, fileName);

	pid = fork();
	if (pid == 0)
	{
		// 1. 파일 이동
		ret = communicateSftpProtocol(fullUrl, path, backPath, targetPath);
		if (ret != 0)
		{
			LOG( 9, _ERROR_, "파일 수신 실패!! ret : (%d)", ret );
			exit(9);
		}

	        // 2. db 삽입
		ret = TRCV_INFO_insert(site_cd, file_fg, fileName);
		if (ret < 0)
  		{
   			LOG( 9, _ERROR_, "INSERT TRCV_INFO 에러 (%d)", ret );
			exit(9);
    	}
   		LOG( 5, _OK_, "TRCV_INFO 삽입 완료!!(%d)", ret );
		COMMIT();

		exit(0);
	}
	wait( &status );

	LOG( 1, _DEBUG_, "Excute Return (%d)", WEXITSTATUS( status ) );

	if( ( WEXITSTATUS( status ) == 0 ) || ( WEXITSTATUS( status ) == 1 ) ) {
		return 0;
	}
	LOG( 9, _ERROR_, "파일 수신 실패!! STATUS (%d)", WEXITSTATUS( status ) );

	return -1;
}
