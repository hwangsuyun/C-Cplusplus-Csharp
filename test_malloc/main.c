#include <stdio.h>

typedef struct {
	char temp[10];
	int count;
} Save;

int main()
{
	// ����ü�� �Ϲ� Ÿ���� ������ �� ��
	// ����, ���� �Ҵ��� ���ο� �������
	// �����ͳ� �迭�� �������� ������ [0], [1]�� 
	// ���� ����� �� ����.
	// ���� ������ ������ �迭�� ��츸 ����
	
	/* ------ ����ü ������ �����Ҵ� ------ */
	Save *test;
	test = (Save *) malloc (sizeof(Save));
	test[1].count = 10;
	test[2].count = 20;
	test[3].count = 30;
	test[4].count = 40;
	free(test);
	/* ------------------------------------ */

	/* --- ����ü ������ �迭 �����Ҵ� ---- */
	Save *tArray[10];
	int i;
	for (i = 0; i < 10; i++)
	{
		tArray[i] = (Save *) malloc (sizeof(Save));
	}
	tArray[1]->count = 10;
	printf("tArray.count(%d)\n", tArray[1]->count);
	for (i = 0; i < 10; i++)
	{
		free(tArray[i]);
	}
	/* ------------------------------------ */
	
	/* ------- ����ü �迭 �����Ҵ� ------- */
	Save array[10];
	memset(array, 0x00, sizeof(array));
	array[1].count = 10;
	printf("array.count(%d)\n", array[1].count);
	/* ------------------------------------ */
	
	/* --------- ������ �����Ҵ� ---------- */
	char *pointer;
	pointer = (char *) malloc (sizeof(char));
	pointer[0] = 'a';
	pointer[1] = 'b';
	pointer[2] = 'c';
	printf("pointer(%s)\n", pointer);
	free(pointer);
	/* ------------------------------------ */
	
	return 0;
}
