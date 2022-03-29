#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib.h"

int SubMain( char *sProcessName, char *sFileName, char *sFileFg )
{
	int ret;
	char sFilePath[ BUFSIZ ]; 
	char sVanFileName[ BUFSIZ ]; 
	char sRowId[ 18 + 1 ];
	PROC_INFO proc_info;
	HTTPDATA httpData;

	LOG( 5, _OK_, "======================================================================" );
	LOG( 5, _OK_, "START: FILE_NAME(%s) FILE_FG(%s)", sFileName, sFileFg );

	/****************************************************************/
	/* Process 기초 정보 등록                                       */
	/****************************************************************/
	memset( (char *)&proc_info, 0x00, sizeof( proc_info ) );
	ret = SetProcInfo( sProcessName, sFileName, sFileFg, &proc_info );
	if( ret < 0 ) {
		LOG( 9, _ERROR_, "File Format(%s)", sFileName );
		return -1;
	}

	/****************************************************************/
	/* Process 세부 정보 등록 / 전송 파일명 생성                    */
	/****************************************************************/
	memset( sRowId, 0x00, sizeof( sRowId ) );
	ret = ProcInfoDDCSelect( &proc_info, sRowId );
	if( ret < 0 ) {
		move_file( proc_info.src_dir, info.error_dir, sFileName, NULL );
		LOG( 5, _OK_, "END: File Move(%s/%s)", info.error_dir, sFileName );
		return -1;
	}
	ShowInfoData( proc_info );

	/****************************************************************/
	/* 실제 파일 확인                                               */
	/****************************************************************/
	memset( sFilePath, 0x00, sizeof( sFilePath ) );
	sprintf( sFilePath, "%s/%s", proc_info.src_dir, sFileName );
	proc_info.file_size = GetFileSize( sFilePath );
	if( proc_info.file_size < 0 ) {
		LOG( 9, _ERROR_, "해당 파일이 없습니다!!(%s)", sFilePath );
		return -1;
	}

	/****************************************************************/
	/* Process 진행 시작 정보 등록                                  */
	/****************************************************************/
	ret = TPROCESS_INFO_Insert( PROCESS_ING, SEND, &proc_info );
	if( ret < 0 ) {
		move_file( proc_info.src_dir, info.error_dir, sFileName, NULL );
		LOG( 9, _OK_, "END: File Move(%s/%s)", info.error_dir, sFileName );
		return -1;
	}
	COMMIT();
	LOG( 5, _OK_, "TPROCESS_INFO Insert. RUN_DATE(%s) RUN_CNT(%d)", proc_info.run_date, proc_info.run_cnt );

	/****************************************************************/
	/* HTTP 정보 초기화                                             */
	/****************************************************************/
	memset( (char *)&httpData, 0x00, sizeof( httpData ) );
	ret = initHttpUtils(&httpData);
	if (ret < 0) {
		move_file( proc_info.src_dir, info.error_dir, sFileName, NULL );
		LOG( 9, _ERROR_, "initHttpUtils. ret(%d)", ret );
		return -1;
	}
	
	/****************************************************************/
	/* 청구 파일 전송						*/
	/****************************************************************/
	ret = SAMSUNGSend( &proc_info, &httpData, sFileName );
	if( ret < 0 ) {
		ErrorProcess( proc_info, "전문 송신 실패!!" );
		move_file( proc_info.src_dir, info.error_dir, sFileName, NULL );
		LOG( 9, _ERROR_, "SAMSUNG Send. ret(%d)", ret );
		LOG( 5, _OK_, "END: File Move(%s/%s)", info.error_dir, sFileName );
		return -1;
	}

	LOG( 5, _OK_, "SAMSUNG Send. FILE_NAME(%s) SIZE(%d)", sFileName, ret );

	/****************************************************************/
	/* 파일 송신 정보 Update                                        */
	/****************************************************************/
	ret = TDDC_FILE_Update( sRowId );
	if( ret < 0 ) {
		ROLLBACK();
		move_file( proc_info.src_dir, info.error_dir, sFileName, NULL );
		ErrorProcess( proc_info, "파일 송신 완료 적용 실패!!" );
		LOG( 5, _OK_, "END: File Move(%s/%s)", info.error_dir, sFileName );
		return -1;
	}

	/****************************************************************/
	/* Process 진행 완료 정보 적용                                  */
	/****************************************************************/
	ret = TPROCESS_INFO_Update( proc_info.site_cd, proc_info.run_date, proc_info.run_cnt, PROCESS_OK );
	if( ret < 0 ) {
		ROLLBACK();
		move_file( proc_info.src_dir, info.error_dir, sFileName, NULL );
		LOG( 5, _OK_, "END: File Move(%s/%s)", info.error_dir, sFileName );
		return -1;
	}
	LOG( 5, _OK_, "TPROCESS_INFO Update. STATE(%d)", PROCESS_OK );

	COMMIT();

	ret = move_file( proc_info.src_dir, proc_info.backup_dir, sFileName, NULL );
	if( ret < 0 ) {
		LOG( 9, _ERROR_, "File Backup. ret(%d)", ret );
		LOG( 9, _ERROR_, "#### (%s/%s)->(%s/%s)", proc_info.src_dir, sFileName, proc_info.backup_dir, sFileName );
		return -1;
	}
	LOG( 5, _OK_, "END: File Backup(%s/%s)", proc_info.backup_dir, sFileName );
	
	return 0;
}

int SetProcInfo( char *sProcessName, char *sFileName, char *file_fg, PROC_INFO *proc_info )
{
	char sBuff[ BUFSIZ ];

	strncpy( proc_info->file_name, sFileName, strlen( sFileName ) );
	strncpy( proc_info->process_name, sProcessName, strlen( sProcessName ) );

	proc_info->process_id = getpid();
	proc_info->pck_size = 0;

	memset( sBuff, 0x00, sizeof( sBuff ) );
	get_filed( sFileName, '.', 1, sBuff );
	if( strlen( sBuff ) <= 0 ) {
		return -1;
	}
	switch( strlen( sBuff ) ) {
		case 6:
			sprintf( proc_info->req_date, "20%.6s", sBuff );
			break;
		default:
			strncpy( proc_info->req_date, sBuff, strlen( sBuff ) );
			break;
	}

	strncpy( proc_info->file_fg, file_fg, strlen( file_fg ) );

	return 0;
}

int initHttpUtils(HTTPDATA *httpData )
{
	char sIniPath[50 + 1];
	char temp[30 + 1];
	char app_id[30 + 1];
	char command[10 + 1];
	char service_id[50 + 1];
	char process_nm[30 + 1];
	int ret;

	memset(temp, 0x00, sizeof(temp));
	ret = GetEnv( "SMWINIDIR", temp );
	if( ret < 0 ) {
        	LOG( 9, _ERROR_, "getenv( FILEDIR ): 환경변수를 확인하세요!!" );
	        return -1;
	}

	memset(sIniPath, 0x00, sizeof(sIniPath));
	sprintf(sIniPath, "%s%s", temp, "/bgd_daemon.ini");

	memset( command, 0x00, sizeof( command ) );
	ReadString( "HTTP_COMMON", "COMMAND", "CM03", command, sizeof( command ) - 1, sIniPath );
	PAD( command );
	strncpy( httpData->command, command, strlen( command ) );

	memset( app_id, 0x00, sizeof( app_id ) );
	ReadString( "HTTP_COMMON", "APP_ID", "SP_TEST_V1", app_id, sizeof( app_id ) - 1, sIniPath );
	PAD( app_id );
	strncpy( httpData->appId, app_id, strlen( app_id ) );

	memset( service_id, 0x00, sizeof( service_id ) );
	ReadString( "HTTP_COMMON", "SERVICE_ID", "tcd/api/tcp/send", service_id, sizeof( service_id ) - 1, sIniPath );
	PAD( service_id );
	strncpy( httpData->serviceId, service_id, strlen( service_id ) );

	memset( process_nm, 0x00, sizeof( process_nm ) );
	ReadString( "HTTP_COMMON", "PROCESS_NM", "test", process_nm, sizeof( process_nm ) - 1, sIniPath );
	PAD( process_nm );
	strncpy( httpData->processNm, process_nm, strlen( process_nm ) );

	LOG( 5, _OK_, "APP_ID     : %s", httpData->appId );
	LOG( 5, _OK_, "COMMAND    : %s", httpData->command );
	LOG( 5, _OK_, "SERVICE_ID : %s", httpData->serviceId );
	LOG( 5, _OK_, "PROCESS_NM : %s", httpData->processNm );

	return 0;
}
