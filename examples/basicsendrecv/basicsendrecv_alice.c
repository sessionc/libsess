#include <stdio.h>
#include <stdlib.h>

#include <libsess.h>

#define X 1

int main(int argc, char *argv[])
{
  int val;
  void *ctx = zmq_init(1);
  role *Bob = sess_client(ctx, ZMQ_PAIR, "tcp://localhost:4242", "");
  role *Charlie = sess_server(ctx, ZMQ_PAIR, "tcp://*:4242", "");

  send_int(Bob, X);
  recv_int(Charlie, &val);

  send_string(Bob, "Test");
  return EXIT_SUCCESS;
}
