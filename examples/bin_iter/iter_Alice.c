#include <stdio.h>
#include <stdlib.h>
#include <libsess.h>
#include <zmq.h>

/**
 * \brief Binary iteration (Alice)
 *
 * The endpoint implementation of iteration of role Alice
 *
 */
int main(int argc, char *argv[])
{
  void *ctx = zmq_init(1);
  role *Bob = sess_client(ctx, ZMQ_PAIR, "tcp://localhost:4242", "Iteration_Alice.spr");

  int *value = NULL;
  receive_int(Bob, &value);
  printf("Received [%d] = 42\n", *value);
  free(value);

  //
  // Main loop
  //

  int i = 0;
  while(s_outwhile(i++<3, 1, Bob)) {
    send_int(Bob, i);
    receive_int(Bob, &value);
    send_string(Bob, "Hello World!");
    free(value);
  }

  zmq_close(Bob);
  zmq_term(ctx);
  return EXIT_SUCCESS;
}
