/*
  stub_rtapi.c

  Fill me in with your platform's RTAPI implementation. Some ANSI C
  stuff is in here already.
*/

#include <stdio.h>		/* vprintf() */
#include <stddef.h>		/* NULL */
#include <stdlib.h>		/* malloc(), sizeof(), atexit(), strtol() */
#include <string.h>		/* strncmp(), strlen() */
#include <stdarg.h>		/* va_* */
#include <ctype.h>		/* isspace */
#include "rtapi.h"		/* these decls */

char *rtapi_strncpy(char *dest, const char *src, rtapi_integer n)
{
  char *ptr;
  rtapi_integer i;

  for (ptr = dest, i = 0; i < n; ptr++, src++, i++) {
    *ptr = *src;
    if (0 == *src) break;
  }

  return dest;
}

rtapi_result rtapi_system(const char *prog, rtapi_integer *result)
{
  int retval;

  retval = system(prog) >> 8;

  if (127 == retval) return RTAPI_ERROR;

  *result = retval;

  return RTAPI_OK;
}

rtapi_prio rtapi_prio_highest(void)
{
  return 1;
}

rtapi_prio rtapi_prio_lowest(void)
{
  return 31;
}

rtapi_prio rtapi_prio_next_higher(rtapi_prio prio)
{
  if (prio == rtapi_prio_highest())
    return prio;

  return prio - 1;
}

rtapi_prio rtapi_prio_next_lower(rtapi_prio prio)
{
  if (prio == rtapi_prio_lowest())
    return prio;

  return prio + 1;
}

rtapi_result rtapi_clock_set_period(rtapi_integer nsecs)
{
  return RTAPI_OK;
}

rtapi_result rtapi_clock_get_time(rtapi_integer * secs, 
				  rtapi_integer * nsecs)
{
  *secs = 0;
  *nsecs = 0;

  return RTAPI_OK;
}

rtapi_result rtapi_clock_get_interval(rtapi_integer start_secs, 
				      rtapi_integer start_nsecs,
				      rtapi_integer end_secs, 
				      rtapi_integer end_nsecs,
				      rtapi_integer * diff_secs, 
				      rtapi_integer * diff_nsecs)
{
  if (end_nsecs < start_nsecs) {
    if (end_secs < start_secs) {
      /* 1.1 - 9.9 */
      *diff_nsecs = start_nsecs - end_nsecs;
      *diff_secs = start_secs - end_secs;
    } else {
      /* 9.1 - 1.9 */
      *diff_nsecs = 1e9 - start_nsecs + end_nsecs;
      *diff_secs = end_secs - start_secs - 1;
    }
  } else {
    if (end_secs < start_secs) {
      /* 1.9 - 9.1 */
      *diff_nsecs = 1e9 - end_nsecs + start_nsecs;
      *diff_secs = start_secs - end_secs - 1;
    } else {
      /* 9.9 - 1.1 */
      *diff_nsecs = end_nsecs - start_nsecs;
      *diff_secs = end_secs - start_secs;
    }
  }

  return RTAPI_OK;
}

void * rtapi_task_new(void)
{
  return NULL;
}

rtapi_result rtapi_task_delete(void *task)
{
  return RTAPI_OK;
}

rtapi_result
rtapi_task_start(void *task,
		 void (*taskcode)(void *),
		 void *taskarg,
		 rtapi_prio prio,
		 rtapi_integer stacksize,
		 rtapi_integer period_nsec, 
		 rtapi_flag uses_fp)
{
  return RTAPI_OK;
}

rtapi_result rtapi_task_stop(void *task)
{
  return RTAPI_OK;
}

rtapi_result rtapi_task_pause(void *task)
{
  return RTAPI_OK;
}

rtapi_result rtapi_task_resume(void *task)
{
  return RTAPI_OK;
}

rtapi_result rtapi_task_set_period(void *task, rtapi_integer period_nsec)
{
  return RTAPI_OK;
}

rtapi_result rtapi_task_init(void)
{
  return RTAPI_OK;
}

rtapi_result rtapi_self_set_period(rtapi_integer period_nsec)
{
  return RTAPI_OK;
}

rtapi_result rtapi_wait(rtapi_integer period_nsec)
{
  return RTAPI_OK;
}

rtapi_result rtapi_task_exit(void)
{
  return RTAPI_OK;
}

rtapi_integer rtapi_task_stack_check(void *task)
{
  return -1;			/* irrelevant on this platform */
}

rtapi_result rtapi_shm_alloc(rtapi_id key, rtapi_integer size, rtapi_id * id, void **ptr)
{
  *id = 0;
  *ptr = NULL;

  return RTAPI_OK;
}

rtapi_result rtapi_shm_free(rtapi_id key, rtapi_integer size, rtapi_id id, const void *ptr)
{
  return RTAPI_OK;
}

void rtapi_print(const char *fmt, ...)
{
  va_list args;

  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

/*
  no support for inb, outb
*/

void rtapi_outb(char byte, rtapi_id port)
{
  return;
}

char rtapi_inb(rtapi_id port)
{
  return 0;
}

/*
  no support for simulated interrupts
*/

rtapi_result rtapi_interrupt_assign_handler(rtapi_id irq, void (*handler) (void))
{
  return RTAPI_OK;
}

rtapi_result rtapi_interrupt_free_handler(rtapi_id irq)
{
  return RTAPI_OK;
}

rtapi_result rtapi_interrupt_enable(rtapi_id irq)
{
  return RTAPI_OK;
}

rtapi_result rtapi_interrupt_disable(rtapi_id irq)
{
  return RTAPI_OK;
}

void * rtapi_mutex_new(rtapi_id key)
{
  return NULL;
}

rtapi_result rtapi_mutex_delete(void * mutex)
{
  return RTAPI_OK;
}

rtapi_result rtapi_mutex_give(void * mutex)
{
  return RTAPI_OK;
}

rtapi_result rtapi_mutex_take(void * mutex)
{
  return RTAPI_OK;
}
/* utility code for a single binary System V IPC semaphore */

#define SEM_TAKE (-1)		/* decrement sembuf.sem_op */
#define SEM_GIVE (1)		/* increment sembuf.sem_op */

/* destroy binary semaphore */
static void semdestroy(int semid)
{
  (void) semctl(semid, 0, IPC_RMID);
  return;
}

/* create binary semaphore, initially given, returning the id */
static int semcreate(int key)
{
  int semid;			/* semaphore id returned */
  int semflg;			/* flag for perms, create, etc. */
  struct sembuf sops;
  
  semflg = 0664;		/* permissions */
  /* leave IPC_CREAT and IPC_EXCL out, we want to see if it's there
     and delete it if so with the semid returned */

  semid = semget((key_t) key, 1, semflg);
  if (-1 == semid) {
    if (errno != ENOENT) {
      /* some other error */
      return -1;
    }
    /* else it just wasn't there, which is what we expected */
  } else {
    /* it's already there, so destroy it */
    semdestroy(semid);
  }

  semflg |= IPC_CREAT;		/* create it afresh */
  if ((semid = semget((key_t) key, 1, semflg)) == -1) {
    return -1;
  }
  /* initialize it to the given state */
  sops.sem_num = 0;
  sops.sem_flg = 0;		/* default is wait for it */
  sops.sem_op = SEM_GIVE;
  if (semop(semid, &sops, 1) == -1) {
    return -1;
  }

  return semid;
}

/* take a binary semaphore */
static int semtake(int semid)
{
  struct sembuf sops;

  sops.sem_num = 0;		/* only one semaphore in set */
  sops.sem_flg = 0;		/* wait indefinitely */
  sops.sem_op = SEM_TAKE;

  return semop(semid, &sops, 1);
}

/* give a binary semaphore */
static int semgive(int semid)
{
  struct sembuf sops;

  sops.sem_num = 0;		/* only one semaphore in set */
  sops.sem_flg = 0;		/* wait indefinitely */
  sops.sem_op = SEM_GIVE;

  if (semctl(semid, 0, GETVAL) == 1) {
    /* it's given-- leave it alone */
    return 0;
  }
  /* it's taken-- suppose now others take it again before
     we give it? they block, and this semgive will release
     one of them */
  return semop(semid, &sops, 1);
}

void * rtapi_sem_new(rtapi_id key)
{
  int * id;

  id = (int *) malloc(sizeof(int));
  if (NULL == (void *) id) return NULL;

  *id = semcreate((key_t) key);

  if (*id == -1) {
    free(id);
    return NULL;
  }
  
  return (void *) id;
}

rtapi_result rtapi_sem_delete(void * sem)
{
  if (NULL != sem) {
    semdestroy(*((int *) sem));
    free(sem);
    return RTAPI_OK;
  }

  return RTAPI_ERROR;
}

rtapi_result rtapi_sem_give(void * sem)
{
  return semgive(*((int *) sem)) == 0 ? RTAPI_OK : RTAPI_ERROR;
}

rtapi_result rtapi_sem_take(void * sem)
{
  return semtake(*((int *) sem)) == 0 ? RTAPI_OK : RTAPI_ERROR;
}

/* The following are from rtapi_app.h, which we don't include here
   since it's only to be included by the main translation unit */

extern void rtapi_app_exit(void);

int rtapi_argc;
char ** rtapi_argv;

rtapi_result rtapi_app_init(int argc, char ** argv)
{
  struct timeval start, end, diff;
  struct timespec ts;
  int t;

  atexit(rtapi_app_exit);

  /* copy argc and argv for use by tasks when they init */
  rtapi_argc = argc;
  rtapi_argv = (char **) malloc(argc * sizeof(char *));
  for (t = 0; t < argc; t++) {
    rtapi_argv[t] = (char *) malloc(strlen(argv[t]) + 1);
    strcpy(rtapi_argv[t], argv[t]);
  }

  /* FIXME */
  for (t = 0; t < argc; t++) printf("%s\n", rtapi_argv[t]);

  ts.tv_sec = 0;
  ts.tv_nsec = 1;		/* this will trigger twice the quantum */

#define NUM 100

  gettimeofday(&start, NULL);
  for (t = 0; t < NUM; t++) {
    nanosleep(&ts, NULL);
  }
  gettimeofday(&end, NULL);
  diff = timerdiff(end, start);

  /*
    The subtracted offset to align requested sleeps with actuals,
    best fit, is 1.5 X quantum, or 0.75 X twice the quantum, which is
    what we just measured. With usec to nsec give 750 X.
  */
  _rtapi_wait_offset_nsec = diff.tv_usec * 750 / NUM;

  return RTAPI_OK;
}

static void quit(int sig)
{
  return;
}

rtapi_result rtapi_app_wait(void)
{
  sigset_t mask;

  /* wait until ^C */
  signal(SIGINT, quit);
  sigemptyset(&mask);
  sigsuspend(&mask);

  return RTAPI_OK;
}

void * rtapi_new(rtapi_integer size)
{
  return malloc(size);
}

void rtapi_free(void * ptr)
{
  free(ptr);
}

static char nothing[] = "";
char * rtapi_arg_get_string(char ** var, char * key)
{
  int len;
  int t;

  len = strlen(key);

  for (t = 0; t < rtapi_argc; t++) {
    if (! strncmp(rtapi_argv[t], key, len) &&
	rtapi_argv[t][len] == '=') {
      *var = &rtapi_argv[t][len + 1];
      return *var;
    }
  }

  *var = nothing;
  return *var;
}

int rtapi_arg_get_int(rtapi_integer * var, char * key)
{
  int len;
  int t;

  len = strlen(key);

  for (t = 0; t < rtapi_argc; t++) {
    if (! strncmp(rtapi_argv[t], key, len) &&
	rtapi_argv[t][len] == '=') {
      *var = (rtapi_integer) strtol(&rtapi_argv[t][len + 1], NULL, 0);
      return *var;
    }
  }

  *var = 0;
  return *var;
}

rtapi_result rtapi_string_to_integer(const char * str, rtapi_integer * var)
{
  int i;
  const char * endptr;

  i = (int) strtol(str, &endptr, 0);
  if (endptr == str ||
      (! isspace(*endptr) && 0 != *endptr)) {
    *var = 0;
    return RTAPI_ERROR;
  }

  *var = (rtapi_integer) i;
  return RTAPI_OK;
}
