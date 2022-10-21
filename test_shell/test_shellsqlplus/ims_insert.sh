# 변수 선언
FLAG=0
ENDFILE='*.end'
SQLFILE='insert.sql'
BACKUPDIR='/FILE/SEND_SFTP_BACKUP/'
SQLLOG='sql.log'
ERRCNT=0

# 파일 유무 체크
if [ -f $ENDFILE ]
then
	chmod 755 $ENDFILE
	chmod 755 $SQLFILE
	FLAG=1
fi

# DB 적용
if [ $FLAG -eq 1 ]
then
	echo "YES"
	sqlplus IMS/IMS@TEST_4 < $SQLFILE > $SQLLOG << EOF
EOF
	ERRCNT=`grep "ERROR" $SQLLOG | wc | awk '{print $1}'`
#mv $SQLFILE $BACKUPDIR
#rm -rf $ENDFILE
else
	echo "NO"
	exit 0
fi

# DB 적용여부 확인

