#include <stdio.h>
#include <string.h>

void insertionSort(char *ar, int num)
{
	int i, j;
	char temp;

	// 두 번째 요소부터 끝까지 순회
	// for (i = 1; i < num; i++) 
	// 첫 번째 요소부터 끝가지 순회
	for (i = 0; i < num; i++) 
	{
		// 앞쪽으로 이동하면서 자기보다 큰 값을 한칸씩 오른쪽으로 이동한다.
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
		// 자기보다 크지 않은 최초의 칸 자리에 자신을 삽입한다.
		ar[j] = temp;
	}
}

int main()
{
	char str[] = "complete";
	printf("정렬 전의 문자열 : %s\n", str);
	insertionSort(str,strlen(str));
	printf("정렬된 문자열 : %s\n", str);
	return 0;
}
