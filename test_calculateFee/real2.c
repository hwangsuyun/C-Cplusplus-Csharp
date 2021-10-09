#include <stdio.h>

int main(int argc, char **argv)
{
	char result[10];   // ��ü ������ ����
	int inputList[7];  // ���� �����Ḧ ���� �迭
	int listSize = sizeof(inputList) / sizeof(int) - 1; // �迭�� ũ�� : 6 
	int i;
	
	if (argc < 2)
	{
		printf("�Է� ���� ��Ȯ���� �ʽ��ϴ�!!\n");
		printf("argv[1] = ��ü ������ ����\n");
		printf("argv[2] = ù��° ������ ����\n");
		printf("argv[3] = �ι�° ������ ����\n");
		printf("argv[4] = ����° ������ ����\n");
		printf("argv[5] = �׹�° ������ ����\n");
		printf("argv[6] = �ټ���° ������ ����\n");
		printf("argv[7] = ������° ������ ����\n");
		printf("argv[8] = �ϰ���° ������ ����\n");
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

// ��� �۾��� �� �Լ�
int calculateFee(int *inputList, int listSize, int result)
{
	int length = listSize;
	int tempList[listSize], finalList[listSize]; // �ӽù迭, ������� ��� �迭
	int step = 0; // �ܰ�
	int i, j;
	int tempListIndex = 0, finalListIndex = 0; //tempList, finalList �迭�� index
	int total = 0; // finalList �迭�� �հ�
	int isChecked = 0; // bool ����, ��� ���� ������ ��

	for (i = 0; i <= listSize - 1; i++) tempList[i] = 0;  // �ι�° �迭 �ʱ�ȭ
	for (i = 0; i <= listSize - 1; i++) finalList[i] = 0; // ����° �迭 �ʱ�ȭ

	while (1)
	{
		// ��� ���� �迭�� �ּҰ� ���� ������ ����, step�� �迭�� ���̰����� Ŀ���� ����
		if ((result < inputList[0]) || (step > length))
		{
			printf("no found the result\n");
			exit(0);
		}

		// tempList �� ���� (inputList�� ������ 1���� ���� �����ϰ� �ִ´�)
		for (i = 0; i <= length; i++)
		{
			if (i == step) continue;
			tempList[tempListIndex] = inputList[i];
			printf("step(%d), tempList(%d)(%d)\n", step, tempListIndex, tempList[tempListIndex]);
			tempListIndex++;
		}

		// tempList�� ���� ū ������ ���� (ex 59 -> 33 -> 32 ...)
		for (listSize - 1; 0 <= listSize; listSize--)
		{
			// tempList�� ���� ū ������ �Ʒ��� ������ ���Ͽ� �� (ex 59 + 33 -> 59 + 32 -> 59 + 28 ...)
			for (j = listSize - 1; 0 <= j; j--)
			{
				printf("listSize(%d), tempList(%d), j(%d)\n", listSize, tempList[j], j);

				if (result == tempList[j]) // ��� ���� tempList�� ���� ������ ����
				{
					isChecked = 1;
					printf("finish\n");
					break;
				}
				else if (result < tempList[j]) // ��� ������ tempList�� ���� ũ�� �ݺ�
				{
					continue;
				}
				else if (result > tempList[j]) // ��� ������ tempList�� ���� ������
				{
					if (result - total < tempList[j]) continue; // finalList�� �հ�� tempList�� ���� ���� ��� ������ ũ�� �ݺ�

					finalList[finalListIndex] = tempList[j]; // ������� tempList�� ���� finalList �迭�� ����
					total = sum(finalList, length); // finalList�� �հ踦 ����
					printf("finalList(%d), total(%d)\n", finalList[finalListIndex], total);
					if (result == total) // ������� �հ谡 ������ ����
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
			total = 0; // finalList �迭 �հ� �ʱ�ȭ
			finalListIndex = 0; // finalList �迭 �ε��� �ʱ�ȭ
			for (i = 0; i <= listSize - 1; i++) finalList[i] = 0; // finalList �迭 �ʱ�ȭ
		} // end of loop
		if (isChecked) break;

		step++; // �ܰ� ����
		tempListIndex = 0; // tempList �迭 �ε��� �ʱ�ȭ
		for (i = 0; i <= listSize - 1; i++) tempList[i] = 0; // tempList �迭 �ʱ�ȭ
		listSize = length; // listSize �ʱ�ȭ

	} // end of loop

	for (i = 0; i <= listSize - 1; i++)
	{
		printf("finalList(%d)(%d)\n", i, finalList[i]);
	}

	return 0;
} // end of calculate

// �迭�� �հ踦 ���ϴ� �Լ�
int sum(int *list, int length)
{
	int i, total = 0;

	for (i = 0; i < length; i++)
	{
		total += list[i];
	}
	return total;
}

