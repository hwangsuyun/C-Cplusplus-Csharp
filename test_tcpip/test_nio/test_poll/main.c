#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <poll.h>

#define TIMEOUT 3
#define BUF_LEN 1024

// poll function
int main(int argc, char *argv[])
{
	struct pollfd fds[1];
	int ret;
	char buf[BUF_LEN];
	int len;

	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN;

	//fds[1].fd = STDOUT_FILENO;
	//fds[1].events = POLLOUT;

	while (1) {
		//ret = poll(fds, 2, TIMEOUT * 1000);
		ret = poll(fds, 1, TIMEOUT * 1000);

		if (ret == -1)
		{
			perror("poll");
		}

		if (!ret)
		{
			printf("%d 초 지났음!.\n", TIMEOUT);
		}

		if (fds[0].revents & POLLIN)
		{
			len = read(STDIN_FILENO, buf, BUF_LEN);
			if (len == -1)
			{
				perror("read");
			}

			if (len)
			{
				buf[len] = '\0';
				printf("read: %s\n", buf);
			}

			if (strncmp(buf, "quit", 4) == 0) break;
		}

		 //if (fds[1].revents & POLLOUT) printf("stdout 써졌음!\n");
	}
	return 0;
}
