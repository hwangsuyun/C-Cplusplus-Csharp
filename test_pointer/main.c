#include <stdio.h>

/* 포인터(인자를 포인터로 넘김) */
void inputName1(char *pName)
{
	pName = (char *) malloc(12);
	strcpy(pName,"Cabin");
}

/* 포인터(인자를 배열로 넘김) */
void inputName2(char *pName)
{
	strcpy(pName,"Cabin");
}

/* 2중 포인터(인자를 이중포인터로 넘김) */
void inputName3(char **pName)
{
	*pName = (char *) malloc(12);
	strcpy(*pName,"Cabin");
}

/* 2중 포인터(인자를 포인터 배열로 넘김) */
void inputName4(char **pName)
{
	strcpy((char *)pName,"Cabin");
}

/* 포인터(인자를 이차원 배열로 넘김) */
void inputName5(char (*pName)[10])
{
	strcpy((char *)pName,"Cabin");
}

int main()
{
	char *name;			/* 포인터 */
	//char name[10];	/* 배열 */
	//char *name[10];	/* 포인터 배열 */
	//char name[1][10];	/* 이차원 배열 */

	inputName1(name);		/* 포인터(인자 : 포인터) */
	//inputName2(name);		/* 포인터(인자 : 배열) */
	//inputName3(&name);		/* 이중포인터(인자 : 이중 포인터) */
	//inputName4(name);		/* 이중포인터(인자 : 포인터 배열) */
	//inputName5(name);		/* 포인터(인자 : 이차원배열) */
	
	printf("이름은 %s 입니다\n", name);
	//free(name1);

	return 0;
}
