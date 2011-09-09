#include <stdio.h>
#include <stdlib.h>
#include <libsess.h>
#include <zmq.h>

/**
 * \brief Multiparty iteration (Bob)
 *
 * The endpoint implementation of multiparty iteration of role Bob
 *
 */
int main(int argc, char *argv[])
{
  void *ctx = zmq_init(1);
  role *Alice = sess_server(ctx, ZMQ_PAIR, "tcp://*:6665", "MPIteration_Bob.spr");
  role *Charlie = sess_client(ctx, ZMQ_PAIR, "tcp://*:6666", "MPIteration_Bob.spr");

  char *rcvd_str;
  int *rcvd_int;

  receive_string(Alice, &rcvd_str);
  printf("Alice said: %s\n", rcvd_str);
  free(rcvd_str);

  // Main loop
  while (s_outwhile(s_inwhile(1, Alice), 1, Charlie)) {
    receive_int(Alice, &rcvd_int);
    send_int(Charlie, *rcvd_int);
    free(rcvd_int);

    receive_int(Charlie, &rcvd_int);
    send_int(Alice, *rcvd_int);
    free(rcvd_int);

    receive_string(Alice, &rcvd_str);
    printf("Alice said: %s\n", rcvd_str);
    send_string(Charlie, rcvd_str);
    free(rcvd_str);
  }

  zmq_close(Alice);
  zmq_close(Charlie);
  zmq_term(ctx);
  return EXIT_SUCCESS;
}
