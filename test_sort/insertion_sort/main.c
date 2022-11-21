#include <stdio.h>
#include <string.h>

void insertionSort(char *ar, int num)
{
	int i, j;
	char temp;

	// �� ��° ��Һ��� ������ ��ȸ
	// for (i = 1; i < num; i++) 
	// ù ��° ��Һ��� ������ ��ȸ
	for (i = 0; i < num; i++) 
	{
		// �������� �̵��ϸ鼭 �ڱ⺸�� ū ���� ��ĭ�� ���������� �̵��Ѵ�.
		for (temp = ar[i], j = i; j > 0; j--) 
		{
			if (ar[j - 1] > temp) 
			{
				ar[j] = ar[j - 1];
			} 
			else 
			{
				break;
			}
		}
		// �ڱ⺸�� ũ�� ���� ������ ĭ �ڸ��� �ڽ��� �����Ѵ�.
		ar[j] = temp;
	}
}

int main()
{
	char str[] = "complete";
	printf("���� ���� ���ڿ� : %s\n", str);
	insertionSort(str,strlen(str));
	printf("���ĵ� ���ڿ� : %s\n", str);
	return 0;
}
