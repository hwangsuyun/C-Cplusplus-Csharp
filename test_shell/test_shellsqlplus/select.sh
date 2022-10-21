# ������ �������� ��� īī������ ���� ��Ű��(ddc_finish)�� �������� ������ �ϱ� ������ ���� ���μ����� ����
export ORACLE_BASE=/oracle
export ORACLE_HOME=$ORACLE_BASE/product/10.2.0/db_1
export ORACLE_SID=ORCL
#export ORA_NLS33=$ORACLE_HOME/ocommon/nls/admin/data
export TNS_ADMIN=$ORACLE_HOME/network/admin
export NLS_LANG=AMERICAN_AMERICA.KO16KSC5601
export PATH=$ORACLE_HOME/bin:/usr/bin:/usr/ccs/bin:/usr/local/bin:$PATH
export LD_LIBRARY_PATH=$ORACLE_HOME/lib:/lib:/usr/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
export SMWDIR=${HOME}
export SMWBIN=${SMWDIR}/appbin
export SMWINIDIR=${SMWBIN}/table
export SMWINI=${SMWINIDIR}/smwinfo.ini
export DBINFO=$SMWDIR/appbin/table/dbstr
export LOGDIR=${SMWDIR}/Log
export SQLDIR=${HOME}/SQL

TOMORROW=`date -d '1 day' '+%Y%m%d'`
CLAIM_YN=`sqlplus -s plusccms/plusccms@TEST_5 << EOF
	      set head off
		  SELECT CLAIM_YN
		    FROM TDATE
	       WHERE DAYS = ${TOMORROW}
		 /
		 exit 
EOF`

cd /home/smw/syhwang
list=`ls | grep app_ddc_finish.kakao`

if [ ${CLAIM_YN} == "N" ] # ������ �������� ���
then
	if [ -z ${list} ] # ���ڿ��� ���̰� 0�ΰ�� (app_ddc_finish.kakao �� ������)
	then
		mv app_ddc_finish app_ddc_finish.kakao
		mv app_ddc_finish.20150924 app_ddc_finish
	fi
elif [ ${CLAIM_YN} == "Y" ] # ������ ������ ���
then
	if [ -z ${list} ] # ���ڿ��� ���̰� 0�� ��� (app_ddc_finish.kakao �� ������)
	then
		exit
	else
		mv app_ddc_finish app_ddc_finish.20150924
		mv app_ddc_finish.kakao app_ddc_finish
	fi 
fi
