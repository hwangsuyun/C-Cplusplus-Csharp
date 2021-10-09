#include <stdio.h>

int main(int argc, char **argv)
{
	char result[10];   // 전체 수수료 차액
	int inputList[7];  // 개별 수수료를 담을 배열
	int listSize = sizeof(inputList) / sizeof(int) - 1; // 배열의 크기 : 6 
	int i;
	
	if (argc < 2)
	{
		printf("입력 값이 정확하지 않습니다!!\n");
		printf("argv[1] = 전체 수수료 차액\n");
		printf("argv[2] = 첫번째 수수료 차액\n");
		printf("argv[3] = 두번째 수수료 차액\n");
		printf("argv[4] = 세번째 수수료 차액\n");
		printf("argv[5] = 네번째 수수료 차액\n");
		printf("argv[6] = 다섯번째 수수료 차액\n");
		printf("argv[7] = 여섯번째 수수료 차액\n");
		printf("argv[8] = 일곱번째 수수료 차액\n");
		exit (0);
	}

	memset(result, 0x00, sizeof(result));
	memcpy(result, argv[1], sizeof(result));
	for (i = 0; i < sizeof(inputList) / sizeof(int); i++)
	{
		inputList[i] = atoi(argv[i + 2]);
	}

	calculateFee(inputList, listSize, atoi(result));
	return 0;
} // end of main

// 계산 작업을 할 함수
int calculateFee(int *inputList, int listSize, int result)
{
	int length = listSize;
	int tempList[listSize], finalList[listSize]; // 임시배열, 결과값이 담길 배열
	int step = 0; // 단계
	int i, j;
	int tempListIndex = 0, finalListIndex = 0; //tempList, finalList 배열의 index
	int total = 0; // finalList 배열의 합계
	int isChecked = 0; // bool 변수, 결과 값과 같으면 참

	for (i = 0; i <= listSize - 1; i++) tempList[i] = 0;  // 두번째 배열 초기화
	for (i = 0; i <= listSize - 1; i++) finalList[i] = 0; // 세번째 배열 초기화

	while (1)
	{
		// 결과 값이 배열의 최소값 보다 작으면 종료, step이 배열의 길이값보다 커지면 종료
		if ((result < inputList[0]) || (step > length))
		{
			printf("no found the result\n");
			exit(0);
		}

		// tempList 값 셋팅 (inputList의 값에서 1개의 값을 제외하고 넣는다)
		for (i = 0; i <= length; i++)
		{
			if (i == step) continue;
			tempList[tempListIndex] = inputList[i];
			printf("step(%d), tempList(%d)(%d)\n", step, tempListIndex, tempList[tempListIndex]);
			tempListIndex++;
		}

		// tempList의 가장 큰 값부터 시작 (ex 59 -> 33 -> 32 ...)
		for (listSize - 1; 0 <= listSize; listSize--)
		{
			// tempList의 가장 큰 값부터 아래의 값들을 더하여 비교 (ex 59 + 33 -> 59 + 32 -> 59 + 28 ...)
			for (j = listSize - 1; 0 <= j; j--)
			{
				printf("listSize(%d), tempList(%d), j(%d)\n", listSize, tempList[j], j);

				if (result == tempList[j]) // 결과 값이 tempList의 값과 같으면 종료
				{
					isChecked = 1;
					printf("finish\n");
					break;
				}
				else if (result < tempList[j]) // 결과 값보다 tempList의 값이 크면 반복
				{
					continue;
				}
				else if (result > tempList[j]) // 결과 값보다 tempList의 값이 작으면
				{
					if (result - total < tempList[j]) continue; // finalList의 합계와 tempList을 더한 값이 결과 값보다 크면 반복

					finalList[finalListIndex] = tempList[j]; // 작은경우 tempList의 값을 finalList 배열로 복사
					total = sum(finalList, length); // finalList의 합계를 구함
					printf("finalList(%d), total(%d)\n", finalList[finalListIndex], total);
					if (result == total) // 결과값과 합계가 같으면 종료
					{
						isChecked = 1;
						printf("finish\n");
						break;
					}
					finalListIndex++;
				}
			} // end of loop

			if (isChecked) break;

			sleep(1);
			total = 0; // finalList 배열 합계 초기화
			finalListIndex = 0; // finalList 배열 인덱스 초기화
			for (i = 0; i <= listSize - 1; i++) finalList[i] = 0; // finalList 배열 초기화
		} // end of loop
		if (isChecked) break;

		step++; // 단계 증가
		tempListIndex = 0; // tempList 배열 인덱스 초기화
		for (i = 0; i <= listSize - 1; i++) tempList[i] = 0; // tempList 배열 초기화
		listSize = length; // listSize 초기화

	} // end of loop

	for (i = 0; i <= listSize - 1; i++)
	{
		printf("finalList(%d)(%d)\n", i, finalList[i]);
	}

	return 0;
} // end of calculate

// 배열의 합계를 구하는 함수
int sum(int *list, int length)
{
	int i, total = 0;

	for (i = 0; i < length; i++)
	{
		total += list[i];
	}
	return total;
}

