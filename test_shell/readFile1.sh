#!/bin/bash

imsi=${PWD}/nds.ini
if [ -f $imsi ]
then
	while read temp
	do
		echo ${temp#*=}
	done < $imsi
else
	echo "nds.ini is not exist!!!!"
fi
