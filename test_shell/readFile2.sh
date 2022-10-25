#!/bin/bash

imsi=${PWD}/nds.ini
if [ -f $imsi ]
then
	echo `awk -F= '{print $2}' nds.ini`
else
	echo "nds.ini is not exist!!!!"
fi
