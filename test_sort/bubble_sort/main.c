#include <stdio.h>
#include <string.h>
#define swap(a,b) { int t; t = a; a = b; b = t; }

void bubbleSort(char *ar, int num)
{
	int i, j;

	// 버블 정렬의 코드 (j가 1부터 시작하여 num - i 까지 도는게 포인트)
	// swap 을 통해서 큰 값을 오른쪽으로 보냄
	for (i = 0; i < num - 1; i++) 
	{
		for (j = 1; j < num - i; j++) 
		{
			if (ar[j - 1] > ar[j]) 
			{
				swap(ar[j - 1], ar[j]);
			}
		}
	}
}

int main()
{
	char str[] = "winapi";
	printf("정렬 전의 문자열 : %s\n", str);
	bubbleSort(str,strlen(str));
	printf("정렬된 문자열 : %s\n", str);
	return 0;
}
