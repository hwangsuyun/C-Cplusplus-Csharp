#include <stdio.h>
#include <string.h>

int checkExcludeDir(char *exDir, char *subDir)
{
    char temp[512];
    char excludeDir[512];
    char *ptr = NULL;

    if (strlen(exDir) > 0)
    {
        memset(temp, 0x00, sizeof(temp));
        memset(excludeDir, 0x00, sizeof(excludeDir));
        memcpy(temp, exDir, sizeof(temp));

        while (1)
        {
            ptr = strchr(temp, ';');
            if (ptr == NULL)
            {
                break;
            }

            memcpy(excludeDir, temp, strlen(temp) - strlen(ptr));
            printf("excludeDir : (%s)\n", excludeDir);

	if (strncmp(subDir, excludeDir, strlen(subDir)) == 0
                    && strncmp(excludeDir, subDir, strlen(excludeDir)) == 0)
            {
                return 1;
            }

            memcpy(temp, ptr + 1, strlen(temp));
        }
        if (strlen(temp) > 0)
        {
            memcpy(excludeDir, temp, strlen(temp));
            printf("excludeDir : (%s)\n", excludeDir);

            if (strncmp(subDir, excludeDir, strlen(subDir)) == 0
                    && strncmp(excludeDir, subDir, strlen(excludeDir)) == 0)
            {
                return 1;
            }
        }
    }

    return 0;
}

int main()
{
    int ret = 0;
    char exDir[512];
    char subDir[512];

    memset(exDir, 0x00, sizeof(exDir));
    memset(subDir, 0x00, sizeof(subDir));
    memcpy(exDir, "/home/xlogfile/excludeDir3;/home/xlogfile/excludeDir33;/home/xlogfile/excludeDir333", sizeof(exDir));
    memcpy(subDir, "/home/xlogfile/excludeDir333", sizeof(subDir));

    ret = checkExcludeDir(exDir, subDir);
    printf("ret : (%d)\n", ret);
    return 0;
}