#include <stdio.h>
#include <time.h>

// 구조체 배열 크기
#define MAX_SHOP 50

// 포인터 구조체 배열 선언
typedef struct {
	char gaMaengNo[12 + 1];		// 가맹점 번호
	char rDate[8 + 1];			// 입금일자
	char sFDate[8 + 1];			// 판매시작일자
	char sTDate[8 + 1];			// 판매종료일자
	int jCnt;					// 접수건수
	int jAmt;					// 접수금액
	int bCnt;					// 반송건수
	int bAmt;					// 반송금액
	int cAmt;					// 수수료합계
} TotalData;

TotalData *tData[MAX_SHOP];

int line;

int initMem(int *dataSize);
int makeTotalData(char *temp, int dataSize);
int freeMem(int dataSize);

int main()
{
	FILE *replyFd;
	char temp[1024];
	int dataSize;
	int r;
	int i = 0;
	int totalAmt = 0, totalCnt = 0, totalSuAmt = 0;;

	if ((replyFd = fopen("FD-REPLY.20150124", "r")) == NULL)
	{       
		printf("file을 열 수 없습니다.\n");
		exit (0);
	}
	
	// 포인터구조체배열 동적할당
	r = initMem(&dataSize);
	if (r < 0) return;
	
	while (1)
	{		
		if (fgets(temp, sizeof(temp), replyFd) == NULL)
		{   
			break;
		}

		r = makeTotalData(temp, dataSize);
		if (r < 0) printf("bansong temp(%s)\n", temp);
	}

	fclose(replyFd);
	
	// 포인터구조체배열 출력
	//for (i = 0; i < dataSize; i++)
	for (i = 0; i < dataSize; i++)
	{
	    if (tData[i]->gaMaengNo[0] == 0x00)
	    {
			break;
	    }	   
		totalCnt += tData[i]->jCnt;
		totalAmt += tData[i]->jAmt;
		totalSuAmt += tData[i]->cAmt;
		printf("tData[%d]->gaMaengNo : (%s), tData[%d]->rDate : (%s), tData[%d]->sFDate : (%s) "
			   "tData[%d]->sTDate : (%s), tData[%d]->jCnt : (%d), tData[%d]->jAmt : (%d), tData[%d]->cAmt : (%d) "
			   "tData[%d]->bCnt : (%d), tData[%d]->bAmt : (%d)\n", 	
				i, tData[i]->gaMaengNo, i, tData[i]->rDate, i, tData[i]->sFDate, i, tData[i]->sTDate, 
				i, tData[i]->jCnt, i, tData[i]->jAmt, i, tData[i]->cAmt, i, tData[i]->bCnt, i, tData[i]->bAmt);
		printf("totalCnt(%d), totalAmt(%d), totalSuAmt(%d)\n", totalCnt, totalAmt, totalSuAmt);
	}

	// 메모리 해제
	r = freeMem(dataSize);
	if (r < 0) return;
	
	/*
	memset((char *)&gaMaeng, 0x00, sizeof(gaMaeng));
	printf("gaMaeng(%s)\n", (char *)&gaMaeng);
	printf("gaMaeng[1]->amt(%s)\n", gaMaeng[1]->amt);
	*/
	printf("finish\n");
	
	return 0;
}


int initMem(int *dataSize)
{
	int i, size;

	size = sizeof(tData)/sizeof(tData[0]);
	
	for (i = 0; i <= size; i++)
	{
		tData[i] = (TotalData *) malloc(sizeof(TotalData));
	}

	*dataSize = size;
	
	return 0;
}

int freeMem(int dataSize)
{
	int i;
	
	// 메모리 해제
	for (i = 0; i < dataSize; i++)
	{
		free(tData[i]);
	}
	
	return 0;
}

int makeTotalData(char *temp, int dataSize)
{
	char tempDate[8 + 1];
	char tempjAmt[12 + 1];
	char tempcAmt[9 + 1];
	int i = 0;
	int count, r;

	// 헤더와 테일이 아닌 데이터 레코드만 걸러냄
	if (temp[0] == '2')
	{   
		count = 0;
		memset(tempDate, 0x00, sizeof(tempDate));
		memset(tempjAmt, 0x00, sizeof(tempjAmt));
		memset(tempcAmt, 0x00, sizeof(tempcAmt));
		memcpy(tempDate, temp + 40, 8);
		memcpy(tempjAmt, temp + 58, 12);
		memcpy(tempcAmt, temp + 121, 9);

		for (i = 0; i <= dataSize; i++)
		{   
			// 구조체배열의 가맹점번호와 읽은 파일의 가맹점 번호가 동일한지 체크
			if (strncmp(temp + 12, tData[i]->gaMaengNo, 12) == 0)
			{   
				if (atoi(tData[i]->sFDate) > atoi(tempDate)) memcpy(tData[i]->sFDate, tempDate, 8); 
				if (atoi(tData[i]->sTDate) <= atoi(tempDate)) memcpy(tData[i]->sTDate, tempDate, 8);

				tData[i]->jCnt++;

				// 승인건
				if (temp[1] == '1')
				{   
					tData[i]->jAmt += atoi(tempjAmt);
					tData[i]->cAmt += atoi(tempcAmt);
				}
				// 취소건
				else if (temp[1] == '2')
				{   
					tData[i]->jAmt -= atoi(tempjAmt);
					tData[i]->cAmt -= atoi(tempcAmt);
				}
				
				// 반송건
				if (strncmp(temp + 109, "0000", 4) != 0)
				{   
					tData[i]->bCnt++;
					// 승인 반송건 
					if (temp[1] == '1') tData[i]->bAmt += atoi(tempjAmt);
					// 취소 반송건
					else if (temp[1] == '2') tData[i]->bAmt -= atoi(tempjAmt);
				}

				count++;
			}
		}
	
		if (count == 0)
		{
			// 구조체배열의 가맹점번호 외에 다른 가맹점 번호가 들어오면 구조체배열에 입력
			memcpy(tData[line]->gaMaengNo, temp + 12, 12);
			memcpy(tData[line]->rDate, temp + 113, 8);
			memcpy(tData[line]->sFDate, tempDate, 8);
			memcpy(tData[line]->sTDate, tempDate, 8);
			tData[line]->jCnt++;

			// 승인건
			if (temp[1] == '1')
			{
				tData[line]->jAmt += atoi(tempjAmt);
				tData[line]->cAmt += atoi(tempcAmt);
			}
			// 취소건
			else if (temp[1] == '2')
			{
				tData[line]->jAmt -= atoi(tempjAmt);
				tData[line]->cAmt -= atoi(tempcAmt);
			}
			// 반송건
			if (strncmp(temp + 109, "0000", 4) != 0)
			{
				tData[line]->bCnt++;
				// 승인 반송건
				if (temp[1] == '1') tData[line]->bAmt += atoi(tempjAmt);
				// 취소 반송건 
				else if (temp[1] == '2') tData[line]->bAmt -= atoi(tempjAmt);
			}
			line++;
		}
	}
	
	// 반송건은 반송 테이블에 바로 저장시키기 위해 리턴값을 다르게 준다.
	if (temp[0] == '2' && (strncmp(temp + 109, "0000", 4) != 0))
	{
		return -1;
	}	


}


