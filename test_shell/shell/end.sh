#!/bin/sh

echo "-----------------------------------------------"
echo "-                                             -" 
echo "-               # Process END!                -"
echo "-                                             -" 
echo "-----------------------------------------------"

kill `ps -ef | egrep "test1" | egrep -v "vi|tail|grep" | awk '{print $2;}'`
kill `ps -ef | egrep "test2" | egrep -v "vi|tail|grep" | awk '{print $2;}'`
kill `ps -ef | egrep "test3" | egrep -v "vi|tail|grep" | awk '{print $2;}'`
kill `ps -ef | egrep "test4" | egrep -v "vi|tail|grep" | awk '{print $2;}'`
