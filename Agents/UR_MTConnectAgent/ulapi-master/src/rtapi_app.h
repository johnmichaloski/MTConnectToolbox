#ifndef RTAPI_APP_H
#define RTAPI_APP_H

#ifdef __KERNEL__

#include <linux/version.h>
#include <linux/module.h>

#define rtapi_app_main init_module
#define rtapi_app_exit cleanup_module
extern int rtapi_app_init(void);
extern int rtapi_app_wait(void);
extern int rtapi_app_unwait(void);

#define RTAPI_APP_ARGS_DECL void
#define RTAPI_APP_ARGS

extern char * rtapi_arg_get_string(char ** var, char * key);
extern int rtapi_arg_get_int(int * var, char * key);

MODULE_LICENSE("GPL");

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)

/*
  The MODULE_PARM() macro takes 2 arguments: the name of the variable
  and its type. The supported variable types are "b": single byte,
  "h": short int, "i": integer, "l": long int and "s": string. Strings
  should be declared as "char *" and insmod will allocate memory for
  them. You should always try to give the variables an initial default
  value. This is kernel code, and you should program defensively. For
  example:

  int myint = 3;
  char *mystr;

  MODULE_PARM (myint, "i");
  MODULE_PARM (mystr, "s");

  Arrays are supported too. An integer value preceding the type in
  MODULE_PARM will indicate an array of some maximum length. Two
  numbers separated by a '-' will give the minimum and maximum number
  of values. For example, an array of shorts with at least 2 and no
  more than 4 values could be declared as:

  int myshortArray[4];
  MODULE_PARM (myintArray, "2-4i");
 */

#define RTAPI_DECL_STRING(name,value) \
char * name = value; \
MODULE_PARM(name, "s");

#define RTAPI_DECL_INT(name,value) \
int name = value; \
MODULE_PARM(name, "i");

#else

/*
  To allow arguments to be passed to your module, declare the variables
  that will take the values of the command line arguments as global and
  then use the module_param() macro, (defined in linux/moduleparam.h) to
  set the mechanism up. At runtime, insmod will fill the variables with
  any command line arguments that are given, like ./insmod mymodule.ko
  myvariable=5. The variable declarations and macros should be placed at
  the beginning of the module for clarity. The example code should clear
  up my admittedly lousy explanation.

  The module_param() macro takes 3 arguments: the name of the
  variable, its type and permissions for the corresponding file in
  sysfs. Integer types can be signed as usual or unsigned. If you'd
  like to use arrays of integers or strings see module_param_array()
  and module_param_string().

  int myint = 3;
  module_param(myint, int, 0);

  Arrays are supported too, but things are a bit different now than
  they were in the 2.4. days. To keep track of the number of
  parameters you need to pass a pointer to a count variable as third
  parameter. At your option, you could also ignore the count and pass
  NULL instead. We show both possibilities here:

  int myintarray[2];
  module_param_array(myintarray, int, NULL, 0); not interested in count

  int myshortarray[4];
  int count;
  module_parm_array(myshortarray, short, & count, 0);
  put count into "count" variable

  E.g., 

  static short int myshort = 1;
  static int myint = 420;
  static long int mylong = 9999;
  static char *mystring = "blah";
  static int myintArray[2] = { -1, -1 };
  static int arr_argc = 0;

  module_param(foo, int, 0000)
  The first param is the parameters name
  The second param is it's data type
  The final argument is the permissions bits, 
  for exposing parameters in sysfs (if non-zero) at a later stage.

  module_param(myshort, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
  MODULE_PARM_DESC(myshort, "A short integer");
  module_param(myint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  MODULE_PARM_DESC(myint, "An integer");
  module_param(mylong, long, S_IRUSR);
  MODULE_PARM_DESC(mylong, "A long integer");
  module_param(mystring, charp, 0000);
  MODULE_PARM_DESC(mystring, "A character string");

  module_param_array(name, type, num, perm);
  The first param is the parameter's (in this case the array's) name The
  second param is the data type of the elements of the array The third
  argument is a pointer to the variable that will store the number of
  elements of the array initialized by the user at module loading time
  The fourth argument is the permission bits

  module_param_array(myintArray, int, &arr_argc, 0000);
  MODULE_PARM_DESC(myintArray, "An array of integers");
 */
#include <linux/moduleparam.h>

#define RTAPI_DECL_STRING(name,value) \
char * name = value; \
module_param(name, charp, 0)

#define RTAPI_DECL_INT(name,value) \
int name = value; \
module_param(name, int, 0)

#endif

#else 

/* This is set so we can switch in code for debug printing, etc. */
#define USING_UNIX 1

#include <stdlib.h>		/* atexit */

/*!
  The entry point for the main application. Mapped to 'main' for Windows
  and Unix, 'init_module' or related in real-time Linux. Your application
  should give this name to its entry point.
 */
#define rtapi_app_main main

/*!
  The main application should call this before any RTAPI functions.
 */
extern int rtapi_app_init(int argc, char ** argv);

/*!
  The main application should call this when finished starting all
  the tasks. This awaits some platform-specific method for stopping
  the main application, such as a signal or input from the user.
 */
extern int rtapi_app_wait(void);
extern int rtapi_app_unwait(void);

#define RTAPI_APP_ARGS_DECL int argc, char * argv[]
#define RTAPI_APP_ARGS argc,argv

extern char * rtapi_arg_get_string(char ** var, char * key);
extern int rtapi_arg_get_int(rtapi_integer * var, char * key);

/*
  Command-line handling is easier with Unix processes. Here we provide
  functions that look at each argument for things like

  KINEMATICS=genhexkins

  and return a pointer to the stuff after the = sign. These are coded
  on an individual argument basis via calls to rtapi_arg_get()
*/

extern int rtapi_argc;
extern char ** rtapi_argv;

#define RTAPI_DECL_STRING(name,value) \
char * name = value;

#define RTAPI_DECL_INT(name,value) \
int name = value;

#endif

#endif /* RTAPI_APP_H */
