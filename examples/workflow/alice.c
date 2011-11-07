#include <stdio.h>
#include <stdlib.h>
#include <libsess.h>

int main(int argc, char **argv)
{
  session *s;
  join_session(&argc, &argv, &s, "Workflow_Alice.spr");
  role *Bob = s->get_role(s, "Bob");

  printf("Process: Alice.\n");

  int v = 42;
  send_int(Bob, v);
  recv_int(Bob, &v);

  end_session(s);
  return EXIT_SUCCESS;
}
