# extract.sh filedate
if [ -z $2 ]
then 
	echo
	echo
	echo "BAD ARGUMENT : extract.sh target_file_name"
	echo
	echo
	exit -1
fi

cd $1
tar xzvf $2
#EOF
