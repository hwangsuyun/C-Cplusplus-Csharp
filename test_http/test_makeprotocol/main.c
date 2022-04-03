#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char npaIp[1024];
int  npaPort = 0;
char appId[1024];
char path[1024];
int sockfd = 0;

int initHttpUtils(char * iniFileName, char * processSection)
{
	/*
	ReadString("HTTP_COMMON", "IP", "127.0.0.1", npaIp, sizeof(npaIp)-1, iniFileName);
	npaPort = ReadInteger("HTTP_COMMON", "PORT", 8080, iniFileName);
	ReadString("HTTP_COMMON", "APP_ID","SP_TEST_V1", appId, sizeof(appId)-1, iniFileName);	
	ReadString(processSection, "PATH"," ",path, sizeof(path)-1, iniFileName );	
	*/
	return 0;
}

int makeHttpHeader(char * outputData,int datalen)
{	char tmpData[1024];
				
	memset(tmpData, 0x00, sizeof(tmpData));
	
	
	sprintf(tmpData,"POST /%s HTTP/1.1\r\nAppid: %s\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\nHOST: %s:%d\r\n",path,appId,datalen,npaIp,npaPort);
/*
	sprintf(tmpData, "%s%s%s%s%s%s%s%s%s%s%s%d%s%s%s%s%s%s%s%d%s",
			"POST", " ", "/", httpData->serviceId, " ", "HTTP/1.1", "\r\n",
			"Host:", " ", proc_info->ip, ":", proc_info->port, "\r\n",
			"Content-Type:", " ", "text/plain", "\r\n",
			"Content-Lengh:", " ", strlen(body-data), "\r\n");
*/
/*
	sprintf(send_buff, "POST /tcd/api/tcp/send HTTP/1.1\r\nHost: 133.50.7.140:8289\r\nContent-Type: text/plain\r\nContent-Length: 75\r\n\r\n{\"authKey\":\"SP_TEST_V1\",\"command\":\"CM03\",\"cardDir\":\"/home/office/FILE/DDC\"}");
*/
	strncpy(outputData, tmpData, strlen(tmpData));

	return 0;
}

int makeHttpProtocol(char * indata, char * outdata)
{	
	char tmpHeader[1024];
	char tmpBody[1024];	
	char tmpResult[1024];
		
	memset(tmpHeader, 0x00, sizeof(tmpHeader));
	memset(tmpBody, 0x00, sizeof(tmpBody));
	memset(tmpResult,0x00, sizeof(tmpResult));
		
	makeHttpHeader(tmpHeader, strlen(indata));
	
	sprintf(tmpResult, "%s\r\n%s", tmpHeader,indata);
	
	strncpy(outdata, tmpResult, strlen(tmpResult));
	
		 			
	return 0;
}

int sendToNpa(char * sendData)
{	
	/*
	int ret;
	
	sockfd = net_connect(npaIp, npaPort);	
	if(sockfd < 0)
	{
		printf("서버연결 실패! ip(%s), port(%d)i\n", npaIp, npaPort);
		return -1;
	}	
	
	ret = net_send(sockfd, sendData, strlen(sendData));
	if(ret < 0)
	{
		close(sockfd);
		printf("[ERROR]Send ERROR : data (%s)\n", sendData);
		return -1;
		
	}		
	return 0;
	*/
}

int makeReplaceReqData(char * siteCd, char * rcvDate, char * copyFileNm, char * filePath, char * outData)
{
	char tmpHttpData[1024];
        char sendHttpData[1024];

        memset(tmpHttpData, 0x00, sizeof(tmpHttpData));
        memset(sendHttpData, 0x00, sizeof(sendHttpData));

        sprintf(tmpHttpData, "siteCd=%s&rcvDate=%s&fileName=%s&filePath=%s",siteCd,rcvDate,copyFileNm,filePath);


        makeHttpProtocol(tmpHttpData,sendHttpData);


        memcpy(outData, sendHttpData, strlen(sendHttpData));


	return 0; 
}

int makeTerminalReqData(char * rcvDate, char * copyFileNm, char * orgCd, char * outData)
{
	char tmpHttpData[1024];
        char sendHttpData[1024];

        memset(tmpHttpData, 0x00, sizeof(tmpHttpData));
        memset(sendHttpData, 0x00, sizeof(sendHttpData));

        sprintf(tmpHttpData, "rcvDate=%s&fileName=%s&orgCd=%s",rcvDate,copyFileNm, orgCd);


        makeHttpProtocol(tmpHttpData,sendHttpData);


        memcpy(outData, sendHttpData, strlen(sendHttpData));


	return 0; 
}

int makeReturnReqData(char * rtnDate, char * siteCd,char * orgCd, char * copyFileNm, char * filePath, char * outData)
{
	char tmpHttpData[1024];
        char sendHttpData[1024];

        memset(tmpHttpData, 0x00, sizeof(tmpHttpData));
        memset(sendHttpData, 0x00, sizeof(sendHttpData));

        sprintf(tmpHttpData, "rtnDate=%s&siteCd=%s&orgCd=%s&fileName=%s&filePath=%s",rtnDate,siteCd,orgCd,copyFileNm,filePath);


        makeHttpProtocol(tmpHttpData,sendHttpData);


        memcpy(outData, sendHttpData, strlen(sendHttpData));


	return 0; 
}
int parseHttpResponse(char * repData )
{
	/*
	int ret =0;
	char tmprepData[2048];
	
	memset(tmprepData, 0x00, sizeof(tmprepData));
		
	ret = net_recv_tm(sockfd, tmprepData, sizeof(tmprepData), 20);
	if(ret < 0)
	{
	   return -1;
	}
	
	strncpy(repData, tmprepData, strlen(tmprepData) );
	
	return 0;
	*/
}

int getSiteCdByOrgCd(char * orgCd, char * outData)
{
        char tmpHttpData[1024];
        char sendHttpData[1024];

        memset(tmpHttpData, 0x00, sizeof(tmpHttpData));
        memset(sendHttpData, 0x00, sizeof(sendHttpData));

        sprintf(tmpHttpData, "orgCd=%s",orgCd);


        makeHttpProtocol(tmpHttpData,sendHttpData);


        memcpy(outData, sendHttpData, strlen(sendHttpData));


        return 0;
}


int main(int argc, char **argv)
{	char tmpData[1024];
	char tmpSiteCd[10 + 1];
	char resultData[1024];
	int ret;

	memset(tmpData, 0x00, sizeof(tmpData));
	memset(resultData, 0x00, sizeof(resultData));
	memset(tmpSiteCd, 0x00, sizeof(tmpSiteCd));

//	sprintf(tmpData, "%s:CGV%s%s:20161017%s%s:FILENAME%s",SITE_CD,CR,RCV_DATE,CR,FILE_NAME,CR);
	initHttpUtils("./httpUtils.ini","RETURN");

	getSiteCdByOrgCd("9900010646", tmpData);

	makeHttpProtocol(tmpData, resultData);
	
	printf("RESULT_DATA : (%s)\n", resultData);

	ret = sendToNpa(resultData);
	if(ret < 0)
	{
		printf("[ERROR]Fail Send Message!\n");
	}
		
	parseHttpResponse(tmpSiteCd);
	
	printf("siteCd: (%s)\n", tmpSiteCd);
	
	return 0;
}

