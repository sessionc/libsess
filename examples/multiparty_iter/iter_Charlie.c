#include <stdio.h>
#include <stdlib.h>
#include <libsess.h>
#include <zmq.h>

/**
 * \brief Multiparty iteration (Alice)
 *
 * The endpoint implementation of multiparty iteration of role Charlie
 *
 */
int main(int argc, char *argv[])
{
  void *ctx = zmq_init(1);
  role *Alice = sess_server(ctx, ZMQ_PAIR, "tcp://*:6667", "MPIteration_Charlie.spr");
  role *Bob = sess_server(ctx, ZMQ_PAIR, "tcp://*:6666", "MPIteration_Charlie.spr");

  int *rcvd_int;
  char *rcvd_str;

  receive_string(Alice, &rcvd_str);
  printf("Alice said: %s\n", rcvd_str);
  free(rcvd_str);

  while (s_inwhile(2, Alice, Bob)) {
    receive_int(Bob, &rcvd_int);
    send_int(Bob, *rcvd_int);
    free(rcvd_int);

    receive_string(Bob, &rcvd_str);
    printf("Bob said: %s\n", rcvd_str);
    free(rcvd_str);
  }

  zmq_close(Alice);
  zmq_close(Bob);
  zmq_term(ctx);
  return EXIT_SUCCESS;
}
