/*
  unix_rtapi.c

  Implementations of RT API functions declared in rtapi.h, for Unix.
  Non-realtime Unix processes would call these functions to simulate
  realtime behavior.
*/

#include <stdio.h>		/* vprintf() */
#include <stddef.h>		/* NULL */
#include <stdlib.h>		/* malloc(), sizeof(), atexit(), strtol() */
#include <string.h>		/* strncmp(), strlen() */
#include <stdarg.h>		/* va_* */
#include <ctype.h>		/* isspace */
#include <pthread.h>		/* pthread_create(), pthread_mutex_t */
#include <unistd.h>		/* usleep() */
#include <signal.h>		/* signal, sigsuspend */
#include <time.h>		/* struct timespec, nanosleep */
#include <sys/time.h>		/* gettimeofday(), struct timeval */
#include <sys/ipc.h>		/* IPC_* */
#include <sys/shm.h>		/* shmget() */
#include <sys/sem.h>
#include <errno.h>
#include <fcntl.h>		/* O_RDONLY, O_NONBLOCK */
#include <termios.h>  		/* tcflush, TCIOFLUSH */
#include <sys/types.h>		/* fd_set, FD_ISSET() */
#include <sys/socket.h>		/* PF_INET, socket(), listen(), bind(), etc. */
#include <sys/stat.h>		/* struct stat */
#include <sys/ioctl.h>
#include <netinet/in.h>		/* struct sockaddr_in */
#include <netdb.h>		/* gethostbyname */
#include "rtapi.h"		/* these decls */
#include "ulapi.h"		/* for the shared memory pass-through */

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
  ulapi_result u_ret;
  ulapi_integer u_res;

  u_ret = ulapi_system(prog, &u_res);

  if (ULAPI_OK == u_ret) {
    *result = u_res;
    return RTAPI_OK;
  }

  *result = 0;
  return RTAPI_ERROR;
}

static rtapi_integer _rtapi_wait_offset_nsec = 0;

/*
  Returns abs val of end - start, as struct timeval
*/
static struct timeval timerdiff(struct timeval end, struct timeval start)
{
  struct timeval diff;

  if (end.tv_usec < start.tv_usec) {
    if (end.tv_sec < start.tv_sec) {
      /* 1.1 - 9.9 */
      diff.tv_usec = start.tv_usec - end.tv_usec;
      diff.tv_sec = start.tv_sec - end.tv_sec;
    } else {
      /* 9.1 - 1.9 */
      diff.tv_usec = 1000000 - start.tv_usec + end.tv_usec;
      diff.tv_sec = end.tv_sec - start.tv_sec - 1;
    }
  } else {
    if (end.tv_sec < start.tv_sec) {
      /* 1.9 - 9.1 */
      diff.tv_usec = 1000000 - end.tv_usec + start.tv_usec;
      diff.tv_sec = start.tv_sec - end.tv_sec - 1;
    } else {
      /* 9.9 - 1.1 */
      diff.tv_usec = end.tv_usec - start.tv_usec;
      diff.tv_sec = end.tv_sec - start.tv_sec;
    }
  }

  return diff;
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

rtapi_integer rtapi_clock_period = 1;

rtapi_result rtapi_clock_set_period(rtapi_integer nsecs)
{
  rtapi_clock_period = nsecs;

  return RTAPI_OK;
}

rtapi_result rtapi_clock_get_time(rtapi_integer * secs, 
				  rtapi_integer * nsecs)
{
  struct timeval tv;

  (void) gettimeofday(&tv, NULL);

  *secs = tv.tv_sec;
  *nsecs = tv.tv_usec * 1000;

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
      *diff_nsecs = 1000000000 - start_nsecs + end_nsecs;
      *diff_secs = end_secs - start_secs - 1;
    }
  } else {
    if (end_secs < start_secs) {
      /* 1.9 - 9.1 */
      *diff_nsecs = 1000000000 - end_nsecs + start_nsecs;
      *diff_secs = start_secs - end_secs - 1;
    } else {
      /* 9.9 - 1.1 */
      *diff_nsecs = end_nsecs - start_nsecs;
      *diff_secs = end_secs - start_secs;
    }
  }

  return RTAPI_OK;
}

rtapi_result rtapi_task_init(rtapi_task_struct *task)
{
  if (0 != pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)) return RTAPI_ERROR;
  if (0 != pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL)) return RTAPI_ERROR;

  return RTAPI_OK;
}

rtapi_task_struct *rtapi_task_new(void)
{
  rtapi_task_struct *ptr = (rtapi_task_struct *) malloc(sizeof(rtapi_task_struct));
  if (RTAPI_OK != rtapi_task_init(ptr)) {
    free(ptr);
    ptr = NULL;
  }

  return ptr;
}

extern rtapi_result rtapi_task_clear(rtapi_task_struct *task)
{
  return RTAPI_OK;
}

rtapi_result rtapi_task_delete(rtapi_task_struct *task)
{
  if (NULL != task) {
    rtapi_task_clear(task);
    free(task);
  }

  return RTAPI_OK;
}

typedef void *(*pthread_task_code)(void *);

rtapi_result
rtapi_task_start(rtapi_task_struct *task,
		 void (*taskcode)(void *),
		 void *taskarg,
		 rtapi_prio prio,
		 rtapi_integer stacksize,
		 rtapi_integer period_nsec, 
		 rtapi_flag uses_fp)
{
  pthread_attr_t attr;
  struct sched_param sched_param;

  pthread_attr_init(&attr);
  sched_param.sched_priority = prio;
  pthread_attr_setschedparam(&attr, &sched_param);
  pthread_create((pthread_t *) task, &attr, (pthread_task_code) taskcode, taskarg);
  pthread_setschedparam(*((pthread_t *) task), SCHED_OTHER, &sched_param);
  /* ignore period_nsec, since we can't handle it in pthreads, so the
     application must call rtapi_wait(period_nsec) */

  return RTAPI_OK;
}

rtapi_result rtapi_task_stop(rtapi_task_struct *task)
{
  return (pthread_cancel(*((pthread_t *) task)) == 0 ? RTAPI_OK : RTAPI_ERROR);
}

rtapi_result rtapi_task_pause(rtapi_task_struct *task)
{
  return RTAPI_OK;
}

rtapi_result rtapi_task_resume(rtapi_task_struct *task)
{
  return RTAPI_OK;
}

rtapi_result rtapi_task_set_period(rtapi_task_struct *task, rtapi_integer period_nsec)
{
  return RTAPI_OK;
}

rtapi_result rtapi_self_set_period(rtapi_integer period_nsec)
{
  return RTAPI_OK;
}

rtapi_result rtapi_wait(rtapi_integer period_nsec)
{
  struct timespec ts;

  if (period_nsec < _rtapi_wait_offset_nsec + 1) period_nsec = 1;
  else period_nsec -= _rtapi_wait_offset_nsec;

  ts.tv_sec = 0;
  ts.tv_nsec = period_nsec;

  (void) nanosleep(&ts, NULL);

  return RTAPI_OK;
}

rtapi_result rtapi_task_exit(void)
{
  return RTAPI_OK;
}

rtapi_integer rtapi_task_stack_check(rtapi_task_struct *task)
{
  return -1;			/* irrelevant on this platform */
}

typedef struct {
  rtapi_id id;
  void * addr;
} unix_shm_struct;

/*
  This user-level user-to-realtime shared memory implemetation is the
  same as normal user-level shared memory. RTAI's realtime side is
  compatible -- no special user-side shared memory code needs to be
  written.
*/

void * rtapi_shm_new(rtapi_id key, rtapi_integer size)
{
  return ulapi_shm_new(key, size);
}

void * rtapi_shm_addr(void * shm)
{
  return ulapi_shm_addr(shm);
}

rtapi_result rtapi_shm_delete(void * shm)
{
  return ulapi_shm_delete(shm);
}

void * rtapi_rtm_new(rtapi_id key, rtapi_integer size)
{
  return ulapi_shm_new(key, size);
}

void * rtapi_rtm_addr(void * rtm)
{
  return ulapi_shm_addr(rtm);
}

rtapi_result rtapi_rtm_delete(void * rtm)
{
  return ulapi_shm_delete(rtm);
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

rtapi_result rtapi_mutex_init(rtapi_mutex_struct *mutex, rtapi_id key)
{
  if (0 == pthread_mutex_init(mutex, NULL)) {
    (void) pthread_mutex_unlock(mutex);
    return RTAPI_OK;
  }
  return RTAPI_ERROR;
}

rtapi_mutex_struct *rtapi_mutex_new(rtapi_id key)
{
  rtapi_mutex_struct *mutex;

  mutex = (rtapi_mutex_struct *) malloc(sizeof(rtapi_mutex_struct));
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

rtapi_result rtapi_mutex_clear(rtapi_mutex_struct *mutex)
{
  (void) pthread_mutex_destroy((pthread_mutex_t *) mutex);

  return RTAPI_OK;
}

rtapi_result rtapi_mutex_delete(rtapi_mutex_struct *mutex)
{
  if (NULL == mutex) return RTAPI_ERROR;

  (void) pthread_mutex_destroy((pthread_mutex_t *) mutex);
  free(mutex);

  return RTAPI_OK;
}

rtapi_result rtapi_mutex_give(rtapi_mutex_struct *mutex)
{
  return (0 == pthread_mutex_unlock((pthread_mutex_t *) mutex) ? RTAPI_OK : RTAPI_ERROR);
}

rtapi_result rtapi_mutex_take(rtapi_mutex_struct *mutex)
{
  return (0 == pthread_mutex_lock((pthread_mutex_t *) mutex) ? RTAPI_OK : RTAPI_ERROR);
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

int rtapi_argc;
char ** rtapi_argv;

rtapi_result rtapi_app_init(int argc, char ** argv)
{
  struct timeval start, end, diff;
  struct timespec ts;
  int t;

  /* copy argc and argv for use by tasks when they init */
  rtapi_argc = argc;
  rtapi_argv = (char **) malloc(argc * sizeof(char *));
  for (t = 0; t < argc; t++) {
    rtapi_argv[t] = (char *) malloc(strlen(argv[t]) + 1);
    strcpy(rtapi_argv[t], argv[t]);
  }

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

void rtapi_exit(void)
{
  raise(SIGINT);
}

void * rtapi_new(rtapi_integer size)
{
  return malloc(size);
}

void rtapi_free(void * ptr)
{
  free(ptr);
}

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

  return *var;
}

rtapi_result rtapi_string_to_integer(const char * str, rtapi_integer * var)
{
  int i;
  char * endptr;

  i = (int) strtol(str, &endptr, 0);
  if (endptr == str ||
      (! isspace(*endptr) && 0 != *endptr)) {
    *var = 0;
    return RTAPI_ERROR;
  }

  *var = (rtapi_integer) i;
  return RTAPI_OK;
}

const char * rtapi_string_skipwhite(const char * str)
{
  const char * ptr = str;

  while (isspace(*ptr)) ptr++;

  return ptr;
}

const char * rtapi_string_skipnonwhite(const char * str)
{
  const char * ptr = str;

  while (! isspace(*ptr) && *ptr != 0) ptr++;

  return ptr;
}

const char * rtapi_string_skipone(const char * str)
{
  const char * ptr;

  ptr = rtapi_string_skipnonwhite(str);
  return rtapi_string_skipwhite(ptr);
}

char * rtapi_string_copyone(char * dst, const char * src)
{
  char * dstptr;
  const char * srcptr;

  if (NULL == dst || NULL == src) return dst;

  dstptr = dst, srcptr = rtapi_string_skipwhite(src);
  while (!isspace(*srcptr) && 0 != *srcptr) {
    *dstptr++ = *srcptr++;
  }
  *dstptr = 0;

  return dst;
}

rtapi_integer rtapi_socket_client(rtapi_integer port,
				  const char * hostname)
{
  int socket_fd;
  struct sockaddr_in server_addr;
  struct hostent *ent;
  struct in_addr *in_a;

  if (NULL == (ent = gethostbyname(hostname))) {
    perror("gethostbyname");
    return -1;
  }
  in_a = (struct in_addr *) ent->h_addr_list[0];

  memset(&server_addr, 0, sizeof(struct sockaddr_in));
  server_addr.sin_family = PF_INET;
  server_addr.sin_addr.s_addr = in_a->s_addr;
  server_addr.sin_port = htons(port);

  if (-1 == (socket_fd = socket(PF_INET, SOCK_STREAM, 0))) {
    perror("socket");
    return -1;
  }

  if (-1 == connect(socket_fd, 
		    (struct sockaddr *) &server_addr,
		    sizeof(struct sockaddr_in))) {
    perror("connect");
    close(socket_fd);
    return -1;
  }

  return socket_fd;
}

rtapi_integer rtapi_socket_server(rtapi_integer port)
{
  int socket_fd;
  struct sockaddr_in myaddr;
  int on;
  struct linger mylinger = { 0 };
  enum {BACKLOG = 5};

  if (-1 == (socket_fd = socket(PF_INET, SOCK_STREAM, 0))) {
    fprintf(stderr, "can't create socket file descriptor\n");
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
    perror("setsockopt");
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
    perror("setsockopt");
    close(socket_fd);
    return -1;
  }

  memset(&myaddr, 0, sizeof(struct sockaddr_in));
  myaddr.sin_family = PF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myaddr.sin_port = htons(port);

  if (-1 == bind(socket_fd, (struct sockaddr *) &myaddr,
		 sizeof(struct sockaddr_in))) {
    perror("bind");
    close(socket_fd);
    return -1;
  }

  if (-1 == listen(socket_fd, BACKLOG)) {
    perror("listen");
    close(socket_fd);
    return -1;
  }

  return socket_fd;
}

rtapi_integer rtapi_socket_get_client(rtapi_integer socket_fd)
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
    perror("select");
    return -1;
  }

  memset(&client_addr, 0, sizeof(struct sockaddr_in));
  client_len = sizeof(struct sockaddr_in);
  client_fd = 
    accept(socket_fd,
	   (struct sockaddr *) &client_addr, 
	   &client_len);
  if (-1 == client_fd) {
    perror("accept");
    return -1;
  }
  
  return client_fd;
}

rtapi_result rtapi_socket_set_nonblocking(rtapi_integer fd)
{
  int flags;

  flags = fcntl(fd, F_GETFL);
  if (-1 == fcntl(fd, F_SETFL, flags | O_NONBLOCK)) {
    return RTAPI_ERROR;
  }

  return RTAPI_OK;
}

rtapi_result rtapi_socket_set_blocking(rtapi_integer fd)
{
  int flags;

  flags = fcntl(fd, F_GETFL);
  if (-1 == fcntl(fd, F_SETFL, flags & ~O_NONBLOCK)) {
    return RTAPI_ERROR;
  }

  return RTAPI_OK;
}

rtapi_integer rtapi_socket_read(rtapi_integer id,
				char * buf,
				rtapi_integer len)
{
  return read(id, buf, len);
}

rtapi_integer rtapi_socket_write(rtapi_integer id,
				 const char * buf,
				 rtapi_integer len)
{
  return write(id, buf, len);
}

rtapi_result rtapi_socket_close(rtapi_integer id)
{
  return 0 == close((int) id) ? RTAPI_OK : RTAPI_ERROR;
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
rtapi_serial_new(void)
{
  return malloc(sizeof(int));
}

rtapi_result
rtapi_serial_delete(void * id)
{
  if (NULL != id) free(id);

  return RTAPI_OK;
}

rtapi_result
rtapi_serial_open(const char * port, void * id)
{
  int fd;

  fd = open(port, O_RDWR);

  if (fd < 0) return RTAPI_ERROR;
  
  *((int *) id) = fd;
  return RTAPI_OK;
}

rtapi_result
rtapi_serial_baud(void *id, int baud)
{
  int fd = *((int *) id);
  struct termios save_termios;

  if (0 != tty_raw(fd, baud, &save_termios)) return RTAPI_ERROR;

  return RTAPI_OK;
}

rtapi_result
rtapi_serial_set_nonblocking(void * id)
{
  int fd;
  int flags;

  fd = *((int *) id);
  flags = fcntl(fd, F_GETFL);
  if (-1 == fcntl(fd, F_SETFL, flags | O_NONBLOCK)) {
    return RTAPI_ERROR;
  }

  return RTAPI_OK;
}

rtapi_result
rtapi_serial_set_blocking(void * id)
{
  int fd;
  int flags;

  fd = *((int *) id);
  flags = fcntl(fd, F_GETFL);
  if (-1 == fcntl(fd, F_SETFL, flags & ~O_NONBLOCK)) {
    return RTAPI_ERROR;
  }

  return RTAPI_OK;
}

rtapi_integer
rtapi_serial_read(void * id,
		  char * buf,
		  rtapi_integer len)
{
  return read(*((int *) id), buf, len);
}

rtapi_integer
rtapi_serial_write(void * id,
		   const char * buf,
		   rtapi_integer len)
{
  return write(*((int *) id), buf, len);
}

rtapi_result
rtapi_serial_close(void * id)
{
  return 0 == close(*((int *) id)) ? RTAPI_OK : RTAPI_ERROR;
}
