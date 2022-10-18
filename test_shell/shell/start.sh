#!/bin/sh

IS_EDISVR="`ps -ef | egrep 'test1' | egrep -v 'vi|tail|grep' | wc | awk '{print $1;}'`"
IS_FTPSVR="`ps -ef | egrep 'test2' | egrep -v 'vi|tail|grep' | wc | awk '{print $1;}'`"
IS_EDISVR_SHINHAN="`ps -ef | egrep 'test3' | egrep -v 'vi|tail|grep' | wc | awk '{print $1;}'`"
IS_EDISVR_KOOKMIN="`ps -ef | egrep 'test4' | egrep -v 'vi|tail|grep' | wc | awk '{print $1;}'`"

if [ $IS_EDISVR -eq 0 ] && [ $IS_FTPSVR -eq 0 ] && [ $IS_EDISVR_SHINHAN -eq 0 ] && [ $IS_EDISVR_KOOKMIN -eq 0 ]
then
	echo "-----------------------------------------------"
	echo "-                                             -" 
	echo "-             # Process START!                -"
	echo "-                                             -" 
	echo "-----------------------------------------------"
	echo "" 

	cd /home/tx/test_syhwang/test/test_shell/shell 
	nohup ./test1 &

	cd /home/tx/test_syhwang/test/test_shell/shell
	nohup ./test2 &

	cd /home/tx/test_syhwang/test/test_shell/shell
	nohup ./test3 &
 
	cd /home/tx/test_syhwang/test/test_shell/shell
	nohup ./test4 &

	echo "" 
else
	echo "-----------------------------------------------"
	echo "-                                             -" 
	echo "-             # Process is running!           -"
	echo "-                                             -" 
	echo "-             -> NOT Re-start!!!              -"
	echo "-                                             -" 
	echo "-----------------------------------------------"
	echo "" 
fi
