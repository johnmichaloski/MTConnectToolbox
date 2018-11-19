/*!
  \file unix_ulapi.c
*/

/*
  Some of these functions are implemented exactly the same way, with
  slightly different names, in unix_rtapi.c. If you change them here,
  change them there.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ulapi.h"		/* these decls */
#include <stdio.h>
#include <stddef.h>		/* NULL */
#include <stdlib.h>		/* malloc */
#include <string.h>		/* memset */
#include <signal.h>		/* kill, SIGINT */
#include <ctype.h>		/* isspace */
#include <errno.h>
#include <pthread.h>		/* pthread_create(), pthread_mutex_t */
#include <time.h>		/* struct timespec, nanosleep */
#include <sys/time.h>		/* gettimeofday(), struct timeval */
#include <sys/types.h>		/* struct stat */
#include <sys/stat.h>		/* stat */
#include <unistd.h>		/* select(), write(), _exit() */
#include <sys/ipc.h>		/* IPC_* */
#include <sys/shm.h>		/* shmget() */
#include <sys/sem.h>
#include <fcntl.h>		/* O_RDONLY, O_NONBLOCK */
#include <termios.h>  		/* tcflush, TCIOFLUSH */
#include <sys/types.h>		/* fd_set, FD_ISSET() */
#include <sys/wait.h>		/* waitpid */
#include <sys/socket.h>		/* PF_INET, socket(), listen(), bind(), etc. */
#include <netinet/in.h>		/* struct sockaddr_in */
#include <netdb.h>		/* gethostbyname */
#include <arpa/inet.h>		/* inet_addr */
#include <sys/stat.h>		/* struct stat */
#include <sys/ioctl.h>
#ifndef NO_DL
#include <dlfcn.h>
#endif

static ulapi_integer ulapi_debug_level = 0;

#define PERROR(x) if (ulapi_debug_level & ULAPI_DEBUG_ERROR) perror(x)

static ulapi_integer _ulapi_wait_offset_nsec = 1;

/*
  'ulapi_time' returns the current time with respect to some arbitrary
  origin that remains constant for the life of the program.

  Several clock functions are considered, in this order:

  'clock_gettime(CLOCK_MONOTONIC_HR)' gets the high-resolution time
  from an arbitrary reference (usually the system uptime, but
  that's not guaranteed). Monotonic means it doesn't go backwards.

  'clock_gettime(CLOCK_MONOTONIC)' is similar but at lower resolution.

  'clock_gettime(CLOCK_REALTIME)' uses the real-time clock.  Its
  resolution varies but is seldom better than 1/18th of a second. It
  is very expensive to read, taking quite a bit of I/O. It is also
  inaccurate and cannot really be disciplined, largely because of its
  low resolution. In comparison, the other time sources are much more
  stable and have enough resolution to be disciplined. Most modern
  computers read the RTC only on startup.

  'gettimeofday' gets the system's best guess at wall time based on
  the Unix epoch, January 1, 1970. It is not monotonic and can go
  backwards. 
*/

ulapi_real ulapi_time(void)
{
#ifdef HAVE_CLOCK_GETTIME
#if defined (CLOCK_MONOTONIC_RT)
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC_RT, &ts);
  return (ulapi_real) (((double) ts.tv_sec) + ((double) ts.tv_nsec) * 1.0e-9);
#elif defined (CLOCK_MONOTONIC)
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (ulapi_real) (((double) ts.tv_sec) + ((double) ts.tv_nsec) * 1.0e-9);
#elif defined (CLOCK_REALTIME)
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return (ulapi_real) (((double) ts.tv_sec) + ((double) ts.tv_nsec) * 1.0e-9);
#else
  /* should have at least CLOCK_REALTIME, so fall back on gettimeofday */
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (ulapi_real) (((double) tv.tv_sec) + ((double) tv.tv_usec) * 1.0e-6);
#endif

#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (ulapi_real) (((double) tv.tv_sec) + ((double) tv.tv_usec) * 1.0e-6);
#endif
}

void ulapi_sleep(ulapi_real secs)
{
  int isecs, insecs;
  struct timespec ts;

  isecs = (int) secs;
  insecs = (int) ((secs - isecs) * 1.0e9);

  ts.tv_sec = isecs;
  ts.tv_nsec = insecs;

  (void) nanosleep(&ts, NULL);
}

static void quit(int sig)
{
  return;
}

ulapi_result ulapi_app_wait(void)
{
  sigset_t mask;

  /* wait until ^C */
  signal(SIGINT, quit);
  sigemptyset(&mask);
  sigsuspend(&mask);

  return ULAPI_OK;
}

ulapi_result ulapi_init(void)
{
  return ULAPI_OK;
}

ulapi_result ulapi_exit(void)
{
  return ULAPI_OK;
}

ulapi_integer ulapi_to_argv(const char *src, char ***argv)
{
  char *cpy;
  char *ptr;
  ulapi_integer count;
  ulapi_flag inquote;

  *argv = NULL;
  cpy = malloc(strlen(src) + 1);
  count = 0;
  inquote = 0;

  for (; ; count++) {
    while (isspace(*src)) src++;
    if (0 == *src) return count;
    ptr = cpy;
    while (0 != *src) {
      if ('"' == *src) {
	inquote = !inquote;
      } else if ((!inquote) && isspace(*src)) {
	break;
      } else {
	*ptr++ = *src;
      }
      src++;
    }
    *ptr++ = 0;
    *argv = realloc(*argv, (count+1)*sizeof(**argv));
    (*argv)[count] = malloc(strlen(cpy) + 1);
    strcpy((*argv)[count], cpy);
  }
   
  free(cpy);

  return count;
}

void ulapi_free_argv(ulapi_integer argc, char **argv)
{
  ulapi_integer t;

  for (t = 0; t < argc; t++) {
    free((char *) argv[t]);
  }
  free(argv);

  return;
}

void ulapi_set_debug(ulapi_integer mask)
{
  ulapi_debug_level = mask;
}

ulapi_integer ulapi_get_debug(void)
{
  return ulapi_debug_level;
}

ulapi_prio ulapi_prio_highest(void)
{
  return 1;
}

ulapi_prio ulapi_prio_lowest(void)
{
  return 31;
}

ulapi_prio ulapi_prio_next_higher(ulapi_prio prio)
{
  if (prio == ulapi_prio_highest())
    return prio;

  return prio - 1;
}

ulapi_prio ulapi_prio_next_lower(ulapi_prio prio)
{
  if (prio == ulapi_prio_lowest())
    return prio;

  return prio + 1;
}

ulapi_result ulapi_task_init(ulapi_task_struct *task)
{
  int policy;
  struct sched_param sched_param;

  if (0 != pthread_getschedparam(pthread_self(), &policy, &sched_param)) return ULAPI_ERROR;
  if (0 != pthread_setschedparam(pthread_self(), SCHED_OTHER, &sched_param)) return ULAPI_ERROR;
  if (0 != pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)) return ULAPI_ERROR;
  if (0 != pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL)) return ULAPI_ERROR;

  return ULAPI_OK;
}

ulapi_task_struct *ulapi_task_new(void)
{
  ulapi_task_struct *ts = malloc(sizeof(ulapi_task_struct));
  if (NULL == ts) return NULL;

  if (ULAPI_OK != ulapi_task_init(ts)) {
    free(ts);
    ts = NULL;
  }

  return ts;
}

ulapi_result ulapi_task_clear(ulapi_task_struct *task)
{
  /* nothing need be done */
  return ULAPI_OK;
}

ulapi_result ulapi_task_delete(ulapi_task_struct *task)
{
  if (NULL != task) {
    (void) ulapi_task_clear(task);
    free(task);
  }

  return ULAPI_OK;
}

typedef void *(*pthread_task_code)(void *);

ulapi_result
ulapi_task_start(ulapi_task_struct *task,
		 void (*taskcode)(void *),
		 void *taskarg,
		 ulapi_prio prio,
		 ulapi_integer period_nsec)
{
  pthread_attr_t attr;
  struct sched_param sched_param;

  pthread_attr_init(&attr);
  sched_param.sched_priority = prio;
  pthread_attr_setschedparam(&attr, &sched_param);
  pthread_create((pthread_t *) task, &attr, (pthread_task_code) taskcode, taskarg);

  return ULAPI_OK;
}

ulapi_result ulapi_task_stop(ulapi_task_struct *task)
{
  return (pthread_cancel(*((ulapi_task_struct *) task)) == 0 ? ULAPI_OK : ULAPI_ERROR);
}

ulapi_result ulapi_task_pause(ulapi_task_struct *task)
{
  return ULAPI_OK;
}

ulapi_result ulapi_task_resume(ulapi_task_struct *task)
{
  return ULAPI_OK;
}

ulapi_result ulapi_task_set_period(ulapi_task_struct *task, ulapi_integer period_nsec)
{
  return ULAPI_OK;
}

ulapi_result ulapi_self_set_period(ulapi_integer period_nsec)
{
  return ULAPI_OK;
}

ulapi_result ulapi_wait(ulapi_integer period_nsec)
{
  struct timespec ts;

  if (period_nsec < _ulapi_wait_offset_nsec + 1) period_nsec = 1;
  else period_nsec -= _ulapi_wait_offset_nsec;

  ts.tv_sec = 0;
  ts.tv_nsec = period_nsec;

  (void) nanosleep(&ts, NULL);

  return ULAPI_OK;
}

void ulapi_task_exit(ulapi_integer retval)
{
  ptrdiff_t p = retval;	/* ptrdiff_t is an integer the same size as a pointer */

  pthread_exit((void *) p);	/* so casting won't raise a size warning */
}

ulapi_result ulapi_task_join(ulapi_task_struct *task, ulapi_integer *retptr)
{
  ulapi_integer retval;
  int ret;

  ret = pthread_join(*((ulapi_task_struct *) task), (void **) &retval);

  if (0 == ret) {
    if (NULL != retptr) {
      *retptr = retval;
    }
    return ULAPI_OK;
  }

  return ULAPI_ERROR;
}

ulapi_integer ulapi_task_id(void)
{
  return (ulapi_integer) pthread_self();
}

typedef struct {
  pid_t pid;
} ulapi_process_struct;

void *ulapi_process_new(void)
{
  ulapi_process_struct *proc;

  proc = (ulapi_process_struct *) malloc(sizeof(ulapi_process_struct));

  return proc;
}

ulapi_result ulapi_process_delete(void *proc)
{
  if (NULL != proc) {
    free(proc);
  }

  return ULAPI_OK;
}

/*
  to_argv takes a string containing args separated by spaces, e.g., 
  "this is the string of args", and a pointer to an array of strings
  into which each arg will be copied.
*/

static int to_argv(char *src, char *dst, char *argv[])
{
  char *ptr;
  int count;
  int inquote;
  int wasspace;

  strcpy(dst, src);

  ptr = dst;
  count = 0;
  inquote = 0;
  wasspace = 1;

  while (*ptr != 0) {

    if (*ptr == 0) {
      break;
    }

    if (*ptr == '"') {
      inquote = !inquote;
    }

    if (isspace(*ptr)) {
      if (inquote) {
	ptr++;
      } else {
	*ptr++ = 0;
	wasspace = 1;
      }
      continue;
    }

    if (wasspace) {
      argv[count] = ptr;
      count++;
    }
    ptr++;
    wasspace = 0;
  }

  argv[count] = NULL;

  return count;
}

#if 0
static int to_argv(char *src, char *dst, char *argv[])
{
  int count;

  strcpy(dst, src);

  for (count = 0; strlen(src); count++) {
    argv[count] = NULL;
    while (isspace(*dst)) {
      *dst = 0;
      dst++;
    }
    if (0 == *dst) break;
    argv[count] = dst;
    while (! isspace(*dst) && 0 != *dst) dst++;
  }

  return count + 1;
}
#endif

ulapi_result ulapi_process_start(void *proc, char *path)
{
  ulapi_result retval = ULAPI_OK;
  pid_t pid;
  int argc;
  char **argv;
  char *dst;
  int maxargc;

  if (NULL == proc) return ULAPI_ERROR;

  pid = fork();
  switch (pid) {
  case -1:
    PERROR("fork");
    retval = ULAPI_ERROR;
    break;
  case 0:
    // we're the child
    maxargc = strlen(path) + 1;
    argv = malloc(maxargc * sizeof(*argv));
    dst = malloc(maxargc);
#undef USE_ULAPI_TO_ARGV
#ifdef USE_ULAPI_TO_ARGV
    argc = ulapi_to_argv(path, &argv);
#else
    argc = to_argv(path, dst, argv);
#endif
    if (ulapi_debug_level & ULAPI_DEBUG_INFO) {
      ulapi_print("ulapi_process_start: starting %s\n", path);
    }
    if (-1 == execvp(argv[0], argv)) {
      /*
	we're still running as a copy of the forked process, which may
	have registered some functions with atexit() that may hang
	us, so we'll call _exit() instead of exit() to avoid these in
	our forked copy and make sure we exit right away.
      */
      PERROR("execvp");
      _exit(1);
    }
    // else we exec'ed OK and would not have gotten here
#ifdef USE_ULAPI_TO_ARGV
    ulapi_free_argv(argc, argv);
#endif
    break;
  default:
    // we're the parent
    ((ulapi_process_struct *) proc)->pid = pid;
    break;
  }

  return retval;
}

ulapi_result ulapi_process_stop(void *proc)
{
  if (NULL == proc) return ULAPI_ERROR;

  kill(((ulapi_process_struct *) proc)->pid, SIGKILL);

  return ULAPI_OK;
}

ulapi_integer ulapi_process_done(void *proc, ulapi_integer *result)
{
  pid_t pid;
  int status;

  if (NULL == proc) return ULAPI_ERROR;

  pid = waitpid(((ulapi_process_struct *) proc)->pid, &status, WNOHANG);

  if (pid == ((ulapi_process_struct *) proc)->pid) {
    /* the process is done */
    if (NULL != result) {
      if (WIFEXITED(status)) {
	*result = WEXITSTATUS(status);
      } else {
	/* process did not terminate normally, perhaps with a signal */
	*result = -1;
      }
    }
    return 1;
  } else if (pid == 0) {
    /* still running */
    return 0;
  } 

  /* else the wait failed */
  if (NULL != result) {
    *result = -1;
  }
  return 1;
}

ulapi_result ulapi_process_wait(void *proc, ulapi_integer *result)
{
  pid_t pid;
  int status;

  if (NULL == proc) return ULAPI_ERROR;

  pid = waitpid(((ulapi_process_struct *) proc)->pid, &status, 0);

  if (pid == ((ulapi_process_struct *) proc)->pid) {
    /* the process is done */
    if (NULL != result) {
      if (WIFEXITED(status)) {
	*result = WEXITSTATUS(status);
      } else {
	/* process did not terminate normally, perhaps with a signal */
	*result = -1;
      }
    }
    return ULAPI_OK;
  }

  /* else the wait failed */
  if (NULL != result) {
    *result = -1;
  }
  return ULAPI_ERROR;
}

ulapi_result ulapi_mutex_init(ulapi_mutex_struct *mutex, ulapi_id key)
{
  /* initialize mutex to default attributes, and give it */
  if (0 == pthread_mutex_init(mutex, NULL)) {
    (void) pthread_mutex_unlock(mutex);
    return ULAPI_OK;
  }
  return ULAPI_ERROR;
}

ulapi_mutex_struct *ulapi_mutex_new(ulapi_id key)
{
  ulapi_mutex_struct *mutex;

  mutex = (ulapi_mutex_struct *) malloc(sizeof(ulapi_mutex_struct));
  if (NULL == mutex) return NULL;

  /* initialize mutex to default attributes, and give it */
  if (0 == pthread_mutex_init(mutex, NULL)) {
    (void) pthread_mutex_unlock(mutex);
    return mutex;
  }
  /* else got an error, so free the mutex and return null */

  free(mutex);
  return NULL;
}

ulapi_result ulapi_mutex_clear(ulapi_mutex_struct *mutex)
{
  (void) pthread_mutex_destroy((pthread_mutex_t *) mutex);

  return ULAPI_OK;
}

ulapi_result ulapi_mutex_delete(ulapi_mutex_struct *mutex)
{
  if (NULL == mutex) return ULAPI_ERROR;

  (void) pthread_mutex_destroy((pthread_mutex_t *) mutex);
  free(mutex);

  return ULAPI_OK;
}

ulapi_result ulapi_mutex_give(ulapi_mutex_struct *mutex)
{
  return (0 == pthread_mutex_unlock((pthread_mutex_t *) mutex) ? ULAPI_OK : ULAPI_ERROR);
}

ulapi_result ulapi_mutex_take(ulapi_mutex_struct *mutex)
{
  return (0 == pthread_mutex_lock((pthread_mutex_t *) mutex) ? ULAPI_OK : ULAPI_ERROR);
}

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
    /* it's already there, so return it */
    return semid;
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

void * ulapi_sem_new(ulapi_id key)
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

ulapi_result ulapi_sem_delete(void * sem)
{
  if (NULL != sem) {
    semdestroy(*((int *) sem));
    free(sem);
    return ULAPI_OK;
  }

  return ULAPI_ERROR;
}

ulapi_result ulapi_sem_give(void * sem)
{
  return semgive(*((int *) sem)) == 0 ? ULAPI_OK : ULAPI_ERROR;
}

ulapi_result ulapi_sem_take(void * sem)
{
  return semtake(*((int *) sem)) == 0 ? ULAPI_OK : ULAPI_ERROR;
}

void * ulapi_cond_new(ulapi_id key)
{
  pthread_cond_t * cond;

  cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  if (NULL == (void *) cond) return NULL;

  if (0 == pthread_cond_init(cond, NULL)) {
    return (void *) cond;
  }
  /* else got an error, so free the condition variable and return null */

  free(cond);
  return NULL;
}

ulapi_result ulapi_cond_delete(void * cond)
{
  if (NULL == (void *) cond) return ULAPI_ERROR;

  (void) pthread_cond_destroy((pthread_cond_t *) cond);
  free(cond);

  return ULAPI_OK;
}

ulapi_result ulapi_cond_signal(void * cond)
{
  return (0 == pthread_cond_signal((pthread_cond_t *) cond) ? ULAPI_OK : ULAPI_ERROR);
}

ulapi_result ulapi_cond_broadcast(void * cond)
{
  return (0 == pthread_cond_broadcast((pthread_cond_t *) cond) ? ULAPI_OK : ULAPI_ERROR);
}

ulapi_result ulapi_cond_wait(void * cond, void * mutex)
{
  return (0 == pthread_cond_wait((pthread_cond_t *) cond, (pthread_mutex_t *) mutex) ? ULAPI_OK : ULAPI_ERROR);
}

typedef struct {
  ulapi_id key;
  ulapi_integer size;
  ulapi_id id;
  void * addr;
} shm_struct;

void * ulapi_shm_new(ulapi_id key, ulapi_integer size)
{
  shm_struct * shm;

  shm = malloc(sizeof(shm_struct));
  if (NULL == (void *) shm) return NULL;

  shm->id = shmget((key_t) key, (int) size, IPC_CREAT | 0666);
  if (-1 == shm->id) {
    PERROR("shmget");
    free(shm);
    return NULL;
  }

  shm->addr = shmat(shm->id, NULL, 0);
  if ((void *) -1 == shm->addr) {
    PERROR("shmat");
    free(shm);
    return NULL;
  }

  return (void *) shm;
}

void * ulapi_shm_addr(void * shm)
{
  return ((shm_struct *) shm)->addr;
}

ulapi_result ulapi_shm_delete(void * shm)
{
  struct shmid_ds d;
  int r1, r2;

  if (NULL == shm) return ULAPI_OK;

  r1 = shmdt(((shm_struct *) shm)->addr);
  r2 = shmctl(IPC_RMID, ((shm_struct *) shm)->id, &d);

  free(shm);

  return (r1 || r2 ? ULAPI_ERROR : ULAPI_OK);
}

#ifdef HAVE_RTAI

typedef struct {
  ulapi_id key;
  void * addr;
} rtm_struct;

void *ulapi_rtm_new(ulapi_id key, ulapi_integer size)
{
  rtm_struct * shm;

  shm = (rtm_struct *) malloc(sizeof(rtm_struct));
  if (NULL == (void *) shm) return NULL;

  shm->addr = rt_shm_alloc(key, size, USE_GFP_KERNEL);
  if (NULL == shm->addr) {
    free(shm);
    return NULL;
  }

  shm->key = key;
  return shm;
}

void *ulapi_rtm_addr(void * shm)
{
  return ((rtm_struct *) shm)->addr;
}

ulapi_result ulapi_rtm_delete(void * shm)
{
  if (NULL != shm) {
    if (NULL != ((rtm_struct *) shm)->addr) {
      rt_shm_free(((rtm_struct *) shm)->key);
    }
    free(shm);
  }

  return ULAPI_OK;
}

#else

/*
  If we don't have RTAI, just use normal shm. The RT side that
  simualates realtime will do the same.
*/

void *ulapi_rtm_new(ulapi_id key, ulapi_integer size)
{
  return ulapi_shm_new(key, size);
}

void *ulapi_rtm_addr(void * shm)
{
  return ulapi_shm_addr(shm);
}

ulapi_result ulapi_rtm_delete(void * shm)
{
  return ulapi_shm_delete(shm);
}

#endif	/* HAVE_RTAI */

ulapi_integer
ulapi_socket_get_client_id_on_interface(ulapi_integer port, const char *hostname, const char *intf)
{
  int socket_fd;
  struct sockaddr_in local_addr;
  struct sockaddr_in server_addr;
  struct hostent *ent;
  struct in_addr *in_a;

  if (-1 == (socket_fd = socket(PF_INET, SOCK_STREAM, 0))) {
    PERROR("socket");
    return -1;
  }

  if (NULL != intf) {
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = PF_INET;
    local_addr.sin_addr.s_addr = inet_addr(intf);
    if (0 != bind(socket_fd, (struct sockaddr *) &local_addr, sizeof(local_addr))) {
      PERROR("bind");
      close(socket_fd);
      return -1;
    }
  }

  if (NULL == (ent = gethostbyname(hostname))) {
    PERROR("gethostbyname");
    return -1;
  }
  in_a = (struct in_addr *) ent->h_addr_list[0];
  memset(&server_addr, 0, sizeof(struct sockaddr_in));
  server_addr.sin_family = PF_INET;
  server_addr.sin_addr.s_addr = in_a->s_addr;
  server_addr.sin_port = htons(port);

  if (-1 == connect(socket_fd, 
		    (struct sockaddr *) &server_addr,
		    sizeof(struct sockaddr_in))) {
    PERROR("connect");
    close(socket_fd);
    return -1;
  }

  return socket_fd;
}

ulapi_integer
ulapi_socket_get_client_id(ulapi_integer port, const char *hostname)
{
  return ulapi_socket_get_client_id_on_interface(port, hostname, NULL);
}

ulapi_integer
ulapi_socket_get_server_id_on_interface(ulapi_integer port, const char *intf)
{
  int socket_fd;
  struct sockaddr_in myaddr;
  int on;
  struct linger mylinger = { 0 };
  enum {BACKLOG = 5};

  if (-1 == (socket_fd = socket(PF_INET, SOCK_STREAM, 0))) {
    PERROR("socket");
    return -1;
  }

  /*
    Turn off bind address checking, and allow
    port numbers to be reused - otherwise the
    TIME_WAIT phenomenon will prevent binding
    to these address.port combinations for
    (2 * MSL) seconds.
  */
  on = 1;
  if (-1 == 
      setsockopt(socket_fd,
		 SOL_SOCKET,
		 SO_REUSEADDR,
		 (const char *) &on,
		 sizeof(on))) {
    PERROR("setsockopt");
    close(socket_fd);
    return -1;
  }

  /*
    When connection is closed, there is a need
    to linger to ensure all data is transmitted.
  */
  mylinger.l_onoff = 1;
  mylinger.l_linger = 30;
  if (-1 ==
      setsockopt(socket_fd,
		 SOL_SOCKET,
		 SO_LINGER,
		 (const char *) &mylinger,
		 sizeof(struct linger))) {
    PERROR("setsockopt");
    close(socket_fd);
    return -1;
  }

  memset(&myaddr, 0, sizeof(struct sockaddr_in));
  myaddr.sin_family = PF_INET;
  if (NULL == intf) {
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  } else {
    myaddr.sin_addr.s_addr = inet_addr(intf);
  }
  myaddr.sin_port = htons(port);

  if (-1 == bind(socket_fd, (struct sockaddr *) &myaddr,
		 sizeof(struct sockaddr_in))) {
    PERROR("bind");
    close(socket_fd);
    return -1;
  }

  if (-1 == listen(socket_fd, BACKLOG)) {
    PERROR("listen");
    close(socket_fd);
    return -1;
  }

  return socket_fd;
}

ulapi_integer
ulapi_socket_get_server_id(ulapi_integer port)
{
  return ulapi_socket_get_server_id_on_interface(port, NULL);
}

ulapi_result
ulapi_getpeername(int s, char *ipstr, size_t iplen, ulapi_integer *port)
{
  struct sockaddr addr;
  struct sockaddr_in *saddr;
  socklen_t len;

  len = sizeof(addr);
  getpeername(s, &addr, &len);

  saddr = (struct sockaddr_in *) &addr;
  *port = ntohs(saddr->sin_port);
  ulapi_strncpy(ipstr, inet_ntoa(saddr->sin_addr), iplen);

  return ULAPI_OK;
}

ulapi_integer
ulapi_socket_get_connection_id(ulapi_integer socket_fd)
{
  fd_set rfds;
  struct sockaddr_in client_addr;
  unsigned int client_len;
  int client_fd;
  int retval;

  do {
    FD_ZERO(&rfds);
    FD_SET(socket_fd, &rfds);
    retval = select(socket_fd + 1, &rfds, NULL, NULL, NULL);
  } while (0 == retval ||
	   ! FD_ISSET(socket_fd, &rfds));

  if (retval == -1) {
    PERROR("select");
    return -1;
  }

  memset(&client_addr, 0, sizeof(struct sockaddr_in));
  client_len = sizeof(struct sockaddr_in);
  client_fd = 
    accept(socket_fd,
	   (struct sockaddr *) &client_addr, 
	   &client_len);
  if (-1 == client_fd) {
    PERROR("accept");
    return -1;
  }
  
  return client_fd;
}

ulapi_integer
ulapi_socket_get_broadcaster_id_on_interface(ulapi_integer port, const char *intf)
{
  struct sockaddr_in addr;
  int fd;
  int perm;

  fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (0 > fd) {
    return -1;
  }

  perm = 1;
  if (0 > setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (void *) &perm, sizeof(perm))) {
    return -1;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  if (NULL == intf) {
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
  } else {
    addr.sin_addr.s_addr = inet_addr(intf);
  }
  addr.sin_port = htons(port);

  if (-1 == connect(fd, 
		    (struct sockaddr *) &addr,
		    sizeof(struct sockaddr_in))) {
    PERROR("connect");
    close(fd);
    return -1;
  }

  return fd;
}

ulapi_integer
ulapi_socket_get_broadcaster_id(ulapi_integer port)
{
  return ulapi_socket_get_broadcaster_id_on_interface(port, NULL);
}

ulapi_integer
ulapi_socket_get_broadcastee_id_on_interface(ulapi_integer port, const char *intf)
{
  struct sockaddr_in addr;
  int fd;
  int retval;
  int on = 1;

  /* Create a best-effort datagram socket using UDP */
  fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (0 > fd) {
    return -1;
  }

  if (-1 == 
      setsockopt(fd,
		 SOL_SOCKET,
		 SO_REUSEADDR,
		 (const char *) &on,
		 sizeof(on))) {
    PERROR("setsockopt 1");
    close(fd);
    return -1;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  if (NULL == intf) {
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
  } else {
    addr.sin_addr.s_addr = inet_addr(intf);
  }
  addr.sin_port = htons(port);

  retval = bind(fd, (struct sockaddr *) &addr, sizeof(addr));

  if (0 > retval) {
    close(fd);
    return -1;
  }

  return fd;
}

ulapi_integer
ulapi_socket_get_broadcastee_id(ulapi_integer port)
{
  return ulapi_socket_get_broadcastee_id_on_interface(port, NULL);
}

/*
   Bit -->  0                           31            Address Range:
           +-+----------------------------+
           |0|       Class A Address      |       0.0.0.0 - 127.255.255.255
           +-+----------------------------+
           +-+-+--------------------------+
           |1 0|     Class B Address      |     128.0.0.0 - 191.255.255.255
           +-+-+--------------------------+
           +-+-+-+------------------------+
           |1 1 0|   Class C Address      |     192.0.0.0 - 223.255.255.255
           +-+-+-+------------------------+
           +-+-+-+-+----------------------+
           |1 1 1 0|  MULTICAST Address   |     224.0.0.0 - 239.255.255.255
           +-+-+-+-+----------------------+
           +-+-+-+-+-+--------------------+
           |1 1 1 1 0|     Reserved       |     240.0.0.0 - 247.255.255.255
           +-+-+-+-+-+--------------------+

	   224.0.0.1 = All systems on the subnet
*/

ulapi_integer
ulapi_socket_get_multicaster_id(ulapi_integer port)
{
  return ulapi_socket_get_multicaster_id_on_interface(port, NULL);
}

ulapi_integer
ulapi_socket_get_multicaster_id_on_interface(ulapi_integer port, const char *intf)
{
  struct sockaddr_in addr;
  int fd;

  fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (0 > fd) {
    return -1;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  if (NULL == intf) {
    addr.sin_addr.s_addr = inet_addr(ULAPI_SOCKET_DEFAULT_MULTICAST_GROUP);
  } else {
    addr.sin_addr.s_addr = inet_addr(intf);
  }
  addr.sin_port = htons(port);

  if (-1 == connect(fd, 
		    (struct sockaddr *) &addr,
		    sizeof(struct sockaddr_in))) {
    PERROR("connect");
    close(fd);
    return -1;
  }

  return fd;
}

ulapi_integer
ulapi_socket_get_multicastee_id(ulapi_integer port)
{
  return ulapi_socket_get_multicastee_id_on_interface(port, NULL);
}

ulapi_integer
ulapi_socket_get_multicastee_id_on_interface(ulapi_integer port, const char *intf)
{
  struct sockaddr_in addr;
  struct ip_mreq mreq;
  int fd;
  int on = 1;

  fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (0 > fd) {
    PERROR("socket");
    return -1;
  }

  if (-1 == 
      setsockopt(fd,
		 SOL_SOCKET,
		 SO_REUSEADDR,
		 (const char *) &on,
		 sizeof(on))) {
    PERROR("setsockopt 1");
    close(fd);
    return -1;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
     
  if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
    PERROR("bind");
    return -1;
  }

  if (NULL == intf) {
    mreq.imr_multiaddr.s_addr = inet_addr(ULAPI_SOCKET_DEFAULT_MULTICAST_GROUP);
  } else {
    mreq.imr_multiaddr.s_addr = inet_addr(intf);
  }
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
    PERROR("setsockopt 2");
    return -1;
  }
  return fd;
}

char *
ulapi_address_to_hostname(ulapi_integer address)
{
  static char string[4 * DIGITS_IN(int) + 3 + 1];

  sprintf(string, "%d.%d.%d.%d",
	  (int) ((address >> 24) & 0xFF),
	  (int) ((address >> 16) & 0xFF),
	  (int) ((address >> 8) & 0xFF),
	  (int) (address & 0xFF));

  return string;
}

ulapi_integer
ulapi_hostname_to_address(const char *hostname)
{
  struct hostent * ent;

  ent = gethostbyname(hostname);
  if (NULL == ent) return (127 << 24) + (0 << 16) + (0 << 8) + 1;
  if (4 != ent->h_length) return 0;

  /* FIXME-- could use ntohl here */
  return ((ent->h_addr_list[0][0] & 0xFF) << 24)
    + ((ent->h_addr_list[0][1] & 0xFF) << 16)
    + ((ent->h_addr_list[0][2] & 0xFF) << 8)
    + (ent->h_addr_list[0][3] & 0xFF);
}

ulapi_integer
ulapi_get_host_address(void)
{
  enum {HOSTNAMELEN = 256};
  char hostname[HOSTNAMELEN];

  if (0 != gethostname(hostname, HOSTNAMELEN)) return 0;
  hostname[HOSTNAMELEN - 1] = 0; /* force null termination */

  return ulapi_hostname_to_address(hostname);
}

ulapi_result
ulapi_socket_set_nonblocking(ulapi_integer fd)
{
  int flags;

  flags = fcntl(fd, F_GETFL);
  if (-1 == fcntl(fd, F_SETFL, flags | O_NONBLOCK)) {
    return ULAPI_ERROR;
  }

  return ULAPI_OK;
}

ulapi_result
ulapi_socket_set_blocking(ulapi_integer fd)
{
  int flags;

  flags = fcntl(fd, F_GETFL);
  if (-1 == fcntl(fd, F_SETFL, flags & ~O_NONBLOCK)) {
    return ULAPI_ERROR;
  }

  return ULAPI_OK;
}

ulapi_integer
ulapi_socket_read(ulapi_integer id,
		  char *buf,
		  ulapi_integer len)
{
  return recv(id, buf, len, 0);
}

ulapi_integer
ulapi_socket_write(ulapi_integer id,
		   const char *buf,
		   ulapi_integer len)
{
  return send(id, buf, len, MSG_NOSIGNAL);
}

ulapi_result
ulapi_socket_close(ulapi_integer id)
{
  return 0 == close((int) id) ? ULAPI_OK : ULAPI_ERROR;
}

/* File descriptor interface */

void * 
ulapi_fd_new(void)
{
  return malloc(sizeof(int));
}

ulapi_result
ulapi_fd_delete(void *id)
{
  if (NULL != id) free(id);

  return ULAPI_OK;
}

ulapi_result
ulapi_std_open(ulapi_stdio io, void *id)
{
  if (ULAPI_STDIN == io) {
    *((int *) id) = STDIN_FILENO;
    return ULAPI_OK;
  }
  if (ULAPI_STDOUT == io) {
    *((int *) id) = STDOUT_FILENO;
    return ULAPI_OK;
  }
  if (ULAPI_STDERR == io) {
    *((int *) id) = STDERR_FILENO;
    return ULAPI_OK;
  }
  return ULAPI_ERROR;
}

ulapi_result
ulapi_fd_stat(const char *path)
{
  struct stat buf;

  return 0 == stat(path, &buf) ? ULAPI_OK : ULAPI_ERROR;
}

ulapi_result
ulapi_fd_open(const char *path, void *id)
{
  int fd;

  fd = open(path, O_RDWR);

  if (fd < 0) return ULAPI_ERROR;
  
  *((int *) id) = fd;
  return ULAPI_OK;
}

ulapi_result
ulapi_fd_set_nonblocking(void *id)
{
  int fd;
  int flags;

  fd = *((int *) id);
  flags = fcntl(fd, F_GETFL);
  if (-1 == fcntl(fd, F_SETFL, flags | O_NONBLOCK)) {
    return ULAPI_ERROR;
  }

  return ULAPI_OK;
}

ulapi_result
ulapi_fd_set_blocking(void *id)
{
  int fd;
  int flags;

  fd = *((int *) id);
  flags = fcntl(fd, F_GETFL);
  if (-1 == fcntl(fd, F_SETFL, flags & ~O_NONBLOCK)) {
    return ULAPI_ERROR;
  }

  return ULAPI_OK;
}

ulapi_integer
ulapi_fd_read(void *id,
	      char *buf,
	      ulapi_integer len)
{
  return read(*((int *) id), buf, len);
}

ulapi_integer
ulapi_fd_write(void *id,
	       const char *buf,
	       ulapi_integer len)
{
  return write(*((int *) id), buf, len);
}

void ulapi_fd_flush(void *id)
{
  if (NULL != id) tcflush(*((int *) id), TCIOFLUSH);
}

void ulapi_fd_drain(void *id)
{
  if (NULL != id) tcdrain(*((int *) id));
}

ulapi_result
ulapi_fd_close(void *id)
{
  return 0 == close(*((int *) id)) ? ULAPI_OK : ULAPI_ERROR;
}

/* Serial interface */

/* Converts a baud rate number, say 9600, into its equivalent token,
   say B9600. */
static int to_bbaud(int baud)
{
  int maxbbaud;

  if (baud <= 50) return B50;
  if (baud <= 75) return B75;
  if (baud <= 110) return B110;
  if (baud <= 134) return B134;
  if (baud <= 150) return B150;
  if (baud <= 200) return B200;
  if (baud <= 300) return B300;
  if (baud <= 600) return B600;
  if (baud <= 1200) return B1200;
  if (baud <= 1800) return B1800;
  if (baud <= 2400) return B2400;
  if (baud <= 4800) return B4800;
  if (baud <= 9600) return B9600;
  if (baud <= 19200) return B19200;
  if (baud <= 38400) return B38400;
  if (baud <= 57600) return B57600;
  maxbbaud = B57600;
#ifdef B115200
  if (baud <= 115200) return B115200;
  maxbbaud = B115200;
#endif
#ifdef B230400
  if (baud <= 230400) return B230400;
  maxbbaud = B230400;
#endif
#ifdef B460800
  if (baud <= 460800) return B460800;
  maxbbaud = B460800;
#endif
  return maxbbaud;
}

/* put terminal into a raw mode, and save original settings */
static int tty_raw(int fd, int baud, struct termios * save_termios)
{
  struct termios buf;
  int status;

  if (tcgetattr(fd, save_termios) < 0) {
    return -1;
  }

  buf = *save_termios;	/* structure copy */

  baud = to_bbaud(baud);
  cfsetispeed(&buf, baud);
  cfsetospeed(&buf, baud);

  buf.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  /* echo off, canonical mode off, extended input
     processing off, signal chars off */

  buf.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  /* no SIGINT on BREAK, CR-to-NL off, input parity
     check off, don't strip 8th bit on input,
     output flow control off */

  buf.c_cflag &= ~(CSIZE | PARENB);
  /* clear size bits, parity checking off */
  buf.c_cflag |= CS8;
  /* set 8 bits/char */

  buf.c_oflag &= ~(OPOST);
  /* output processing off */

  buf.c_cc[VMIN] = 1;	/* Case B: 1 byte at a time, no timer */
  buf.c_cc[VTIME] = 0;

  if (tcsetattr(fd, TCSAFLUSH, &buf) < 0) {
    return -1;
  }

  ioctl(fd, TIOCMGET, &status);
#if 1
  /* enable DTR and RTS */
  status &= ~(TIOCM_DTR | TIOCM_RTS);
#else
  /* disable DTR and RTS */
  status |= (TIOCM_DTR | TIOCM_RTS);
#endif
  ioctl(fd, TIOCMSET, &status);

  return 0;
}

void * 
ulapi_serial_new(void)
{
  return ulapi_fd_new();
}

ulapi_result
ulapi_serial_delete(void *id)
{
  return ulapi_fd_delete(id);
}

ulapi_result
ulapi_serial_open(const char *port, void *id)
{
  return ulapi_fd_open(port, id);
}

ulapi_result
ulapi_serial_baud(void *id, int baud)
{
  int fd = *((int *) id);
  struct termios save_termios;

  if (0 != tty_raw(fd, baud, &save_termios)) return ULAPI_ERROR;

  return ULAPI_OK;
}

ulapi_result
ulapi_serial_set_nonblocking(void *id)
{
  return ulapi_fd_set_nonblocking(id);
}

ulapi_result
ulapi_serial_set_blocking(void *id)
{
  return ulapi_fd_set_blocking(id);
}

ulapi_integer
ulapi_serial_read(void *id,
		  char *buf,
		  ulapi_integer len)
{
  return ulapi_fd_read(id, buf, len);
}

ulapi_integer
ulapi_serial_write(void *id,
		   const char *buf,
		   ulapi_integer len)
{
  return ulapi_fd_write(id, buf, len);
}

ulapi_result
ulapi_serial_close(void *id)
{
  return ulapi_fd_close(id);
}

#ifdef NO_DL

void *ulapi_dl_open(const char *objname, char *errstr, int errlen)
{
  return NULL;
}

void ulapi_dl_close(void *handle)
{
  return;
}

void *ulapi_dl_sym(void *handle, const char *name, char *errstr, int errlen)
{
  return NULL;
}

#else

void *ulapi_dl_open(const char *objname, char *errstr, int errlen)
{
  void *handle;
  void *error;

  dlerror();
  handle = dlopen(objname, RTLD_LAZY);
  error = dlerror();

  if (NULL == handle) {
    if (NULL != errstr && NULL != error) {
      strncpy(errstr, error, errlen);
      errstr[errlen - 1] = 0;
    }
    return NULL;
  }

  if (NULL != errstr) {
    errstr[0] = 0;
  }
  return handle;
}

void ulapi_dl_close(void *handle)
{
  if (NULL != handle) {
    dlclose(handle);
  }
}

void *ulapi_dl_sym(void *handle, const char *name, char *errstr, int errlen)
{
  void *sym;
  void *error;

  dlerror();
  sym = dlsym(handle, name);
  error = dlerror();

  if (error != NULL) {
    if (NULL != errstr) {
      strncpy(errstr, error, errlen);
      errstr[errlen - 1] = 0;
    }
    return NULL;
  }

  if (NULL != errstr) {
    errstr[0] = 0;
  }
  return sym;
}

#endif

ulapi_result ulapi_system(const char *prog, ulapi_integer *result)
{
  int retval;

  retval = system(prog) >> 8;

  if (127 == retval) return ULAPI_ERROR;

  *result = retval;

  return ULAPI_OK;
}

ulapi_flag ulapi_ispath(const char *path)
{
  if ((NULL == path) || 
      (NULL == strchr(path, '/'))) {
    return 0;
  }

  return 1;
}


char *ulapi_fixpath(const char *path, char *fix, size_t len)
{
  const char *pathptr = path;
  char *fixptr = fix;
  char c;

  if (NULL == fixptr) return NULL;
  if (len <= 0) return fixptr;

  while (0 != *pathptr && --len > 0) {
    c = *pathptr++;
    if ('\\' == c) c = '/';
    *fixptr++ = c;
  }

  *fixptr = 0;

  return fix;
}

char *ulapi_basename(const char *path, char *base)
{
  const char *endp, *startp;

  if (path == NULL || *path == '\0') {
    strcpy(base, ".");
    return base;
  }

  endp = path + strlen(path) - 1;
  while (endp > path && *endp == '/')
    endp--;

  if (endp == path && *endp == '/') {
    strcpy(base, "/");
    return base;
  }

  startp = endp;
  while (startp > path && *(startp - 1) != '/') startp--;

  strcpy(base, startp);
  base[(endp - startp) + 1] = 0;

  return base;
}

char *ulapi_dirname(const char *path, char *base)
{
  const char *endp;

  if (path == NULL || *path == '\0') {
    strcpy(base, ".");
    return base;
  }

  endp = path + strlen(path) - 1;
  while (endp > path && *endp == '/') endp--;

  while (endp > path && *endp != '/') endp--;

  if (endp == path) {
    strcpy(base, *endp == '/' ? "/" : ".");
    return base;
  }

  do {
    endp--;
  } while (endp > path && *endp == '/');

  strcpy(base, path);
  base[(endp - path) + 1] = 0;

  return base;
}

#ifdef NEED_GETOPT

/*
 * Copyright (c) 2002 Todd C. Miller <Todd.Miller@courtesan.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Dieter Baron and Thomas Klausner.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#define warnx printf

#define REPLACE_GETOPT

#ifdef REPLACE_GETOPT
int	opterr = 1;		/* if error message should be printed */
int	optind = 1;		/* index into parent argv vector */
int	optopt = '?';		/* character checked for validity */
char    *optarg = NULL;		/* argument associated with option */
#endif
int	optreset = 0;		/* reset getopt */

#define PRINT_ERROR	((opterr) && (*options != ':'))

#define FLAG_PERMUTE	0x01	/* permute non-options to the end of argv */
#define FLAG_ALLARGS	0x02	/* treat non-options as args to option "-1" */
#define FLAG_LONGONLY	0x04	/* operate as getopt_long_only */

/* return values */
#define	BADCH		(int)'?'
#define	BADARG		((*options == ':') ? (int)':' : (int)'?')
#define	INORDER 	(int)1

#define	EMSG		""

static int getopt_internal(int, char * const *, const char *,
			   const struct option *, int *, int);
static int parse_long_options(char * const *, const char *,
			      const struct option *, int *, int);
static int gcd(int, int);
static void permute_args(int, int, int, char * const *);

static char *place = EMSG; /* option letter processing */

/* XXX: set optreset to 1 rather than these two */
static int nonopt_start = -1; /* first non option argument (for permute) */
static int nonopt_end = -1;   /* first option after non options (for permute) */

/* Error messages */
static const char recargchar[] = "option requires an argument -- %c";
static const char recargstring[] = "option requires an argument -- %s";
static const char ambig[] = "ambiguous option -- %.*s";
static const char noarg[] = "option doesn't take an argument -- %.*s";
static const char illoptchar[] = "unknown option -- %c";
static const char illoptstring[] = "unknown option -- %s";

/*
 * Compute the greatest common divisor of a and b.
 */
static int
gcd(int a, int b)
{
  int c;

  c = a % b;
  while (c != 0) {
    a = b;
    b = c;
    c = a % b;
  }

  return (b);
}

/*
 * Exchange the block from nonopt_start to nonopt_end with the block
 * from nonopt_end to opt_end (keeping the same order of arguments
 * in each block).
 */
static void
permute_args(int panonopt_start, int panonopt_end, int opt_end,
	     char * const *nargv)
{
  int cstart, cyclelen, i, j, ncycle, nnonopts, nopts, pos;
  char *swap;

  /*
   * compute lengths of blocks and number and size of cycles
   */
  nnonopts = panonopt_end - panonopt_start;
  nopts = opt_end - panonopt_end;
  ncycle = gcd(nnonopts, nopts);
  cyclelen = (opt_end - panonopt_start) / ncycle;

  for (i = 0; i < ncycle; i++) {
    cstart = panonopt_end+i;
    pos = cstart;
    for (j = 0; j < cyclelen; j++) {
      if (pos >= panonopt_end)
	pos -= nnonopts;
      else
	pos += nopts;
      swap = nargv[pos];
      /* LINTED const cast */
      ((char **) nargv)[pos] = nargv[cstart];
      /* LINTED const cast */
      ((char **)nargv)[cstart] = swap;
    }
  }
}

/*
 * parse_long_options --
 *	Parse long options in argc/argv argument vector.
 * Returns -1 if short_too is set and the option does not match long_options.
 */
static int
parse_long_options(char * const *nargv, const char *options,
		   const struct option *long_options, int *idx, int short_too)
{
  char *current_argv, *has_equal;
  size_t current_argv_len;
  int i, match;

  current_argv = place;
  match = -1;

  optind++;

  if ((has_equal = strchr(current_argv, '=')) != NULL) {
    /* argument found (--option=arg) */
    current_argv_len = has_equal - current_argv;
    has_equal++;
  } else
    current_argv_len = strlen(current_argv);

  for (i = 0; long_options[i].name; i++) {
    /* find matching long option */
    if (strncmp(current_argv, long_options[i].name,
		current_argv_len))
      continue;

    if (strlen(long_options[i].name) == current_argv_len) {
      /* exact match */
      match = i;
      break;
    }
    /*
     * If this is a known short option, don't allow
     * a partial match of a single character.
     */
    if (short_too && current_argv_len == 1)
      continue;

    if (match == -1)	/* partial match */
      match = i;
    else {
      /* ambiguous abbreviation */
      if (PRINT_ERROR)
	warnx(ambig, (int)current_argv_len,
	      current_argv);
      optopt = 0;
      return (BADCH);
    }
  }
  if (match != -1) {		/* option found */
    if (long_options[match].has_arg == no_argument
	&& has_equal) {
      if (PRINT_ERROR)
	warnx(noarg, (int)current_argv_len,
	      current_argv);
      /*
       * XXX: GNU sets optopt to val regardless of flag
       */
      if (long_options[match].flag == NULL)
	optopt = long_options[match].val;
      else
	optopt = 0;
      return (BADARG);
    }
    if (long_options[match].has_arg == required_argument ||
	long_options[match].has_arg == optional_argument) {
      if (has_equal)
	optarg = has_equal;
      else if (long_options[match].has_arg ==
	       required_argument) {
	/*
	 * optional argument doesn't use next nargv
	 */
	optarg = nargv[optind++];
      }
    }
    if ((long_options[match].has_arg == required_argument)
	&& (optarg == NULL)) {
      /*
       * Missing argument; leading ':' indicates no error
       * should be generated.
       */
      if (PRINT_ERROR)
	warnx(recargstring,
	      current_argv);
      /*
       * XXX: GNU sets optopt to val regardless of flag
       */
      if (long_options[match].flag == NULL)
	optopt = long_options[match].val;
      else
	optopt = 0;
      --optind;
      return (BADARG);
    }
  } else {			/* unknown option */
    if (short_too) {
      --optind;
      return (-1);
    }
    if (PRINT_ERROR)
      warnx(illoptstring, current_argv);
    optopt = 0;
    return (BADCH);
  }
  if (idx)
    *idx = match;
  if (long_options[match].flag) {
    *long_options[match].flag = long_options[match].val;
    return (0);
  } else
    return (long_options[match].val);
}

/*
 * getopt_internal --
 *	Parse argc/argv argument vector.  Called by user level routines.
 */
static int
getopt_internal(int nargc, char * const *nargv, const char *options,
		const struct option *long_options, int *idx, int flags)
{
  char *oli;				/* option letter list index */
  int optchar, short_too;
  static int posixly_correct = -1;

  if (options == NULL)
    return (-1);

  /*
   * Disable GNU extensions if POSIXLY_CORRECT is set or options
   * string begins with a '+'.
   */
  if (posixly_correct == -1)
    posixly_correct = (getenv("POSIXLY_CORRECT") != NULL);
  if (posixly_correct || *options == '+')
    flags &= ~FLAG_PERMUTE;
  else if (*options == '-')
    flags |= FLAG_ALLARGS;
  if (*options == '+' || *options == '-')
    options++;

  /*
   * XXX Some GNU programs (like cvs) set optind to 0 instead of
   * XXX using optreset.  Work around this braindamage.
   */
  if (optind == 0)
    optind = optreset = 1;

  optarg = NULL;
  if (optreset)
    nonopt_start = nonopt_end = -1;
start:
  if (optreset || !*place) {		/* update scanning pointer */
    optreset = 0;
    if (optind >= nargc) {          /* end of argument vector */
      place = EMSG;
      if (nonopt_end != -1) {
	/* do permutation, if we have to */
	permute_args(nonopt_start, nonopt_end,
		     optind, nargv);
	optind -= nonopt_end - nonopt_start;
      }
      else if (nonopt_start != -1) {
	/*
	 * If we skipped non-options, set optind
	 * to the first of them.
	 */
	optind = nonopt_start;
      }
      nonopt_start = nonopt_end = -1;
      return (-1);
    }
    if (*(place = nargv[optind]) != '-' ||
	(place[1] == '\0' && strchr(options, '-') == NULL)) {
      place = EMSG;		/* found non-option */
      if (flags & FLAG_ALLARGS) {
	/*
	 * GNU extension:
	 * return non-option as argument to option 1
	 */
	optarg = nargv[optind++];
	return (INORDER);
      }
      if (!(flags & FLAG_PERMUTE)) {
	/*
	 * If no permutation wanted, stop parsing
	 * at first non-option.
	 */
	return (-1);
      }
      /* do permutation */
      if (nonopt_start == -1)
	nonopt_start = optind;
      else if (nonopt_end != -1) {
	permute_args(nonopt_start, nonopt_end,
		     optind, nargv);
	nonopt_start = optind -
	  (nonopt_end - nonopt_start);
	nonopt_end = -1;
      }
      optind++;
      /* process next argument */
      goto start;
    }
    if (nonopt_start != -1 && nonopt_end == -1)
      nonopt_end = optind;

    /*
     * Check for "--" or "--foo" with no long options
     * but if place is simply "-" leave it unmolested.
     */
    if (place[1] != '\0' && *++place == '-' &&
	(place[1] == '\0' || long_options == NULL)) {
      optind++;
      place = EMSG;
      /*
       * We found an option (--), so if we skipped
       * non-options, we have to permute.
       */
      if (nonopt_end != -1) {
	permute_args(nonopt_start, nonopt_end,
		     optind, nargv);
	optind -= nonopt_end - nonopt_start;
      }
      nonopt_start = nonopt_end = -1;
      return (-1);
    }
  }

  /*
   * Check long options if:
   *  1) we were passed some
   *  2) the arg is not just "-"
   *  3) either the arg starts with -- we are getopt_long_only()
   */
  if (long_options != NULL && place != nargv[optind] &&
      (*place == '-' || (flags & FLAG_LONGONLY))) {
    short_too = 0;
    if (*place == '-')
      place++;		/* --foo long option */
    else if (*place != ':' && strchr(options, *place) != NULL)
      short_too = 1;		/* could be short option too */

    optchar = parse_long_options(nargv, options, long_options,
				 idx, short_too);
    if (optchar != -1) {
      place = EMSG;
      return (optchar);
    }
  }

  if ((optchar = (int)*place++) == (int)':' ||
      (oli = strchr(options, optchar)) == NULL) {
    /*
     * If the user didn't specify '-' as an option,
     * assume it means -1 as POSIX specifies.
     */
    if (optchar == (int)'-')
      return (-1);
    /* option letter unknown or ':' */
    if (!*place)
      ++optind;
    if (PRINT_ERROR)
      warnx(illoptchar, optchar);
    optopt = optchar;
    return (BADCH);
  }
  if (long_options != NULL && optchar == 'W' && oli[1] == ';') {
    /* -W long-option */
    if (*place)			/* no space */
      /* NOTHING */;
    else if (++optind >= nargc) {	/* no arg */
      place = EMSG;
      if (PRINT_ERROR)
	warnx(recargchar, optchar);
      optopt = optchar;
      return (BADARG);
    } else				/* white space */
      place = nargv[optind];
    optchar = parse_long_options(nargv, options, long_options,
				 idx, 0);
    place = EMSG;
    return (optchar);
  }
  if (*++oli != ':') {			/* doesn't take argument */
    if (!*place)
      ++optind;
  } else {				/* takes (optional) argument */
    optarg = NULL;
    if (*place)			/* no white space */
      optarg = place;
    /* XXX: disable test for :: if PC? (GNU doesn't) */
    else if (oli[1] != ':') {	/* arg not optional */
      if (++optind >= nargc) {	/* no arg */
	place = EMSG;
	if (PRINT_ERROR)
	  warnx(recargchar, optchar);
	optopt = optchar;
	return (BADARG);
      } else
	optarg = nargv[optind];
    }
    place = EMSG;
    ++optind;
  }
  /* dump back option letter */
  return (optchar);
}

#ifdef REPLACE_GETOPT
/*
 * getopt --
 *	Parse argc/argv argument vector.
 *
 * [eventually this will replace the BSD getopt]
 */
int
getopt(int nargc, char * const *nargv, const char *options)
{

  /*
   * We dont' pass FLAG_PERMUTE to getopt_internal() since
   * the BSD getopt(3) (unlike GNU) has never done this.
   *
   * Furthermore, since many privileged programs call getopt()
   * before dropping privileges it makes sense to keep things
   * as simple (and bug-free) as possible.
   */
  return (getopt_internal(nargc, nargv, options, NULL, NULL, 0));
}
#endif /* REPLACE_GETOPT */

/*
 * getopt_long --
 *	Parse argc/argv argument vector.
 */
int
getopt_long(int nargc, char * const * nargv, const char * options, const struct option * long_options, int * idx)
{

  return (getopt_internal(nargc, nargv, options, long_options, idx,
			  FLAG_PERMUTE));
}

/*
 * getopt_long_only --
 *	Parse argc/argv argument vector.
 */
int
getopt_long_only(int nargc, char * const * nargv, const char * options, const struct option * long_options, int * idx)
{

  return (getopt_internal(nargc, nargv, options, long_options, idx,
			  FLAG_PERMUTE|FLAG_LONGONLY));
}

#endif	/* NEED_GETOPT */
