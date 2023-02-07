#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int
readline(char *new_argv[MAXARG], int cur)
{
  char buf[1024];
  int n = 0;
  while(read(0, buf + n, 1)) {
    if(n == 1023) {
      fprintf(2, "the argument is too long...\n");
      exit(1);
    }

    if(buf[n] == '\n') break;
    n ++ ;
  }

  buf[n] = '\0';
  if(n == 0) return 0;

  int offset = 0;
  while(offset < n) {
    new_argv[cur ++ ] = buf + offset;
    while(buf[offset] != ' ' && offset < n) {
      offset ++ ;
    }

    while(buf[offset] == ' ' && offset < n) {
      buf[offset ++ ] = '\0';
    }
  }

  return cur;
}

void
run(char *cmd, char *new_argv[])
{
	if(fork() == 0) {
		exec(cmd, new_argv);
    exit(0);
	}
	return;
}

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(2, "Usage: xargs command (arg ...)\n");
		exit(1);
	}

  char *cmd = malloc(strlen(argv[1]) + 1);
  char *new_argv[MAXARG];
  strcpy(cmd, argv[1]);

  for(int i = 1; i < argc; i ++ ) {
    new_argv[i - 1] = malloc(strlen(argv[i]) + 1);
    strcpy(new_argv[i - 1], argv[i]);
  }

  int cur = 0;
  while((cur = readline(new_argv, argc - 1)) != 0) {
    new_argv[cur] = 0;

    run(cmd, new_argv);

    wait(0);
  }

	exit(0);
}