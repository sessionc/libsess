/**
 * \file
 * Connection manager component of Session C runtime.
 * The main purpose of the manager is to generate a suitable
 * connection configuration file from given global Scribble desciption.
 *
 */

#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connmgr.h"
#include "parser.h"

#define MAX_NR_OF_ROLES 100
#define MAX_HOSTNAME_LENGTH 256


/**
 * Load a hosts file (ie. sequential list of hosts) into memory.
 */
int connmgr_load_hosts(const char *hostsfile, char *hosts[])
{
  FILE *hosts_fp;
  int host_idx = 0;
  char buf[MAX_HOSTNAME_LENGTH]; // XXX: Potential buffer overflow.

  if ((hosts_fp = fopen(hostsfile, "r")) == NULL) {
    perror("fopen(hostfile)");
    return 0;
  }
  while (fscanf(hosts_fp, "%s\n", buf) != EOF) {
    hosts[host_idx] = malloc(sizeof(char)*MAX_HOSTNAME_LENGTH); // XXX
    strncpy(hosts[host_idx], buf, MAX_HOSTNAME_LENGTH-1);
    host_idx++;
  }
  fclose(hosts_fp);

  return host_idx;
}


/**
 * Load and parse a global Scribble to extract the roles into memory.
 *
 */
int connmgr_load_roles(const char *scribble, char *roles[])
{
  // XXX Size of roles[] is MAX_NR_OF_ROLES
  return parse_roles(scribble, roles);
}


/**
 * Initialise Connection manager with given roles and hosts list.
 */
int connmgr_init(conn_rec **conns, char **roles, int roles_count,
                                    char **hosts, int hosts_count,
                                    int start_port)
{
  // Allocate memory for conn_rec.
  int nr_of_connections = (roles_count*(roles_count-1))/2; // N * (N-1) / 2
  *conns = (conn_rec *)malloc(sizeof(conn_rec) * nr_of_connections);
  conn_rec *cr = *conns; // Alias.

  int i, j, k, l;

  unsigned port_nr;
  for (i=0, k=0; i<roles_count; ++i) {
    for (j=i+1; j<roles_count; ++j) {
      assert(k<nr_of_connections);
      cr[k].from = malloc(sizeof(char)*(strlen(roles[i])+1));
      strncpy(cr[k].from, roles[i], strlen(roles[i]));
      cr[k].from[strlen(roles[i])] = 0; // NULL-termination.

      cr[k].to = malloc(sizeof(char)*(strlen(roles[j])+1));
      strncpy(cr[k].to, roles[j], strlen(roles[j]));
      cr[k].to[strlen(roles[j])] = 0; // NULL-termination.

      cr[k].host = malloc(sizeof(char)*(strlen(hosts[j])+1));
      strncpy(cr[k].host, hosts[j], strlen(hosts[j]));
      cr[k].host[strlen(hosts[j])] = 0; // NULL-termination.

      // Find next unoccupied port.
      port_nr = -1;
      for (l=0; l<k; ++l) {
        if (strcmp(cr[l].to, cr[k].to) == 0) {
          port_nr = cr[l].port;
        }
      }
      cr[k].port = (port_nr==-1 ? start_port : port_nr+1);

      ++k;
    }
  }
  return nr_of_connections;
}


/**
 * Write connection record array to file.
 */
void connmgr_write(const char *outfile, const conn_rec conns[], int nr_of_conns, int nr_of_roles)
{
  FILE *out_fp;
  int conn_idx;

  if (strcmp(outfile, "-") == 0) {
    out_fp = stdout;
  } else {
    out_fp = fopen(outfile, "w");
  }

  fprintf(out_fp, "%d %d\n", nr_of_roles, nr_of_conns);

  for (conn_idx=0; conn_idx<nr_of_conns; ++conn_idx) {
    fprintf(out_fp, "%s %s %s %d\n",
              conns[conn_idx].from,
              conns[conn_idx].to,
              conns[conn_idx].host,
              conns[conn_idx].port);
  }
  if (out_fp != stdout) fclose(out_fp);
}


/**
 * Read from file the connection record array.
 */
int connmgr_read(const char *infile, conn_rec **conns, int *nr_of_roles)
{
  FILE *in_fp;
  int conn_idx = 0;
  int nr_of_conns = 0;

  conn_rec *cr;

  if ((in_fp = fopen(infile, "r")) == NULL) {
    perror("fopen(connsfile)");
    return 0;
  }

  fscanf(in_fp, "%d %d\n", nr_of_roles, &nr_of_conns);
  *conns = malloc(sizeof(conn_rec) * nr_of_conns);
  cr = *conns;

  for (conn_idx=0; conn_idx<nr_of_conns; ++conn_idx) {
    cr[conn_idx].from = malloc(sizeof(char) * MAX_HOSTNAME_LENGTH);
    cr[conn_idx].to   = malloc(sizeof(char) * MAX_HOSTNAME_LENGTH);
    cr[conn_idx].host = malloc(sizeof(char) * MAX_HOSTNAME_LENGTH);
    fscanf(in_fp, "%s %s %s %u\n", cr[conn_idx].from, cr[conn_idx].to, cr[conn_idx].host, &cr[conn_idx].port);
#ifdef __DEBUG__
    fprintf(stderr, "Debug/%s: Line %d %s->%s %s:%u\n", __FUNCTION__, conn_idx, cr[conn_idx].from, cr[conn_idx].to, cr[conn_idx].host, cr[conn_idx].port);
#endif
  }

  fclose(in_fp);

  return conn_idx;
}

