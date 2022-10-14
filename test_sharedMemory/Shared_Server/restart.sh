EXE_PGM="Shared_Server"
DEFAULT_PROCESS="grep|vi|tail|cat|echo|ps"
RUNNED_PGM=`ps -ef | grep $EXE_PGM | grep $USER | egrep -v $DEFAULT_PROCESS | awk '{print $8}' `
KILLED_PGM=`ps -ef | grep $EXE_PGM | grep $USER | egrep -v $DEFAULT_PROCESS | awk '{print $2}' `
RUNNED_LEN=`expr "$RUNNED_PGM" : '.*'`

if [ 0 -ne $RUNNED_LEN ]
then
	echo $EXE_PGM is running.
	echo "=============================="
	ps -ef | grep Shared_Server
	echo "=============================="
else
	echo $EXE_PGM is not running.
	start.sh
fi
