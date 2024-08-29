#include <stdio.h>
#include "myLib.h"

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
