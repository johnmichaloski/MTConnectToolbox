/*
  stub_ulapi.c

  Fill me in with your platform's implementation of user-level API to
  real-time functions. Some ANSI C stuff is in here already.
*/

#include <stddef.h>		/* NULL */
#include "ulapi.h"

ulapi_result ulapi_init(void)
{
  return ULAPI_OK;
}

ulapi_result ulapi_exit(void)
{
  return ULAPI_OK;
}

ulapi_result ulapi_shm_alloc(ulapi_integer key,
			     ulapi_integer size, 
			     ulapi_integer *id, 
			     void **ptr)
{
  *id = 0;
  *ptr = NULL;

  return ULAPI_OK;
}

ulapi_result ulapi_shm_free(ulapi_integer key,
			    ulapi_integer size,
			    ulapi_integer id, 
			    const void *ptr)
{
  return ULAPI_OK;
}
