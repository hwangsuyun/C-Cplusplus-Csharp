#include <stdio.h>

/* ������(���ڸ� �����ͷ� �ѱ�) */
void inputName1(char *pName)
{
	pName = (char *) malloc(12);
	strcpy(pName,"Cabin");
}

/* ������(���ڸ� �迭�� �ѱ�) */
void inputName2(char *pName)
{
	strcpy(pName,"Cabin");
}

/* 2�� ������(���ڸ� ���������ͷ� �ѱ�) */
void inputName3(char **pName)
{
	*pName = (char *) malloc(12);
	strcpy(*pName,"Cabin");
}

/* 2�� ������(���ڸ� ������ �迭�� �ѱ�) */
void inputName4(char **pName)
{
	strcpy((char *)pName,"Cabin");
}

/* ������(���ڸ� ������ �迭�� �ѱ�) */
void inputName5(char (*pName)[10])
{
	strcpy((char *)pName,"Cabin");
}

int main()
{
	char *name;			/* ������ */
	//char name[10];	/* �迭 */
	//char *name[10];	/* ������ �迭 */
	//char name[1][10];	/* ������ �迭 */

	inputName1(name);		/* ������(���� : ������) */
	//inputName2(name);		/* ������(���� : �迭) */
	//inputName3(&name);		/* ����������(���� : ���� ������) */
	//inputName4(name);		/* ����������(���� : ������ �迭) */
	//inputName5(name);		/* ������(���� : �������迭) */
	
	printf("�̸��� %s �Դϴ�\n", name);
	//free(name1);

	return 0;
}
