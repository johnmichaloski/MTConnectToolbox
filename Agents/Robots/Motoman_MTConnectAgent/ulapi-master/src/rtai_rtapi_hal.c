/*!
  \file rtai_rtapi_hal.c

  \brief Does any necessary hardware configuration, such as setting
  up the base timer period.
*/

/*!
  \defgroup RTAIHAL The RTAI Hardware Abstraction Layer

  The RTAI implementation of the \ref RTAPI real-time application
  programming interface has a hardware abstraction layer (HAL) that
  sets the timer base period and initializes global variables used by
  real-time processes.

  RTAPI_HAL_NSECS_PER_PERIOD is the base period of the timer. It
  should be as small as possible, since all tasks in all RT
  applications will run at multiples of this period. If a high-speed
  task like a stepper motor controller is also running, this should be
  set to be as small as that task needs.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <linux/kernel.h>	/* EXPORT_SYMBOL */
#include "rtapi.h"
#include "rtapi_app.h"		/* rtapi_app_XXX, RTAPI_HAL_NSECS_PER_PERIOD */

rtapi_integer rtapi_clock_period = 1;
EXPORT_SYMBOL(rtapi_clock_period);

RTAPI_DECL_INT(DEBUG, 0);
RTAPI_DECL_INT(RTAPI_HAL_NSECS_PER_PERIOD, 100000);

int rtapi_app_main(RTAPI_APP_ARGS_DECL)
{
  if (0 != rtapi_app_init(RTAPI_APP_ARGS)) {
    rtapi_print("can't init rtapi\n");
    return -1;
  }

  /* get command line args */
  (void) rtapi_arg_get_int(&DEBUG, "DEBUG");
  if (DEBUG) rtapi_print("using DEBUG = %d\n", DEBUG);
  (void) rtapi_arg_get_int(&RTAPI_HAL_NSECS_PER_PERIOD, "RTAPI_HAL_NSECS_PER_PERIOD");
  if (DEBUG) rtapi_print("using RTAPI_HAL_NSECS_PER_PERIOD=%d\n", RTAPI_HAL_NSECS_PER_PERIOD);

  /* set the base timer period */
  rtapi_clock_set_period(RTAPI_HAL_NSECS_PER_PERIOD);
  rtapi_clock_period = RTAPI_HAL_NSECS_PER_PERIOD;

  if (DEBUG) rtapi_print("gohal started with period %d\n", rtapi_clock_period);

  return rtapi_app_wait();
}
