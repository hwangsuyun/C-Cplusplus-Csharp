#!/bin/sh

TO_MON=`date '+%Y%m'`
TO_NUM=`date '+%Y%m' -d '1 month ago'`

cd /home/tx/test_syhwang
tar cvfz $TO_NUM.tgz ./*.log
rm -rf ./*.log
mv $TO_NUM.tgz ./Server
								   
