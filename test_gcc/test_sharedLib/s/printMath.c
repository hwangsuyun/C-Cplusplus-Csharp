#include <stdio.h>
#include "printMath.h"

int printMath(Math math)
{
	printf("math.a(%d)\n", math.a);
	printf("math.b(%d)\n", math.b);
	printf("math.c(%d)\n", math.c);
	printf("math.result(%d)\n", math.result);

	return 0;
}
