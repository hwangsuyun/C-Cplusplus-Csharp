#include <stdio.h>
#include <string.h>
#define swap(a,b) { int t; t = a; a = b; b = t; }

void bubbleSort(char *ar, int num)
{
	int i, j;

	// ���� ������ �ڵ� (j�� 1���� �����Ͽ� num - i ���� ���°� ����Ʈ)
	// swap �� ���ؼ� ū ���� ���������� ����
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
	printf("���� ���� ���ڿ� : %s\n", str);
	bubbleSort(str,strlen(str));
	printf("���ĵ� ���ڿ� : %s\n", str);
	return 0;
}
