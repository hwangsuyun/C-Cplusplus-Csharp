#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

char **temp1;
char **temp2;

int initialize()
{
    temp1 = (char **)malloc(sizeof(char *) * 100);
    if (temp1 == NULL) return -1;
    temp1[0] = (char *)malloc(sizeof(char *) * 100 * 512);
    if (temp1[0] == NULL) return -1;
    for (int i = 1; i < 100; i++)
    {
        temp1[i] = temp1[i - 1] + 512;
    }

    temp2 = (char **)malloc(sizeof(char *) * 100);
    if (temp2 == NULL) return -1;
    temp2[0] = (char *)malloc(sizeof(char *) * 100 * 512);
    if (temp2[0] == NULL) return -1;
    for (int i = 1; i < 100; i++)
    {
        temp2[i] = temp2[i - 1] + 512;
    }

    return 0;
}

int copyFirstData()
{
    memcpy(temp1[0], "home/syhwang/test/1.txt", 100);
    memcpy(temp1[1], "home/syhwang/test/2.txt", 100);
    memcpy(temp1[2], "home/syhwang/test/3.txt", 100);
    memcpy(temp1[3], "home/syhwang/test/4.txt", 100);
    memcpy(temp1[4], "home/syhwang/test/5.txt", 100);
    memcpy(temp1[5], "home/syhwang/test/6.txt", 100);
    memcpy(temp1[6], "home/syhwang/test/7.txt", 100);
    memcpy(temp1[7], "home/syhwang/test/8.txt", 100);
    memcpy(temp1[8], "home/syhwang/test/9.txt", 100);
    memcpy(temp1[9], "home/syhwang/test/0.txt", 100);

    return 0;
}

int compare()
{
    bool flag = false;
    int index = -1;

    for (int i = 0; i < 100; i++)
    {
        if (strlen(temp1[i]) == 0) break;

        for (int j = 0; j < 100; j++)
        {
            if (strlen(temp2[j]) == 0 && index == -1)
            {
                index = j;
            }

            if (strncmp(temp1[i], temp2[j], strlen(temp1[i])) == 0)
            {
                flag = true;
                break;
            }
        }

        if (!flag)
        {
            memcpy(temp2[index], temp1[i], strlen(temp1[i]));
        }

        index = -1;
        flag = false;
    }

    return 0;
}

int printData()
{
    for (int i = 0; i < 100; i++)
    {
        //if (strlen(temp2[i]) == 0) break;
	if (i > 11) break;

        printf("temp2[%d] : (%s)\n", i, temp2[i]);
    }

    return 0;
}

int copySecondData()
{
    for (int i = 0; i < 100; i++)
    {
        if (strlen(temp1[i]) == 0) break;

        memset(temp1[i], 0x00, 100);
    }

    memcpy(temp1[0], "test.txt", 100);

    return 0;
}

int deleteData()
{
    memset(temp2[3], 0x00, 100);
    memset(temp2[8], 0x00, 100);
    return 0;
}

int copyThirdData()
{
    for (int i = 0; i < 100; i++)
    {
        if (strlen(temp1[i]) == 0) break;

        memset(temp1[i], 0x00, 100);
    }

    memcpy(temp1[0], "test333.txt", 100);
    memcpy(temp1[1], "test444.txt", 100);
    memcpy(temp1[2], "test555.txt", 100);

    return 0;
}

int freeData()
{
    if (temp1 != NULL)
    {
        free(temp1[0]);
        free(temp1);
    }
    if (temp2 != NULL)
    {
        free(temp2[0]);
        free(temp2);
    }

    return 0;
}

int main()
{
    char data[100]; 
    memset(data, 0x00, sizeof(data));

    initialize();
    copyFirstData();
    compare();
    printData();
    printf("\n");

    copySecondData();
    compare();
    printData();
    printf("\n");

    deleteData();
    printData();
    printf("\n");

    copyThirdData();
    compare();
    printData();
    printf("\n"); 

    freeData();
    return 0;
}
