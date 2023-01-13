#include "kernel/types.h"
#include "user/user.h"

void
prime_filter(int read_end)
{
  int p;
  if (!read(read_end, &p, sizeof(int))) {
    close(read_end);
    exit(0);
  }
  printf("prime %d\n", p);

  int fd[2];
  if (pipe(fd) == -1) {
    fprintf(2, "Failed to create pipe\n");
    exit(1);
  }

  int pid = fork();
  if (pid == 0) {
    close(fd[1]);
    prime_filter(fd[0]);
    close(fd[0]);
    exit(0);
  } else if (pid > 0) {
    close(fd[0]);

    int n;
    while (read(read_end, &n, sizeof(int))) {
      if (n % p)
        write(fd[1], &n, sizeof(int));
    }

    close(fd[1]);
    wait(0);
    exit(0);
  } else {
    fprintf(2, "Failed to create child process\n");
    exit(1);
  }

  exit(0);
}

int
main()
{
  int fd[2];
  if (pipe(fd) == -1) {
    fprintf(2, "Failed to create pipe\n");
    exit(1);
  }

  int pid = fork();
  if (pid == 0) { // child
    close(fd[1]);
    prime_filter(fd[0]);
    close(fd[0]);
    exit(0);
  } else if (pid > 0) { // parent
    close(fd[0]);

    // feed the numbers 2 through 35 into the pipeline
    for (int n = 2; n <= 35; n++)
      write(fd[1], &n, sizeof(int));

    close(fd[1]);
    wait(0);
    exit(0);
  } else {
    fprintf(2, "Failed to create child process\n");
    exit(1);
  }

  exit(0);
}
