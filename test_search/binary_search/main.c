#include <stdio.h>

int binarySearch(int *ar, int num, int key)
{
	int upper, lower, mid;
	lower = 0;
	upper = num - 1;

	for (;;) {
		mid = (upper + lower) / 2;
		if (ar[mid] == key) return mid;
		if (ar[mid] > key) 
		{
			upper = mid - 1;
		} 
		else 
		{
			lower = mid + 1;
		}
		if (upper <= lower) {
			return -1;
		}
	}
}

int main()
{
	int ar[] = {2, 6, 13, 19, 21, 21, 23, 29, 35, 48, 62, 89, 90, 95, 99, 102, 109, 208, 629};
	int num;
	int key, idx;
	num = sizeof(ar) / sizeof(ar[0]);
	key = 29;

	idx = binarySearch(ar, num, key);
	if (idx == -1) {
		printf("찾는 값이 없습니다.\n");
	} 
	else 
	{
		printf("찾는 값은 %d번째에 있습니다.\n",idx);
	}
	return 0;
}
