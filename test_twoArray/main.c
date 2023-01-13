#include <stdio.h>

int function(char file_name[][51])
{
	printf("file_name[0](%s)\n", file_name[0]);;
	printf("file_name[1](%s)\n", file_name[1]);;
	printf("file_name[2](%s)\n", file_name[2]);;
}

int main()
{
	char file_name[3][51];

	memset(file_name, 0x00, sizeof(file_name));
	
	strcpy( file_name[0], "111" );
	strcpy( file_name[1], "222" );
	strcpy( file_name[2], "333" );
	
	function(file_name);
	
	return 0;
}
