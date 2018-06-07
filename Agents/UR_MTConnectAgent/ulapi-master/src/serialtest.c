#include <stdio.h>
#include <stdlib.h>
#include "ulapi.h"

static void task_code(void *serial)
{
  enum {BUFFERLEN = 80};
  char buffer[BUFFERLEN];
  int nchars;
  int t;

  for (;;) {
    nchars = ulapi_serial_read(serial, buffer, sizeof(buffer)-1);
	if (nchars < 0) break;
	else if (nchars == 0) printf("nothing\n");
    else {
      for (t = 0; t < nchars; t++) fputc(buffer[t], stdout);
	  fflush(stdout);
	}
  }

  printf("done\n");

  return;
}

/* syntax: serial <port> <baud> */

int main(int argc, char * argv[])
{
#define BUFFERLEN 256
  char buffer[BUFFERLEN];
  char *port;
  int baud;
  void *serial;
  void *task;

  if (argc < 3) {
    fprintf(stderr, "syntax: serial <port> <baud>\n");
    return 1;
  }

  port = argv[1];
  baud = atoi(argv[2]);

  if (ULAPI_OK != ulapi_init()) {
    fprintf(stderr, "ulapi_init error\n");
    return 1;
  }

  if (NULL == (serial = ulapi_serial_new())) {
    fprintf(stderr, "Can't get a serial object\n");
    return 1;
  }

  if (ULAPI_OK != ulapi_serial_open(port, serial)) {
    fprintf(stderr, "Can't open port %s\n", port);
    return 1;
  }

  if (ULAPI_OK != ulapi_serial_baud(serial, baud)) {
    fprintf(stderr, "Can't set baud rate to %d\n", baud);
    return 1;
  }

  task = ulapi_task_new();
  ulapi_task_start(task, task_code, serial, ulapi_prio_lowest(), 0);

  while (! feof(stdin)) {
	int retval;
    printf("> ");
    fflush(stdout);
    if (NULL == fgets(buffer, BUFFERLEN, stdin)) break;
    retval = ulapi_serial_write(serial, buffer, strlen(buffer));
	printf("return: %d\n", retval);
  }

  return 0;
}
