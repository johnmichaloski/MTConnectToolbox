/*!
  \file semtest.c

  \brief Test program for ulapi semaphores.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stddef.h>
#include <signal.h>
#include "ulapi.h"		/* these decls */

#define SEM_KEY 12321

/*
  Syntax: semtest <arg>

  With any arg, is the master, otherwise is the slave.

  Master takes, prints, waits a second, gives, waits a second.

  Slave takes, prints, gives, takes, etc. with no waits.

  Slave should run really fast until there's a master.
*/

static int done = 0;
static void quit(int sig)
{
  done = 1;
}

int main(int argc, char *argv[])
{
  int master;
  void *sem;

  if (argc > 1) master = 1;
  else master = 0;

  if (ULAPI_OK != ulapi_init()) {
    fprintf(stderr, "can't init ulapi\n");
    return 1;
  }

  sem = ulapi_sem_new(SEM_KEY);
  if (master) ulapi_sem_give(sem);

  if (NULL == sem) {
    fprintf(stderr, "can't create semaphore\n");
    return 1;
  }

  signal(SIGINT, quit);

  while (! done) {
    printf("trying it\n");
    ulapi_sem_take(sem);
    printf("got it\n");
    if (master) ulapi_sleep(1);
    ulapi_sem_give(sem);
    printf("gave it\n");
    if (master) ulapi_sleep(1);
  }

  ulapi_sem_delete(sem);

  return 0;
}
