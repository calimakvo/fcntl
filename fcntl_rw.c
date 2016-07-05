#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

static int end  = 0;

int lock(char *);
void unlock(int fd);
void sighandler(int sig);
void errexit(void);

void errexit()
{
  printf("fcntl failure errno=%d\n", errno);
  perror("fail");
  exit(EXIT_FAILURE);
}

void unlock(int fd)
{
  close(fd);
}

int lock(char *lockfile)
{
  int lockfd, res;
  struct flock region;

  if ((lockfd = open(lockfile, O_WRONLY|O_CREAT, 0600)) == -1) {
    puts("open failed");
    exit(EXIT_FAILURE);
  }

  region.l_type = F_WRLCK;
  region.l_whence = SEEK_SET;
  region.l_start = 0;
  region.l_len = 0; /* all bytes lock */

  do{
    res = fcntl(lockfd, F_SETLKW, &region);
    if(res == -1) {
      switch(errno) {
        case EAGAIN:
        case EACCES:
        case EINTR: /* F_SETLKW */
          if(errno == EINTR) {
            printf("Get EINTR\n");
          }
          printf("Retry lock...\n");
          sleep(1);
          break;
        default:
	  errexit();
      }
    }
  } while(res == -1 && end == 0);
  puts("Lock success.");
  return (lockfd);
}

void sighandler(int sig)
{
  if(sig == SIGINT) {
    puts("get SIGINT");
    end = 1;
  }
  if(sig == SIGUSR1) {
    puts("get SIGUSR1");
  }
}
 
int main(void)
{
  int fd;
  struct sigaction act, oact;
  act.sa_handler = sighandler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
#ifdef SA_INTERRUPT
  act.sa_flags |= SA_INTERRUPT;
#elif SA_RESTART
  act.sa_flags |= SA_RESTART;
#endif
  if(sigaction(SIGINT, &act, &oact) < 0) {
    errexit();
  }

  if(sigaction(SIGUSR1, &act, &oact) < 0) {
    errexit();
  }

  fd = lock("/tmp/fuck.lock");
  printf("lock fd = %d\n", fd);
  while(end == 0) {
    printf("run loop...\n");
    sleep(1);
  }
  unlock(fd);
  puts("Close fd");
  return 0;
}
