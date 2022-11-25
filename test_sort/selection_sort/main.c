#include <stdio.h>
#include <string.h>

#define swap(a,b) { int t; t = a; a = b; b = t; }

void selectionSort(char *ar, int num)
{
	int minIdx;
	int i, j;

	// 배열 전체 순회.
	for (i = 0; i < num - 1 ; i++) 
	{
		// i이후의 최소값을 찾는다.
		for (minIdx = i, j = i + 1; j < num; j++) 
		{
			if (ar[minIdx] > ar[j]) 
			{
				minIdx = j;
			}
		}
		// 최소값을 현재항과 교환.
		if (minIdx != i) 
		{
			swap(ar[minIdx], ar[i]);
		}
	}
}

int  main()
{
	char str[] = "winapi";
	printf("정렬 전의 문자열 : %s\n", str);
	selectionSort(str, strlen(str));
	printf("정렬된 문자열 : %s\n", str);
	return 0;
}
