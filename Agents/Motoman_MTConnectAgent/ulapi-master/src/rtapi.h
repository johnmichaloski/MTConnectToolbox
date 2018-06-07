/*!
  \file rtapi.h

  \brief Declarations of the real-time application programming interface,
  RTAPI, for portable access to creating and manipulating tasks and
  inter-task communication and synchronization.
*/

/*!
  \defgroup RTAPI Real-Time API

  The Real-Time Application Programming Interface (RTAPI) provides a
  portable interface to functions that create and manipulate real-time
  tasks and associated inter-task communication and synchronization.
  The RTAPI is the real-time counterpart to the \ref ULAPI.

  RTAPI tasks are assumed to share an address space, such as thread
  in a single process or tasks in a real-time operating system.

  Shared objects, such as mutexes, semaphores, FIFOs and other
  inter-task communication mechanisms, need a key for creation but are
  referenced by the object returned by their creation. In some cases
  (like POSIX threads), the key is unnecessary but is included in the
  API in case a particular implementation requires them. For example,
  if an implementation doesn't support POSIX threads, semaphores
  requiring an identifying key can be substituted and the calling API
  won't need to change.
*/

#ifndef RTAPI_H
#define RTAPI_H

#ifdef __cplusplus
extern "C" {
#if 0
} /* just to match one above, for indenters */
#endif
#endif

/* make sure we have enough string space to print numbers as strings */
#ifndef DIGITS_IN
#define DIGITS_IN(x) (sizeof(x) * 3 + 1)
#endif

#if defined(RTAPI_RESULT_CHAR)
typedef char rtapi_result;
#else
typedef int rtapi_result;
#endif

#if defined(RTAPI_PRIO_CHAR)
typedef char rtapi_prio;
#else
typedef int rtapi_prio;
#endif

#if defined(RTAPI_INTEGER_LONG)
typedef long int rtapi_integer;
#else
typedef int rtapi_integer;
#endif

#if defined(RTAPI_ID_LONG)
typedef long int rtapi_id;
#else
typedef int rtapi_id;
#endif

#if defined(RTAPI_FLAG_INT)
typedef int rtapi_flag;
#else
typedef char rtapi_flag;
#endif

enum {
  RTAPI_OK = 0,
  RTAPI_ERROR,
  RTAPI_IMPL_ERROR,
  RTAPI_BAD_ARGS
};

/*
  As with ULAPI, priorities range from rtapi_prio_lowest() to
  rtapi_prio_highest(), inclusive. To use this API, use one of two
  methods:

  Set your lowest priority task to rtapi_prio_lowest(), and for each
  task of the next lowest priority, set their priorities to
  rtapi_prio_next_higher(previous).

  Or,

  Set your highest priority task to rtapi_prio_highest(), and for each
  task of the next highest priority, set their priorities to
  rtapi_prio_next_lower(previous).
*/

extern char *rtapi_strncpy(char *dest, const char *src, rtapi_integer n);

extern rtapi_prio rtapi_prio_highest(void);
extern rtapi_prio rtapi_prio_lowest(void);
extern rtapi_prio rtapi_prio_next_higher(rtapi_prio prio);
extern rtapi_prio rtapi_prio_next_lower(rtapi_prio prio);

/*! Sets the clock base period to the value provided in nanoseconds. */
extern rtapi_result rtapi_clock_set_period(rtapi_integer nsecs);

/*! Global variable that holds the clock period */
extern rtapi_integer rtapi_clock_period;

extern rtapi_result rtapi_clock_get_time(rtapi_integer *secs,
					 rtapi_integer *nsecs);

extern rtapi_result rtapi_clock_get_interval(rtapi_integer start_secs,
					     rtapi_integer start_nsecs,
					     rtapi_integer end_secs,
					     rtapi_integer end_nsecs,
					     rtapi_integer *diff_secs,
					     rtapi_integer *diff_nsecs);

/*!
  Any task can call this to get the whole process to exit.
*/
extern void rtapi_exit(void);

#ifdef WIN32

#include "ulapi.h"	/* ulapi_task_struct */
typedef ulapi_task_struct rtapi_task_struct;
typedef ulapi_mutex_struct rtapi_mutex_struct;

#else

#ifdef HAVE_RTAI

#include <rtai_sched.h>		/* RT_TASK */

typedef RT_TASK rtapi_task_struct;

#else

#include <pthread.h>		/* pthread_t */

typedef pthread_t rtapi_task_struct;
typedef pthread_mutex_t rtapi_mutex_struct;

#endif

#endif

/*!
  Allocates space for a platform-specific data structure that holds
  the task information. Pass this to the \a rtapi_task_ functions.
*/
extern rtapi_result rtapi_task_init(rtapi_task_struct *task);
extern rtapi_task_struct *rtapi_task_new(void);
extern rtapi_result rtapi_task_clear(rtapi_task_struct *task);
extern rtapi_result rtapi_task_delete(rtapi_task_struct *task);

typedef void (*rtapi_task_code)(void *code);

extern rtapi_result rtapi_task_start(rtapi_task_struct *task,
				     void (*taskcode)(void *),
				     void *taskarg,
				     rtapi_prio prio,
				     rtapi_integer stacksize,
				     rtapi_integer period_nsec,
				     rtapi_flag uses_fp);
extern rtapi_result rtapi_task_stop(rtapi_task_struct *task);
extern rtapi_result rtapi_task_pause(rtapi_task_struct *task);
extern rtapi_result rtapi_task_resume(rtapi_task_struct *task);
extern rtapi_result rtapi_task_set_period(rtapi_task_struct *task,
					  rtapi_integer period_nsec);

/*!
  Checks the integrity of the stack by looking for a well-known
  pattern, 0xDEADBEEF, to which the stack was initialized on startup.
  Returns a positive number of stack words (typically integer size) if
  there is still stack to spare, 0 if the stack was overrwritten, or
  -1 if the stack check is irrelevant on this platform.
 */
extern rtapi_integer rtapi_task_stack_check(rtapi_task_struct *task);

extern rtapi_result rtapi_self_set_period(rtapi_integer period_nsec);
extern rtapi_result rtapi_wait(rtapi_integer period_nsec);
extern rtapi_result rtapi_task_exit(void);

/*!
  Allocates space for a platform-specific data structure that holds
  the shared memory configuration. Pass this to \a rtapi_shm_addr
  to get a pointer to the actual shared memory.
*/
extern void *rtapi_shm_new(rtapi_id key, rtapi_integer size);
/*!
  Returns a pointer to the actual shared memory, given a shared memory
  data structure previously created with \a rtapi_shm_new.
 */
extern void *rtapi_shm_addr(void *shm);
/*!
  Deletes shared memory previously allocated with \a rtapi_shm_new.
 */
extern rtapi_result rtapi_shm_delete(void *shm);

extern void *rtapi_rtm_new(rtapi_id key, rtapi_integer size);
extern void *rtapi_rtm_addr(void *shm);
extern rtapi_result rtapi_rtm_delete(void *shm);

extern void rtapi_print(const char *fmt, ...);

extern void rtapi_outb(char byte, rtapi_id port);
extern char rtapi_inb(rtapi_id port);

extern rtapi_result rtapi_interrupt_assign_handler(rtapi_id irq,
						   void (*handler) (void));
extern rtapi_result rtapi_interrupt_free_handler(rtapi_id irq);
extern rtapi_result rtapi_interrupt_enable(rtapi_id irq);
extern rtapi_result rtapi_interrupt_disable(rtapi_id irq);

extern rtapi_result rtapi_mutex_init(rtapi_mutex_struct *mutex, rtapi_id key);
/*!
  Returns a pointer to an implementation-defined structure that
  is passed to the other mutex functions, or NULL if no mutex can
  be created.
*/
extern rtapi_mutex_struct *rtapi_mutex_new(rtapi_id key);

extern rtapi_result rtapi_mutex_clear(rtapi_mutex_struct *mutex);

/*! Deletes the mutex. */
extern rtapi_result rtapi_mutex_delete(rtapi_mutex_struct *mutex);

/*! Releases the mutex, signifying that the associated shared resource 
  is now free for another task to take. */
extern rtapi_result rtapi_mutex_give(rtapi_mutex_struct *mutex);

/*! Takes the mutex, signifying that the associated shared resource
  will now be used by the task. If the mutex is already taken, this
  blocks the caller until the mutex is given. */
extern rtapi_result rtapi_mutex_take(rtapi_mutex_struct *mutex);

extern void *rtapi_sem_new(rtapi_id key);
extern rtapi_result rtapi_sem_delete(void *sem);
extern rtapi_result rtapi_sem_give(void *sem);
extern rtapi_result rtapi_sem_take(void *sem);

extern void *rtapi_new(rtapi_integer size);
extern void rtapi_free(void *ptr);

extern rtapi_result rtapi_string_to_integer(const char *str, rtapi_integer *var);

extern const char *rtapi_string_skipwhite(const char *str);
extern const char *rtapi_string_skipnonwhite(const char *str);
extern const char *rtapi_string_skipone(const char *str);
extern char *rtapi_string_copyone(char *dst, const char *src);

/*
  FIXME -- switch to the newer interface, e.g., 

  rtapi_socket_get_server_id_on_interface
  rtapi_socket_get_client_id.

  This will impact the Galil code. 
*/

/*!
  Connects as a client to the socket server on \a port and \a host.
  Returns the integer socket descriptor for later sends and receives.
*/
extern rtapi_integer rtapi_socket_client(rtapi_integer port, const char *host);

/*!
  Creates a server connection to the \a port. Returns the integer
  socket descriptor for later use in \a
  rtapi_socket_get_client_connection.
*/
extern rtapi_integer rtapi_socket_server(rtapi_integer port);

/*!
  Gets a connection from a client. Returns the integer socket descriptor
  for later sends and receives.
*/
extern rtapi_integer rtapi_socket_get_client(rtapi_integer id);

/*!
  Sets the socket to be nonblocking.
*/
extern rtapi_result rtapi_socket_set_nonblocking(rtapi_integer id);

/*!
  Sets the socket to be blocking.
*/
extern rtapi_result rtapi_socket_set_blocking(rtapi_integer id);

/*!
  Reads up to \a len bytes from socket \a id into \a buf. Returns the
  number of bytes read, or -1 on error.
*/
extern rtapi_integer rtapi_socket_read(rtapi_integer id, char *buf, rtapi_integer len);

/*!
  Writes \a len bytes from \a buf to socket \a id. Returns the
  number of bytes written, or -1 on error.
*/
extern rtapi_integer rtapi_socket_write(rtapi_integer id, const char *buf, rtapi_integer len);

/*!
  Closes the socket id, whether that for a client, for a server, or
  to a client.
*/
extern rtapi_result rtapi_socket_close(rtapi_integer id);

/*
  Serial communication API
*/

/*! Allocates space for a serial port identifier, and returns its pointer. */
extern void *rtapi_serial_new(void);

/*! Deallocates a previously allocated serial port identifier. */
extern rtapi_result rtapi_serial_delete(void *id);

/*!
  Opens a connection to a serial port. Fills in the identifier and
  returns RTAPI_OK if it worked, otherwise returns RTAPI_ERROR.
 */
extern rtapi_result rtapi_serial_open(const char *port, void *id);

extern rtapi_result rtapi_serial_baud(void *id, int baud);

/*!
  Sets the serial port descriptor \a id to be blocking or nonblocking.
*/
extern rtapi_result rtapi_serial_set_nonblocking(void *id);

extern rtapi_result rtapi_serial_set_blocking(void *id);

/*!
  Reads up to \a len bytes from serial port descriptor \a id into \a
  buf. Returns the number of bytes read, or -1 on error.
 */
extern rtapi_integer rtapi_serial_read(void *id, char *buf, rtapi_integer len);

/*!
  Writes \a len bytes from \a buf to serial port descriptor \a
  id. Returns the number of bytes written, or -1 on error.
 */
extern rtapi_integer rtapi_serial_write(void *id, const char *buf, rtapi_integer len);

/*!
  Closes the serial port descriptor.
 */
extern rtapi_result rtapi_serial_close(void *id);

#ifdef WIN32
#define rtapi_snprintf sprintf_s
#define rtapi_strcasecmp stricmp
#else
#define rtapi_snprintf snprintf
#define rtapi_strcasecmp strcasecmp
#endif

/*!
  Intended for real-time controllers that execute in normal user space.
  Executes 'prog'. Returns RTAPI_OK if it was able to execute, placing
  the program return value in 'result'. Otherwise, returns ULAPI_ERROR
  and leaves 'result' alone.
*/
extern rtapi_result rtapi_system(const char *prog, rtapi_integer *result);

#ifdef __cplusplus
#if 0
{			  /* just to match one below, for indenters */
#endif
}
#endif

#endif /* RTAPI_H */
