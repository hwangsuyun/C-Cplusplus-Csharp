#include <stdio.h>

int and();
int or();
int exclusiveOr();
int conservatism();
int leftShift();
int rightShift();
int operatePointer();

int main()
{
	and();
	or();
	exclusiveOr();
	conservatism();
	leftShift();
	rightShift();
	operatePointer();
	
	return 0;
}

int and()
{
	
	 //  & ������ (and ������)
	 //  ��Ʈ ���� ���꿡�� �� ���� ��Ʈ�� ��� 1�� �� 1�� ��ȯ��.
	int num1 = 20, num2 = 16;
	int result1;
	result1 = num1 & num2;
	printf("��Ʈ ���� & ������ ��� %d \n", result1); //����� 16
	return 0;
}

int or()
{
	// | ������ (OR ������)
	// ��Ʈ ���� ���꿡�� �� ���� ��Ʈ ���� �ϳ��� 1�� �� 1�� ��ȯ��
	int num1 = 20, num2 = 16;
	int result1;
	result1 = num1 | num2;
	printf("��Ʈ ���� | ������ ��� %d \n", result1);  //����� 20
	return 0;
}

int exclusiveOr()
{
	// ^ ������ (Exclusive OR ������)
	// ��Ʈ ���� ���꿡�� �� ���� ��Ʈ�� ���� ���� ���� ��� 1�� ��ȯ��
	int num1 = 20, num2 = 16;
	int result1;
	result1 = num1 ^ num2;
	printf("��Ʈ ���� ^ ������ ��� %d \n", result1);  //����� 4
	return 0;
}

int conservatism()
{
	// ~ ������ (ƿ�� ������) == ���� ����
	// ��Ʈ ���� ���꿡�� ��Ʈ�� ������Ų��.}
	 int num1 = 20;
	 int result1;  
	 result1 = ~num1;
	 printf("��Ʈ ���� ~ ������ ��� %d \n", result1);  //����� -21
	 return 0;
}

int leftShift()
{
	/*
	   ����Ʈ ������ (<< , >>)
	   ��Ʈ ���꿡�� ���� , ������ ���� n ���ڸ��� �̵���Ŵ
	   = 10������ �����ϸ� 2�� ���ϰ� , ������ �ǹ�	 

	   ���� shift�� ��� ���� ������ 0���� ä��� 
	   ������ shift�� ��� Ư���� ��Ȳ(����) �Ͻ� 1�� ä���
	   (msb �� �����϶� 1�̹Ƿ�)

	   ��������
	   not �����ڴ� ��Ŀ���� �� ���� ���ڸ� 1.���� ���� 2. 1�� ���ϴ� ���̰�

	   ������ shift�� ��Ŀ����� ���� ���ڿ��� ��Ʈ ���� �̵����� ��ȣ(+-)�� MBS �� �����ϰ�,
	   ���� ��Ʈ ���� ������ 10����ǥ�⿡ 2�� ������ �ٽ� 2������ ǥ���Ѵ�.
	 */
	int num1 = 10;
	int result1;
	result1 = num1 << 2;
	printf("��Ʈ ���� << ������ ��� %d \n", result1);
	return 0;
}

int rightShift()
{
	int num1 = 10;
	int num2 = -10;
	int result1;
	int result2;
	result1 = num1 >> 1;
	result2 = num2 >> 1;
	printf("��Ʈ ���� >> ������ ��� %d \n", result1);
	printf("��Ʈ ���� >> ������ ��� %d \n", result2);
	return 0;
}

// �������� ����
int operatePointer()
{
	char temp;
	char *pointer;
	int idx;
	
	pointer = "1";
	temp = *pointer;
	//idx = atoi(&temp);
	idx = temp - '0';  // '0' = 0x30
	
	printf("pointer(%c)\n", *pointer);
	printf("temp(%c)\n", temp);
	printf("idx(%d)\n", idx);
	
	return 0;
}
