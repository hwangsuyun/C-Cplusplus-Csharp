#ifndef __SFTP_LIB_H__
#define __SFTP_LIB_H__

struct FtpFile {
    const char *filename;
    FILE *stream;
};      
    
typedef struct
{   
    char*   data;   // 버퍼
    size_t  size;   // 사이즈
}CONTEXTDATA;

size_t callbackWriteMemory(void *ptr, size_t size, size_t nmemb, void *stream);
static void freeContextData(CONTEXTDATA *p);
static size_t callbackWriteFile(void *buffer, size_t size, size_t nmemb, void *stream);
int checkEndFile(char *url);
int communicateSftpProtocol(char *fullUrl, char *path, char* backPath, char *targetPath);

#endif
