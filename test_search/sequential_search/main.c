#include <stdio.h>

int linearSearch(int *ar, int num, int key)
{
	int i;
	for (i = 0; i < num; i++) 
	{
		if (ar[i] == key) 
		{
			return i;
		}
	}
	return -1;
}

int main()
{
	int ar[] = {23, 47, 19, 63, 57, 26, 75, 73, 82, 89, 47, 11};
	int num;
	int key, idx;

	num = sizeof(ar) / sizeof(ar[0]);
	key = 75;
	idx = linearSearch(ar, num, key);
	if (idx == -1) 
	{
		printf("찾는 값이 없습니다.\n");
	} 
	else 
	{
		printf("찾는 값은 %d번째에 있습니다.\n",idx);
	}
	return 0;
}
