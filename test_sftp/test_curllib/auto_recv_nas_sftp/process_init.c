#include <stdio.h>

#include "common.h"
#include "process_init.h"

int initialize( char *sIniPath )
{
	info.sleep_time = ReadInteger( "RECV", "SleepTime", 60, sIniPath ); 

	LOG( 5, _OK_, "SLEEPTIME: %d", info.sleep_time );

	return 0;
}
