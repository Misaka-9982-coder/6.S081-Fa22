#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define END_READ   0
#define END_WRITE  1

int
main(int argc, char *argv[])
{
  int p2c[2], c2p[2];

  pipe(p2c);
  pipe(c2p);

  if(fork() != 0) {

    if(write(p2c[END_WRITE], "!", 1) != 1) {
      fprintf(2, "failed to write in parent\n");
      exit(1);
    }

    wait(0);

    printf("%d: received pong\n", getpid());
  } else {
    char buf[2];

    if(read(p2c[END_READ], buf, 1) != 1) {
      fprintf(2, "failed to read in child\n");
      exit(1);
    }

    printf("%d: received ping\n", getpid());

    if(write(c2p[END_WRITE], buf, 1) != 1) {
      fprintf(2, "failed to write in child\n");
      exit(1);
    }
  }

  exit(0);
}