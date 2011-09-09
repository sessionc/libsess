#include <stdio.h>
#include <stdlib.h>

#include <libsess.h>

int main(int argc, char *argv[])
{
  int val;
  void *ctx = zmq_init(1);
  role *Alice = sess_client(ctx, ZMQ_PAIR, "tcp://localhost:4242", "");

  char *x;
  while (1) {
    recv_int(Alice, &val);
    send_int(Alice, 42);
  }

  return EXIT_SUCCESS;
}
