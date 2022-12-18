#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

typedef struct {
    char **dirFileName;
    int dirFileCount;
} ROOTDIRECTORY;
ROOTDIRECTORY *rootDirectory;

int main()
{
    // size_t malloc_usable_size (void *ptr);
    // this function return using memory size

    rootDirectory = malloc(sizeof(ROOTDIRECTORY));
    //printf("rootDirectory size : (%d)\n", sizeof(rootDirectory));
    printf("rootDirectory size : (%d)\n", malloc_usable_size(rootDirectory));

    rootDirectory->dirFileName = (char **)malloc(sizeof(char *) * 100);
    //printf("rootDirectory->dirFileName size : (%d)\n", sizeof(rootDirectory->dirFileName));
    printf("rootDirectory->dirFileName size : (%d)\n", malloc_usable_size(rootDirectory->dirFileName));
    if (rootDirectory->dirFileName == NULL)
    {
        printf("rootDirectory->dirFileName is NULL\n");
        return -1;
    }
    rootDirectory->dirFileName[0] = (char *)malloc(sizeof(char) * 100 * 512);
    printf("rootDirectory->dirFileName[0] size : (%d)\n", malloc_usable_size(rootDirectory->dirFileName[0]));
    if (rootDirectory->dirFileName[0] == NULL)
    {
        printf("rootDirectory->dirFileName[0] is NULL\n");
        return -1;
    }

    for (int i = 1; i < 100; i++)
    {
		rootDirectory->dirFileName[i] = rootDirectory->dirFileName[i - 1] + 512;
    }

    for (int i = 0; i < 10; i++)
    {
        memcpy(rootDirectory->dirFileName[i], "123", sizeof(rootDirectory->dirFileName[i]));
        printf("rootDirectory->dirFileName[%d] : (%s)\n", i, rootDirectory->dirFileName[i]);
    }
    rootDirectory->dirFileCount = 10;
    printf("rootDirectory->dirFileCount : (%d)\n", rootDirectory->dirFileCount);

    // memset(rootDirectory, 0x00, sizeof(ROOTDIRECTORY)); (X)
    // This structure cannot be initialized by memset. 
    // because of rootDirectory->dirFileName[0] = (char *)malloc(sizeof(char) * 100 * 512); , 
    //         rootDirectory->dirFileName[i] = rootDirectory->dirFileName[i - 1] + 512; 
    // this logics are to cause an error        
    // segmant fault 발생
    for (int i = 0; i < 10; i++)
    {
        memset(rootDirectory->dirFileName[i], 0x00, sizeof(rootDirectory->dirFileName[i]));
        printf("rootDirectory->dirFileName[%d] : (%s)\n", i, rootDirectory->dirFileName[i]);
    }
    rootDirectory->dirFileCount = 0;
    printf("rootDirectory->dirFileCount : (%d)\n", rootDirectory->dirFileCount);

    if (rootDirectory->dirFileName != NULL)
    {
        free(rootDirectory->dirFileName[0]);
        printf("free rootDirectory->dirFileName[0]\n");
        free(rootDirectory->dirFileName);
        printf("free rootDirectory->dirFileName\n");
    }

    if (rootDirectory != NULL)
    {
        free(rootDirectory);
        printf("free rootDirectory\n");
	}

    return 0;
}
