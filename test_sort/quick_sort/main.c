#include <stdio.h>
#include <string.h>

#define swap(a,b) { int t; t = a; a = b; b = t; }
void quickSort(char *ar, int num)
{
	int left, right;
	char key;

	// ������ 1�̸� ���� ��
	if (num <= 1) return;

	// ���ذ� ���� : �迭���� ���� �� ��� (�߰������� ���ϸ� �� ���� ���ĵȴ�)
	key=ar[num - 1];
	for (left = 0, right = num - 2;; left++, right--) 
	{
		while (ar[left] < key) { left++; }
		while (ar[right] > key) { right--; }

		// �¿찡 ������ ��
		if (left >= right) break; 

		swap(ar[left], ar[right]);
	}

	// ���ذ��� i��ġ�� �� ��ȯ
	swap(ar[left], ar[num - 1]);

	// ���� ���� ����
	quickSort(ar, left);

	// ������ ���� ����
	quickSort(ar + left + 1, num - left - 1);

}

int main()
{
	char str[] = "greathuman";
	printf("���� ���� ���ڿ� : %s\n", str);
	quickSort(str,strlen(str));
	printf("���ĵ� ���ڿ� : %s\n", str);
	return 0;
}
