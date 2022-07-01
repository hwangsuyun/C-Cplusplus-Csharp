#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>

/* 파일 디스크립터 'fd'에서 가용 inotify 이벤트를 모두 읽어 들인다.
   wd는 argv 내 디렉터리들에 대한 감시 디스크립터들의 테이블이다.
   argc는 wd 및 argv의 길이이다.
   argv는 감시 대상 디렉터리들의 목록이다.
   wd 및 argv의 0번 항목은 쓰지 않는다. */

static void
handle_events(int fd, int *wd, int argc, char *argv[])
{
    /* 어떤 시스템에서는 올바로 정렬돼 있지 않으면 정수 변수를
       읽을 수 없다. 다른 시스템에서는 정렬이 안 맞으면 성능이
       떨어질 수 있다. 따라서 inotify 파일 디스크립터에서 읽을 때
       쓰는 버퍼의 정렬이 struct inotify_event와 같아야 한다. */

    char buf[4096]
        __attribute__ ((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    ssize_t len;

    /* inotify 파일 디스크립터에서 이벤트를 읽을 수 있는 동안 반복하기. */

    for (;;) {

        /* 이벤트 읽기 */

        len = read(fd, buf, sizeof(buf));
        if (len == -1 && errno != EAGAIN) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        /* 논블로킹 read()에서 읽을 이벤트가 없으면 -1을 반환하며
           errno를 EAGAIN으로 설정한다. 그러면 루프에서 빠져나간다. */

        if (len <= 0)
            break;

        /* 버퍼 내 모든 이벤트 돌기. */

        for (char *ptr = buf; ptr < buf + len;
                ptr += sizeof(struct inotify_event) + event->len) {

            event = (const struct inotify_event *) ptr;

            /* 이벤트 타입 찍기. */

            if (event->mask & IN_OPEN)
                printf("IN_OPEN: ");
            if (event->mask & IN_CLOSE_NOWRITE)
                printf("IN_CLOSE_NOWRITE: ");
            if (event->mask & IN_CLOSE_WRITE)
                printf("IN_CLOSE_WRITE: ");

            /* 감시 대상 디렉터리 이름 찍기. */

            for (int i = 1; i < argc; ++i) {
                if (wd[i] == event->wd) {
                    printf("%s/", argv[i]);
                    break;
                }
             }

            /* 파일 이름 찍기. */

            if (event->len)
                printf("%s", event->name);

            /* 파일 시스템 객체 종류 찍기. */

            if (event->mask & IN_ISDIR)
                printf(" [directory]\n");
            else
                printf(" [file]\n");
        }
    }
}

int
main(int argc, char *argv[])
{
    char buf;
    int fd, i, poll_num;
    int *wd;
    nfds_t nfds;
    struct pollfd fds[2];

    if (argc < 2) {
        printf("Usage: %s PATH [PATH ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Press ENTER key to terminate.\n");

    /* inotify API 접근을 위한 파일 디스크립터 만들기. */

    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1) {
        perror("inotify_init1");
        exit(EXIT_FAILURE);
    }

    /* 감시 디스크립터를 위한 메모리 할당하기. */

    wd = calloc(argc, sizeof(int));
    if (wd == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* 디렉터리에 다음 이벤트 표시:
       - 파일 열림
       - 파일 닫힘 */

    for (i = 1; i < argc; i++) {
        wd[i] = inotify_add_watch(fd, argv[i],
                                  IN_OPEN | IN_CLOSE);
        if (wd[i] == -1) {
            fprintf(stderr, "Cannot watch '%s': %s\n",
                    argv[i], strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    /* 폴링 준비하기. */

    nfds = 2;

    fds[0].fd = STDIN_FILENO;       /* 콘솔 입력 */
    fds[0].events = POLLIN;

    fds[1].fd = fd;                 /* inotify 입력 */
    fds[1].events = POLLIN;

    /* 이벤트 및/또는 터미널 입력 기다리기. */

    printf("Listening for events.\n");
    while (1) {
        poll_num = poll(fds, nfds, -1);
        if (poll_num == -1) {
            if (errno == EINTR)
                continue;
            perror("poll");
            exit(EXIT_FAILURE);
        }
      
        if (poll_num > 0) {

            if (fds[0].revents & POLLIN) {

                /* 콘솔 입력 있음. stdin 비우고 끝내기. */

                while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    continue;
                break;
            }

            if (fds[1].revents & POLLIN) {

                /* inotify 이벤트 있음. */

                handle_events(fd, wd, argc, argv);
            }
        }
    }

    printf("Listening for events stopped.\n");

    /* inotify 파일 디스크립터 닫기. */

    close(fd);

    free(wd);
    //exit(EXIT_SUCCESS);
    return 0;
}