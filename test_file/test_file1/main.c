#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


typedef struct {
	char    headerFlag[4];
	char    len[4];
	char    terminalId[10];
	char    workCode[2];
	char    tranFg[2];
	char    wccFg[1];
	char    sequence[5];
	char    version[2];
	char    encrypt[1];
	char    dummy[10];
	char    saleReqDate[8];
	char    memberId[10];
	char    productFg[4];
	char    finishDate[8];
	char    productCd[18];
	char    fileNo[2];

	char	appData[8];
	char	appTime[6];
	
	char    cardNo[40];
	char    soonSaleAmt[10];
	char    svcAmt[10];
	char    tax[10];
	char    saleAmt[10];
	char    byerFg[1];
	char    originalDate[8];
	char    orgAppNo[10];
	char    cancel_reason_fg[1];
	char    filler[30];
	char	endch;
} REQFILE;

int copy(char *data, REQFILE *req);
void get_date_buff(char *str);
int makeFile();

int main()
{
	int r;

	r = makeFile();
	printf("r = %d\n", r);
	
	return 0;
}

int copy(char *data, REQFILE *req)
{
	char dateTime[20];
	char strLen[4 + 1];
	int len = 0;
	
	memset(strLen, 0x00, sizeof(strLen));
	memset(dateTime, 0x00, sizeof(dateTime));
	get_date_buff(dateTime);
	
	strncpy(req->headerFlag, data, 4);				len += 4;
	
	if (data[0] == 'C')
	{
		sprintf(strLen, "%04d", sizeof(*req) - 1);
		strncpy(req->len, strLen, 4);				len += 4;
	}
	else
	{
		strncpy(req->len, data + len, 4);			len += 4;
	}
	
	strncpy(req->terminalId, data + len, 10);		len += 10;
	strncpy(req->workCode, data + len, 2);			len += 2;
	strncpy(req->tranFg, data + len, 2);			len += 2;
	strncpy(req->wccFg, data + len, 1);				len += 1;
	strncpy(req->sequence, data + len, 5);			len += 5;
	strncpy(req->version, data + len, 2);			len += 2;
	strncpy(req->encrypt, data + len, 1);			len += 1;
	strncpy(req->dummy, data + len, 10);			len += 10;
	strncpy(req->saleReqDate, data + len, 8);		len += 8;
	strncpy(req->memberId, data + len, 10);			len += 10;
	strncpy(req->productFg, data + len, 4);			len += 4;
	strncpy(req->finishDate, data + len, 8);		len += 8;
	strncpy(req->productCd, data + len, 18);		len += 18;
	strncpy(req->fileNo, data + len, 2);			len += 2;
	
	if (data[0] == 'C')
	{
		strncpy(req->appData, dateTime, 8);				
		strncpy(req->appTime, dateTime + 8, 6);			
	}
	else
	{
		strncpy(req->appData, "        ", 8);				
		strncpy(req->appTime, "      ", 6);			
	}
	
	strncpy(req->cardNo, data + len, 40);			len += 40;
	strncpy(req->soonSaleAmt, data + len, 10);		len += 10;
	strncpy(req->svcAmt, data + len, 10);			len += 10;
	strncpy(req->tax, data + len, 10);				len += 10;
	strncpy(req->saleAmt, data + len, 10);			len += 10;
	strncpy(req->byerFg, data + len, 1);			len += 1;
	strncpy(req->originalDate, data + len, 8);		len += 8;
	strncpy(req->orgAppNo, data + len, 10);			len += 10;
	strncpy(req->cancel_reason_fg, data + len, 1);	len += 1;
	strncpy(req->filler, data + len, 30);			len += 30;

	return 0;
}

void get_date_buff(char *str)
{
	int yy, mm, dd, hh, min, ss;

	long clock;
	struct tm *tmbuf;

	time(&clock);
	tmbuf = localtime(&clock);

	yy = tmbuf->tm_year;
	mm = tmbuf->tm_mon + 1;
	dd = tmbuf->tm_mday;
	hh = tmbuf->tm_hour;
	min = tmbuf->tm_min;
	ss = tmbuf->tm_sec;

	sprintf(str, "%04d%02d%02d%02d%02d%02d", yy + 1900, mm, dd, hh, min, ss);
}

int makeFile()
{
	FILE *oldFd, *newFd;
	char oldFile[128];
	char newFile[128];
	char data[2048];
	REQFILE req;

	memset(oldFile, 0x00, sizeof(oldFile));
	memset(newFile, 0x00, sizeof(newFile));

	memcpy(oldFile, "KCBCASHI_DK.20141229-2.8", sizeof(oldFile));
	memcpy(newFile, "KCBCASHI_DK.20141229-2.8.new", sizeof(newFile));

	if ((oldFd = fopen(oldFile, "r")) == NULL)
	{
		printf("error1\n");
		return -1;
	}

	while (1)
	{
		memset(data, 0x00, sizeof(data));
		memset((char *)&req, 0x00, sizeof(req));

		if(fgets(data, 1024, oldFd) == NULL)
		{
			printf("end\n");
			break;
		}
		printf("data(%s)\n", data);
		copy(data, &req);

		if ((newFd = fopen(newFile, "a+")) == NULL)
		{
			printf("error2\n");
			fclose(newFd);
			return -1;
		}

		fputs((char *)&req, newFd);
		fputs("\n", newFd);
		fclose(newFd);
	}

	fclose(oldFd);

	unlink(oldFile);
	if (rename(newFile, oldFile) != 0)
	{
		printf("이름변경실패\n");
		return -1;
	}
	
	return 0;
}
