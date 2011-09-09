#include <stdio.h>
#include <stdlib.h>
#include <libsess.h>
#include <zmq.h>

/**
 * \brief Binary iteration (Bob)
 *
 * The endpoint implementation of iteration of role Bob
 *
 */
int main(int argc, char *argv[])
{
  void *ctx = zmq_init(1);
  role *Alice = sess_server(ctx, ZMQ_PAIR, "tcp://*:4242", "Iteration_Bob.spr");

  send_int(Alice, 42); 

  //
  // Main loop
  //

  int *value = NULL;
  char *str = NULL;
  while(s_inwhile(1, Alice)) {
    receive_int(Alice, &value);
    send_int(Alice, *value);
    receive_string(Alice, &str);
    printf("Received number: %d, string: %s\n", *value, str);
  }

  zmq_close(Alice);
  zmq_term(ctx);
  return EXIT_SUCCESS;
}
