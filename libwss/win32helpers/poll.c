#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <ws2tcpip.h>

int poll (struct pollfd *fds, int cnt, int timeout_ms)
{
    int n;
    int i;
    int cfd;

    fd_set fdreads;
    fd_set fdwrites;
    fd_set fderrors;
    struct timeval tv;

    FD_ZERO(&fdreads);
    FD_ZERO(&fdwrites);
    FD_ZERO(&fderrors);

    for (i = 0; i < cnt; ++i) {
        FD_SET(fds[i].fd, &fdreads);
        FD_SET(fds[i].fd, &fdwrites);
        FD_SET(fds[i].fd, &fderrors);
    }

    tv.tv_sec = 0;
    tv.tv_usec = timeout_ms * 1000;
    fprintf(stderr, "before poll over select...\n");
    n = select(cnt+1, &fdreads, &fdwrites, &fderrors, &tv);
    fprintf(stderr, "after poll over select: %d\n", n);
    if (n < 0) {
        return n;
    }

    if (n == 0) {
        return 0;
    }

    if (n > 0) {
        for (i = 0; i < cnt; ++i) {
	  if (FD_ISSET(fds[i].fd, &fdreads)) {
	     fds[i].revents = POLLIN;
	  }
	  if (FD_ISSET(fds[i].fd, &fdwrites)) {
	    fds[i].revents |= POLLOUT;
	  }

	  if (FD_ISSET(fds[i].fd, &fderrors)) {
	    fds[i].revents |= POLLERR;
	  }
        }
    }

    return n;
}
