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
	
	 //  & 연산자 (and 연산자)
	 //  비트 단위 연산에서 두 개의 비트가 모두 1일 때 1을 반환함.
	int num1 = 20, num2 = 16;
	int result1;
	result1 = num1 & num2;
	printf("비트 단위 & 연산의 결과 %d \n", result1); //결과는 16
	return 0;
}

int or()
{
	// | 연산자 (OR 연산자)
	// 비트 단위 연산에서 두 개의 비트 중의 하나가 1일 때 1을 반환함
	int num1 = 20, num2 = 16;
	int result1;
	result1 = num1 | num2;
	printf("비트 단위 | 연산의 결과 %d \n", result1);  //결과는 20
	return 0;
}

int exclusiveOr()
{
	// ^ 연산자 (Exclusive OR 연산자)
	// 비트 단위 연산에서 두 개의 비트가 서로 같지 않을 경우 1을 반환함
	int num1 = 20, num2 = 16;
	int result1;
	result1 = num1 ^ num2;
	printf("비트 단위 ^ 연산의 결과 %d \n", result1);  //결과는 4
	return 0;
}

int conservatism()
{
	// ~ 연산자 (틸드 연산자) == 보수 연산
	// 비트 단위 연산에서 비트를 반전시킨다.}
	 int num1 = 20;
	 int result1;  
	 result1 = ~num1;
	 printf("비트 단위 ~ 연산의 결과 %d \n", result1);  //결과는 -21
	 return 0;
}

int leftShift()
{
	/*
	   쉬프트 연산자 (<< , >>)
	   비트 연산에서 왼쪽 , 오른쪽 으로 n 한자리씩 이동시킴
	   = 10진수로 고찰하면 2를 곱하고 , 나누는 의미	 

	   왼쪽 shift의 경우 남은 공간은 0으로 채우고 
	   오른쪽 shift의 경우 특수한 상황(음수) 일시 1을 채운다
	   (msb 가 음수일때 1이므로)

	   주의할점
	   not 연산자는 메커니즘 상 원래 숫자를 1.보수 한후 2. 1을 더하는 것이고

	   오른쪽 shift는 메커니즘상 원래 숫자에서 비트 단위 이동후의 부호(+-)를 MBS 로 조정하고,
	   원래 비트 단위 숫자의 10진수표기에 2를 나눈후 다시 2진수로 표기한다.
	 */
	int num1 = 10;
	int result1;
	result1 = num1 << 2;
	printf("비트 단위 << 연산의 결과 %d \n", result1);
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
	printf("비트 단위 >> 연산의 결과 %d \n", result1);
	printf("비트 단위 >> 연산의 결과 %d \n", result2);
	return 0;
}

// 포인터의 연산
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
