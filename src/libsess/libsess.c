/**
 * \file
 * This file is the main source file of session C runtime library (libsess)
 *
 * \headerfile <libsess.h>
 * \headerfile "st_node.h"
 * \headerfile "parser.h"
 *
 */

#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include <zmq.h>

#include <libsess.h>

#include "connmgr.h"
#include "parser.h"
#include "st_node.h"

#define OUTWHILE_SYNC_MAGIC 0x42

/**
 * \brief Helper function to deallocate send queue.
 *
 */
void _dealloc(void *data, void *hint)
{
  free(data);
}


/**
 * Helper function to lookup a role in a session.
 */
role *find_role_in_session(session *s, char *role_name)
{
  int i;
  for (i=0; i<s->endpoints_count; ++i) {
    if (strcmp(s->endpoints[i]->role_name, role_name) == 0) {
      return s->endpoints[i]->role_ptr;
    }
  }
  fprintf(stderr, "%s: Role %s not found in session.\n",
                    __FUNCTION__, role_name);
#ifdef __DEBUG__
  dump_session(s);
#endif
  return NULL;
}


/**
 * Session initiation, involves three steps:
 *  (1) Load configuration from filesystem supplied as command line argument
 *  (2) Load endpoint scribble and extract relevant configuration
 *  (3) Create a session variable with connected endpoints
 */
void join_session(int *argc, char ***argv, session **s, const char *scribble)
{
  conn_rec *conns; // Array of connection records
  host_map *role_hosts; // Role-to-host mapping
  int nr_of_conns; // Number of connections
  int nr_of_roles; // Number of roles

  int conn_idx;
  int endpoint_idx; 

  int option;
  char *config_file = NULL;

  // Invoke getopt to extract arguments we need
  while (1) {
    static struct option long_options[] = {
      {"conf", required_argument, 0, 'c'},
      {0, 0, 0, 0}
    };

    int option_idx = 0;
    option = getopt_long(*argc, *argv, "c:", long_options, &option_idx);

    if (option == -1) break;

    switch (option) {
      case 'c':
        config_file = malloc(sizeof(char) * (strlen(optarg)+1));
        strcpy(config_file, optarg);
        fprintf(stderr, "Using configuration file %s\n", config_file);
        break;
    }
  }
  
  if (config_file == NULL) {
    config_file = malloc(sizeof(char) * 10);
    config_file = "conn.conf"; // Default config file
  }

  *s = (session *)malloc(sizeof(session));
  session *sess = *s; // Alias

  nr_of_conns = connmgr_read(config_file, &conns, &role_hosts, &nr_of_roles);

  // Extract role_name from Scribble
  char *role_name;
  parse_rolename(scribble, &role_name);

  sess->endpoints = malloc(sizeof(endpoint_t *) * (nr_of_roles-1));

  sess->ctx = zmq_init(1);

  for (endpoint_idx=0, conn_idx=0; conn_idx<nr_of_conns; ++conn_idx) {
    if (strcmp(conns[conn_idx].from, role_name) == 0) { // As a client.
      sess->endpoints[endpoint_idx] = malloc(sizeof(endpoint_t));
      sess->endpoints[endpoint_idx]->role_name
          = malloc(sizeof(char) * (strlen(conns[conn_idx].to)+1));
      strcpy(sess->endpoints[endpoint_idx]->role_name, conns[conn_idx].to);

      sprintf(sess->endpoints[endpoint_idx]->uri,
                "tcp://%s:%u",
                conns[conn_idx].host,
                conns[conn_idx].port);
#ifdef __DEBUG__
      fprintf(stderr, "Connection (as client) %s -> %s is %s\n",
                        conns[conn_idx].from,
                        conns[conn_idx].to,
                        sess->endpoints[endpoint_idx]->uri);
#endif
      if ((sess->endpoints[endpoint_idx]->role_ptr = zmq_socket(sess->ctx, ZMQ_PAIR)) == NULL) {
        perror("zmq_socket");
      }
      if (zmq_connect(sess->endpoints[endpoint_idx]->role_ptr, sess->endpoints[endpoint_idx]->uri) != 0) {
        perror("zmq_connect");
      }
      sess->endpoints_count++;
      endpoint_idx++;
    }

    if (strcmp(conns[conn_idx].to, role_name) == 0) { // As a server.
      sess->endpoints[endpoint_idx] = malloc(sizeof(endpoint_t));
      sess->endpoints[endpoint_idx]->role_name
          = malloc(sizeof(char) * (strlen(conns[conn_idx].from)+1));
      strcpy(sess->endpoints[endpoint_idx]->role_name, conns[conn_idx].from);

      sprintf(sess->endpoints[endpoint_idx]->uri, "tcp://*:%u",conns[conn_idx].port);
#ifdef __DEBUG__
      fprintf(stderr, "Connection (as server) %s -> %s is %s\n", conns[conn_idx].from, conns[conn_idx].to, sess->endpoints[endpoint_idx]->uri);
#endif
      if ((sess->endpoints[endpoint_idx]->role_ptr = zmq_socket(sess->ctx, ZMQ_PAIR)) == NULL) {
        perror("zmq_socket");
      }
      if (zmq_bind(sess->endpoints[endpoint_idx]->role_ptr, sess->endpoints[endpoint_idx]->uri) != 0) {
        perror("zmq_bind");
      }
      sess->endpoints_count++;
      endpoint_idx++;
    }
  }

  sess->get_role = &find_role_in_session;

  // TODO Implicit barrier synchronisation here.
#ifdef __DEBUG__
  fprintf(stderr, "Created session <%p> with %u endpoints\n", *s, (*s)->endpoints_count);
#endif
}


/**
 * Dump the content of a session in a human readable form for debugging.
 */
void dump_session(const session *s)
{
  unsigned endpoint_idx;
  printf("---- Dumping session <%p> ---- \n", s);
  printf("Number of endpoints: %u\n", s->endpoints_count);
  for (endpoint_idx=0; endpoint_idx<s->endpoints_count; ++endpoint_idx) {
    printf("Endpoint %d { role_name: %s, role_ptr: <%p>, uri: %s }\n",
              endpoint_idx,
              s->endpoints[endpoint_idx]->role_name,
              s->endpoints[endpoint_idx]->role_ptr,
              s->endpoints[endpoint_idx]->uri
    );
  }
  printf("ZMQ Context: %p\n", s->ctx);
  printf("---- End dumping session <%p> ----\n", s);
}



/**
 *
 *
 */
void end_session(session *s)
{
  // TODO Do some extra checking
  //      eg. make sure session is finished.
  //     
  unsigned endpoint_idx;
  unsigned endpoints_count = s->endpoints_count;

  for (endpoint_idx=endpoints_count-1; endpoint_idx>0; --endpoint_idx) {
#ifdef __DEBUG__
  fprintf(stderr, " -- Disconnecting endpoint %d\n", endpoint_idx);
#endif
    if (zmq_close(s->endpoints[endpoint_idx]->role_ptr) != 0) {
      perror("zmq_close");
    }
    free(s->endpoints[endpoint_idx]);
  }
  free(s->endpoints);

  zmq_term(s->ctx);
  s->get_role = NULL;
  free(s);
#ifdef __DEBUG__
  fprintf(stderr, "Terminated session <%p>\n", s);
#endif
}


/**
 * Set up a server socket and run session initiation checks.
 * Involves looking at Scribble files specified by both session initiation
 * to extract the roles.
 *
 * This is obsolete and will be removed in a future version.
 */
role *sess_server(void *ctx, int type, const char *uri, const char *scribble)
{
#ifdef __DEBUG__
  fprintf(stderr, " server_init\n   * bind(%s)\n   * Scribble(%s)\n", uri, scribble);
#endif

  role *r = zmq_socket(ctx, type);
  char *their_scribble;
  st_node *r_node;
  st_node *node;

  if (zmq_bind(r, uri) != 0) perror(__FUNCTION__);

//  receive_string(r, &their_scribble); // Send scribble filename

//  node   = parse(scribble);
//  r_node = parse(their_scribble);

#ifdef __DEBUG__
  fprintf(stderr, "%s: Session initiaion\n", __FUNCTION__);//, node->role, r_node->role);
#endif

  return r;
}


/**
 * Set up a client socket and run session initiation check.
 * The actual check is done locally in \ref sess_server and this function simply
 * forwards the Scribble filename to \ref sess_server.
 *
 * This is obsolete and will be removed in a future version.
 */
role *sess_client(void *ctx, int type, const char *uri, const char *scribble)
{
#ifdef __DEBUG__
  fprintf(stderr, " client_init\n   * connect(%s)\n   * Scribble(%s)\n", uri, scribble);
#endif

  role *r = zmq_socket(ctx, type);

  if (zmq_connect(r, uri) != 0) perror(__FUNCTION__);

//  send_string(r, scribble); // Receive scribble filename

  return r;
}


/* ----- Send --------------------------------------------------------------- */


int send_int(role *r, int val)
{
  int rc = 0;
  zmq_msg_t msg;

#ifdef __DEBUG__
  fprintf(stderr, " --> %s(%d) ", __FUNCTION__, val);
#endif

  int *buf = malloc(sizeof(int));
  memcpy(buf, &val, sizeof(int));

  zmq_msg_init_data(&msg, buf, sizeof(int), _dealloc, NULL);
  rc = zmq_send(r, &msg, 0);
  zmq_msg_close(&msg);
 
#ifdef __DEBUG__
  fprintf(stderr, ".\n");
#endif

  return rc;
}


int send_int_array(role *r, const int arr[], size_t length)
{
  int rc = 0;
  zmq_msg_t msg;
  size_t size = sizeof(int) * length;

#ifdef __DEBUG__
  fprintf(stderr, " --> %s(size=%zu) ", __FUNCTION__, size);
#endif

  // Copy to send buffer
  int *send_buffer = (int *)malloc(size);
  memcpy(send_buffer, arr, size);

  zmq_msg_init_data(&msg, send_buffer, size, _dealloc, NULL);
  rc = zmq_send(r, &msg, 0);
  zmq_msg_close(&msg);
 
#ifdef __DEBUG__
  fprintf(stderr, ".\n");
#endif

  return rc;
}


int send_char(role *r, char val)
{
  int rc = 0;
  zmq_msg_t msg;

#ifdef __DEBUG__
  fprintf(stderr, " --> %s(%d) ", __FUNCTION__, val);
#endif

  int *buf = malloc(sizeof(char));
  memcpy(buf, &val, sizeof(char));

  zmq_msg_init_data(&msg, buf, sizeof(char), _dealloc, NULL);
  rc = zmq_send(r, &msg, 0);
  zmq_msg_close(&msg);
 
#ifdef __DEBUG__
  fprintf(stderr, ".\n");
#endif

  return rc;
}


int send_string(role *r, const char *string)
{
  int rc = 0;
  size_t size = strlen(string);
  zmq_msg_t msg;

#ifdef __DEBUG__
  char *send_buffer = (char *)malloc(size + 1);
  memcpy(send_buffer, string, size);
  send_buffer[size] = 0;
  fprintf(stderr, " --> %s(%s/%zu) ", __FUNCTION__, send_buffer, size);
#else
  char *send_buffer = (char *)malloc(size);
  strncpy(send_buffer, string, size);
#endif

  zmq_msg_init_data(&msg, send_buffer, size, _dealloc, NULL);
  rc = zmq_send(r, &msg, 0);
  zmq_msg_close(&msg);
 
#ifdef __DEBUG__
  fprintf(stderr, ".\n");
#endif

  return rc;
}


int send_double(role *r, double val)
{
  int rc = 0;
  zmq_msg_t msg;

#ifdef __DEBUG__
  fprintf(stderr, " --> %s(%f) ", __FUNCTION__, val);
#endif

  double *buf = malloc(sizeof(double));
  memcpy(buf, &val, sizeof(double));

  zmq_msg_init_data(&msg, buf, sizeof(double), _dealloc, NULL);
  rc = zmq_send(r, &msg, 0);
  zmq_msg_close(&msg);
 
#ifdef __DEBUG__
  fprintf(stderr, ".\n");
#endif

  return rc;
}


int send_double_array(role *r, const double arr[], size_t length)
{
  int rc = 0;
  zmq_msg_t msg;
  size_t size = sizeof(double) * length;

#ifdef __DEBUG__
  fprintf(stderr, " --> %s(size=%zu) ", __FUNCTION__, size);
#endif

  // Copy to send buffer
  double *send_buffer = (double *)malloc(size);
  memcpy(send_buffer, arr, size);

  zmq_msg_init_data(&msg, send_buffer, size, _dealloc, NULL);
  rc = zmq_send(r, &msg, 0);
  zmq_msg_close(&msg);
 
#ifdef __DEBUG__
  fprintf(stderr, ".\n");
#endif

  return rc;
}


/* ----- Receive ------------------------------------------------------------ */


int receive_int(role *r, int **dst)
{
  int rc = 0;
  zmq_msg_t msg;

#ifdef __DEBUG__
  fprintf(stderr, " <-- %s() ", __FUNCTION__);
#endif

  zmq_msg_init(&msg);
  rc = zmq_recv(r, &msg, 0);
  *dst = (int *)malloc(sizeof(int));
  assert(zmq_msg_size(&msg) == sizeof(int));
  memcpy(*dst, (int *)zmq_msg_data(&msg), zmq_msg_size(&msg));
  zmq_msg_close(&msg);

#ifdef __DEBUG__
  fprintf(stderr, "[%d] .\n", **dst);
#endif

  return rc;
}


int recv_int(role *r, int *dst)
{
  int rc = 0;
  zmq_msg_t msg;

#ifdef __DEBUG__
  fprintf(stderr, " <-- %s() ", __FUNCTION__);
#endif

  zmq_msg_init(&msg);
  rc = zmq_recv(r, &msg, 0);
  assert(zmq_msg_size(&msg) == sizeof(int));
  memcpy(dst, (int *)zmq_msg_data(&msg), zmq_msg_size(&msg));
  zmq_msg_close(&msg);

#ifdef __DEBUG__
  fprintf(stderr, "[%d] .\n", *dst);
#endif

  return rc;
}


int receive_int_array(role *r, int **arr, size_t *length)
{
  int rc = 0;
  zmq_msg_t msg;
  size_t size = -1;

#ifdef __DEBUG__
  fprintf(stderr, " <-- %s() ", __FUNCTION__);
#endif

  zmq_msg_init(&msg);
  rc = zmq_recv(r, &msg, 0);
  size = zmq_msg_size(&msg);
  *arr = (int *)malloc(size);
  memcpy(*arr, (int *)zmq_msg_data(&msg), size);
  if (size % sizeof(int) == 0) {
    *length = size / sizeof(int);
  }
  zmq_msg_close(&msg);

#ifdef __DEBUG__
  fprintf(stderr, "[%d/%zu] .\n", **arr, *length);
#endif

  return rc;
}


int recv_int_array(role *r, int *arr, size_t *arr_size)
{
  int rc = 0;
  zmq_msg_t msg;
  size_t size = -1;

#ifdef __DEBUG__
  fprintf(stderr, " <-- %s() ", __FUNCTION__);
#endif

  zmq_msg_init(&msg);
  rc = zmq_recv(r, &msg, 0);
  size = zmq_msg_size(&msg);
  if (*arr_size * sizeof(int) >= size) {
    memcpy(arr, (int *)zmq_msg_data(&msg), size);
    if (size % sizeof(int) == 0) {
      *arr_size = size / sizeof(int);
    }
  } else {
    memcpy(arr, (int *)zmq_msg_data(&msg), *arr_size * sizeof(int));
    fprintf(stderr,
      "%s: Received data (%zu bytes) > memory size (%zu), data truncated\n",
      __FUNCTION__, size, *arr_size);
  }
  zmq_msg_close(&msg);

#ifdef __DEBUG__
  fprintf(stderr, "[%d/%zu] .\n", *arr, *arr_size);
#endif

  return rc;
}


int receive_char(role *r, char **dst)
{
  int rc = 0;
  zmq_msg_t msg;

#ifdef __DEBUG__
  fprintf(stderr, " <-- %s() ", __FUNCTION__);
#endif

  zmq_msg_init(&msg);
  rc = zmq_recv(r, &msg, 0);
  *dst = (char *)malloc(sizeof(char));
  assert(zmq_msg_size(&msg) == sizeof(char));
  memcpy(*dst, (char *)zmq_msg_data(&msg), zmq_msg_size(&msg));
  zmq_msg_close(&msg);

#ifdef __DEBUG__
  fprintf(stderr, "[%c] .\n", **dst);
#endif

  return rc;
}


int recv_char(role *r, char *dst)
{
  int rc = 0;
  zmq_msg_t msg;

#ifdef __DEBUG__
  fprintf(stderr, " <-- %s() ", __FUNCTION__);
#endif

  zmq_msg_init(&msg);
  rc = zmq_recv(r, &msg, 0);
  assert(zmq_msg_size(&msg) == sizeof(char));
  memcpy(dst, (char *)zmq_msg_data(&msg), zmq_msg_size(&msg));
  zmq_msg_close(&msg);

#ifdef __DEBUG__
  fprintf(stderr, "[%c] .\n", *dst);
#endif

  return rc;
}


int receive_string(role *r, char **dst)
{
  int rc = 0;
  size_t size = -1;
  zmq_msg_t msg;

#ifdef __DEBUG__
  fprintf(stderr, " <-- %s() ", __FUNCTION__);
#endif

  zmq_msg_init(&msg);
  rc = zmq_recv(r, &msg, 0);
  size = zmq_msg_size(&msg);
  *dst = (char *)malloc(size + 1);
  strncpy(*dst, zmq_msg_data(&msg), size);
  (*dst)[size] = 0; // NULL-terminate
  zmq_msg_close(&msg);

#ifdef __DEBUG__
  fprintf(stderr, "[%s/%zu] .\n", *dst, size);
#endif

  return rc;
}


int receive_double(role *r, double **dst)
{
  int rc = 0;
  zmq_msg_t msg;

#ifdef __DEBUG__
  fprintf(stderr, " <-- %s() ", __FUNCTION__);
#endif

  zmq_msg_init(&msg);
  rc = zmq_recv(r, &msg, 0);
  *dst = (double *)malloc(sizeof(double));
  assert(zmq_msg_size(&msg) == sizeof(double));
  memcpy(*dst, (double *)zmq_msg_data(&msg), zmq_msg_size(&msg));
  zmq_msg_close(&msg);

#ifdef __DEBUG__
  fprintf(stderr, "[%f] .\n", **dst);
#endif

  return rc;
}


int recv_double(role *r, double *dst)
{
  int rc = 0;
  zmq_msg_t msg;

#ifdef __DEBUG__
  fprintf(stderr, " <-- %s() ", __FUNCTION__);
#endif

  zmq_msg_init(&msg);
  rc = zmq_recv(r, &msg, 0);
  assert(zmq_msg_size(&msg) == sizeof(double));
  memcpy(dst, (double *)zmq_msg_data(&msg), zmq_msg_size(&msg));
  zmq_msg_close(&msg);

#ifdef __DEBUG__
  fprintf(stderr, "[%f] .\n", *dst);
#endif

  return rc;
}


int receive_double_array(role *r, double **arr, size_t *length)
{
  int rc = 0;
  zmq_msg_t msg;
  size_t size = -1;

#ifdef __DEBUG__
  fprintf(stderr, " <-- %s() ", __FUNCTION__);
#endif

  zmq_msg_init(&msg);
  rc = zmq_recv(r, &msg, 0);
  size = zmq_msg_size(&msg);
  *arr = (double *)malloc(size);
  memcpy(*arr, (double *)zmq_msg_data(&msg), size);
  if (size % sizeof(double) == 0) {
    *length = size / sizeof(double);
  }
  zmq_msg_close(&msg);

#ifdef __DEBUG__
  fprintf(stderr, "[%f/%zu] .\n", **arr, *length);
#endif

  return rc;
}


int recv_double_array(role *r, double *arr, size_t *arr_size)
{
  int rc = 0;
  zmq_msg_t msg;
  size_t size = -1;

#ifdef __DEBUG__
  fprintf(stderr, " <-- %s() ", __FUNCTION__);
#endif

  zmq_msg_init(&msg);
  rc = zmq_recv(r, &msg, 0);
  size = zmq_msg_size(&msg);
  if (*arr_size * sizeof(double) >= size) {
    memcpy(arr, (double *)zmq_msg_data(&msg), size);
    if (size % sizeof(double) == 0) {
      *arr_size = size / sizeof(double);
    }
  } else {
    memcpy(arr, (double *)zmq_msg_data(&msg), *arr_size * sizeof(double));
    fprintf(stderr,
      "%s: Received data (%zu bytes) > memory size (%zu), data truncated\n",
      __FUNCTION__, size, *arr_size);
  }
  zmq_msg_close(&msg);

#ifdef __DEBUG__
  fprintf(stderr, "[%f/%zu] .\n", *arr, *arr_size);
#endif

  return rc;
}


/* ----- Choice wrappers ---------------------------------------------------- */


inline int outbranch(role *r, const int choice)
{
  return send_int(r, choice);
}


inline int inbranch(role *r, int **choice)
{
  return receive_int(r, choice);
}


/* ----- Iteration ---------------------------------------------------------- */

/**
 * Distributed while loop, forwards loop condition to all roles in argument.
 * Uses varyarg to take variable number of roles but need to specify how many.
 * The dual is \ref inwhile.
 */
int outwhile(int cond, int nr_of_roles, ...)
{
  int i;
  role *r;
  va_list roles;

#ifdef __DEBUG__
  fprintf(stderr, " --> outwhile@%d {\n", nr_of_roles);
#endif

  va_start(roles, nr_of_roles);
  for (i=0; i<nr_of_roles; i++) {
    r = va_arg(roles, role *);
#ifdef __DEBUG__
    fprintf(stderr, "   +");
#endif
    send_int(r, cond);
  }
  va_end(roles);

#ifdef __DEBUG__
  fprintf(stderr, " } cond=%d\n", cond);
#endif

  return cond;
}


/**
 * Distributed while loop, collects loop condition from all roles in argument.
 * Uses varyarg to take variable number of roles but need to specify how many.
 * If any of the roles send a conflicting condition, it returns -1 and prints
 * an error message.
 * The dual is \ref outwhile.
 *
 */
int inwhile(int nr_of_roles, ...)
{
  int i;
  role *r;
  int cond = -1;
  int tmp_cond;
  va_list roles;

#ifdef __DEBUG__
  fprintf(stderr, " <-- inwhile@%d {\n", nr_of_roles);
#endif

  va_start(roles, nr_of_roles);
  for (i=0; i<nr_of_roles; i++) {
    r = va_arg(roles, role *);
#ifdef __DEBUG__
    fprintf(stderr, "   -");
#endif
    recv_int(r, &tmp_cond);
    if (i==0) cond = tmp_cond;
    if (cond ^ tmp_cond) {
      fprintf(stderr, "Warning: inwhile condition mismatch!\n");
      va_end(roles); // Cleanup
      return -1;
    }
  }
  va_end(roles);

  if (cond == -1) {
    fprintf(stderr, "Error: inwhile with no roles!\n");
    return -1;
  }

#ifdef __DEBUG__
  fprintf(stderr, " } cond=%d\n", cond);
#endif

  return cond;
}

/**
 * Distributed while loop, forwards loop condition to all roles in argument.
 * Uses varyarg to take variable number of roles but need to specify how many.
 *
 * This version synchronises between the roles by requiring a reply from all
 * roles.
 * The dual is \ref s_inwhile.
 *
 */
int s_outwhile(int cond, int nr_of_roles, ...)
{
  int i;
  role *r;
  va_list roles;
  int *sync_reply = NULL;

#ifdef __DEBUG__
  fprintf(stderr, " --> outwhile@%d {\n", nr_of_roles);
#endif

  va_start(roles, nr_of_roles);
  for (i=0; i<nr_of_roles; i++) {
    r = va_arg(roles, role *);
#ifdef __DEBUG__
    fprintf(stderr, "   +");
#endif
    send_int(r, cond);
  }
  va_end(roles);

  va_start(roles, nr_of_roles);
  for (i=0; i<nr_of_roles; i++) {
    r = va_arg(roles, role *);
#ifdef __DEBUG__
    fprintf(stderr, "   +s:"); // Sync step
#endif
    receive_int(r, &sync_reply);
    assert(*sync_reply == OUTWHILE_SYNC_MAGIC);
  }
  va_end(roles);

#ifdef __DEBUG__
  fprintf(stderr, " } cond=%d\n", cond);
#endif

  return cond;
}


/**
 * Distributed while loop, collects loop condition from all roles in argument.
 * Uses varyarg to take variable number of roles but need to specify how many.
 * If any of the roles send a conflicting condition, it returns -1 and prints
 * an error message.
 *
 * This version synchronises between the roles by issuing a reply to all roles.
 * The dual is \ref s_outwhile.
 *
 */
int s_inwhile(int nr_of_roles, ...)
{
  int i;
  role *r;
  int cond = -1;
  int *tmp_cond = NULL;
  va_list roles;

#ifdef __DEBUG__
  fprintf(stderr, " <-- inwhile@%d {\n", nr_of_roles);
#endif

  va_start(roles, nr_of_roles);
  for (i=0; i<nr_of_roles; i++) {
    r = va_arg(roles, role *);
#ifdef __DEBUG__
    fprintf(stderr, "   -");
#endif
    receive_int(r, &tmp_cond);
    if (i==0) cond = *tmp_cond;
    if (cond ^ *tmp_cond) {
      fprintf(stderr, "Warning: inwhile condition mismatch!\n");
      free(tmp_cond);
      va_end(roles); // Cleanup
      return -1;
    }
  }
  free(tmp_cond);
  va_end(roles);

  va_start(roles, nr_of_roles);
  for (i=0; i<nr_of_roles; i++) {
    r = va_arg(roles, role *);
#ifdef __DEBUG__
    fprintf(stderr, "   -s: ");
#endif
    send_int(r, OUTWHILE_SYNC_MAGIC); // Synchronisation
  }
  va_end(roles);

  if (cond == -1) {
    fprintf(stderr, "Error: inwhile with no roles!\n");
    return -1;
  }

#ifdef __DEBUG__
  fprintf(stderr, " } cond=%d\n", cond);
#endif

  return cond;
}
