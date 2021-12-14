#include <stdio.h>
#include <string.h>

#define STX 0x02
#define ETX 0x03
#define FS 0x1c

typedef struct
{
	char sale_date[9];
	char sale_time[7];
	char terminal_id[11];
	char card_no[41];
	char expiration_date[5];
	char in_code[3];
	char pay_amount[10];
	char installment_month[3];
	char trade_gubun[3];
	char card_name[41];
	char approve_no[14];
	char van_index[3];
	char check_card_flag[2];
	char store_code[7];
	char pos_id[5];
	char created_date[15];
	char return_msg[41];
	char server_info[21];
	char status[2];
} insertData;

void makeData(char *temp);
int initMemory(insertData *in);
int copyMemory(insertData *in, char *temp);
int printMemory(insertData in);
int getDiffFS(char **pCurrStr, char **pNextStr);
int getDummyCH(char *buf, char data[][100], char ch);

int main()
{
	char temp[1024];
	insertData in;
	
	makeData(temp);
	initMemory(&in);
	printMemory(in);
	copyMemory(&in , temp);
	printMemory(in);
	
	return 0;
}

void makeData(char *temp)
{
	sprintf(temp, "%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c",
			"20141015-130000", FS, 
			"20141015",FS, 
			"130000", FS,
			"0000270404", FS, 
			"5387208140166290", FS, 
			"1602", FS, 
			"  ", FS, 
			"000000100", FS, 
			"00", FS, 
			"CB", FS, 
			"신한카드                        ", FS, 
			"39120745    ", FS, 
			"7 ", FS, 
			"0", FS, 
			"2704", FS, 
			"0404", FS,
			"20140722", FS, 
			"신용 정상 승인  OK :39120745    ", FS, 
			"133.9.100.122", FS, 
			"O", FS 
		   );
	
	printf("temp(%s)\n", temp);
}

int initMemory(insertData *in)
{
	memset(in->sale_date,           0x00,   sizeof(in->sale_date));
	memset(in->sale_time,           0x00,   sizeof(in->sale_time));
	memset(in->terminal_id,         0x00,   sizeof(in->terminal_id));
	memset(in->card_no,             0x00,   sizeof(in->card_no));
	memset(in->expiration_date,     0x00,   sizeof(in->expiration_date));
	memset(in->in_code,             0x00,   sizeof(in->in_code));
	memset(in->pay_amount,          0x00,   sizeof(in->pay_amount));
	memset(in->installment_month,   0x00,   sizeof(in->installment_month));
	memset(in->trade_gubun,         0x00,   sizeof(in->trade_gubun));
	memset(in->card_name,           0x00,   sizeof(in->card_name));
	memset(in->approve_no,          0x00,   sizeof(in->approve_no));
	memset(in->van_index,           0x00,   sizeof(in->van_index));
	memset(in->check_card_flag,     0x00,   sizeof(in->check_card_flag));
	memset(in->store_code,          0x00,   sizeof(in->store_code));
	memset(in->pos_id,              0x00,   sizeof(in->pos_id));
	memset(in->created_date,        0x00,   sizeof(in->created_date));
	memset(in->return_msg,          0x00,   sizeof(in->return_msg));
	memset(in->server_info,         0x00,   sizeof(in->server_info));
	memset(in->status,              0x00,   sizeof(in->status));

	return 0;
}

int printMemory(insertData in)
{
	printf("in.sale_date(%s)\n", in.sale_date);
	printf("in.sale_time(%s)\n", in.sale_time);
	printf("in.terminal_id(%s)\n", in.terminal_id);
	printf("in.card_no(%s)\n", in.card_no);
	printf("in.expiration_date(%s)\n", in.expiration_date);
	printf("in.in_code(%s)\n", in.in_code);
	printf("in.pay_amount(%s)\n", in.pay_amount);
	printf("in.installment_month(%s)\n", in.installment_month);
	printf("in.trade_gubun(%s)\n", in.trade_gubun);
	printf("in.card_name(%s)\n", in.card_name);
	printf("in.approve_no(%s)\n", in.approve_no);
	printf("in.van_index(%s)\n", in.van_index);
	printf("in.check_card_flag(%s)\n", in.check_card_flag);
	printf("in.store_code(%s)\n", in.store_code);
	printf("in.pos_id(%s)\n", in.pos_id);
	printf("in.created_date(%s)\n", in.created_date);
	printf("in.return_msg(%s)\n", in.return_msg);
	printf("in.server_info(%s)\n", in.server_info);
	printf("in.status(%s)\n", in.status);
	
	return 0;
}

#if 1
int getDummyCH(char *buf, char data[][100], char ch)
{
	int  i = 0;
	char *temp;

	while(1)
	{
		temp = strchr(buf, ch);
		memset(data[i], 0x00, sizeof(data[i]));
		if (temp == NULL)
		{
			memcpy(data[i], buf, strlen(buf));
			i++;
			break;
		}
		memcpy(data[i], buf, strlen(buf) - strlen(temp));
		//printf("data[i](%s)\n", data[i]);
		i++;
		strcpy(buf, temp + 1);
	}

	return i;
}

int copyMemory(insertData *in, char *temp)
{
	int i;  
	int count = 0;
	char tempData[25][100];

	// FS 의 갯수 체크
	for(i = 0; i < 250; i++)
	{       
		if (temp[i] == FS) count++;
	}

	printf("FS : %d\n", count); 

	for (i = 0; i <= sizeof(in); i++)
	{
		if (temp[i] == 0x00) temp[i] = 0x20;
	}
	
	getDummyCH(temp, tempData, FS);
	
	memcpy(in->sale_date,           tempData[1],    sizeof(in->sale_date));
	memcpy(in->sale_time,           tempData[2],    sizeof(in->sale_time));
	memcpy(in->terminal_id,         tempData[3],    sizeof(in->terminal_id));
	memcpy(in->card_no,             tempData[4],    sizeof(in->card_no));
	memcpy(in->expiration_date,     tempData[5],    sizeof(in->expiration_date));
	memcpy(in->in_code,             tempData[6],    sizeof(in->in_code));
	memcpy(in->pay_amount,          tempData[7],    sizeof(in->pay_amount));
	memcpy(in->installment_month,   tempData[8],    sizeof(in->installment_month));
	memcpy(in->trade_gubun,         tempData[9],    sizeof(in->trade_gubun));
	memcpy(in->card_name,           tempData[10],   sizeof(in->card_name));
	memcpy(in->approve_no,          tempData[11],   sizeof(in->approve_no));
	memcpy(in->van_index,           tempData[12],   sizeof(in->van_index));
	memcpy(in->check_card_flag,     tempData[13],   sizeof(in->check_card_flag));
	memcpy(in->store_code,          tempData[14],   sizeof(in->store_code));
	memcpy(in->pos_id,              tempData[15],   sizeof(in->pos_id));
	memcpy(in->created_date,        tempData[16],   sizeof(in->created_date));
	memcpy(in->return_msg,          tempData[17],   sizeof(in->return_msg));
	memcpy(in->server_info,         tempData[18],   sizeof(in->server_info));
	memcpy(in->status,              tempData[19],   sizeof(in->status));
	
	return 0;
}
#endif

#if 0
int getDiffFS(char **pCurrStr, char **pNextStr)
{
	int diffFS = 0;

	*pCurrStr = *pNextStr + 1;
	*pNextStr = strchr(*pNextStr + 1, 0x1C);

	diffFS = *pNextStr - *pCurrStr;

	return diffFS;
}


int copyMemory(insertData *in, char *temp)
{
	int len = 0, diffFS = 0;
	int i;  
	int count = 0;
	char *pCurrStr = temp, *pNextStr = strchr(pCurrStr, 0x1C);

	diffFS = pNextStr - pCurrStr;   len += diffFS; 

	// FS 의 갯수 체크
	for(i = 0; i < 250; i++)
	{       
		if (temp[i] == FS) count++;
	}

	printf("FS : %d\n", count); 

	for (i = 0; i <= sizeof(in); i++)
	{
		if (temp[i] == 0x00) temp[i] = 0x20;
	}
	
	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;  
	memcpy(in->sale_date, temp + len, diffFS);   len += diffFS; 

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;  
	memcpy(in->sale_time, temp + len, diffFS);   len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;  
	memcpy(in->terminal_id, temp + len, diffFS); len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;
	memcpy(in->card_no, temp + len, diffFS);     len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;
	memcpy(in->expiration_date, temp + len, diffFS);    len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;
	memcpy(in->in_code, temp + len, diffFS);   len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;
	memcpy(in->pay_amount, temp + len, diffFS);   len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;
	memcpy(in->installment_month, temp + len, diffFS);   len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;
	memcpy(in->trade_gubun, temp + len, diffFS);   len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;
	memcpy(in->card_name, temp + len, diffFS);   len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;
	memcpy(in->approve_no, temp + len, diffFS);   len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;
	memcpy(in->van_index, temp + len, diffFS);   len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;
	memcpy(in->check_card_flag, temp + len, diffFS);   len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;
	memcpy(in->store_code, temp + len, diffFS);   len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;
	memcpy(in->pos_id, temp + len, diffFS);   len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;
	memcpy(in->created_date, temp + len, diffFS);   len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;
	memcpy(in->return_msg, temp + len, diffFS);   len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;
	memcpy(in->server_info, temp + len, diffFS);  len+=diffFS;

	diffFS = getDiffFS(&pCurrStr, &pNextStr);   len++;
	memcpy(in->status, temp + len, diffFS);

	return 0;
}
#endif
