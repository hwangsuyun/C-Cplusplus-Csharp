#define				KFTC_VANCD			1
#define				NICE_VANCD			2
#define				CSQUARE_VANCD			13
#define				KIS_VANCD			5
#define				KSNET_VANCD			6
#define				KOCES_VANCD			14
#define				KT_VANCD			12
#define				KCP_VANCD			9
#define				KICC_VAN_CD			4
#define				KOVAN_VANCD			8

#define				STX		0x02
#define				ETX		0x03
#define				FS		0x1c
#define				CR			0x0D

#define	ISP_DATA_DIR	"/tmp/.IspData"

#define			FUNCTION_LEN			2
#define			DATA_LENGTH_LEN			4
#define			DATA_LEN				1024

typedef struct {
        char                    encrypt_ver[1];
        char                    dll_version[2];
	char			data_length[4];
	char			data[ DATA_LEN + 2048 ];
	char			filler;
} TRAN_DATA;


typedef struct{
	char	type[1];
	char	max_len[4];
	char	seed_yn[1];
	char	version[2];
	char	real_len[4];
} INHEADER;

typedef struct {
	char			junmun_type[1];
	char			tran_fg[ 2 ];
	char			app_fg[ 2 ];
	char			site_cd[ 10 ];
	char			terminal_id[ 10 ];
	char			pos_no[ 10 ];
	char			sale_dt [ 14 ];
	char			sale_seq[ 16 ];
	char			card_wcc;
	char			card_dummy[ 40 ];
	char			ins_mm[ 2 ];
	char			svc_amt[ 7 ];
	char			tax_amt[ 7 ];
	char			sale_amt[ 9 ];
	char			org_date[ 8 ];
	char			org_app_no[ 12 ];
	char			org_van_cd[ 2 ];
	char			card_pin[ 6 ];
	char			person_nbr[ 13 ];
	char			dummy[ 100 ];
	char			inner_dummy [ 50 ];
	char			confirm_key[16];
	char			sign_pad_data[2096];
	char			end_ch;
} DLL50_REQ_FORMAT;

typedef struct {
	char			tran_fg[ 2 ];
	char			app_fg [ 2 ];
	char			site_cd[ 10 ];
	char			van_cd[ 2 ];
	char			inner_rep_cd[ 2 ];
	char			rep_cd[ 4 ];
	char			card_app_no[ 12 ];
	char			tran_dt[ 14 ];
	char			tran_seq[ 16 ];
	char			card_no[ 20 ];
	char			viss_cd[ 4 ];
	char			viss_nm[ 20 ];
	char			vaco_cd[ 4 ];
	char			vaco_nm[ 20 ];
	char			shop_id[ 16 ];
	char			occur_point[ 9 ];
	char			useable_point[ 9 ];
	char			sum_point[ 9 ];
	char			notice[ 64 ];
	char			dummy[ 100 ];
	char			end_ch;
} DLL50_REP_FORMAT;


typedef struct{
	char			req_juntype			[ 1 ];
	char			req_tranfg			[ 2 ];
	char			req_appfg			[ 2 ];
	char			req_sitecd			[ 10 ];
	char			req_terminalid		[ 10 ];
	char			req_posno			[ 10 ];
	char			req_saledt			[ 14 ];
	char			req_saleseq			[ 16 ];
	char			req_cardwcc;
	char			req_carddummy		[ 40 ];
	char			req_insmm			[ 2 ];
	char			req_svcamt			[ 7 ];
	char			req_taxamt			[ 7 ];
	char			req_saleamt			[ 9 ];
	char			req_orgdate			[ 8 ];
	char			req_orgappno		[ 12 ];
	char			req_orgvancd		[ 2 ];
	char			req_cardpin			[ 6 ];
	char			req_personnbr		[ 13 ];
	char			req_dummy			[ 100 ];
	char			req_indummy			[ 50 ];
	char			req_bizdate			[ 8 ];
	char			rep_vancd			[ 2 ];
	char			rep_innerrepcd		[ 2 ];
	char			rep_repcd			[ 4 ];
	char			rep_cardappno		[ 12 ];
	char			rep_trandt			[ 14 ];
	char			rep_visscd			[ 4 ];
	char			rep_vissnm			[ 20 ];
	char			rep_vacocd			[ 4 ];
	char			rep_vaconm			[ 20 ];
	char			rep_shopid			[ 16 ];
	char			rep_occurpoint		[ 9 ];
	char			rep_useablepoint	[ 9 ];
	char			rep_remainpoint		[ 9 ];
	char			rep_notice			[ 64 ];
	char			rep_dummy			[ 100 ];
	char			sign_pad_data		[2096];
	char			end_ch;						
} INNERJUNMUNDATA;						/*	내부 전문 포멧		*/

typedef struct{
	char			packet_version			[ 3 ];
	char			packet_length			[ 4 ];
	char			req_tranfg			[ 2 ];
	char			req_appfg			[ 2 ];
	char			req_sitecd			[ 10 ];
	char			req_terminalid			[ 10 ];
	char			req_posno			[ 10 ];
	char			req_saledt			[ 14 ];
	char			req_saleseq			[ 16 ];
	char			req_cardwcc;
	char			req_carddummy			[ 40 ];
	char			req_insmm			[ 2 ];
	char			req_svcamt			[ 7 ];
	char			req_taxamt			[ 7 ];
	char			req_saleamt			[ 9 ];
	char			req_orgdate			[ 8 ];
	char			req_orgappno			[ 12 ];
	char			req_orgvancd			[ 2 ];
	char			req_cardpin			[ 6 ];
	char			req_personnbr			[ 13 ];
	char			req_dummy			[ 100 ];
	char			req_indummy			[ 50 ];
	char			indummy				[ 100 ];
	char			req_bizdate			[ 8 ];
	char			req_signyn;
	char			req_signsaveyn;
	char			rep_terminalid			[ 10 ];
	char			rep_vancd			[ 2 ];
	char			rep_innerrepcd			[ 2 ];
	char			rep_repcd			[ 4 ];
	char			rep_cardappno			[ 12 ];
	char			rep_trandt			[ 14 ];
	char			rep_visscd			[ 4 ];
	char			rep_vissnm			[ 20 ];
	char			rep_vacocd			[ 4 ];
	char			rep_vaconm			[ 20 ];
	char			rep_shopid			[ 16 ];
	char			rep_occurpoint			[ 9 ];
	char			rep_useablepoint		[ 9 ];
	char			rep_remainpoint			[ 9 ];
	char			rep_notice			[ 64 ];
	char			rep_dummy			[ 100 ];
	char			rep_cardtype			[ 3 ];
	char			rep_cardfg			[ 3 ];
	char			rep_cusappno			[ 12 ];
	char			rep_cususepoint			[ 12 ];
	char			rep_cusbalpoint			[ 12 ];
	char			rep_adddummy			[ 100 ];
	char			rep_ntranfg			[ 3 ];
	char			rep_ntranseq			[ 16 ];
	char			rep_signlen			[ 4 ];
	char			end_ch;						
} INNERSAVE;





typedef struct{
    char            req_tranfg[ 2 ];
    char            req_sitecd[ 10 ];
    char            req_terminalid[ 10 ];
    char            req_posno[ 10 ];
    char            req_cardwcc;
    char            req_carddummy[ 40 ];
    char            req_insmm[ 2 ];
    char            req_svcamt[ 7 ];
    char            req_taxamt[ 7 ];
    char            req_saleamt[ 9 ];
    char            req_orgdate[ 8 ];
    char            req_orgappno[ 12 ];
    char            req_orgvancd[ 2 ];
    char            req_cardpin[ 6 ];
    char            req_personnbr[ 13 ];
    char            req_bonuswcc;
    char            req_bonusdummy[ 40 ];
    char            req_bonusappno[ 12 ];
    char            req_bonuspin[ 10 ];
    char            req_dummy[ 100 ];
    char            req_indummy[ 50 ];
    char            req_bizdate[ 8 ];
    char            rep_vancd[ 2 ];
    char            rep_innerrepcd[ 2 ];
    char            rep_repcd[ 4 ];
    char            rep_cardappno[ 12 ];
    char            rep_trandt[ 14 ];
    char            rep_transeq[ 16 ];
    char            rep_visscd[ 4 ];
    char            rep_vissnm[ 20 ];
    char            rep_vacocd[ 4 ];
    char            rep_vaconm[ 20 ];
    char            rep_shopid[ 16 ];
    char            rep_customnm[ 10 ];
    char            rep_occurpoint[ 9 ];
    char            rep_useablepoint[ 9 ];
    char            rep_sumpoint[ 9 ];
    char            rep_specialpoint[ 9 ];
    char            rep_bonusappno[ 12 ];
    char            rep_notice[ 64 ];
    char            rep_dummy[ 100 ];
    char            serial[ 6 ];
    char            end_ch;
} INNERJUNMUNDATA3;                      /*  내부 전문 포멧      */



