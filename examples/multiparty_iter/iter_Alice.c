#include <stdio.h>
#include <stdlib.h>
#include <libsess.h>
#include <zmq.h>

/**
 * \brief Multiparty iteration (Alice)
 *
 * The endpoint implementation of multiparty iteration of role Alice
 *
 */
int main(int argc, char *argv[])
{
  void *ctx = zmq_init(1);
  role *Bob = sess_client(ctx, ZMQ_PAIR, "tcp://localhost:6665", "MPIteration_Alice.spr");
  role *Charlie = sess_client(ctx, ZMQ_PAIR, "tcp://localhost:6667", "MPIteration_Alice.spr");

  int i = 0;
  int *rcvd_int;
  char buf_str[50];

  send_string(Bob, "Start, Bob");
  send_string(Charlie, "Start, Charlie");

  // Main loop
  while (s_outwhile(i++<3, 2, Bob, Charlie)) {
    send_int(Bob, i);
    receive_int(Bob, &rcvd_int);
    free(rcvd_int);

    snprintf(buf_str, 50, "Hello World @ %d\n", i);
    send_string(Bob, buf_str);
  }

  zmq_close(Bob);
  zmq_close(Charlie);
  zmq_term(ctx);
  return EXIT_SUCCESS;
}
