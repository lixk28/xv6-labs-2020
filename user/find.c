#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

static char buf[512] = { '\0' };
static char *p = buf;

void
find(char *path, char *name)
{
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    exit(1);
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    exit(1);
  }

  *p++ = '/';

  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;

    memcpy(p, de.name, DIRSIZ);

    // name matches
    if (!strcmp(p, name))
      printf("%s\n", buf);

    if (stat(buf, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", buf);
      exit(1);
    }

    // recurse into subdirectories but . and .. excluded
    if (strcmp(p, ".") && strcmp(p, "..") && st.type == T_DIR) {
      p += strlen(de.name);
      find(buf, name);
      p -= strlen(de.name);
    }

    memset(p, '\0', DIRSIZ);
  }

  *p-- = '\0';

  return;
}

int
main(int argc, char *argv[])
{
  if (argc != 3) {
    fprintf(2, "usage: find <dirpath> <name>\n");
    exit(1);
  }

  char *path = argv[1];
  char *name = argv[2];

  *p++ = '.';

  find(path, name);

  exit(0);
}
