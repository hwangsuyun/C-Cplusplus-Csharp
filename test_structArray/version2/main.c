#include <stdio.h>
#include <time.h>

// ����ü �迭 ũ��
#define MAX_SHOP 50

// ������ ����ü �迭 ����
typedef struct {
	char gaMaengNo[12 + 1];		// ������ ��ȣ
	char rDate[8 + 1];			// �Ա�����
	char sFDate[8 + 1];			// �ǸŽ�������
	char sTDate[8 + 1];			// �Ǹ���������
	int jCnt;					// �����Ǽ�
	int jAmt;					// �����ݾ�
	int bCnt;					// �ݼ۰Ǽ�
	int bAmt;					// �ݼ۱ݾ�
	int cAmt;					// �������հ�
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
		printf("file�� �� �� �����ϴ�.\n");
		exit (0);
	}
	
	// �����ͱ���ü�迭 �����Ҵ�
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
	
	// �����ͱ���ü�迭 ���
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

	// �޸� ����
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
	
	// �޸� ����
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

	// ����� ������ �ƴ� ������ ���ڵ常 �ɷ���
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
			// ����ü�迭�� ��������ȣ�� ���� ������ ������ ��ȣ�� �������� üũ
			if (strncmp(temp + 12, tData[i]->gaMaengNo, 12) == 0)
			{   
				if (atoi(tData[i]->sFDate) > atoi(tempDate)) memcpy(tData[i]->sFDate, tempDate, 8); 
				if (atoi(tData[i]->sTDate) <= atoi(tempDate)) memcpy(tData[i]->sTDate, tempDate, 8);

				tData[i]->jCnt++;

				// ���ΰ�
				if (temp[1] == '1')
				{   
					tData[i]->jAmt += atoi(tempjAmt);
					tData[i]->cAmt += atoi(tempcAmt);
				}
				// ��Ұ�
				else if (temp[1] == '2')
				{   
					tData[i]->jAmt -= atoi(tempjAmt);
					tData[i]->cAmt -= atoi(tempcAmt);
				}
				
				// �ݼ۰�
				if (strncmp(temp + 109, "0000", 4) != 0)
				{   
					tData[i]->bCnt++;
					// ���� �ݼ۰� 
					if (temp[1] == '1') tData[i]->bAmt += atoi(tempjAmt);
					// ��� �ݼ۰�
					else if (temp[1] == '2') tData[i]->bAmt -= atoi(tempjAmt);
				}

				count++;
			}
		}
	
		if (count == 0)
		{
			// ����ü�迭�� ��������ȣ �ܿ� �ٸ� ������ ��ȣ�� ������ ����ü�迭�� �Է�
			memcpy(tData[line]->gaMaengNo, temp + 12, 12);
			memcpy(tData[line]->rDate, temp + 113, 8);
			memcpy(tData[line]->sFDate, tempDate, 8);
			memcpy(tData[line]->sTDate, tempDate, 8);
			tData[line]->jCnt++;

			// ���ΰ�
			if (temp[1] == '1')
			{
				tData[line]->jAmt += atoi(tempjAmt);
				tData[line]->cAmt += atoi(tempcAmt);
			}
			// ��Ұ�
			else if (temp[1] == '2')
			{
				tData[line]->jAmt -= atoi(tempjAmt);
				tData[line]->cAmt -= atoi(tempcAmt);
			}
			// �ݼ۰�
			if (strncmp(temp + 109, "0000", 4) != 0)
			{
				tData[line]->bCnt++;
				// ���� �ݼ۰�
				if (temp[1] == '1') tData[line]->bAmt += atoi(tempjAmt);
				// ��� �ݼ۰� 
				else if (temp[1] == '2') tData[line]->bAmt -= atoi(tempjAmt);
			}
			line++;
		}
	}
	
	// �ݼ۰��� �ݼ� ���̺� �ٷ� �����Ű�� ���� ���ϰ��� �ٸ��� �ش�.
	if (temp[0] == '2' && (strncmp(temp + 109, "0000", 4) != 0))
	{
		return -1;
	}	


}


