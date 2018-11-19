/*!
  \file dltest.c

  \brief Test code for loading a shared library.
  loaded.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <string.h>
#include "ulapi.h"

int main(int argc, char *argv[])
{
#define ERRSTR_LEN 80
  char errstr[ERRSTR_LEN];
  void *handle;
  int (*sum)(int, int);
  int (*diff)(int, int);

  if (argc < 2) {
    fprintf(stderr, "syntax: dltest <obj name>\n");
    return 1;
  }

  handle = ulapi_dl_open(argv[1], errstr, ERRSTR_LEN);
  if (NULL == handle) {
    fprintf(stderr, "can't load %s: %s\n", argv[1], errstr);
    return 1;
  }

#define DLSYM(FUNC,NAME)						\
  *(void **) (&FUNC) = ulapi_dl_sym(handle, NAME, errstr, ERRSTR_LEN);	\
  if (0 != errstr[0]) {							\
    fprintf(stderr, "can't look up %s: %s", NAME, errstr);		\
    ulapi_dl_close(handle);						\
    handle = NULL;							\
    return 1;								\
  }

  DLSYM(sum, "sum");
  DLSYM(diff, "diff");

  printf("%d %d\n", (*sum)(1, 2), (*diff)(3, 4));

  ulapi_dl_close(handle);

  return 0;
}
