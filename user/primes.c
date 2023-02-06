#include "kernel/types.h"
#include "user/user.h"

#define END_READ   0
#define END_WRITE  1

void
sieve(int pipe_l[2])
{
  int prime, cur;
  if(read(pipe_l[END_READ], &prime, sizeof(prime)) != sizeof(prime)) {
    fprintf(2, "failed to read from the pipe_l\n");
    exit(1);
  }

  printf("prime %d\n", prime);

  if(read(pipe_l[END_READ], &cur, sizeof(cur)) == sizeof(cur)) {
    int pipe_r[2];
    pipe(pipe_r);

    if(fork() != 0) {
      do {
        if(cur % prime == 0) {
          continue;
        }

        if(write(pipe_r[END_WRITE], &cur, sizeof(cur)) != sizeof(cur)) {
          fprintf(2, "failed to write into the pipe_r");
          exit(1);
        }
      } while(read(pipe_l[END_READ], &cur, sizeof(cur)) == sizeof(cur));

      close(pipe_r[END_WRITE]);
      close(pipe_l[END_READ]);

      wait(0);
    } else {
      close(pipe_r[END_WRITE]);
      close(pipe_l[END_READ]);
      sieve(pipe_r);
    }
  }

  exit(1);
}

int
main(int argc, char *argv[])
{
  int input[2];
  pipe(input);
  int start = 2, end = 35;

  if(fork() != 0) {
    close(input[END_READ]);

    for(int i = start; i <= end; i ++ ) {
      if(write(input[END_WRITE], &i, sizeof(i)) != sizeof(i)) {
        fprintf(2, "failed to write %d into the pipe in the parent", i);
        exit(1);
      }
    }

    close(input[END_WRITE]);
    wait(0);
  } else {
    close(input[END_WRITE]);
    sieve(input);
  }

  exit(0);
}