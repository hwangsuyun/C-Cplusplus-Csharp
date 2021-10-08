#include <stdio.h>

int main()
{
	int list0[7]; // 첫번째 배열
	int listSize = sizeof(list0) / sizeof(int) - 1; // 배열의 크기 : 6 
	int length = listSize;
	int list1[listSize], list2[listSize];; // 두번째, 세번째 배열
	int result = 68; // 결과값
	int step = 0; // 단계
	int i, j = 0, index = 0, total = 0, check = 0, cnt = 0;

	// 첫번째 배열 초기화 
	list0[0] = 7;
	list0[1] = 7;
	list0[2] = 20;
	list0[3] = 28;
	list0[4] = 32;
	list0[5] = 33;
	list0[6] = 59;
	
	for (i = 0; i <= listSize - 1; i++) list1[i] = 0; // 두번째 배열 초기화
	for (i = 0; i <= listSize - 1; i++) list2[i] = 0; // 세번째 배열 초기화

	while (1)
	{
		for (i = 0; i <= length; i++)
		{
			if (i == step) continue;
			list1[j] = list0[i];
			printf("step(%d), list1(%d)(%d)\n", step, j, list1[j]);
			j++;
		}
	
		for (listSize - 1; 0 <= listSize; listSize--)
		{
			for (cnt = listSize - 1; 0 <= cnt; cnt--)
			{
				printf("listSize(%d), list1(%d), cnt(%d)\n", listSize, list1[cnt], cnt);
				//if (cnt == 0) break;

				if (result == list1[cnt]) 
				{
					printf("1\n");
					break;
				}
				else if (result < list1[cnt])
				{
					continue;
				}
				else if (result > list1[cnt])
				{
					if (result - total < list1[cnt]) continue;

					list2[index] = list1[cnt];
					total = sum(list2, length);
					printf("list2(%d), total(%d)\n", list2[index], total);
					if (result == total)
					{
						check = 1;
						printf("finish\n");
						break;
					}
					index++;
				}
			} // end of loop
		
			if (check == 1) break;

			sleep(1);
			total = 0;
			index = 0;

			//for (i = 0; i <= listSize - 1; i++) list1[i] = 0;
			for (i = 0; i <= listSize - 1; i++) list2[i] = 0;
		}

		if (check == 1) break;
			
		step++;
		j = 0;
		listSize = 6;

	} // end of loop

	printf("check(%d)\n", check);
	for (i = 0; i <= listSize - 1; i++)
	{
		printf("list2(%d)(%d)\n", i, list2[i]);
	}
	
	return 0;
} // end of main

int sum(int *list, int length)
{
	int i, total = 0;
	
	for (i = 0; i < length; i++)
	{
 		total += list[i];
	}
	return total;
}
