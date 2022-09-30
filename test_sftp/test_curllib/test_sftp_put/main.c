#include <stdio.h>
#include <curl/curl.h>
#include <sys/stat.h>

/*
 * This example shows an FTP upload, with a rename of the file just after
 * a successful upload.
 *
 * Example based on source code provided by Erick Nuwendam. Thanks!
 */

#define LOCAL_FILE      "feed.txt"
#define RENAME_FILE_TO  "feed.zip"

/* NOTE: if you want this example to work on Windows with libcurl as a
   DLL, you MUST also provide a read callback with CURLOPT_READFUNCTION.
   Failing to do so will give you a crash since a DLL may not use the
   variable's memory when passed in to it from an app like this. */
static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	/* in real-world cases, this would probably get this data differently
	   as this fread() stuff is exactly what the library already would do
	   by default internally */
	size_t retcode = fread(ptr, size, nmemb, stream);

	printf("*** We read %d bytes from file\n", retcode);
	return retcode;
}

int main(void)
{
	CURL *curl;
	CURLcode res;
	FILE *hd_src;
  	struct stat file_info;
    curl_off_t fsize;  
	double speed_upload, total_time;
	
	char *REMOTE_URL = "sftp://smw:smw@133.9.100.5:2222/home/smw/feed.txt";
	
	struct curl_slist *headerlist=NULL;
	/* FTP의 경우 RNTO 명령어를 사용, SFTP의 경우 rename 기존파일명 바뀔파일명 */
    //static const char buf_2 [] = "RNTO " RENAME_FILE_TO;
    static const char buf_2 [] = "rename " LOCAL_FILE " " RENAME_FILE_TO;
	
	/* get the file size of the local file */
	if(stat(LOCAL_FILE, &file_info)) {
		printf("Couldnt open '%s'\n", LOCAL_FILE);
		return 1;
	}
	fsize = (curl_off_t)file_info.st_size;

	printf("Local file size: %" CURL_FORMAT_CURL_OFF_T " bytes.\n", fsize);
	
	/* get a FILE * of the same file */
	hd_src = fopen(LOCAL_FILE, "rb");
	
	/* In windows, this will init the winsock stuff */
	// curl 초기화
	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */
	// context 생성
	curl = curl_easy_init();
	if (curl) {
		/* build a list of commands to pass to libcurl */	
		headerlist = curl_slist_append(headerlist, buf_2);

		/* we want to use our own read function */
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
		
		/* enable uploading */
 	    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);	

		/* specify target */
		curl_easy_setopt(curl, CURLOPT_URL, REMOTE_URL);

		/* pass in that last of FTP commands to run after the transfer */
	    curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);
		
		/* now specify which file to upload */
		curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);

		/* Set the size of the file to upload (optional).  If you give a *_LARGE
		   option you MUST make sure that the type of the passed-in argument is a
		   curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
		   make sure that to pass in a type 'long' argument. */
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)fsize);
		
		// display information
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		/* Now run off and do what you've been told! */
		res = curl_easy_perform(curl);
		
		/* now extract transfer info */
		curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD, &speed_upload);
		curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);
		printf("Speed: %.3f bytes/sec during %.3f seconds\n", speed_upload, total_time);
		
		/* clean up the FTP commands list */
		curl_slist_free_all (headerlist);
		
		/* always cleanup */
		curl_easy_cleanup(curl);

		if(CURLE_OK != res) {
			/* we failed */ 
			fprintf(stderr, "curl told us %d\n", res);
		}
		
		/* close the local file */
		fclose(hd_src);
	}

	curl_global_cleanup();

	return 0;
}
