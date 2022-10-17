#!/bin/sh

TO_MON=`date '+%Y%m'`
TO_DAY=`date '+%d'`
FLAG=0

if [ $TO_DAY -le 10 ]; then
	FLAG=1
elif [ $TO_DAY -le 20 ]; then
	FLAG=2
else 
	FLAG=3
fi

tar cvfz $TO_MON-$FLAG.tgz /home/tx/test_syhwang/*.log
rm -rf /home/tx/test_syhwang/*.log
mv $TO_MON-$FLAG.tgz /home/tx/test_syhwang/Server
