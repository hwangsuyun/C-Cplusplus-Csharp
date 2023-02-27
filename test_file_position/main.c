#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>

pthread_mutex_t positionFile_lock;

void removeChr( char *pSrc, char c )
{
	char *pTmp = NULL;

	if( pSrc == NULL )
	{
		return;
	}

	pTmp = pSrc;

	for ( ; *pSrc != '\0' ; pSrc ++ )
	{
		if ( *pSrc != c )
		{
			*pTmp = *pSrc;
			pTmp ++;
		}
	}

	*pTmp = '\0';
}

char *readLine( char * line, int *idx )
{
	char *pToken = NULL;
	char *pFileName = NULL;

	if( NULL == line || NULL == idx )
		return NULL;

	removeChr(line,'\n');

	pToken = strstr(line, "LOGFILE");
	if ( pToken == NULL ) {
		return NULL;
	}
	pToken += strlen("LOGFILE");

	*idx = atoi( pToken );
	if ( *idx < 0 )
		*idx = 0;

	pFileName = strchr( pToken, '=' );
	if ( pFileName == NULL )
		return NULL;
	pFileName++;

	pToken = strchr( pFileName, '>' );
	if ( pToken )
		*pToken = 0;
	return pFileName;
}

int openFile(char *pFile)
{
	return open(pFile, O_RDONLY);
}

int xreadLine( int fd, char *pBuf, int iLen )
{
	int l = 0;
	int m = 0;
	char *pTmp = NULL;

	l = read( fd, pBuf, iLen );
	if( l <= 0 )
	{
		return -1;
	}

	if((pTmp = strchr(pBuf,'\n')) == NULL)
	{
		return 0;
	}

	*(pTmp) = 0x00;

	m = strlen(pBuf);

	lseek( fd, -(l-m-1), SEEK_CUR );

	return 1;
}

int writePositionConf(char *aCfgPath, char *aFilePath, int fileIndex, int iMsgSize)
{
	int  rFp = 0;
	int  wFp = 0;
	char tmpFileName[1024] = {0};
	char backupFileName[1024] = {0};
	char sBuffer[1024] = {0};
	char wBuffer[1024] = {0};
	int  wLen = 0;
	char *pFileName = NULL;
	char *pInodeNum = NULL;
	char *pSeekByte = NULL;
	int  idx = 0;
	int  iWriteFlag = 0;
	int  iNodeNum = 0;
	int  iSeekByte = 0;
	struct stat st;

	//  printf("cfg:%s / path:%s idx:%d len:%d\n", pDevEvent->aCfgPath, pDevEvent->aFilePath, pDevEvent->fileIndex, pDevEvent->iMsgSize + 1 );

	if ( stat( aFilePath, &st ) < 0 ) {
		printf("file stat fail\n"); 
		return -1;
	}

	snprintf( tmpFileName, sizeof(tmpFileName), "%s.tmp", aCfgPath );
	snprintf( backupFileName, sizeof(backupFileName), "%s.bak", aCfgPath );

	pthread_mutex_lock(&positionFile_lock);
	wFp = open ( tmpFileName, O_WRONLY | O_CREAT );
	if ( wFp < 0 ) {
		return -1;
	}
	rFp = openFile ( aCfgPath );
	if ( rFp >= 0 ) {

		while( xreadLine(rFp, sBuffer, sizeof(sBuffer)-1) >= 0) {
			// '#' 주석은 읽을 필요 없음
			if ( sBuffer[ 0 ] == '#' ) {
				write( wFp, sBuffer, strlen(sBuffer) );
				continue;
			}
			pFileName = readLine( sBuffer, &idx );
			if ( pFileName == NULL ) {
				write( wFp, sBuffer, strlen(sBuffer) );
				continue;
			}

			if ( idx != fileIndex ) {
				//              printf("idx:%d fileIndex:%d filename:%s\n", idx, fileIndex, pFileName );
				wLen = snprintf( wBuffer, sizeof(wBuffer), "%s>\n", sBuffer );
				write( wFp, wBuffer, wLen );
				continue;
			}

			iWriteFlag = 1;
			pInodeNum = strchr ( pFileName, ':' );
			if ( pInodeNum == NULL ) {
				printf("read fail: inodeNum. file position initialized ( %s )\n", aFilePath );
				wLen = snprintf( wBuffer, sizeof(wBuffer), "<LOGFILE%d=%s:%d:%d>\n", fileIndex, aFilePath, st.st_ino, iMsgSize + 1 );
				write( wFp, wBuffer, wLen );
				continue;
			} else if ( pInodeNum == pFileName ) {
				printf("read fail: inodeNum2. file position initialized ( %s )\n", aFilePath );
				wLen = snprintf( wBuffer, sizeof(wBuffer), "<LOGFILE%d=%s:%d:%d>\n", fileIndex, aFilePath, st.st_ino, iMsgSize + 1 );
				write( wFp, wBuffer, wLen );
				continue;
			}
			*pInodeNum = 0;
			pInodeNum++;

			pSeekByte = strchr ( pInodeNum, ':' );
			if ( pSeekByte == NULL ) {
				printf("read fail: seekByte. file position initialized ( %s )\n", aFilePath );
				wLen = snprintf( wBuffer, sizeof(wBuffer), "<LOGFILE%d=%s:%d:%d>\n", fileIndex, aFilePath, st.st_ino, iMsgSize + 1 );
				write( wFp, wBuffer, wLen );
				continue;
			} else if ( pSeekByte == pInodeNum ) {
				printf("read fail: seekByte. file position initialized ( %s )\n", aFilePath );
				wLen = snprintf( wBuffer, sizeof(wBuffer), "<LOGFILE%d=%s:%d:%d>\n", fileIndex, aFilePath, st.st_ino, iMsgSize + 1 );
				write( wFp, wBuffer, wLen );
				continue;
			}
			*pSeekByte = 0;
			pSeekByte++;

			iNodeNum = strtoul( pInodeNum, NULL, 10 );
			iSeekByte = strtoul( pSeekByte, NULL, 10 );

			if ( ( strcmp( pFileName, aFilePath ) != 0 ) ||
					( iNodeNum != st.st_ino ) ||
					( iSeekByte > st.st_size ) ) {
				// 다른 파일인 경우 처음부터 Count
				printf("file position initialized ( %s )\n", aFilePath );
				wLen = snprintf( wBuffer, sizeof(wBuffer), "<LOGFILE%d=%s:%d:%d>\n", fileIndex, aFilePath, st.st_ino, iMsgSize + 1 );
				write( wFp, wBuffer, wLen );
			} else {
				// 같은 파일인 경우 누적 Count
				wLen = snprintf( wBuffer, sizeof(wBuffer), "<LOGFILE%d=%s:%d:%d>\n", fileIndex, aFilePath, st.st_ino, iSeekByte + iMsgSize + 1 );
				write( wFp, wBuffer, wLen );
			}
		}
		close( rFp );

		if ( iWriteFlag == 0 ) {
			printf("not found index(%d). file position initialized ( %s )\n", fileIndex, aFilePath );
			wLen = snprintf( wBuffer, sizeof(wBuffer), "<LOGFILE%d=%s:%d:%d>\n", fileIndex, aFilePath, st.st_ino, iMsgSize + 1 );
			write( wFp, wBuffer, wLen );
		}
		close( wFp );

		if ( rename( aCfgPath, backupFileName ) < 0 ) {
			unlink( tmpFileName );
			pthread_mutex_unlock(&positionFile_lock);
			return -1;
		}

		if ( rename( tmpFileName, aCfgPath ) < 0 ) {
			rename( backupFileName, aCfgPath );
			pthread_mutex_unlock(&positionFile_lock);
			return -2;
		}
		unlink( backupFileName );
	} else {    // Read 할 파일이 없는 경우 쓰기만 하고 끝냄
		printf("position file read fail\n");
		wLen = snprintf( wBuffer, sizeof(wBuffer), "<LOGFILE%d=%s:%d:%d>\n", fileIndex, aFilePath, st.st_ino, iMsgSize + 1 );
		write( wFp, wBuffer, wLen );
		close( wFp );

		if ( rename( tmpFileName, aCfgPath ) < 0 ) {
			unlink( tmpFileName );
			pthread_mutex_unlock(&positionFile_lock);
			return -2;
		}
	}

	pthread_mutex_unlock(&positionFile_lock);
	return 0;
}

int main()
{
	pthread_mutex_init( &positionFile_lock, NULL );

	writePositionConf( "logfile_position.conf", "/home/syhwang/test_position/access.log", 0, 10 );
	return 0;
}
