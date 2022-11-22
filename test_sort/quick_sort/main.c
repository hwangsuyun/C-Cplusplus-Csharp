#include <stdio.h>
#include <string.h>

#define swap(a,b) { int t; t = a; a = b; b = t; }
void quickSort(char *ar, int num)
{
	int left, right;
	char key;

	// 구간이 1이면 정렬 끝
	if (num <= 1) return;

	// 기준값 결정 : 배열상의 제일 끝 요소 (중간값으로 정하면 더 빨리 정렬된다)
	key=ar[num - 1];
	for (left = 0, right = num - 2;; left++, right--) 
	{
		while (ar[left] < key) { left++; }
		while (ar[right] > key) { right--; }

		// 좌우가 만나면 끝
		if (left >= right) break; 

		swap(ar[left], ar[right]);
	}

	// 기준값과 i위치의 값 교환
	swap(ar[left], ar[num - 1]);

	// 왼쪽 구간 정렬
	quickSort(ar, left);

	// 오른쪽 구간 정렬
	quickSort(ar + left + 1, num - left - 1);

}

int main()
{
	char str[] = "greathuman";
	printf("정렬 전의 문자열 : %s\n", str);
	quickSort(str,strlen(str));
	printf("정렬된 문자열 : %s\n", str);
	return 0;
}
