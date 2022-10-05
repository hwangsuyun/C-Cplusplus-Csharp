#define	__TF	"dir_lib.c"

#include	<stdio.h>
#include	<sys/types.h>
#include	<dirent.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<unistd.h>

#include	"lib_fn.h"

int	__dir_open_flag = 0;
DIR	*__dirp;
char	__read_dir_path[100];

int Create_Dir_PathFile(char *dirfile_name)
{
static char *func = "Create_Dir_PathFile";

	int	fd;

	dbg(1, "START: dirfile_name(%s). <%s:%s>", dirfile_name, func, __TF);

	if(access(dirfile_name, F_OK) == 0) { 
		dbg(1, "OK: dirfile_name(%s) aleady Exist. <%s:%s>", dirfile_name, func, __TF);
		return(0);
	}

	if(Create_Dir_Path(dirfile_name) < 0) {
		dbg(2, "ERR: Create_Dir_Path(%s) Error. <%s:%s>", dirfile_name, func, __TF);
		return(-1);
	}
	
	fd = creat(dirfile_name, 0666);
	close(fd);

	if(access(dirfile_name, F_OK) < 0) {
		dbg(1, "ERR: dirfile_name(%s) create ERROR. <%s:%s>", dirfile_name, func, __TF);
		return(-1);
	}

	dbg(1, "OK: OK Return. dirfile_name(%s). <%s:%s>", dirfile_name, func, __TF);
	return(0);
}

int Create_Dir_Path(char *d_name)
{
	int	r;

	r = __create_dir_path(d_name, 1);
	return(r);
}

int __create_dir_path(char *d_name, char new_call)
{
	static  char	dir_path[1024];

	char	*ptr;
	char	in_dir[200], dir_name[200];
	int	r;
	char	*func = "__create_dir_path";

	if(new_call) {
		memset(dir_path, 0x0, sizeof(dir_path));
	}

	memset(dir_name, 0x0, sizeof(dir_name));
	ptr = strchr(d_name, '/');
	if(ptr == NULL) {
		return(0);
	}

	strcpy(dir_name, dir_path);
	strncat(dir_name, d_name, ptr - d_name);

	if((strlen(dir_name) > 0) && (access(dir_name, F_OK) < 0)) {
		r = mkdir(dir_name, 0777);
		if(r < 0) {
			return(-1);
		}
	}

	strcpy(dir_path, dir_name);
	strcat(dir_path, "/");

	r = __create_dir_path(ptr + 1, 0);
	if(r < 0) {
		return(-1);
	}

	return(0);
}

int Open_ReadDir(char *d_name)
{
	if(__dir_open_flag) {
		dbg(9, "WORN: open flag is aleady Setted");
		__dir_open_flag = 0;
		closedir(__dirp);
	}

	strcpy(__read_dir_path, d_name);
	__dirp = opendir (__read_dir_path);
	if(__dirp == NULL){
		dbg(9, "ERR: opendir(%s)", d_name);
		return(-1);
	}

	__dir_open_flag = 1;
	dbg(1, "OK: Open_ReadDir(%s) return 0", d_name);
	return(0);
}

void Close_ReadDir(void)
{
	if(__dir_open_flag) {
		__dir_open_flag = 0;
		closedir(__dirp);
	}
	else {
		dbg(9, "WORN: open_flag(%d) is not Setted", __dir_open_flag);
	}
}

int Get_ReadDirFile(char *f_name)
{
	struct dirent	*d;
	struct stat		s;
	char	fullpath[1024];

	if(__dir_open_flag == 0) {
		dbg(9, "ERR: __dirp is NOT Opened");
		return(-1);
	}

	while(1) {
		d = readdir(__dirp);
		if (d == 0){
			dbg(1, "END of FILE. return 0");
			return(0);
		}
		
		if ( d->d_ino == 0 ){
			dbg(1, "WORN: d_ino=0. FILE(%s)", d->d_name);
			continue;
		}
		sprintf(fullpath,"%s/%s", __read_dir_path, d->d_name);

		if ( stat(fullpath, &s ) == -1 ){
			dbg(9, "ERR: stat error. FILE(%s)", d->d_name);
			continue;
		}

		if((s.st_mode & S_IFDIR) != 0){
			dbg(1, "WORN: FILE(%s) is DIRECTORY", d->d_name);
			continue;
		}

		strcpy(f_name, d->d_name);
		dbg(2, "OK: Get_ReadDirFile(%s)", f_name);
		return(1);
	}
}

int Get_DirFile(char *d_name, char *f_name)
{
	DIR			*dp;
	struct dirent	*d;
	struct stat		s;
	char	full_file_name[100];
	
	dbg(1, "START: Get_DirFile");

	dp = opendir ( d_name );
	if ( dp == NULL ){
		dbg(9, "ERR: opendir(%s). return -1", d_name);
		return (-1);
	}

	while(1){
		d = readdir(dp);
		if (d == 0){
			dbg(2, "END: Get_DirFile. No file. return 0");
			closedir(dp);
			return(0);
		}
		
		if ( d->d_ino == 0 ){
			dbg(5, "WORN: d_ino is 0");
			continue;
		}

		sprintf(full_file_name, "%s/%s", d_name, d->d_name);
		if ( stat(full_file_name, &s ) == -1 ){
			dbg(9, "ERR: stat(%s) error", full_file_name);
			continue;
		}
		
		if((s.st_mode & S_IFDIR) != 0){
			dbg(2, "WORN: (%s) is Directory", full_file_name);
			continue;
		}

		strcpy(f_name, d->d_name);
		closedir(dp);
		dbg(1, "OK: f_name(%s). return 1", f_name);
		return (1);
	}
}

int SaveErr(char *err_data)
{
	FILE	*fp;
	int	r;
	char	date_buff[15];
	char	log_dir[100];
	
	memset(date_buff, 0x00, sizeof(date_buff));
	memset(log_dir, 0x00, sizeof(log_dir));

	r     = access( "./SaveErr" , F_OK);
	if( r < 0 ) {
	mkdir( "./SaveErr" , 0700 );
	mkdir( "./SaveErr/BackUp", 0700 );
	}

	get_datetime_buff(date_buff);
	sprintf(log_dir, "./SaveErr/%s", date_buff);

	fp = fopen(log_dir, "a+");
	if(fp == NULL){
		return -1;
	}

	fprintf( fp, "%s\n", err_data);
	
	fclose(fp);

	return 0;
}

int SaveOk(char *mtext)
{
	FILE	*fp;
	int	r;
	char	date_buff[15];
	char	log_dir[100];
	
	memset(date_buff, 0x00, sizeof(date_buff));
	memset(log_dir, 0x00, sizeof(log_dir));

	r     = access( "./SaveErr" , F_OK);
	if( r < 0 ) {
	mkdir( "./SaveErr" , 0700 );
	mkdir( "./SaveErr/BackUp", 0700 );
	}

	r     = access( "./SaveErr/BackUp" , F_OK);
	if( r < 0 ) {
	mkdir( "./SaveErr/BackUp", 0700 );
	}

	get_date_buff(date_buff);
	sprintf(log_dir, "./SaveErr/BackUp/%s", date_buff);

	fp = fopen(log_dir, "a+");
	if(fp == NULL){
		return -1;
	}

	fprintf( fp, "%s\n", mtext);
	
	fclose(fp);

	return 0;
}
