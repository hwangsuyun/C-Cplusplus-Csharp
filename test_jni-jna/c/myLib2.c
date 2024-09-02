#include <stdio.h>
#include "myLib.h"

void callbackTrigger(EventCallback callback) 
{
	int size = 100;
	char answer[size];
	memcpy(answer, "callbackTrigger", sizeof(answer));
	callback(answer, strlen(answer));
}

void printHello()
{
	printf("Hello World !!! jna\n");
	callInnerFunction();
}

int printString(char* str)
{
	printf("%s\n", str);
	return -9;
}

void callInnerFunction()
{
	printf("InnerFunction\n");
}
