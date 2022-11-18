#include <stdlib.h>
#include <string.h>

int gethostname(char *name, size_t len)
{
    strcpy(name, "myhost");
    name[len-1] = ' ';

    return 0;
}