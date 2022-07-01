#include <stdio.h>
#include <sys/inotify.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

#include <sys/epoll.h>
#include <errno.h>

#include <unistd.h>

//int inotify_init(void);
//int inotify_add_watch(int fd, const char *pathname, uint32_t mask);

//static void handle_events(int fd, int *wd, int argc, char *argv[])
static void handle_events(int fd, int wd, char *argv)
{
    /* 어떤 시스템에서는 올바로 정렬돼 있지 않으면 정수 변수를
       읽을 수 없다. 다른 시스템에서는 정렬이 안 맞으면 성능이
       떨어질 수 있다. 따라서 inotify 파일 디스크립터에서 읽을 때
       쓰는 버퍼의 정렬이 struct inotify_event와 같아야 한다. */

    //char buf[4096]
    //__attribute__ ((aligned(__alignof__(struct inotify_event))));
    char buf[4096];
    const struct inotify_event *event;
    ssize_t len;

    /* inotify 파일 디스크립터에서 이벤트를 읽을 수 있는 동안 반복하기. */

    for (;;) {
        /* 이벤트 읽기 */
        memset(buf, 0x00, sizeof(buf));
        len = read(fd, buf, sizeof(buf));
        if (len == -1 && errno != EAGAIN) {
            printf("read failed\n");
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
            //for (int i = 1; i < argc; ++i) {
                if (wd == event->wd) {
                    //printf("%s/", argv[i]);
                    printf("%s/", argv);
                    //break;
                }
            //}

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

int main(int argc, char** argv)
{
  char buf[64];
  fd_set set;
  struct timeval timeout;
  FD_ZERO (&set);
  int fd = 0;

  memset(buf, 0x00, sizeof(buf));
  sprintf(buf, "%s", "/home/syhwang/test/test_inotify/files");
  printf("buf(%s)\n", buf);

  //int inotify = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
  fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
  //int inotify = inotify_init1(IN_NONBLOCK);

  if (fd < 0) {
    printf("Err inotify_init1\n");
    return 1;
  }

  //uint32_t mask = IN_DELETE | IN_MODIFY | IN_MOVE | IN_ONLYDIR | IN_MOVE_SELF |
  //    IN_DELETE_SELF;

  //int ret = inotify_add_watch(inotify, buf, IN_MODIFY);
  int wd = inotify_add_watch(fd, buf, IN_CREATE | IN_DELETE | IN_DELETE_SELF );
  if (wd == -1) {
      fprintf(stderr, "Cannot watch '%s': %s\n",
              argv, strerror(errno));
      exit(EXIT_FAILURE);
  }

  int epollfd = epoll_create1(EPOLL_CLOEXEC);
    if (epollfd < 0) {
      printf("ERR epoll_creat1\n");
      return 1;
    }

  // Add inotifyfd to epoll set
  struct epoll_event ev;
  //std::memset(&ev, 0, sizeof(ev));
  ev.events = EPOLLIN;
  ev.data.fd = fd;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
    printf("ERR epoll_ctl\n"); //<< ::strerror_r(errno, buf, sizeof(buf));
    return 1;
  }
  int kMaxEvents = 10;
  struct epoll_event events[kMaxEvents];
  int n = 0;
  /*
  do {
    n = epoll_wait(epollfd, events, kMaxEvents, -1);
  } while (n < 0 && errno == EINTR);
  */
  while (1)
  {
    n = epoll_wait(epollfd, events, kMaxEvents, -1);
    if (n == -1) {
        if (errno == EINTR)
            continue;
        perror("poll");
        exit(EXIT_FAILURE);
    }

 if (n > 0) {
        if (ev.events & EPOLLIN) {
            /* inotify 이벤트 있음. */
            //handle_events(fd, wd, argc, argv);
            handle_events(fd, wd, buf);
        }
    }
  }
  printf("Listening for events stopped.\n");

  /* inotify 파일 디스크립터 닫기. */

  close(fd);
  free(&wd);

  return 0;
}