#include <stdio.h>

typedef struct {
	char    stx;
	char    msgtype     [2];
	char    termid      [10];
	char    posentry    [1];
	char    trackII     [37];
	char    fs;
	char    instmonth   [2];
	char    totamt      [9];
	char    serviceamt  [9];
	char    wki         [2];
	char    password    [16];
	char    authno      [8];        /* 원승인번호 */
	char    tradedt     [6];        /* 원거래일자(YYMMDD) */
	char    goodcode    [6];
	char    dongleinfo  [4];
	char    etx;
	char    crc         [2];
	char    cr;
} _dongle_cancel_req;

//void canDataKBCard(_dongle_cancel_req *req, char *out, int *sLen);
void canDataKBCard(char *in, char *out, int *sLen);

int main()
{
	char r_pck[512];
	char cardSD[512];
	int cardSDLen = 0;

	memset(r_pck, 0x0, sizeof(r_pck));
	
	canDataKBCard(r_pck, cardSD, &cardSDLen);
	//canDataKBCard((_dongle_cancel_req *)r_pck, cardSD, &cardSDLen);
	return 0;
}

/*
void canDataKBCard(_dongle_cancel_req *in, char *out, int *sLen)
{
	_dongle_cancel_req *req = (_dongle_cancel_req *) in;

	memcpy(req->totamt, "1004", sizeof(req->totamt));

	printf("req->totamt(%s)\n", req->totamt);
}
*/

void canDataKBCard(char *in, char *out, int *sLen)
{
	_dongle_cancel_req *req = (_dongle_cancel_req *) in;

	memcpy(req->totamt, "1004", sizeof(req->totamt));

	printf("req->totamt(%s)\n", req->totamt);
}
