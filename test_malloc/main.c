#include <stdio.h>

typedef struct {
	char temp[10];
	int count;
} Save;

int main()
{
	// 구조체나 일반 타입의 변수나 둘 다
	// 동적, 정적 할당의 여부와 관계없이
	// 포인터나 배열로 선언하지 않으면 [0], [1]과 
	// 같이 사용할 수 없다.
	// 오직 포인터 변수나 배열의 경우만 가능
	
	/* ------ 구조체 포인터 동적할당 ------ */
	Save *test;
	test = (Save *) malloc (sizeof(Save));
	test[1].count = 10;
	test[2].count = 20;
	test[3].count = 30;
	test[4].count = 40;
	free(test);
	/* ------------------------------------ */

	/* --- 구조체 포인터 배열 동적할당 ---- */
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
	
	/* ------- 구조체 배열 정적할당 ------- */
	Save array[10];
	memset(array, 0x00, sizeof(array));
	array[1].count = 10;
	printf("array.count(%d)\n", array[1].count);
	/* ------------------------------------ */
	
	/* --------- 포인터 동적할당 ---------- */
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
