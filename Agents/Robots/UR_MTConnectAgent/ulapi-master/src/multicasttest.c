/*!
  \file multicasttest.c

  \brief Tests send and receive multicast sockets.
*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "ulapi.h"

int main(int argc, char *argv[])
{
  enum {BUFFERLEN = 80};
  int option;
  ulapi_integer port;
  int is_client;
  ulapi_integer fd;
  int nchars;
  char intf[BUFFERLEN] = "224.0.0.1";
  char buffer[BUFFERLEN];

  ulapi_opterr = 0;
  is_client = 0;	 /* start as server unless host is provided */
  port = -1;		 /* start with invalid port number */

  for (;;) {
    option = ulapi_getopt(argc, argv, ":p:i:c");
    if (option == -1)
      break;

    switch (option) {
    case 'p':
      port = atoi(ulapi_optarg);
      break;

    case 'i':
      strncpy(intf, ulapi_optarg, sizeof(intf));
      intf[sizeof(intf) - 1] = 0;
      break;

    case 'c':
      is_client = 1;
      break;

    case ':':
      fprintf(stderr, "missing value for -%c\n", ulapi_optopt);
      return 1;
      break;

    default:			/* '?' */
      fprintf(stderr, "unrecognized option -%c\n", ulapi_optopt);
      return 1;
      break;
    }
  }
  if (ulapi_optind < argc) {
    fprintf(stderr, "extra non-option characters: %s\n", argv[ulapi_optind]);
    return 1;
  }

  if (port < 0) {
    fprintf(stderr, "need a port\n");
    return 1;
  }

  if (ULAPI_OK != ulapi_init()) {
    fprintf(stderr, "ulapi_init error\n");
    return 1;
  }

  ulapi_set_debug(ULAPI_DEBUG_ALL);

  if (is_client) {
    fd = ulapi_socket_get_multicastee_id_on_interface(port, intf);
    if (fd <= 0) {
      fprintf(stderr, "can't open socket\n");
      return 1;
    }
    while (1) {
      memset(buffer, 0, sizeof(buffer));
      nchars = ulapi_socket_read(fd, buffer, sizeof(buffer));
      if (nchars <= 0) break;
      printf("%s\n", buffer);
    }
  } else {
    fd = ulapi_socket_get_multicaster_id_on_interface(port, intf);
    if (fd <= 0) {
      fprintf(stderr, "can't open socket\n");
      return 1;
    }
    while (! feof(stdin)) {
      memset(buffer, 0, sizeof(buffer));
      if (NULL == fgets(buffer, sizeof(buffer), stdin)) break;
      ulapi_socket_write(fd, buffer, strlen(buffer));
    }
  }

  return 0;
}
