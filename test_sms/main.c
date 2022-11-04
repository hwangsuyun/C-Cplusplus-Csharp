#include <stdio.h>

int sendSMS(char *ip, int port, char *data);

int main()
{
	char ip[20]; 
	int port, r;
	char data[30];

	memset(data, 0x00, sizeof(data));
	memset(ip, 0x00, sizeof(ip));
	memcpy(data, "sms://Failed", sizeof(data));
	memcpy(ip, "133.33.136.120", sizeof(ip));
	port = 1120; 
	
	r = sendSMS(ip, port, data);
	
	if (r < 0)
	{
		memcpy(ip, "127.0.0.1", sizeof(ip));
		port = 1120;
		r = sendSMS(ip, port, data);
	}
	
	return 0;
}

int sendSMS(char *ip, int port, char *data)
{   
	char sendData[256], recvData[256];
	char temp[15], date[9], time[7];
	int r;
	int sLen;

	//dbg(6, "sendSMS");

	memset(sendData, 0x00, sizeof(sendData));

	// 1. 전문 생성
	get_datetime_buff(temp);
	memcpy(date, temp, 8); 
	memcpy(time, temp + 8, sizeof(time));
	//dbg(6, "date(%s), time(%s)", date, time);

	sprintf(sendData, "%s%s%d%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
			"S", "~", getpid(), "~", date, "~", time, "~", "~", 
			"post", "~", "~", "~", "~", "~", "5", "~", data
		   );

	sLen = strlen(sendData);
	sendData[sLen] = 0x0D;
	sendData[sLen + 1] = 0x00;
	sLen++;

	// 2. SMS 전문 전송
	while (1)
	{   
		r = TCP_Svr_Comm(svr_comm_version, ip, port, sendData, sLen, recvData, sizeof(recvData)-1, svr_comm_tmout);
		if (r < 0 && r != -300)
		{       
			//dbg(9, "ERR: SMS fail. r(%d).ip(%s).port(%d).sLen(%d).sendData(%s)", r, ip, port, sLen, sendData);
			return -1;
		}    
		else
		{   
			//dbg(6, "OK: SMS succeed. r(%d).ip(%s).port(%d).sLen(%d).sendData(%s)", r, ip, port, sLen, sendData);
			return 0;
		}

	}

}

