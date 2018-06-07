/*
  rtai_rtapi.c

  Implementations of RT API functions declared in rtapi.h, for RTAI.
  These are called by RTAI realtime controllers.
*/

#ifndef KERNEL_VERSION
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif

#include <stddef.h>		/* NULL, sizeof */
#include <stdarg.h>		/* va_* */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/time.h>		/* struct timespec */
#include <linux/ctype.h>

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)
#include <linux/slab.h>		/* kmalloc(), kfree(), GFP_USER */
#else
#include <linux/malloc.h>	/* kmalloc(), kfree(), GFP_USER */
#endif

#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_shm.h>
#include <rtai_sem.h>		/* SEM, rt_sem_init() */

/* get inb(), outb(), ioperm() */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,4,17)
#include <asm/io.h>
#else
#include <sys/io.h>
#endif

#include "rtapi.h"		/* these decls */

#define CHECK_STACK

/* declare vsprintf() explicitly instead of including all of <stdio.h>,
   since we don't need all of it and it may cause problems */
extern int vsprintf(char *s, const char *format, va_list arg);

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
EXPORT_SYMBOL(rtapi_strncpy);

/* can't do this in a real real-time system */
rtapi_result rtapi_system(const char *prog, rtapi_integer *result)
{
  *resuit = 0;
  return RTAPI_ERROR;
}
EXPORT_SYMBOL(rtapi_system);

rtapi_prio rtapi_prio_highest(void)
{
  return 0;
}
EXPORT_SYMBOL(rtapi_prio_highest);

rtapi_prio rtapi_prio_lowest(void)
{
  return RT_SCHED_LOWEST_PRIORITY;
}
EXPORT_SYMBOL(rtapi_prio_lowest);

rtapi_prio rtapi_prio_next_higher(rtapi_prio prio)
{
  if (prio == rtapi_prio_highest())
    return prio;

  return prio - 1;
}
EXPORT_SYMBOL(rtapi_prio_next_higher);

rtapi_prio rtapi_prio_next_lower(rtapi_prio prio)
{
  if (prio == rtapi_prio_lowest())
    return prio;

  return prio + 1;
}
EXPORT_SYMBOL(rtapi_prio_next_lower);

rtapi_result rtapi_clock_set_period(rtapi_integer nsecs)
{
  stop_rt_timer();
  rt_set_periodic_mode();
  start_rt_timer(nano2count((RTIME) nsecs));

  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_clock_set_period);

rtapi_result rtapi_clock_get_time(rtapi_integer *secs, rtapi_integer *nsecs)
{
  struct timespec ts;

  count2timespec(rt_get_time(), &ts);
  *secs = (rtapi_integer) ts.tv_sec;
  *nsecs = (rtapi_integer) ts.tv_nsec;

  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_clock_get_time);

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
EXPORT_SYMBOL(rtapi_clock_get_interval);

void *rtapi_task_new(void)
{
  return (void *) kmalloc(sizeof(RT_TASK), GFP_USER);
}
EXPORT_SYMBOL(rtapi_task_new);

rtapi_result rtapi_task_delete(void *task)
{
  if (0 == task) {
    return RTAPI_ERROR;
  }

  kfree(task);
  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_task_delete);

/*
  Initialize the stack with a recognizable pattern, one that will
  be looked for in rt_task_stack_check() to see how much was actually
  used.
*/

#ifdef CHECK_STACK

static int pattern = 0xDEADBEEF;

static int 
rt_task_stack_init(RT_TASK *task)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)
  long int *ptr;
#else
  int *ptr;
#endif

  /*
    Clobber the stack with a recognizable pattern. The bottom,
    task->stack_bottom, is writeable. The top, task->stack,
    points to some RTAI data and can't be written.
  */
  for (ptr = task->stack_bottom; ptr < task->stack; ptr++) {
    *ptr = pattern;
  }

  return 0;
}

/*
  Check the stack and see how much was used by comparing what's left
  with the initialization pattern.
*/
static int
rt_task_stack_check(RT_TASK *task)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)
  long int *ptr;
#else
  int *ptr;
#endif
  int unused;

  /*
    Read up from the bottom and count the unused bytes.
  */
  for (unused = 0, ptr = task->stack_bottom; ptr < task->stack; ptr++) {
    if (*ptr != pattern) {
      break;
    }
    unused += sizeof(int);
  }

  return unused;
}

#endif

rtapi_integer rtapi_task_stack_check(void *task)
{
#ifdef CHECK_STACK
  return (rtapi_integer) rt_task_stack_check((RT_TASK *) task);
#else
  return 0;
#endif
}
EXPORT_SYMBOL(rtapi_task_stack_check);

rtapi_result rtapi_task_start(void *task,
			      void (*taskcode)(void *),
			      void *taskarg,
			      rtapi_prio prio,
			      rtapi_integer stacksize,
			      rtapi_integer period_nsec, rtapi_flag uses_fp)
{
  int retval;

  retval = rt_task_init((RT_TASK *) task, taskcode, (int) taskarg, stacksize, prio, 1, 0);
  if (0 != retval) {
    return RTAPI_ERROR;
  }

#ifdef CHECK_STACK
  /* initialize the stack with a recognizable pattern */
  (void) rt_task_stack_init((RT_TASK *) task);
#endif

  retval = rt_task_use_fpu((RT_TASK *) task, (int) uses_fp);
  if (0 != retval) {
    return RTAPI_ERROR;
  }

  if (period_nsec > 0) {
    retval = rt_task_make_periodic((RT_TASK *) task, rt_get_time(),
				   nano2count((RTIME) period_nsec));
  } else {
    retval = rt_task_resume((RT_TASK *) task);
  }

  return (retval == 0 ? RTAPI_OK : RTAPI_ERROR);
}
EXPORT_SYMBOL(rtapi_task_start);

rtapi_result rtapi_task_stop(void *task)
{
  return (rt_task_delete((RT_TASK *) task) == 0 ? RTAPI_OK : RTAPI_ERROR);
}
EXPORT_SYMBOL(rtapi_task_stop);

rtapi_result rtapi_task_pause(void *task)
{
  return (rt_task_suspend((RT_TASK *) task) == 0 ? RTAPI_OK : RTAPI_ERROR);
}
EXPORT_SYMBOL(rtapi_task_pause);

rtapi_result rtapi_task_resume(void *task)
{
  return (rt_task_resume((RT_TASK *) task) == 0 ? RTAPI_OK : RTAPI_ERROR);
}
EXPORT_SYMBOL(rtapi_task_resume);

rtapi_result rtapi_task_set_period(void *task, rtapi_integer period_nsec)
{
  return (rt_task_make_periodic((RT_TASK *) task, rt_get_time(),
				nano2count((RTIME) period_nsec)) ==
	  0 ? RTAPI_OK : RTAPI_ERROR);
}
EXPORT_SYMBOL(rtapi_task_set_period);

rtapi_result rtapi_task_init(void)
{
  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_task_init);

rtapi_result rtapi_self_set_period(rtapi_integer period_nsec)
{
  return (rt_task_make_periodic(rt_whoami(), rt_get_time(),
				nano2count((RTIME) period_nsec)) ==
	  0 ? RTAPI_OK : RTAPI_ERROR);
}
EXPORT_SYMBOL(rtapi_self_set_period);

rtapi_result rtapi_wait(rtapi_integer period_nsec)
{
  rt_task_wait_period();

  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_wait);

rtapi_result rtapi_task_exit(void)
{
  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_task_exit);

typedef struct {
  rtapi_id key;
  void *addr;
} rtai_shm_struct;

/*
  The rtapi_shm_xxx functions are to create shared memory between
  real-time proceses only. This is the same as for sharing RT memory
  up to user-level processes.

  The rtapi_rtm_xxx functions are to create shared memory up to
  user-level processes. These are implemented as calls to the
  rtapi_shm_xxx functions.
*/

void *rtapi_shm_new(rtapi_id key, rtapi_integer size)
{
  rtai_shm_struct *shm;

  shm = (rtai_shm_struct *) kmalloc(sizeof(rtai_shm_struct), GFP_USER);
  if (NULL == shm) return NULL;

  shm->addr = rtai_kmalloc(key, size);
  if (NULL == shm->addr) {
    kfree(shm);
    return NULL;
  }
  shm->key = key;
  
  return (void *) shm;
}
EXPORT_SYMBOL(rtapi_shm_new);

void *rtapi_shm_addr(void *shm)
{
  return ((rtai_shm_struct *) shm)->addr;
}
EXPORT_SYMBOL(rtapi_shm_addr);

rtapi_result rtapi_shm_delete(void *shm)
{
  if (NULL == shm) return RTAPI_ERROR;
  rtai_kfree(((rtai_shm_struct *) shm)->key);
  kfree(shm);
  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_shm_delete);

void *rtapi_rtm_new(rtapi_id key, rtapi_integer size)
{
  return rtapi_shm_new(key, size);
}
EXPORT_SYMBOL(rtapi_rtm_new);

void *rtapi_rtm_addr(void *rtm)
{
  return rtapi_shm_addr(rtm);
}
EXPORT_SYMBOL(rtapi_rtm_addr);

rtapi_result rtapi_shm_delete(void *rtm)
{
  return rtapi_shm_delete(rtm);
}
EXPORT_SYMBOL(rtapi_rtm_delete);

void rtapi_print(const char *fmt, ...)
{
  va_list args;
  enum { BUFFERLEN = 256 };
  static char buffer[BUFFERLEN];
  /* the call to vsprintf() will put arbitrarily many characters into
     'buffer', and we can't easily check how many beforehand. This may
     cause a buffer overflow. The correct way to do this is as in 
     Exercise 7-3 in K&R's _C Language, 2nd Ed_ book */
  va_start(args, fmt);
  vsprintf(buffer, fmt, args);
  rt_printk(buffer);
  va_end(args);
}
EXPORT_SYMBOL(rtapi_print);

void rtapi_outb(char byte, rtapi_id port)
{
  outb(byte, port);
}
EXPORT_SYMBOL(rtapi_outb);

char rtapi_inb(rtapi_id port)
{
  return inb(port);
}
EXPORT_SYMBOL(rtapi_inb);

rtapi_result rtapi_interrupt_assign_handler(rtapi_id irq,
					    void (*handler) (void))
{
  return (rt_request_global_irq(irq, handler) == 0 ? RTAPI_OK : RTAPI_ERROR);
}
EXPORT_SYMBOL(rtapi_interrupt_assign_handler);

rtapi_result rtapi_interrupt_free_handler(rtapi_id irq)
{
  rt_shutdown_irq(irq);

  return (rt_free_global_irq(irq) == 0 ? RTAPI_OK : RTAPI_ERROR);
}
EXPORT_SYMBOL(rtapi_interrupt_free_handler);

rtapi_result rtapi_interrupt_enable(rtapi_id irq)
{
  rt_startup_irq(irq);

  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_interrupt_enable);

rtapi_result rtapi_interrupt_disable(rtapi_id irq)
{
  rt_shutdown_irq(irq);

  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_interrupt_disable);

void *rtapi_mutex_new(rtapi_id key)
{
  SEM *semid;

  semid = (SEM *) kmalloc(sizeof(SEM), GFP_USER);
  if (NULL == semid) return NULL;

  rt_sem_init(semid, 1);	/* 1 = initially given */
  return (void *) semid;
}
EXPORT_SYMBOL(rtapi_mutex_new);

rtapi_result rtapi_mutex_delete(void *sem)
{
  if (NULL == sem) return RTAPI_ERROR;
  kfree(sem);
  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_mutex_delete);

rtapi_result rtapi_mutex_give(void *sem)
{
  rt_sem_signal(sem);

  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_mutex_give);

rtapi_result rtapi_mutex_take(void *sem)
{
  rt_sem_wait(sem);

  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_mutex_take);

void *rtapi_sem_new(rtapi_id key)
{
  return rtapi_mutex_new(key);
}
EXPORT_SYMBOL(rtapi_sem_new);

rtapi_result rtapi_sem_delete(void *sem)
{
  return rtapi_mutex_delete(sem);
}
EXPORT_SYMBOL(rtapi_sem_delete);

rtapi_result rtapi_sem_give(void *sem)
{
  return rtapi_mutex_give(sem);
}
EXPORT_SYMBOL(rtapi_sem_give);

rtapi_result rtapi_sem_take(void *sem)
{
  return rtapi_mutex_take(sem);
}
EXPORT_SYMBOL(rtapi_sem_take);

/*
  The following are from rtapi_app.h, which we don't include here
  since it's only to be included by the main translation unit.
*/
rtapi_result
rtapi_app_init(void)
{
  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_app_init);

/*
  We need not do anything to keep tasks alive waiting for termination.
  Insmod will return and the tasks will stay alive until an rmmod.
*/
rtapi_result
rtapi_app_wait(void)
{
  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_app_wait);

void
rtapi_exit(void)
{
  return RTAPI_OK;	       /* ignore, since rmmod does the exit */
}
EXPORT_SYMBOL(rtapi_exit);

void *
rtapi_new(rtapi_integer size)
{
  return (void *) kmalloc(size, GFP_USER);
}
EXPORT_SYMBOL(rtapi_new);

void 
rtapi_free(void *ptr)
{
  kfree(ptr);
}
EXPORT_SYMBOL(rtapi_free);

/*
  insmod will have already set these variables, so just leave them alone
*/

char *
rtapi_arg_get_string(char ** var, char *key)
{
  return *var;
}
EXPORT_SYMBOL(rtapi_arg_get_string);

int
rtapi_arg_get_int(rtapi_integer *var, char *key)
{
  return *var;
}
EXPORT_SYMBOL(rtapi_arg_get_int);

rtapi_result
rtapi_string_to_integer(const char *str, rtapi_integer *var)
{
  int i;
  const char *endptr;

  i = (int) simple_strtol(str, &endptr, 0);
  if (endptr == str ||
      (! isspace(*endptr) && 0 != *endptr)) {
    *var = 0;
    return RTAPI_ERROR;
  }

  *var = (rtapi_integer) i;
  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_string_to_integer);

const char *
rtapi_string_skipwhite(const char *str)
{
  const char *ptr = str;

  while (isspace(*ptr)) ptr++;

  return ptr;
}
EXPORT_SYMBOL(rtapi_string_skipwhite);

const char *
rtapi_string_skipnonwhite(const char *str)
{
  const char *ptr = str;

  while (! isspace(*ptr) && *ptr != 0) ptr++;

  return ptr;
}
EXPORT_SYMBOL(rtapi_string_skipnonwhite);

const char *
rtapi_string_skipone(const char *str)
{
  const char *ptr;

  ptr = rtapi_string_skipnonwhite(str);
  return rtapi_string_skipwhite(ptr);
}
EXPORT_SYMBOL(rtapi_string_skipone);

char *
rtapi_string_copyone(char *dst, const char *src)
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
EXPORT_SYMBOL(rtapi_string_copyone);

/* we don't have RTAI sockets, so these will all return 'unimplemented' */

rtapi_integer
rtapi_socket_client(rtapi_integer port, const char *host)
{
  return -1;
}
EXPORT_SYMBOL(rtapi_socket_client);

rtapi_integer
rtapi_socket_server(rtapi_integer port)
{
  return -1;
}
EXPORT_SYMBOL(rtapi_socket_server);

rtapi_integer
rtapi_socket_get_client(rtapi_integer id)
{
  return -1;
}
EXPORT_SYMBOL(rtapi_socket_get_client);

rtapi_result
rtapi_socket_set_nonblocking(rtapi_integer id)
{
  return RTAPI_ERROR;
}
EXPORT_SYMBOL(rtapi_socket_set_nonblocking);

rtapi_result
rtapi_socket_set_blocking(rtapi_integer id)
{
  return RTAPI_ERROR;
}
EXPORT_SYMBOL(rtapi_socket_set_blocking);

rtapi_integer
rtapi_socket_read(rtapi_integer id, char *buf, rtapi_integer len)
{
  return -1;
}
EXPORT_SYMBOL(rtapi_socket_read);

rtapi_integer
rtapi_socket_write(rtapi_integer id, const char *buf, rtapi_integer len)
{
  return -1;
}
EXPORT_SYMBOL(rtapi_socket_write);

rtapi_result
rtapi_socket_close(rtapi_integer id)
{
  return RTAPI_ERROR;
}
EXPORT_SYMBOL(rtapi_socket_close);

/*
  At the moment, we don't have RTAI serial ports, so these all return
  errors. There is an RT serial comm interface we should use one day.
*/

static int rtapi_serial_id = -1; /* dummy id */

void *
rtapi_serial_new(void)
{
  return &rtapi_serial_id;
}
EXPORT_SYMBOL(rtapi_serial_new);

rtapi_result
rtapi_serial_delete(void *id)
{
  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_serial_delete);

rtapi_result
rtapi_serial_open(const char *port, void *id)
{
  return RTAPI_ERROR;
}
EXPORT_SYMBOL(rtapi_serial_open);

rtapi_result
rtapi_serial_baud(void *id, int baud)
{
  return RTAPI_ERROR;
}
EXPORT_SYMBOL(rtapi_serial_baud);

rtapi_result
rtapi_serial_set_nonblocking(void *id)
{
  return RTAPI_ERROR;
}
EXPORT_SYMBOL(rtapi_serial_set_nonblocking);

rtapi_result
rtapi_serial_set_blocking(void *id)
{
  return RTAPI_ERROR;
}
EXPORT_SYMBOL(rtapi_serial_set_blocking);

rtapi_integer
rtapi_serial_read(void *id,
		  char *buf,
		  rtapi_integer len)
{
  return -1;
}
EXPORT_SYMBOL(rtapi_serial_read);

rtapi_integer
rtapi_serial_write(void *id,
		   const char *buf,
		   rtapi_integer len)
{
  return -1;
}
EXPORT_SYMBOL(rtapi_serial_write);

rtapi_result
rtapi_serial_close(void *id)
{
  return RTAPI_OK;
}
EXPORT_SYMBOL(rtapi_serial_close);
