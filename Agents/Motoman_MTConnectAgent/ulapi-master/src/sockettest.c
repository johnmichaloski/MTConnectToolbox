/*!
  \file sockettest.c

  \brief Tests client and server socket communication.
*/

#include <stdio.h>		/* stdin, stderr */
#include <stddef.h>		/* NULL, sizeof */
#include <stdlib.h>		/* malloc, free, atoi */
#include <string.h>		/* strncpy, strlen */
#include "ulapi.h"

typedef struct {
  void *client_read_task;
  ulapi_integer socket_id;
} client_read_args;

typedef struct {
  void *server_task;
  ulapi_integer client_id;
} server_args;

void client_read_code(void *args)
{
  void *client_read_task;
  ulapi_integer socket_id;
  ulapi_integer nchars;
  enum {BUFFERLEN = 80};
  char inbuf[BUFFERLEN];

  client_read_task = ((client_read_args *) args)->client_read_task;
  socket_id = ((client_read_args *) args)->socket_id;

  for (;;) {
  nchars = ulapi_socket_read(socket_id, inbuf, sizeof(inbuf)-1);
  if (-1 == nchars) {
    fprintf(stderr, "connection closed\n");
    break;
  }
  if (0 == nchars) {
    fprintf(stderr, "end of file\n");
    break;
  }
  inbuf[nchars] = 0;
  printf("read %d chars: ``%s''\n", nchars, inbuf);
  }


  ulapi_socket_close(socket_id);

  printf("client read thread done, closed %d\n", socket_id);

  ulapi_task_delete(client_read_task);

  return;
}

void server_code(void *args)
{
  void *server_task;
  ulapi_integer client_id;
  ulapi_integer nchars;
  enum {BUFFERLEN = 80};
  char inbuf[BUFFERLEN];

  server_task = ((server_args *) args)->server_task;
  client_id = ((server_args *) args)->client_id;
  free(args);

  for (;;) {
    nchars = ulapi_socket_read(client_id, inbuf, sizeof(inbuf));
    if (-1 == nchars) {
      break;
    }
    if (0 == nchars) {
      break;
    }
    printf("got %d chars: ``%s''\n", nchars, inbuf);
    ulapi_socket_write(client_id, inbuf, nchars);
  }

  ulapi_socket_close(client_id);

  printf("server thread done, closed %d\n", client_id);

  ulapi_task_delete(server_task);

  return;
}

int main(int argc, char *argv[])
{
  enum {BUFFERLEN = 80};
  int option;
  int is_client;
  int is_broadcastee;
  ulapi_integer port;
  ulapi_integer socket_id;
  ulapi_integer client_id;
  ulapi_integer broadcast_id;
  ulapi_integer nchars;
  char host[BUFFERLEN] = "localhost";
  char intf[BUFFERLEN] = "localhost";
  char inbuf[BUFFERLEN];
  char outbuf[BUFFERLEN];
  char *intfptr;
  char *ptr;
  void *client_read_task;
  void *server_task;
  client_read_args client_read_args_inst;
  server_args *server_args_ptr;
  char connection_addr[BUFFERLEN];
  int connection_port;

  intfptr = NULL;
  ulapi_opterr = 0;
  is_client = 0;	 /* start as server unless host is provided */
  is_broadcastee = 0;	 /* client reads as usual, not broadcast port */
  port = -1;		 /* start with invalid port number */

  for (;;) {
    option = ulapi_getopt(argc, argv, ":p:h:i:b");
    if (option == -1)
      break;

    switch (option) {
    case 'p':
      port = atoi(ulapi_optarg);
      break;

    case 'h':
      strncpy(host, ulapi_optarg, sizeof(host));
      host[sizeof(host) - 1] = 0;
      is_client = 1;
      break;

    case 'i':
      strncpy(intf, ulapi_optarg, sizeof(intf));
      intf[sizeof(intf) - 1] = 0;
      intfptr = intf;
      break;

    case 'b':
      is_client = 1;
      is_broadcastee = 1;
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
    if (is_broadcastee) {
      socket_id = ulapi_socket_get_broadcastee_id_on_interface(port, intfptr);
      if (socket_id < 0) {
	fprintf(stderr, "can't connect to %s:%d\n", host, (int) port);
	ulapi_exit();
	return 1;
      } else {
	printf("listening to broadcasts on port %d\n", (int) port);
      }
    } else {
      socket_id = ulapi_socket_get_client_id_on_interface(port, host, intfptr);
      if (socket_id < 0) {
	fprintf(stderr, "can't connect to %s on port %d\n", host, (int) port);
	ulapi_exit();
	return 1;
      } else {
	printf("connected to %s on port %d\n", host, (int) port);
      }
    }

    client_read_task = ulapi_task_new();
    client_read_args_inst.client_read_task = client_read_task;
    client_read_args_inst.socket_id = socket_id;
    ulapi_task_start(client_read_task, client_read_code, &client_read_args_inst, ulapi_prio_lowest(), 0);

    for (;;) {
      if (is_broadcastee) {
	/* just wait until ^D to end, since client task handles reads */
	if (NULL == fgets(outbuf, sizeof(outbuf), stdin)) {
	  break;
	}
      } else {
	if (NULL == fgets(outbuf, sizeof(outbuf), stdin)) {
	  break;
	}
	for (ptr = outbuf; *ptr != '\n' && *ptr != 0; ptr++); *ptr = 0;
	nchars = ulapi_socket_write(socket_id, outbuf, ptr-outbuf+1);
	if (nchars < 0) break;
	printf("wrote %d chars\n", nchars);
      }
    }
  } else {
    socket_id = ulapi_socket_get_server_id_on_interface(port, intfptr);
    if (socket_id < 0) {
      fprintf(stderr, "can't serve port %d on interface %s\n", (int) port, NULL == intfptr ? "default" : intfptr);
      ulapi_exit();
      return 1;
    }
    printf("serving port %d on interface %s\n", (int) port, NULL == intfptr ? "default" : intfptr);

    broadcast_id = ulapi_socket_get_broadcaster_id_on_interface(port, intfptr);
    if (0 > broadcast_id) {
      fprintf(stderr, "can't broadcast to %d, ignoring\n", (int) port);
    }

    for (;;) {
      printf("waiting for client connection...\n");
      client_id = ulapi_socket_get_connection_id(socket_id);
      if (client_id < 0) {
	break;
      }
      ulapi_getpeername(client_id, connection_addr, sizeof(connection_addr), &connection_port);
      printf("got one on fd %d from %s on port %d\n", client_id, connection_addr, connection_port);

      if (0 <= broadcast_id) {
		  ulapi_socket_write(broadcast_id, "you have a neighbor", strlen("you have a neighbor"));
	  }

      server_task = ulapi_task_new();

      server_args_ptr = malloc(sizeof(server_args));
      server_args_ptr->server_task = server_task;
      server_args_ptr->client_id = client_id;
      ulapi_task_start(server_task, server_code, server_args_ptr, ulapi_prio_lowest(), 0);
    }
  }

  ulapi_exit();
  return 0;
}

