#include <stdlib.h>
#include <libsess.h>

int main(int argc, char **argv)
{
  session *s;
  join_session(&argc, &argv, &s, "Workflow_Bob.spr");
  role *Bob = s->get_role(s, "Alice");

  int v;
  recv_int(Bob, &v);
  send_int(Bob, v);

  end_session(s);
  return EXIT_SUCCESS;
}
