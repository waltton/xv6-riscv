#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void
strprepend(char *dst, char *src, int n)
{
  memmove(dst + strlen(src), dst, n - strlen(src));
  memmove(dst, src, strlen(src));
}

int
main(int argc, char *argv[])
{
  int fd;
  struct dirent de;
  char dir[256] = ".";
  char out[256] = "";
  int a, prev_inum, inum, root;

  root = 0;
  inum = 0;
  prev_inum = 0;

  while (!root) {
    fd = open(dir, O_RDONLY);

    a = 0;
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      a++;

      if (a == 1) {
        inum = de.inum;
        continue;
      }

      // inum for '.' and '..' is the same, so it must have reached the root
      if (a == 2 && inum == de.inum) {
        root = 1;
        continue;
      }

      if (de.inum == 0) {
        break;
      }

      if (prev_inum == de.inum) {
        strprepend(out, de.name, sizeof(out));
        strprepend(out, "/", sizeof(out));
      }
    }

    close(fd);

    prev_inum = inum;

    memmove(dir + strlen(dir), "/..", strlen("/.."));
  }

  fprintf(1, "%s\n", out);

  exit(0);
}
