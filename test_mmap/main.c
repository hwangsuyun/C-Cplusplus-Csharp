#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int fd = 0;
    char *file;
    struct stat sb;
    char buf[100] = {0x00,};
    int flag = PROT_WRITE | PROT_READ;

    if (argc < 2)
    {
        printf("Usage: input\n");
        return -1;
    }

    if ((fd = open(argv[1], O_RDWR | O_CREAT)) < 0)
    {
        printf("file open error\n");
        return -1;
    }

    if (fstat(fd, &sb) < 0)
    {
        printf("fstat error\n");
        return -1;
    }

    if ((file = mmap(0, 40, flag, MAP_SHARED, fd, 0)) == -1)
    {
        printf("mmap error\n");
        return -1;
    }

    printf("%s\n", file);

    if (munmap(file, 40) == -1)
    {
        printf("munmap error\n");
        return -1;
    }

    close(fd);
    return 0;
}	
