/*!
  \file ulapi.h

  \brief Declarations of the user-level application programming interface,
  ULAPI, for portable communication between user-level processes and 
  real-time tasks conforming to the \ref RTAPI real-time API.
*/

/*!
  \defgroup ULAPI User-Level API

  The User-Level Application Programming Interface (ULAPI) provides a
  portable interface for communicating with real-time tasks conforming
  to the \ref RTAPI real-time API.
*/

#ifndef ULAPI_H
#define ULAPI_H

#include <stdio.h>		/* printf */
#include <string.h>		/* stricmp, strcasecmp */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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

#if defined(ULAPI_RESULT_CHAR)
typedef char ulapi_result;
#else
typedef int ulapi_result;
#endif

#if defined(ULAPI_INTEGER_LONG)
typedef long int ulapi_integer;
#else
typedef int ulapi_integer;
#endif

#if defined(ULAPI_ID_LONG)
typedef long int ulapi_id;
#else
typedef int ulapi_id;
#endif

#if defined(ULAPI_FLAG_INT)
typedef int ulapi_flag;
#else
typedef char ulapi_flag;
#endif

#if defined(ULAPI_REAL_FLOAT)
typedef float ulapi_real;
#elif defined(ULAPI_REAL_LONG_DOUBLE)
typedef long double ulapi_real;
#else
typedef double ulapi_real;
#endif

/*! Returns a real-valued number of seconds wiith respect
  to some arbitrary origin that remains constant for the life of the
  program. */
extern ulapi_real ulapi_time(void);

/*! Puts the calling thread to sleep for a period of \a secs seconds. */
extern void ulapi_sleep(ulapi_real secs);

/*! Puts the application to sleep indefinitely until a signal */
extern ulapi_result ulapi_app_wait(void);

enum {
  ULAPI_OK = 0,
  ULAPI_ERROR,
  ULAPI_IMPL_ERROR,
  ULAPI_BAD_ARGS
};

/*!
  Call this once before any other ULAPI functions, passing
  UL_USE_DEFAULT for the default ULAPI implementation, or one of the
  platform-specific values like UL_USE_UNIX or UL_USE_RTAI to force an
  implementation, if available. Returns ULAPI_OK if successful,
  otherwise ULAPI_RESULT_IMPL_ERROR if the implementation specified by
  \a sel is not available, or ULAPI_RESULT_BAD_ARGS if \a sel is not
  recognized.
*/
extern ulapi_result ulapi_init(void);
extern ulapi_result ulapi_exit(void);

extern ulapi_integer ulapi_to_argv(const char *str, char ***argv);
extern void ulapi_free_argv(ulapi_integer argc, char **argv);

/*!
  Set and get the debug level.
*/
enum {ULAPI_DEBUG_WARN = 0x01,
      ULAPI_DEBUG_ERROR = 0x02,
      ULAPI_DEBUG_INFO = 0x04,
      ULAPI_DEBUG_ALL = 0xFF};

extern void ulapi_set_debug(ulapi_integer mask);

/*!
  As with RTAPI, priorities range from ulapi_prio_lowest() to
  ulapi_prio_highest(), inclusive. To use this API, use one of two
  methods:

  Set your lowest priority task to ulapi_prio_lowest(), and for each
  task of the next lowest priority, set their priorities to
  ulapi_prio_next_higher(previous).

  Or,

  Set your highest priority task to ulapi_prio_highest(), and for each
  task of the next highest priority, set their priorities to
  ulapi_prio_next_lower(previous).
*/

#if defined(ULAPI_PRIO_CHAR)
typedef char ulapi_prio;
#else
typedef int ulapi_prio;
#endif

extern ulapi_prio ulapi_prio_highest(void);
extern ulapi_prio ulapi_prio_lowest(void);
extern ulapi_prio ulapi_prio_next_higher(ulapi_prio prio);
extern ulapi_prio ulapi_prio_next_lower(ulapi_prio prio);

/*!
  Allocates space for a platform-specific data structure that holds
  the task information. Pass this to the \a ulapi_task_ functions.
*/

#ifdef WIN32

#include <windows.h>

typedef struct {
  HANDLE hThread;
  DWORD dwThreadId;
} ulapi_task_struct;

typedef struct {
  HANDLE hMutex;
} ulapi_mutex_struct;

#else

#include <pthread.h>		/* pthread_t */

typedef pthread_t ulapi_task_struct;

typedef pthread_mutex_t ulapi_mutex_struct;

#endif

extern ulapi_result ulapi_task_init(ulapi_task_struct *task);
extern ulapi_task_struct *ulapi_task_new(void);
extern ulapi_result ulapi_task_clear(ulapi_task_struct *);
extern ulapi_result ulapi_task_delete(ulapi_task_struct *task);

typedef void (*ulapi_task_code)(void *);

extern ulapi_result ulapi_task_start(ulapi_task_struct *task,
				     void (*taskcode)(void *),
				     void *taskarg,
				     ulapi_prio prio,
				     ulapi_integer period_nsec);

extern ulapi_result ulapi_task_stop(ulapi_task_struct *);
extern ulapi_result ulapi_task_pause(ulapi_task_struct *);
extern ulapi_result ulapi_task_resume(ulapi_task_struct *);
extern ulapi_result ulapi_task_set_period(ulapi_task_struct *, ulapi_integer period_nsec);
extern ulapi_result ulapi_self_set_period(ulapi_integer period_nsec);
extern ulapi_result ulapi_wait(ulapi_integer period_nsec);

/*!
  Terminates the calling task, saving \a retval for later reference by
  a task that may call \a ulapi_task_join.
*/
extern void ulapi_task_exit(ulapi_integer retval);

/*!
  Suspends the caller until the \a task has exited, copying that task's
  exit value into \a retval if \a retval is not NULL.
*/
extern ulapi_result ulapi_task_join(ulapi_task_struct *, ulapi_integer *retval);
extern ulapi_integer ulapi_task_id(void);

/*!
  Allocates a new process handle.
*/
extern void *ulapi_process_new(void);
/*!
  Removes storage allocated for a process identifier or handle.
*/
extern ulapi_result ulapi_process_delete(void *proc);
/*!
  Executes the program and its args as in the \a path string.
  \a proc is the already-created handle from ulapi_process_new.
 */
extern ulapi_result ulapi_process_start(void *proc, char *path);
extern ulapi_result ulapi_process_stop(void *proc);
/*! Returns non-zero the process is done, otherwise returns zero */
extern ulapi_integer ulapi_process_done(void *proc, ulapi_integer *result);
extern ulapi_result ulapi_process_wait(void *proc, ulapi_integer *result);

/*! Initializes a mutex with the key provided. */
extern ulapi_result ulapi_mutex_init(ulapi_mutex_struct *mutex, ulapi_id key);

/*!
  Returns a pointer to an implementation-defined structure that
  is passed to the other mutex functions, or NULL if no mutex can
  be created.
*/
extern ulapi_mutex_struct *ulapi_mutex_new(ulapi_id key);

/*! Removes the resources associated with this mutex, but leaves the mutex
  valid for other tasks that may be using it. */
ulapi_result ulapi_mutex_clear(ulapi_mutex_struct *mutex);

/*! Deletes the mutex. */
extern ulapi_result ulapi_mutex_delete(ulapi_mutex_struct *mutex);

/*| Releases the mutex, signifying that the associated shared resource 
  is now free for another task to take. */
extern ulapi_result ulapi_mutex_give(ulapi_mutex_struct *mutex);

/*! Takes the mutex, signifying that the associated shared resource
  will now be used by the task. If the mutex is already taken, this
  blocks the caller until the mutex is given. */
extern ulapi_result ulapi_mutex_take(ulapi_mutex_struct *mutex);

extern void *ulapi_sem_new(ulapi_id key);
extern ulapi_result ulapi_sem_delete(void *sem);
extern ulapi_result ulapi_sem_give(void *sem);
extern ulapi_result ulapi_sem_take(void *sem);

/*!
  Returns a pointer to an implementation-defined structure that is
  passed to the other condition variable functions, or NULL if no
  condition variable can be created.
*/
extern void *ulapi_cond_new(ulapi_id key);

/*! Deletes the condition variable. */
extern ulapi_result ulapi_cond_delete(void *cond);

/*| Signals that the condition variable has reached its release value */
extern ulapi_result ulapi_cond_signal(void *cond);

/*| Broadcasts that the condition variable has reached its release value */
extern ulapi_result ulapi_cond_broadcast(void *cond);

/*! Waits until the condition variable has reached its release value */
extern ulapi_result ulapi_cond_wait(void *cond, void *mutex);

/*!
  Allocates space for a platform-specific data structure that holds
  the shared memory configuration. Pass this to \a ulapi_shm_addr
  to get a pointer to the actual shared memory.
*/
extern void *ulapi_shm_new(ulapi_id key, ulapi_integer size);
/*!
  Returns a pointer to the actual shared memory, given a shared memory
  data structure previously created with \a ulapi_shm_new.
 */
extern void *ulapi_shm_addr(void *shm);
/*!
  Deletes shared memory previously allocated with \a ulapi_shm_new.
 */
extern ulapi_result ulapi_shm_delete(void *shm);

/*!
  User-to-realtime shared memory.  Allocates space for a
  platform-specific data structure that holds the RT shared memory
  configuration. Pass this to \a ulapi_rtm_addr to get a pointer to
  the actual RT shared memory.
*/
extern void *ulapi_rtm_new(ulapi_id key, ulapi_integer size);

/*!
  Returns a pointer to the actual RT memory, given a RT memory
  data structure previously created with \a ulapi_shm_new.
 */
extern void *ulapi_rtm_addr(void *shm);
/*!
  Deletes RT memory previously allocated with \a ulapi_shm_new.
 */
extern ulapi_result ulapi_rtm_delete(void *shm);

/*!
  Connects as a client to the socket server on \a port and \a host.
  Returns the integer socket descriptor for later sends and receives.
*/
extern ulapi_integer ulapi_socket_get_client_id(ulapi_integer port, const char *host);
extern ulapi_integer ulapi_socket_get_client_id_on_interface(ulapi_integer port, const char *hostname, const char *intf);

/*!
  Creates a server connection to the \a port. Returns the integer
  socket descriptor for later use in \a
  ulapi_socket_get_client_connection.
*/
extern ulapi_integer ulapi_socket_get_server_id(ulapi_integer port);
/*! Equivalent to ulapi_socket_get_server_id but with a specified interface. */
extern ulapi_socket_get_server_id_on_interface(ulapi_integer port, const char *intf);

/*!
  Called by a server to gets a connection from a client. Returns the
  integer socket descriptor for later sends and receives.
*/
extern ulapi_integer ulapi_socket_get_connection_id(ulapi_integer id);

extern ulapi_result ulapi_getpeername(ulapi_integer id, char *ipstr, size_t iplen, ulapi_integer *port);

/*!
  Gets an fd for broadcast writing.
*/
extern ulapi_integer ulapi_socket_get_broadcaster_id(ulapi_integer port);
/*! Equivalent to ulapi_socket_get_broadcaster_id but with a specified interface. */
extern ulapi_integer ulapi_socket_get_broadcaster_id_on_interface(ulapi_integer port, const char *intf);

/*!
  Gets an fd for broadcast reading.
*/
extern ulapi_integer ulapi_socket_get_broadcastee_id(ulapi_integer port);
/*! Equivalent to ulapi_socket_get_broadcastee_id but with a specified interface. */
extern ulapi_integer ulapi_socket_get_broadcastee_id_on_interface(ulapi_integer port, const char *intf);

#define ULAPI_SOCKET_DEFAULT_MULTICAST_GROUP "224.0.0.1"

/*!
  Gets an fd for multicasting across the entire subnet.
 */
extern ulapi_integer ulapi_socket_get_multicaster_id(ulapi_integer port);
/*!
  Equivalent to ulapi_socket_get_multicaster_id but with a specified interface group.
 */
extern ulapi_integer ulapi_socket_get_multicaster_id_on_interface(ulapi_integer port, const char *intf);
/*!
  Gets an fd for multicast reading.
*/
extern ulapi_integer ulapi_socket_get_multicastee_id(ulapi_integer port);
/*!
  Equivalent to ulapi_socket_get_multicastee_id but with a specified interface group. */
extern ulapi_integer ulapi_socket_get_multicastee_id_on_interface(ulapi_integer port, const char *intf);

/*!
  Sets the socket to be blocking or nonblocking.
*/
extern ulapi_result ulapi_socket_set_nonblocking(ulapi_integer id);
extern ulapi_result ulapi_socket_set_blocking(ulapi_integer id);

extern char *ulapi_address_to_hostname(ulapi_integer address);
extern ulapi_integer ulapi_hostname_to_address(const char *hostname);
extern ulapi_integer ulapi_get_host_address(void);

/*!
  Reads up to \a len bytes from socket \a id into \a buf. Returns the
  number of bytes read, or -1 on error.
 */
extern ulapi_integer ulapi_socket_read(ulapi_integer id, char *buf, ulapi_integer len);

/*!
  Writes \a len bytes from \a buf to socket \a id. Returns the
  number of bytes written, or -1 on error.
 */
extern ulapi_integer ulapi_socket_write(ulapi_integer id, const char *buf, ulapi_integer len);

/*!
  Broadcasts \a len bytes from \a buf to socket \a id using port -a
  port. Returns the number of bytes written, or -1 on error.
 */
extern ulapi_integer ulapi_socket_broadcast(ulapi_integer id, ulapi_integer port, const char *buf, ulapi_integer len);

/*!
  Closes the socket id, whether that for a client, for a server, or to
  a client, broadcast or otherwise.
 */
extern ulapi_result ulapi_socket_close(ulapi_integer id);

/*
  File descriptor (fd) API
*/

typedef enum {
  ULAPI_STDIN,
  ULAPI_STDOUT,
  ULAPI_STDERR
} ulapi_stdio;

/*! Allocates space for a file identifier, and returns its pointer. */
extern void *
ulapi_fd_new(void);

/*! Deallocates a previously allocated file identifier. */
extern ulapi_result
ulapi_fd_delete(void *id);

/*! Copies the stdio file descriptor into \a id. */
extern ulapi_result
ulapi_std_open(ulapi_stdio io,	/*!< one of ULAPI_STDIN,STDOUT,STDERR */
	       void *id	/*!< pointer to fd where it will be copied */
	       );

/*!
  Checks to see if a file exists. Returns ULAPI_OK if so, otherwise
  returns ULAPI_ERROR.
*/
extern ulapi_result
ulapi_fd_stat(const char *path);

/*!
  Opens a connection to a file. Fills in the identifier and
  returns ULAPI_OK if it worked, otherwise returns ULAPI_ERROR.
 */
extern ulapi_result
ulapi_fd_open(const char *path, void *id);

/*!
  Sets the file descriptor \a id to be blocking or nonblocking.
*/
extern ulapi_result
ulapi_fd_set_nonblocking(void *id);

extern ulapi_result
ulapi_fd_set_blocking(void *id);

/*!
  Reads up to \a len bytes from file descriptor \a id into \a
  buf. Returns the number of bytes read, or -1 on error.
 */
extern ulapi_integer
ulapi_fd_read(void *id, char *buf, ulapi_integer len);

/*!
  Writes \a len bytes from \a buf to file descriptor \a
  id. Returns the number of bytes written, or -1 on error.
 */
extern ulapi_integer
ulapi_fd_write(void *id, const char *buf, ulapi_integer len);

/*! Flushes the file descriptor \a id. */
extern void ulapi_fd_flush(void *id);

/*! Drains the file descriptor \a id. */
extern void ulapi_fd_drain(void *id);

/*!
  Closes the file descriptor.
 */
extern ulapi_result
ulapi_fd_close(void *id);

/*
  Serial communication API, similar to file descriptor API
*/

/*! Allocates space for a serial port identifier, and returns its pointer. */
extern void *
ulapi_serial_new(void);

/*! Deallocates a previously allocated serial port identifier. */
extern ulapi_result
ulapi_serial_delete(void *id);

/*!
  Opens a connection to a serial port. Fills in the identifier and
  returns ULAPI_OK if it worked, otherwise returns ULAPI_ERROR.
 */
extern ulapi_result
ulapi_serial_open(const char *port, void *id);

extern ulapi_result
ulapi_serial_baud(void *id, int baud);

/*!
  Sets the serial port descriptor \a id to be blocking or nonblocking.
*/
extern ulapi_result
ulapi_serial_set_nonblocking(void *id);

extern ulapi_result
ulapi_serial_set_blocking(void *id);

/*!
  Reads up to \a len bytes from serial port descriptor \a id into \a
  buf. Returns the number of bytes read, or -1 on error.
 */
extern ulapi_integer
ulapi_serial_read(void *id, char *buf, ulapi_integer len);

/*!
  Writes \a len bytes from \a buf to serial port descriptor \a
  id. Returns the number of bytes written, or -1 on error.
 */
extern ulapi_integer
ulapi_serial_write(void *id, const char *buf, ulapi_integer len);

/*!
  Closes the serial port descriptor.
 */
extern ulapi_result
ulapi_serial_close(void *id);

#define ULAPI_DECL_SHARED
#ifdef WIN32
#undef ULAPI_DECL_SHARED
#define ULAPI_DECL_SHARED __declspec(dllexport)
#endif

/*!
  Opens a shared library \a objname and returns a pointer to it, for
  subsequent reference by ulapi_dl_sym and ulapi_dl_close. If the
  object cannot be opened, NULL is returned. If \a errstr is not NULL,
  up to \a errlen characters of a descriptive error string is copied
  into it.
*/
extern void *ulapi_dl_open(const char *objname, char *errstr, int errlen);

/*!
  Closes a shared library handle previously returned by ulapi_dl_open.
*/
extern void ulapi_dl_close(void *handle);

/*!
  Returns a pointer to a shared object (e.g., a function) with symbol name
  \a name. The shared object could be NULL, so to detect an error, pass
  a pointer to an error string \a errstr, and its length \a errlen, and
  check the string against an empty string.
*/
extern void *ulapi_dl_sym(void *handle, const char *name, char *errstr, int errlen);

/*!
  Executes 'prog'. Returns ULAPI_OK if it was able to execute, placing
  the program return value in 'result'. Otherwise, returns ULAPI_ERROR
  and leaves 'result' alone.
*/
extern ulapi_result ulapi_system(const char *prog, ulapi_integer *result);

#define ulapi_print printf
#ifdef WIN32
#define ulapi_pathsep "\\"
#define ulapi_snprintf sprintf_s
#define ulapi_strcasecmp _stricmp
#define ulapi_strdup _strdup
#define ulapi_sscanf sscanf
#define ulapi_strncpy strncpy
#else
#define ulapi_pathsep "/"
#define ulapi_snprintf snprintf
#define ulapi_strcasecmp strcasecmp
#define ulapi_strdup strdup
#define ulapi_sscanf sscanf
#define ulapi_strncpy strncpy
#endif

/*!
  Returns non-zero if the path has a path separator in it, otherwise
  it returns zero, signifying a simple file name with no path.
*/
extern ulapi_flag ulapi_ispath(const char *path);

extern char *ulapi_fixpath(const char *path, char *fix, size_t len);
extern char *ulapi_basename(const char *path, char *base);
extern char *ulapi_dirname(const char *path, char *dir);

/* Native implementation of getopt */

#define ulapi_getopt getopt
#define ulapi_optarg optarg
#define ulapi_optind optind
#define ulapi_opterr opterr
#define ulapi_optopt optopt
#define ulapi_getopt_long getopt_long

#ifdef WIN32
#define NEED_GETOPT
#endif

#ifndef NEED_GETOPT

#include <getopt.h>

#else

struct option
{
  const char *name;
  /* has_arg can't be an enum because some compilers complain about
     type mismatches in all the code that assumes it is an int.  */
  int has_arg;
  int *flag;
  int val;
};

/* Names for the values of the `has_arg' field of `struct option'.  */

# define no_argument		0
# define required_argument	1
# define optional_argument	2

extern int	opterr;
extern int	optind;
extern int	optopt;
extern char    *optarg;
extern int	optreset;

extern int getopt(int nargc, char * const *nargv, const char *options);

extern int getopt_long(int nargc, char * const * nargv, const char * options, const struct option * long_options, int * idx);

#endif	/* NEED_GETOPT */

#ifdef __cplusplus
#if 0
{			  /* just to match one below, for indenters */
#endif
}
#endif

#endif /* ULAPI_H */
