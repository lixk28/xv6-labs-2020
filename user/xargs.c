#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAX_LINE_SIZE 512

void
parse_args(char *line, char *args[], int j)
{
  int i = 0;
  while (i < MAX_LINE_SIZE) {
    if (!line[i])
      break;

    while (line[i] == ' ' && i < MAX_LINE_SIZE)
      i++;

    int k = i;
    while (line[k] != ' ' && k < MAX_LINE_SIZE)
      k++;
    int len = k - i;

    char *arg = malloc(sizeof(char) * (len + 1));
    memcpy(arg, line + i, len);
    arg[len] = '\0';

    args[j++] = arg;
    i = k;
  }
}

void
xargs(int argc, char *argv[])
{
  // argv[0] is xargs
  // argv[1] is the executable
  char *path = argv[1];
  char *args[MAXARG] = {0};

  // copy the initial arguments
  int i = 1, j = 0;
  while (i < argc)
    args[j++] = argv[i++];

  static char line[MAX_LINE_SIZE] = {'\0'};
  int k = 0;
  char c = '\0';
  while (read(0, &c, sizeof(char)) == sizeof(char)) {
    if (c == '\n') {
      // parse line arguments
      parse_args(line, args, j);
      memset(line, '\0', MAX_LINE_SIZE);
      k = 0;

      int pid = fork();
      if (pid == -1) {
        fprintf(2, "xargs: failed to create child process\n");
        exit(1);
      }

      // child execute the command and exit
      if (pid == 0) {
        if (exec(path, args) == -1) {
          fprintf(2, "xargs: failed to execute %s\n", path);
          exit(1);
        }
        exit(0);
      }

      // parent waits for child to finish
      wait(0);

      for (int x = j; x < MAXARG; x++) {
        if (args[x])
          free(args[x]);
      }
    } else {
      line[k++] = c;
    }
  }
}

int
main(int argc, char *argv[])
{
  xargs(argc, argv);
  exit(0);
}
