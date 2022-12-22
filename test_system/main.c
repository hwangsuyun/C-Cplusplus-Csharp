#include <stdio.h>
#include <unistd.h>

int ReqExtract(char *fileName)
{
	char    extFileName[128];
	char    tmp_path[128];
	int     ret;
	        
	memset(extFileName, 0x00, 128);
	memset(tmp_path, 0x00, 128);

	sprintf(extFileName, "%s/%s %s %s", getenv( "PWD" ), "extract.sh", "/home/tx/test_syhwang/test/test_system", fileName);
	printf("Extract Excute [%s]\n", extFileName);

	sprintf(tmp_path, "%s/%s", "/home/tx/test_syhwang/test/test_system", fileName);
	ret = access(tmp_path, R_OK);
	if( ret < 0 ) {
		printf("_ERROR_ File Not Exist. File Path [%s] ", tmp_path);
		return -1;      
	}

	ret = system(extFileName);
	if( ret < 0 ) {
		return -1;      
	}

	return 0;       
}

int main()
{
	int ret;
	char file_name[50];

	memset(file_name, 0x00, sizeof(file_name));
	memcpy(file_name, "KCBCASHI_DK.20150105-1.8.tgz", sizeof(file_name));
	
	ret = ReqExtract(file_name);
	if( ret < 0 ) {
		printf(" _ERROR Req Extract [%s]", file_name );
		exit (0);
	}
	return 0;
}
