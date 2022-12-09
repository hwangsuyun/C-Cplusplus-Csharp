#include <stdio.h>

#define STX 0x02
#define ETX 0x03
#define FS  0x1c
#define	MAX_BUFF_SZ	4100

typedef struct{
	char    trml_id[8];
	char    trans_seq[30];
	char    director_code[8];
	char    post_code[8];
	char    client_user_id[8];
	char    desk_num[8];
	char    accept_num[21];
	char    work_flag[2];
	char    unit_flag[2];
	char    keyin_flag;
	char    trade_flag;
	char    agency_code;
	char    process_flag;
	char    msg_code[2];
	char    trade_date[14];
	char    maker_code[2];
	char    ctrl_code;
	char    amount_index;
	char    trade_seq_num[4];
	char    trml_div_code[2];
	char    card_num[16];
	char    allot_time[2];
	char    total_trade_price[12];
	char    cash_pay_price[9];
	char    org_agree_num[8];
	char    org_trade_date[8];
	char    service_price[8];
	char    tax_price[20];
	char    addition_field[50];
	char    early_sale_date[6];
	char    valid_interval[4];
	char    trade_div;
	char    point_card_data[37];
	char    req_code[4];
	char    buy_code[3];
	char    buy_seq_num[4];
	char    agree_date[13];
	char    trade_org_num[12];
	char    agree_num[8];
	char    sale_name[14];
	char    shop_num[15];
	char    buy_name[14];
	char    cash_agree_price[8];
	char    notice[80];
	char    point_agree_num[8];
	char    happen_point[7];
	char    available_point[7];
	char    cumulation_point[7];
	char    point_msg[32];
	char    point_notice[64];
	char    finish_flag[2];
	char    batch_status_code[2];
	char    cancel_flag;
	char    delete_flag;
	char    update_user_id[10];
	char    batch_flag;
	char    plevel[2];
	char    colligate_code[8];
	char    init_buy_code[3];
	char    init_shop_num[15];
	char    tnt_price[12];
	char    regipo_code[5];
	char    day_night_div_cd;
	char    accept_date[8];
	char    trade_price[20];
	char    batch_cancel_flag;
	char    loss_treat_flag;
	char    pre_loss_treat_flag;
	char    endch;
} POST_SAVE;

POST_SAVE post_save;

void Run_BData(char *client_recv);
//int makeReqSaveData(char *client_recv, char *save_send, int *wcc_flag);
//int makeBData(char *client_recv, char *svr_s_data, int *wcc_flag);
//int makeRepSaveData(char *svr_r_data,  char *save_send);
int makeReqSaveData(char *client_recv, char *save_send, int *wcc_flag, POST_SAVE *save);
int makeBData(char *client_recv, char *svr_s_data, int *wcc_flag, POST_SAVE *save);
int makeRepSaveData(char *svr_r_data,  char *save_send, POST_SAVE *save);

//Lib
void NulltoSpace(char *buf, int size);
int Get_Dummy_CH(char *buf, char data[][100], char ch);
void LPAD(char *src);
void RPAD(char *src);
void PAD(char *src);
////////////////////////////////////////

int main()
{
	char client_recv[1024];
	char header[21];
    char termid[11];
    char wcc;
    char cardno[41];
    char insmm[3];
    char saleamt[13];;
    char cashtemp[50];
    char orgappno[16];
    char orgdate[9];
	char passwd[17];
	char svcamt[13];
	char taxamt[13];
	char filler[101];

	memset(client_recv, 0x00, sizeof(client_recv));
	memset( header, 0x00, sizeof(header) );
	memset( termid, 0x00, sizeof(termid) );
	wcc = 0x20;
	memset( cardno, 0x00, sizeof(cardno) );
	memset( insmm, 0x00, sizeof(insmm) );
	memset( saleamt, 0x00, sizeof(saleamt) );
	memset( cashtemp, 0x00, sizeof(cashtemp) );
	memset( orgappno, 0x00, sizeof(orgappno) );
	memset( orgdate, 0x00, sizeof(orgdate) );
	memset( passwd, 0x00, sizeof(passwd) );
	memset( svcamt, 0x00, sizeof(svcamt) );
	memset( taxamt, 0x00, sizeof(taxamt) );
	memset( filler, 0x00, sizeof(filler) );

	strcpy( header, "EF88888888D1PI012440" );
	strcpy( termid, "1081758");
	wcc = '@';
	sprintf( cardno, "%s%c%s", "5387208140166290", 0x1c, "1602" );
	strcpy( saleamt, "100" );
	strcpy( insmm, "00" ); 
	strcpy( cashtemp, "" );
	strcpy( orgappno, "" );
	strcpy( orgdate, "" );

	sprintf(client_recv, "%c%s%c%s%c%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s",
			0x02, header,
			0x1c, termid,
			0x1c, wcc, cardno,
			0x1c, insmm,
			0x1c, saleamt,
			0x1c, cashtemp,
			0x1c, orgappno,
			0x1c, orgdate,
			0x1c, passwd,
			0x1c, svcamt,
			0x1c, taxamt,
			0x1c, filler,
			0x03, "0#01#1#40004#40004#40424#101#yts730129#201410104042410190400#RW#RA#A#06#kplis#1#0#S#20140812#0#N#N#N"
		   );

	Run_BData(client_recv);
}

void Run_BData(char *client_recv)
{
	char    *__fn = "Run_BData";
	
	POST_SAVE save;
	char save_send[1024];
	char recv_buff[1024];
	char svr_s_data[MAX_BUFF_SZ];
	char svr_r_data[MAX_BUFF_SZ];
	int wcc_flag = 0;
	int r;

	memset((char*)&save, 0x00, sizeof(save));
	//memset((char*)&post_save, 0x00, sizeof(post_save));
	memset(save_send, 0x00, sizeof(save_send));
	memset(svr_s_data, 0x00, sizeof(svr_s_data));

	r = makeReqSaveData(client_recv, save_send, &wcc_flag, &save);
	if (r < 0) return;

	memset(recv_buff, 0x00, sizeof(recv_buff));

	//strcpy( (char *)&post_save, recv_buff );
	strcpy( (char *)&save, recv_buff );

	r = makeBData(client_recv, svr_s_data, &wcc_flag, &save);

	r = makeRepSaveData(svr_r_data, save_send, &save);
	if (r < 0) return;

	//printf("post_save.accept_num(%s)\n", post_save.accept_num);
	//printf("post_save.maker_code(%s)\n", post_save.maker_code);
	//printf("post_save.trml_id(%s)\n", post_save.trml_id);
	//printf("post_save.card_num(%s)\n", post_save.card_num);
	//printf("post_save.total_trade_price(%s)\n", post_save.total_trade_price);
	//printf("post_save.org_agree_num(%s)\n", post_save.org_agree_num);
	//printf("post_save.notice(%s)\n", post_save.notice);
	printf("save.accept_num(%s)\n", save.accept_num);
	printf("save.maker_code(%s)\n", save.maker_code);
	printf("save.trml_id(%s)\n", save.trml_id);
	printf("save.card_num(%s)\n", save.card_num);
	printf("save.total_trade_price(%s)\n", save.total_trade_price);
	printf("save.org_agree_num(%s)\n", save.org_agree_num);
	printf("save.notice(%s)\n", save.notice);

	return;
}

int makeReqSaveData(char *client_recv, char *save_send, int *wcc_flag, POST_SAVE *save)
{
	char    recv_buff[1024];
	char    kicc_data[20][100];
	char    add_buff[200];
	char    add_buff_arr[30][100];
	char    *ptr;
	int     r;

	memset(recv_buff, 0x00, sizeof(recv_buff));
	memcpy(recv_buff, client_recv+1, strlen(client_recv));

	ptr = strchr(recv_buff, ETX);
	if( ptr == NULL ){
		printf("[ERR][NOT FOUND SAVE DATA]\n");
		return -1;
	}
	memcpy( add_buff, ptr+1, strlen(ptr+1) );
	*ptr = 0x00;

	memset((char *)add_buff_arr, 0x00, sizeof(add_buff_arr));
	r = Get_Dummy_CH(add_buff, add_buff_arr, '#');
	if(r != 22){
		printf("[ERR][SAVE DATA FORMAT ERR(%d)]\n", r);
		return -1;
	}

	memset((char *)kicc_data, 0x00, sizeof(kicc_data));
	r = Get_Dummy_CH( recv_buff, kicc_data, FS );
	if(r < 0){
		printf("[ERR][Junmun Format ERR(%s)]\n", recv_buff);
		return -1;
	}

	PAD(add_buff_arr[8]);
	memcpy(save->accept_num, add_buff_arr[8], strlen(add_buff_arr[8]));
	//memcpy(post_save.accept_num, add_buff_arr[8], strlen(add_buff_arr[8]));

	memcpy( save->maker_code, "SE", 2 );
	memcpy( save->trml_id, kicc_data[1], strlen(kicc_data[1]));
	//memcpy( post_save.maker_code, "SE", 2 );
	//memcpy( post_save.trml_id, kicc_data[1], strlen(kicc_data[1]));

	if( kicc_data[2][0] == '@' ){
		*wcc_flag = 1;
		memcpy(save->card_num, kicc_data[2]+1, 16);
		//memcpy(post_save.card_num, kicc_data[2]+1, 16);
	}
	else if( kicc_data[2][0] == 'A' ){
		*wcc_flag = 0;
		ptr = strchr(kicc_data[2], '=');
		if(ptr == NULL){
			printf("[ERR][CARD NUM ERR(%s)]\n", kicc_data[2]);
			return -1;
		}
		memcpy(save->card_num, kicc_data[2]+1, ptr -(kicc_data[2]+1));
		//memcpy(post_save.card_num, kicc_data[2]+1, ptr -(kicc_data[2]+1));
	}
	else{
		printf("[ERR][SAVE DATA FORMAT ERR]\n");
		return -1;
	}

	PAD(kicc_data[4+ *wcc_flag]);
	memcpy(save->total_trade_price, kicc_data[4+ *wcc_flag], strlen(kicc_data[4+ *wcc_flag]));
	//memcpy(post_save.total_trade_price, kicc_data[4+ *wcc_flag], strlen(kicc_data[4+ *wcc_flag]));

	NulltoSpace((char *)save, sizeof(save)-1);
	strcpy( save_send, (char *)save );
	//NulltoSpace((char *)&post_save, sizeof(post_save)-1);
	//strcpy( save_send, (char *)&post_save );

	return 0;
}

int makeBData(char *client_recv, char *svr_s_data, int *wcc_flag, POST_SAVE *save)
{
	char    date_buff[15];
	int     i;
	int     fs_cnt = 0;
	int     sn_pos = 0;

	//makePostReqPacket( forkCnt, client_recv, svr_s_data );
	//sn_pos = 21;

	for( i = 0 ; i < strlen(client_recv) ; i++ )
	{
		svr_s_data[sn_pos] = client_recv[i];
		sn_pos++;

		if( client_recv[i] == FS )
			fs_cnt++;

		if( fs_cnt == *wcc_flag+6 )
		{
			memcpy( &svr_s_data[sn_pos], save->org_agree_num, 8 );
			//memcpy( &svr_s_data[sn_pos], post_save.org_agree_num, 8 );
			sn_pos = sn_pos + 8;
		}

		if( fs_cnt == *wcc_flag+7 )
		{
			memset(date_buff, 0x00, sizeof(date_buff));
			//get_datetime_buff(date_buff);

			memcpy( &svr_s_data[sn_pos], date_buff+2, 6 );
			sn_pos = sn_pos + 6;
		}
	}

	svr_s_data[0] = 'B';
	memcpy( svr_s_data+32, "D2PI", 4 );

	return 0;
}

int makeRepSaveData(char *svr_r_data, char *save_send, POST_SAVE *save)
{
	char    recv_buff[1024];
	char    kicc_data[20][100];
	int     r;

	memset(recv_buff, 0x00, sizeof(recv_buff));
	memcpy(recv_buff, svr_r_data+1, strlen(svr_r_data));

	memset((char *)kicc_data, 0x00, sizeof(kicc_data));
	r = Get_Dummy_CH( recv_buff, kicc_data, FS );
	if(r < 0){
		printf("[ERR][Junmun Format ERR(%s)]\n", recv_buff);
		return -1;
	}

	memcpy( save->maker_code, "UP", 2 );
	//memcpy( post_save.maker_code, "UP", 2 );
	if( strncmp( kicc_data[2], "0000", 4 ) == 0 )
	{
		memcpy( save->notice, "Nack 취소", 9 );
		//memcpy( post_save.notice, "Nack 취소", 9 );
	}
	else
	{
		PAD(kicc_data[15]);
		sprintf( save->notice, "Nack 취소 실패(%s)", kicc_data[15] );
		//sprintf( post_save.notice, "Nack 취소 실패(%s)", kicc_data[15] );
	}

	NulltoSpace((char *)save, sizeof(save)-1);
	strcpy( save_send, (char *)save );
	//NulltoSpace((char *)&post_save, sizeof(post_save)-1);
	//strcpy( save_send, (char *)&post_save );

	return 0;
}


void NulltoSpace(char *buf, int size)
{
	int i;

	for(i = 0 ; i < size ; i++){
		if(buf[i] == 0x0){
			buf[i] = ' ';
		}
	}
}

int Get_Dummy_CH(char *buf, char data[][100], char ch)
{       
	int i = 0;
	char *temp;

	while(1)
	{   
		temp = strchr(buf, ch);
		memset(data[i], 0x00, sizeof(data[i]));
		if ( temp == NULL ){
			memcpy(data[i], buf, strlen(buf));
			i++;
			break;
		}
		memcpy(data[i], buf, strlen(buf) - strlen(temp));
		i++;
		strcpy(buf, temp + 1);
	}

	return i;
}

void LPAD(char *src)
{
	int i = 0;  
	int j = 0;  
	char *tmpStr;

	tmpStr = (char *)malloc(strlen(src) + 1);
	strcpy(tmpStr, src);

	for( ; ; ) {
		if( !(*( tmpStr + i ) == 0x20 || *( tmpStr + i ) == 0x09 ) ) break;
		i++;    
	}

	strcpy(src, tmpStr + i);  
	free(tmpStr);
}

void RPAD(char *src)
{
	int i = 0;
	int j = 0;
	int len;
	char *tmpStr;

	len = strlen( src );
	tmpStr = ( char * )malloc( len + 1 );
	strcpy(tmpStr , src);

	for( i = len - 1 ; i != 0 ; i-- ) {
		if( *( tmpStr + i ) == 0x20 || *( tmpStr + i ) == 0x09 ) {
			src[ i ]    = 0x00;
		} else {
			break;
		}
	}

	free(tmpStr);
}

void PAD(char *src)
{
	RPAD(src);
	LPAD(src);
}

