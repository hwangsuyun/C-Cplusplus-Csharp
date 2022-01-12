#include <stdio.h>

#define SECTION "CREDITINFO"
#define iFName "nds.ini"

int Fork_Cnt = 0;

int main()
{
	char dbg_dir[100];
	char dbg_file[128];
	char d_buff[20];
	int d_flag;
	
	d_flag = readInteger(SECTION, "DEBUGLEVEL", 5, iFName);
	memset(dbg_dir, 0x0, sizeof(dbg_dir));
	readString(SECTION, "DEBUGDIR", "", dbg_dir, sizeof(dbg_dir), iFName);
	memset(d_buff, 0x0, sizeof(d_buff));
	get_date_buff(d_buff);
	sprintf(dbg_file, "%s/d%s.dat", dbg_dir, d_buff);
	dbg_start(d_flag, dbg_dir, dbg_file);

	printf("d_flag(%d)\n", d_flag);
	printf("dbg_dir(%s)\n", dbg_dir);
	printf("dbg_file(%s)\n", dbg_file);
	
	dbg(5, "dbg start");
	
	return 0;
}
