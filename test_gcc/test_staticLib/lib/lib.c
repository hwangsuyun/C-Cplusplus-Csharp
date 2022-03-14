#include <stdio.h>
#include "lib.h"

int initMath(Math *math)
{
	math->a = 10;
	math->b = 20;
	math->c = 30;

	math->result = math->a + math->b + math->c;

	return 0;
}
