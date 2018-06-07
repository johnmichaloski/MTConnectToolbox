/*
  win32_rtapi.c

  Implementations of RT API functions declared in rtapi.h, for WIN32.
  Non-realtime Windows processes would call these functions to simulate
  realtime behavior.
*/

/* turn off sprintf warnings */
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

/* Windows XP */
#define _WIN32_WINNT 0x501

#include <stdio.h>		/* vprintf() */
#include <stddef.h>		/* NULL */
#include <stdlib.h>		/* malloc(), sizeof(), atexit(), strtol() */
#include <string.h>		/* strncmp(), strlen() */
#include <stdarg.h>		/* va_* */
#include <ctype.h>		/* isspace */
#include <windows.h>
#include "rtapi.h"		/* these decls */
#include "ulapi.h"		/* for the pass-through implementations, e.g., shm, serial */

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

/*
  Threads are scheduled to run based on their scheduling priority. Each
  thread is assigned a scheduling priority. The priority levels range
  from zero (lowest priority) to 31 (highest priority). Only the
  zero-page thread can have a priority of zero. (The zero-page thread is
  a system thread responsible for zeroing any free pages when there are
  no other threads that need to run.)

  The system treats all threads with the same priority as equal. The
  system assigns time slices in a round-robin fashion to all threads
  with the highest priority. If none of these threads are ready to run,
  the system assigns time slices in a round-robin fashion to all threads
  with the next highest priority. If a higher-priority thread becomes
  available to run, the system ceases to execute the lower-priority
  thread (without allowing it to finish using its time slice), and
  assigns a full time slice to the higher-priority thread.

  Not all numbers are valid. These are the valid ones for the normal
  priority class, from lowest to highest:

  THREAD_PRIORITY_IDLE
  THREAD_PRIORITY_LOWEST
  THREAD_PRIORITY_BELOW_NORMAL
  THREAD_PRIORITY_NORMAL
  THREAD_PRIORITY_ABOVE_NORMAL
  THREAD_PRIORITY_HIGHEST
  THREAD_PRIORITY_TIME_CRITICAL
*/
rtapi_prio rtapi_prio_highest(void)
{
  return THREAD_PRIORITY_TIME_CRITICAL;
}

rtapi_prio rtapi_prio_lowest(void)
{
  /* we won't use idle priority */
  return THREAD_PRIORITY_LOWEST;
}

rtapi_prio rtapi_prio_next_higher(rtapi_prio prio)
{
  int newprio;

  switch (prio) {
  case THREAD_PRIORITY_IDLE:
  case THREAD_PRIORITY_LOWEST:
    newprio = THREAD_PRIORITY_BELOW_NORMAL;
    break;
  case THREAD_PRIORITY_BELOW_NORMAL:
    newprio = THREAD_PRIORITY_NORMAL;
    break;
  case THREAD_PRIORITY_NORMAL:
    newprio = THREAD_PRIORITY_ABOVE_NORMAL;
    break;
  case THREAD_PRIORITY_ABOVE_NORMAL:
    newprio = THREAD_PRIORITY_HIGHEST;
    break;
  case THREAD_PRIORITY_HIGHEST:
    newprio = THREAD_PRIORITY_TIME_CRITICAL;
    break;
  default:
    newprio = prio;
    break;
  }

  return newprio;
}

rtapi_prio rtapi_prio_next_lower(rtapi_prio prio)
{
  int newprio;

  switch (prio) {
  case THREAD_PRIORITY_TIME_CRITICAL:
    newprio = THREAD_PRIORITY_HIGHEST;
    break;
  case THREAD_PRIORITY_HIGHEST:
    newprio = THREAD_PRIORITY_ABOVE_NORMAL;
    break;
  case THREAD_PRIORITY_ABOVE_NORMAL:
    newprio = THREAD_PRIORITY_NORMAL;
    break;
  case THREAD_PRIORITY_NORMAL:
    newprio = THREAD_PRIORITY_BELOW_NORMAL;
    break;
  case THREAD_PRIORITY_BELOW_NORMAL:
    newprio = THREAD_PRIORITY_LOWEST;
    break;
  default:
    newprio = prio;
    break;
  }

  return newprio;
}

/* we don't do anything with the period other than record it
 */
rtapi_integer rtapi_clock_period = 1;

rtapi_result rtapi_clock_set_period(rtapi_integer nsecs)
{
  rtapi_clock_period = nsecs;

  return RTAPI_OK;
}

static double secsPerCount = 1.0;
static double countsPerSec = 1.0;

/* LARGE_INTEGER to double */
#define LI2D(li) (((double) (li).HighPart) * 4294967296.0 + ((double) (li).LowPart))

rtapi_result rtapi_clock_get_time(rtapi_integer *secs, 
				  rtapi_integer *nsecs)
{
  LARGE_INTEGER counts;
  double dtot;
  int isecs, insecs;

  QueryPerformanceCounter(&counts);
  dtot = LI2D(counts) * secsPerCount;

  isecs = (int) dtot;
  insecs = (int) ((dtot - isecs) * 1.0e9);

  *secs = isecs;
  *nsecs = insecs;

  return RTAPI_OK;
}

rtapi_result rtapi_clock_get_interval(rtapi_integer start_secs, 
				      rtapi_integer start_nsecs,
				      rtapi_integer end_secs, 
				      rtapi_integer end_nsecs,
				      rtapi_integer *diff_secs, 
				      rtapi_integer *diff_nsecs)
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
  return ULAPI_OK == ulapi_task_init(task) ? RTAPI_OK : RTAPI_ERROR;
}

rtapi_task_struct *rtapi_task_new(void)
{
  return ulapi_task_new();
}

rtapi_result rtapi_task_clear(rtapi_task_struct *task)
{
  return ulapi_task_clear(task);
}

rtapi_result rtapi_task_delete(rtapi_task_struct *task)
{
  return ULAPI_OK == ulapi_task_delete(task) ? RTAPI_OK : RTAPI_ERROR;
}

rtapi_result
rtapi_task_start(rtapi_task_struct *task,
		 void (*taskcode)(void *),
		 void *taskarg,
		 rtapi_prio prio,
		 rtapi_integer stacksize,
		 rtapi_integer period_nsec, 
		 rtapi_flag uses_fp)
{
  return ULAPI_OK == ulapi_task_start(task, taskcode, taskarg, prio, period_nsec) ? RTAPI_OK : RTAPI_ERROR;
}

rtapi_result
rtapi_task_stop(rtapi_task_struct *task)
{
  return ulapi_task_stop(task);
}

rtapi_result
rtapi_task_pause(rtapi_task_struct *task)
{
  return ulapi_task_pause(task);
}

rtapi_result
rtapi_task_resume(rtapi_task_struct *task)
{
  return ulapi_task_resume(task);
}

rtapi_result
rtapi_task_set_period(rtapi_task_struct *task, rtapi_integer period_nsec)
{
  return ulapi_task_set_period(task, period_nsec);
}

rtapi_result
rtapi_self_set_period(rtapi_integer period_nsec)
{
  return ulapi_self_set_period(period_nsec);
}

rtapi_result
rtapi_wait(rtapi_integer period_nsec)
{
  return ulapi_wait(period_nsec);
}

rtapi_result
rtapi_task_exit(void)
{
  ulapi_task_exit(0);
  return RTAPI_OK;
}

rtapi_integer
rtapi_task_stack_check(rtapi_task_struct *task)
{
  return -1;			/* irrelevant on this platform */
}

void *rtapi_shm_new(rtapi_id key, rtapi_integer size)
{
  return ulapi_shm_new(key, size);
}

void *rtapi_shm_addr(void *shm)
{
  return ulapi_shm_addr(shm);
}

rtapi_result rtapi_shm_delete(void *shm)
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
  return ulapi_mutex_init(mutex, key);
}

rtapi_mutex_struct *rtapi_mutex_new(rtapi_id key)
{
  return ulapi_mutex_new(key);
}

rtapi_result rtapi_mutex_delete(rtapi_mutex_struct *mutex)
{
  return ulapi_mutex_delete(mutex);
}

rtapi_result rtapi_mutex_give(rtapi_mutex_struct *mutex)
{
  return ulapi_mutex_give(mutex);
}

rtapi_result rtapi_mutex_take(rtapi_mutex_struct *mutex)
{
  return ulapi_mutex_take(mutex);
} 

void *rtapi_sem_new(rtapi_id key)
{
  return ulapi_sem_new(key);
}

rtapi_result rtapi_sem_delete(void *sem)
{
  return ulapi_sem_delete(sem);
}

rtapi_result rtapi_sem_give(void *sem)
{
  return ulapi_sem_give(sem);
}

rtapi_result rtapi_sem_take(void *sem)
{
  return ulapi_sem_take(sem);
}

/* The following are from rtapi_app.h, which we don't include here
   since it's only to be included by the main translation unit */

int rtapi_argc;
char **rtapi_argv;

static HANDLE exitEvent = NULL;

rtapi_result
rtapi_app_init(int argc, char **argv)
{
  BOOL retval;
  LARGE_INTEGER freq;
  int t;

  exitEvent = 
    CreateEvent(NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		NULL); // object name

  if (exitEvent == NULL) {
    rtapi_print("CreateEvent failed (%d)\n", GetLastError());
    return RTAPI_ERROR;
  }

  retval = QueryPerformanceFrequency(&freq);
  if (0 == retval) {
    fprintf(stderr, "no performance counter\n");
  } else {
    countsPerSec = LI2D(freq);
    if (countsPerSec != 0.0) {
      secsPerCount = 1.0 / countsPerSec;
    }
    /* else leave alone */
  }
  rtapi_print("%g secsPerCount\n", secsPerCount);

  /* copy argc and argv for use by tasks when they init */
  rtapi_argc = argc;
  rtapi_argv = (char **) malloc(argc * sizeof(char *));
  for (t = 0; t < argc; t++) {
    rtapi_argv[t] = (char *) malloc(strlen(argv[t]) + 1);
    strcpy(rtapi_argv[t], argv[t]);
  }

  /* FIXME */
  for (t = 0; t < argc; t++) rtapi_print("%s\n", rtapi_argv[t]);

  return RTAPI_OK;
}

rtapi_result
rtapi_app_wait(void)
{
  DWORD dwWaitResult;
  rtapi_result retval;

  dwWaitResult = WaitForSingleObject(exitEvent, INFINITE);

  retval = (dwWaitResult == WAIT_OBJECT_0 ? RTAPI_OK : RTAPI_ERROR);
            
  CloseHandle(exitEvent);

  return retval;
}

void rtapi_exit(void)
{
  if (! SetEvent(exitEvent)) {
    rtapi_print("SetEvent failed (%d)\n", GetLastError());
    return;
  }
}

void *
rtapi_new(rtapi_integer size)
{
  return malloc(size);
}

void
rtapi_free(void *ptr)
{
  free(ptr);
}

char *
rtapi_arg_get_string(char **var, char *key)
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

int
rtapi_arg_get_int(rtapi_integer *var, char *key)
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

rtapi_result
rtapi_string_to_integer(const char *str, rtapi_integer *var)
{
  int i;
  char *endptr;

  i = (int) strtol((char *) str, &endptr, 0);
  if (endptr == str ||
      (! isspace(*endptr) && 0 != *endptr)) {
    *var = 0;
    return RTAPI_ERROR;
  }

  *var = (rtapi_integer) i;
  return RTAPI_OK;
}

const char *rtapi_string_skipwhite(const char *str)
{
  const char *ptr = str;

  while (isspace(*ptr)) ptr++;

  return ptr;
}

const char *rtapi_string_skipnonwhite(const char *str)
{
  const char *ptr = str;

  while (! isspace(*ptr) && *ptr != 0) ptr++;

  return ptr;
}

const char *rtapi_string_skipone(const char *str)
{
  const char *ptr;

  ptr = rtapi_string_skipnonwhite(str);
  return rtapi_string_skipwhite(ptr);
}

char *rtapi_string_copyone(char *dst, const char *src)
{
  char *dstptr;
  const char *srcptr;

  if (NULL == dst || NULL == src) return dst;

  dstptr = dst, srcptr = rtapi_string_skipwhite(src);
  while (!isspace(*srcptr) && 0 != *srcptr) {
    *dstptr++ = *srcptr++;
  }
  *dstptr = 0;

  return dst;
}

rtapi_integer rtapi_socket_client(rtapi_integer port,
				  const char *hostname)
{
  return ulapi_socket_get_client_id(port, hostname);
}

rtapi_integer rtapi_socket_server(rtapi_integer port)
{
  return ulapi_socket_get_server_id(port);
}

rtapi_integer rtapi_socket_get_client(rtapi_integer socket_fd)
{
  return ulapi_socket_get_connection_id(socket_fd);
}

rtapi_result rtapi_socket_set_nonblocking(rtapi_integer fd)
{
  return ulapi_socket_set_nonblocking(fd);
}

rtapi_result rtapi_socket_set_blocking(rtapi_integer fd)
{
  return ulapi_socket_set_blocking(fd);
}

rtapi_integer rtapi_socket_read(rtapi_integer id,
				char *buf,
				rtapi_integer len)
{
  return ulapi_socket_read(id, buf, len);
}

rtapi_integer rtapi_socket_write(rtapi_integer id,
				 const char *buf,
				 rtapi_integer len)
{
  return ulapi_socket_write(id, buf, len);
}

rtapi_result rtapi_socket_close(rtapi_integer id)
{
  return ulapi_socket_close(id);
}

/* Serial interface */

void *rtapi_serial_new(void)
{
  return ulapi_serial_new();
}

rtapi_result rtapi_serial_delete(void *id)
{
  return ulapi_serial_delete(id);
}

rtapi_result rtapi_serial_open(const char *port, void *id)
{
  return ulapi_serial_open(port, id);
}

rtapi_result rtapi_serial_baud(void *id, int baud)
{
  return ulapi_serial_baud(id, baud);
}

rtapi_result rtapi_serial_set_nonblocking(void *id)
{
  return ulapi_serial_set_nonblocking(id);
}

rtapi_result rtapi_serial_set_blocking(void *id)
{
  return ulapi_serial_set_blocking(id);
}

rtapi_integer rtapi_serial_read(void *id,
				char *buf,
				rtapi_integer len)
{
  return ulapi_serial_read(id, buf, len);
}

rtapi_integer rtapi_serial_write(void *id,
				 const char *buf,
				 rtapi_integer len)
{
  return ulapi_serial_write(id, buf, len);
}

rtapi_result rtapi_serial_close(void *id)
{
  return ulapi_serial_close(id);
}
