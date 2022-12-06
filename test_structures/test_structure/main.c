#include <stdio.h>

typedef struct {
	char sitecd[11];
	char tranfg[11];
	char repcd[11];
	char vacocd[11];
	char trandt[11];
	char cardappno[11];
	char vissnm[11];
	char shopid[11];
	char vaconm[11];
	char notice[11];
} SaveDB;

int stepOne(SaveDB *save);
int stepTwo(SaveDB *save);

int main()
{
	int r;
	SaveDB save;

	r = stepOne(&save);
	printf("main1 : sitecd(%s)\n", save.sitecd);
	printf("main1 : tranfg(%s)\n", save.tranfg);
	printf("main1 : repcd(%s)\n", save.repcd);
	printf("main1 : vacocd(%s)\n", save.vacocd);
	printf("main1 : trandt(%s)\n", save.trandt);
	printf("main1 : cardappno(%s)\n", save.cardappno);
	printf("main1 : vissnm(%s)\n", save.vissnm);
	printf("main1 : shopid(%s)\n", save.shopid);
	printf("main1 : vaconm(%s)\n", save.vaconm);
	printf("main1 : notice(%s)\n", save.notice);
	
	r = stepTwo(&save);
	printf("main2 : sitecd(%s)\n", save.sitecd);
	printf("main2 : sitecd(%s)\n", save.sitecd);
	printf("main2 : tranfg(%s)\n", save.tranfg);
	printf("main2 : repcd(%s)\n", save.repcd);
	printf("main2 : vacocd(%s)\n", save.vacocd);
	printf("main2 : trandt(%s)\n", save.trandt);
	printf("main2 : cardappno(%s)\n", save.cardappno);
	printf("main2 : vissnm(%s)\n", save.vissnm);
	printf("main2 : shopid(%s)\n", save.shopid);
	printf("main2 : vaconm(%s)\n", save.vaconm);
	printf("main2 : notice(%s)\n", save.notice);

	return 0;
}

int stepOne(SaveDB *save)
{
	memcpy(save->sitecd, "SITECD1", sizeof(save->sitecd));
	memcpy(save->tranfg, "TRANFG1", sizeof(save->tranfg));
	memcpy(save->repcd, "REPCD1", sizeof(save->repcd));
	memcpy(save->vacocd, "VACOCD1", sizeof(save->vacocd));
	memcpy(save->trandt, "TRANDT1", sizeof(save->trandt));
	memcpy(save->cardappno, "CARDAPPNO1", sizeof(save->cardappno));
	memcpy(save->vissnm, "VISSNM1", sizeof(save->vissnm));
	memcpy(save->shopid, "SHOPID1", sizeof(save->shopid));
	memcpy(save->vaconm, "VACONM1", sizeof(save->vaconm));
	memcpy(save->notice, "NOTICE1", sizeof(save->notice));
	printf("stepOne : sitecd(%s)\n", save->sitecd);
	printf("stepOne : tranfg(%s)\n", save->tranfg);
	printf("stepOne : repcd(%s)\n", save->repcd);
	printf("stepOne : vacocd(%s)\n", save->vacocd);
	printf("stepOne : trandt(%s)\n", save->trandt);
	printf("stepOne : cardappno(%s)\n", save->cardappno);
	printf("stepOne : vissnm(%s)\n", save->vissnm);
	printf("stepOne : shopid(%s)\n", save->shopid);
	printf("stepOne : vaconm(%s)\n", save->vaconm);
	printf("stepOne : notice(%s)\n", save->notice);
	
	return 0;
}

int stepTwo(SaveDB *save)
{
	memcpy(save->sitecd, "SITECD2", sizeof(save->sitecd));
	memcpy(save->tranfg, "TRANFG2", sizeof(save->tranfg));
	memcpy(save->repcd, "REPCD2", sizeof(save->repcd));
	memcpy(save->vacocd, "VACOCD2", sizeof(save->vacocd));
	memcpy(save->trandt, "TRANDT2", sizeof(save->trandt));
	memcpy(save->cardappno, "CARDAPPNO2", sizeof(save->cardappno));
	memcpy(save->vissnm, "VISSNM2", sizeof(save->vissnm));
	memcpy(save->shopid, "SHOPID2", sizeof(save->shopid));
	memcpy(save->vaconm, "VACONM2", sizeof(save->vaconm));
	memcpy(save->notice, "NOTICE2", sizeof(save->notice));
	printf("stepTwo : sitecd(%s)\n", save->sitecd);
	printf("stepTwo : tranfg(%s)\n", save->tranfg);
	printf("stepTwo : repcd(%s)\n", save->repcd);
	printf("stepTwo : vacocd(%s)\n", save->vacocd);
	printf("stepTwo : trandt(%s)\n", save->trandt);
	printf("stepTwo : cardappno(%s)\n", save->cardappno);
	printf("stepTwo : vissnm(%s)\n", save->vissnm);
	printf("stepTwo : shopid(%s)\n", save->shopid);
	printf("stepTwo : vaconm(%s)\n", save->vaconm);
	printf("stepTwo : notice(%s)\n", save->notice);
	
	return 0;
}
