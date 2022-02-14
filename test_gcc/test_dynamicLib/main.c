// main.c

#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>

#include "mymath.h"

int main(int argc, char **argv)
{
	void *handle = NULL;
	int (*result)(int, int);

	handle = dlopen("./libmymath.so", RTLD_NOW);

	if( !handle ) {
		printf("fail to dlopen, %s\n", dlerror());
		return 0;
	}

	result = dlsym(handle, "sum");

	if( dlerror() != NULL) {
		printf("fial to dlsym, %s\n", dlerror());
		return 0;
	}

	printf("10 + 20 = %d\n", result(10, 20));
	dlclose(handle);
	return 0;
}
