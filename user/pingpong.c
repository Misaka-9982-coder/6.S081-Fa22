#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define END_READ   0
#define END_WRITE  1

int
main(int argc, char *argv[])
{
  int p2c[2], c2p[2];

  pipe(p2c);  // 创建用于 父进程 -> 子进程 的管道
  pipe(c2p);  // 创建用于 子进程 -> 父进程 的管道

  if(fork() != 0) {
    char buf[2];

    // 1. 父进程 首先向 子进程 发送字节
    if(write(p2c[END_WRITE], "!", 1) != 1) {
      fprintf(2, "failed to write in parent\n");
      exit(1);
    }

    close(p2c[END_WRITE]);
    wait(0);

    // 2. 父进程 发送完成后，开始等待 子进程 的回复
    if(read(c2p[END_READ], buf, 1) != 1) {
        fprintf(2, "failed to read in parent");
        exit(1);
    }

    // 5. 子进程 收到数据， read 返回， 输出 pong
    printf("%d: received pong\n", getpid());
    close(p2c[END_READ]);
  } else {
    char buf[2];

    // 3. 子进程 读取管道， 收到 父进程 发送的字节数据
    if(read(p2c[END_READ], buf, 1) != 1) {
      fprintf(2, "failed to read in child\n");
      exit(1);
    }

    close(c2p[END_READ]);
    printf("%d: received ping\n", getpid());

    // 4. 子进程 通过 子->父 管道，将字节送回 父进程
    if(write(c2p[END_WRITE], buf, 1) != 1) {
      fprintf(2, "failed to write in child\n");
      exit(1);
    }

    close(c2p[END_WRITE]);
  }

  exit(0);
}