#include <stdio.h>
#include <string.h>

#define swap(a,b) { int t; t = a; a = b; b = t; }

void selectionSort(char *ar, int num)
{
	int minIdx;
	int i, j;

	// �迭 ��ü ��ȸ.
	for (i = 0; i < num - 1 ; i++) 
	{
		// i������ �ּҰ��� ã�´�.
		for (minIdx = i, j = i + 1; j < num; j++) 
		{
			if (ar[minIdx] > ar[j]) 
			{
				minIdx = j;
			}
		}
		// �ּҰ��� �����װ� ��ȯ.
		if (minIdx != i) 
		{
			swap(ar[minIdx], ar[i]);
		}
	}
}

int  main()
{
	char str[] = "winapi";
	printf("���� ���� ���ڿ� : %s\n", str);
	selectionSort(str, strlen(str));
	printf("���ĵ� ���ڿ� : %s\n", str);
	return 0;
}
