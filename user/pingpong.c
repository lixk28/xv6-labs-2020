#include "kernel/types.h"
#include "user/user.h"

int
main()
{
  int fd[2];

  if (pipe(fd) == -1) {
    fprintf(2, "Failed to create pipe\n");
    exit(1);
  }

  char buf[32] = {'\0'};

  int pid = fork();
  if (pid == 0) { // child
    read(fd[0], buf, 4);
    close(fd[0]);
    printf("%d: received %s\n", getpid(), buf);
    write(fd[1], "pong", 4);
    close(fd[1]);
    exit(0);
  } else if (pid > 0) { // parent
    write(fd[1], "ping", 4);
    close(fd[1]);
    wait(0);
    read(fd[0], buf, 4);
    close(fd[0]);
    printf("%d: received %s\n", getpid(), buf);
    exit(0);
  } else {
    fprintf(2, "Failed to create child process\n");
    exit(1);
  }

  exit(0);
}
