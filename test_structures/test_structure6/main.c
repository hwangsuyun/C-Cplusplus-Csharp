#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

typedef struct {
    char **dirFileName;
    int dirFileCount;
} ROOTDIRECTORY;
ROOTDIRECTORY **rootDirectory;

int main()
{
    // size_t malloc_usable_size (void *ptr);
    // this function return using memory size

    rootDirectory = (ROOTDIRECTORY **)malloc(sizeof(ROOTDIRECTORY) * 10);
    if (rootDirectory == NULL)
    {
        return -1;
    }
    //printf("rootDirectory size : (%d)\n", sizeof(rootDirectory));
    printf("rootDirectory size : (%d)\n", malloc_usable_size(rootDirectory));

    rootDirectory[0] = (ROOTDIRECTORY *)malloc(sizeof(ROOTDIRECTORY) * 10 * 10);
    if(rootDirectory[0] == NULL)
    {
        return -1;
    }
    for (int i = 1; i < 10; i++)
    {
        rootDirectory[i] = rootDirectory[i - 1] + 10;
    }

    for (int i = 0; i < 10; i++)
    {
        rootDirectory[i]->dirFileName = (char **)malloc(sizeof(char *) * 100);
        //printf("rootDirectory->dirFileName size : (%d)\n", sizeof(rootDirectory->dirFileName));
        printf("rootDirectory[%d]->dirFileName size : (%d)\n", i, malloc_usable_size(rootDirectory[i]->dirFileName));
        if (rootDirectory[i]->dirFileName == NULL)
        {
            printf("rootDirectory[%d]->dirFileName is NULL\n", i);
            return -1;
        }
        rootDirectory[i]->dirFileName[0] = (char *)malloc(sizeof(char) * 100 * 512);
        printf("rootDirectory[%d]->dirFileName[0] size : (%d)\n", i, malloc_usable_size(rootDirectory[i]->dirFileName[0]));
        if (rootDirectory[i]->dirFileName[0] == NULL)
        {
            printf("rootDirectory[%d]->dirFileName[0] is NULL\n", i);
            return -1;
        }

        for (int j = 1; j < 100; j++)
        {
            rootDirectory[i]->dirFileName[j] = rootDirectory[i]->dirFileName[j - 1] + 512;
        }

        for (int j = 0; j < 10; j++)
        {
            memcpy(rootDirectory[i]->dirFileName[j], "123", sizeof(rootDirectory[i]->dirFileName[j]));
            printf("rootDirectory[%d]->dirFileName[%d] : (%s)\n", i, j, rootDirectory[i]->dirFileName[j]);
        }
        rootDirectory[i]->dirFileCount = 10;
        printf("rootDirectory[%d]->dirFileCount : (%d)\n", i, rootDirectory[i]->dirFileCount);

        for (int j = 0; j < 10; j++)
        {
            memset(rootDirectory[i]->dirFileName[j], 0x00, sizeof(rootDirectory[i]->dirFileName[j]));
            printf("rootDirectory[%d]->dirFileName[%d] : (%s)\n", i, j, rootDirectory[i]->dirFileName[j]);
        }
        rootDirectory[i]->dirFileCount = 0;
        printf("rootDirectory[%d]->dirFileCount : (%d)\n", i, rootDirectory[i]->dirFileCount);
    }

    // memset(rootDirectory, 0x00, sizeof(ROOTDIRECTORY)); (X)
    // This structure cannot be initialized by memset. 
    // because of rootDirectory->dirFileName[0] = (char *)malloc(sizeof(char) * 100 * 512); , 
    //         rootDirectory->dirFileName[i] = rootDirectory->dirFileName[i - 1] + 512; 
    // this logics are to cause an error        
    // segmant fault 발생
    /*
    for (int i = 0; i < 10; i++)
    {
        memset(rootDirectory->dirFileName[i], 0x00, sizeof(rootDirectory->dirFileName[i]));
        printf("rootDirectory->dirFileName[%d] : (%s)\n", i, rootDirectory->dirFileName[i]);
    }
    rootDirectory->dirFileCount = 0;
    printf("rootDirectory->dirFileCount : (%d)\n", rootDirectory->dirFileCount);
    */

    for (int i = 0; i < 10; i++)
    {
        if (rootDirectory[i]->dirFileName != NULL)
        {
            free(rootDirectory[i]->dirFileName[0]);
            rootDirectory[i]->dirFileName[0] = NULL;
            printf("free rootDirectory[%d]->dirFileName[0]\n", i);
            free(rootDirectory[i]->dirFileName);
            rootDirectory[i]->dirFileName = NULL;
            printf("free rootDirectory[%d]->dirFileName\n", i);
        }
    }

    if (rootDirectory != NULL)
    {
        free(rootDirectory[0]);
        rootDirectory[0] = NULL;
        printf("free rootDirectory[0]\n");
        free(rootDirectory);
        rootDirectory = NULL;
        printf("free rootDirectory\n");
    }

    return 0;
}