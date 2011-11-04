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


/**
 * Load a hosts file (ie. sequential list of hosts) into memory.
 */
int connmgr_load_hosts(const char *hostsfile, char ***hosts)
{
#ifdef __DEBUG__
  fprintf(stderr, "Loading hosts\n");
#endif
  FILE *hosts_fp;
  int host_idx = 0;
  char buf[MAX_HOSTNAME_LENGTH]; // XXX: Potential buffer overflow.

  *hosts = malloc(sizeof(char *) * MAX_NR_OF_ROLES);

  if ((hosts_fp = fopen(hostsfile, "r")) == NULL) {
    perror("fopen(hostfile)");
    return 0;
  }
  while (fscanf(hosts_fp, "%s", buf) != EOF) {
    *hosts[host_idx] = malloc(sizeof(char) * MAX_HOSTNAME_LENGTH); // XXX
    strncpy(*hosts[host_idx], buf, MAX_HOSTNAME_LENGTH-1);
#ifdef __DEBUG__
    fprintf(stderr, "hosts[%d]=%s\n", host_idx, *hosts[host_idx]);
#endif
    host_idx++;
  }
  fclose(hosts_fp);

  return host_idx;
}


/**
 * Load and parse a global Scribble to extract the roles into memory.
 *
 */
int connmgr_load_roles(const char *scribble, char ***roles)
{
#ifdef __DEBUG__
  fprintf(stderr, "Loading roles\n");
#endif
  // XXX Size of roles[] is MAX_NR_OF_ROLES
  *roles = malloc(sizeof(char *) * MAX_NR_OF_ROLES);
  return parse_roles(scribble, *roles);
}


/**
 * Initialise Connection manager with given roles and hosts list.
 */
int connmgr_init(conn_rec **conns, host_map **role_hosts,
                 char **roles, int roles_count,
                 char **hosts, int hosts_count,
                 int start_port)
{
#ifdef __DEBUG__
  fprintf(stderr, "Generating connection configuration\n");
#endif
  // Allocate memory for conn_rec.
  int nr_of_connections = (roles_count*(roles_count-1))/2; // N * (N-1) / 2
  *conns = (conn_rec *)malloc(sizeof(conn_rec) * nr_of_connections);
  conn_rec *cr = *conns; // Alias.

  // Create a map of role to host.
  *role_hosts = (host_map *)malloc(sizeof(host_map) * roles_count);
  host_map *rh = *role_hosts;

  if (roles_count < hosts_count) {
    fprintf(stderr, "Warning: Number of hosts is less than number of roles.");
  }
  int role_idx, host_idx;
  for (role_idx=0; role_idx<roles_count; ++role_idx) {
    rh[role_idx].role = malloc(sizeof(char) * (strlen(roles[role_idx]) + 1));
    strcpy(rh[role_idx].role, roles[role_idx]);
    rh[role_idx].role[strlen(roles[role_idx])] = 0; // NULL-termination.

    host_idx = role_idx % hosts_count; 
    rh[role_idx].host = malloc(sizeof(char) * (strlen(hosts[host_idx]) + 1));
    strcpy(rh[role_idx].host, hosts[host_idx]);
    rh[role_idx].host[strlen(hosts[host_idx])] = 0; // NULL-termination.
  }

  unsigned port_nr;
  int conn_idx, conn2_idx, map_idx, role2_idx;
  for (role_idx=0, conn_idx=0; role_idx<roles_count; ++role_idx) {
    for (role2_idx=role_idx+1; role2_idx<roles_count; ++role2_idx) {
      assert(conn_idx<nr_of_connections);
      // Set from-role.
      cr[conn_idx].from = malloc(sizeof(char) * (strlen(roles[role_idx]) + 1));
      strncpy(cr[conn_idx].from, roles[role_idx], strlen(roles[role_idx]));
      cr[conn_idx].from[strlen(roles[role_idx])] = 0; // NULL-termination.

      // Set to-role.
      cr[conn_idx].to = malloc(sizeof(char) * (strlen(roles[role2_idx]) + 1));
      strncpy(cr[conn_idx].to, roles[role2_idx], strlen(roles[role2_idx]));
      cr[conn_idx].to[strlen(roles[role2_idx])] = 0; // NULL-termination.

      // Lookup host from role_hosts map.
      for (map_idx=0; map_idx<roles_count; ++map_idx) {
        if (strcmp(cr[conn_idx].to, rh[map_idx].role) == 0) {
          cr[conn_idx].host = malloc(sizeof(char) * (strlen(rh[map_idx].host) + 1));
          strcpy(cr[conn_idx].host, rh[map_idx].host);
          cr[conn_idx].host[strlen(rh[map_idx].host)] = 0; // NULL-termination.
        }
      }

      // Find next unoccupied port.
      port_nr = -1;
      for (conn2_idx=0; conn2_idx<conn_idx; ++conn2_idx) {
        if (strcmp(cr[conn2_idx].to, cr[conn_idx].to) == 0) {
          port_nr = cr[conn2_idx].port;
        }
      }
      cr[conn_idx].port = (port_nr==-1 ? start_port : port_nr+1);

      ++conn_idx;
    }
  }
  return nr_of_connections;
}


/**
 * Write connection record array to file.
 */
void connmgr_write(const char *outfile, const conn_rec conns[], int nr_of_conns,
                                        const host_map role_hosts[], int nr_of_roles)
{
  FILE *out_fp;
  int conn_idx, role_idx;

  if (strcmp(outfile, "-") == 0) {
    out_fp = stdout;
  } else {
    out_fp = fopen(outfile, "w");
  }

  fprintf(out_fp, "%d %d\n", nr_of_roles, nr_of_conns);

  for (role_idx=0; role_idx<nr_of_roles; ++role_idx) {
    fprintf(out_fp, "%s %s\n",
              role_hosts[role_idx].role,
              role_hosts[role_idx].host);
  }

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
int connmgr_read(const char *infile, conn_rec **conns, host_map **role_hosts, int *nr_of_roles)
{
  FILE *in_fp;
  int conn_idx, role_idx;
  int nr_of_conns = 0;

  conn_rec *cr;
  host_map *rh;

  if ((in_fp = fopen(infile, "r")) == NULL) {
    perror("fopen(connsfile)");
    return 0;
  }

  fscanf(in_fp, "%d %d", nr_of_roles, &nr_of_conns);
  *conns      = malloc(sizeof(conn_rec) * nr_of_conns);
  *role_hosts = malloc(sizeof(host_map) * (*nr_of_roles));
  cr = *conns;
  rh = *role_hosts;

  for (role_idx=0; role_idx<*nr_of_roles; ++role_idx) {
    rh[role_idx].role = malloc(sizeof(char) * MAX_HOSTNAME_LENGTH);
    rh[role_idx].host = malloc(sizeof(char) * MAX_HOSTNAME_LENGTH);
    fscanf(in_fp, "%s %s", rh[role_idx].role, rh[role_idx].host);
#ifdef __DEBUG__
    fprintf(stderr, "Debug/%s: #%d %s %s\n",
                      __FUNCTION__, role_idx, rh[role_idx].role, rh[role_idx].host);
#endif
  }

  for (conn_idx=0; conn_idx<nr_of_conns; ++conn_idx) {
    cr[conn_idx].from = malloc(sizeof(char) * MAX_HOSTNAME_LENGTH);
    cr[conn_idx].to   = malloc(sizeof(char) * MAX_HOSTNAME_LENGTH);
    cr[conn_idx].host = malloc(sizeof(char) * MAX_HOSTNAME_LENGTH);
    fscanf(in_fp, "%s %s %s %u\n", cr[conn_idx].from, cr[conn_idx].to, cr[conn_idx].host, &cr[conn_idx].port);
#ifdef __DEBUG__
    fprintf(stderr, "Debug/%s: #%d %s->%s %s:%u\n",
                      __FUNCTION__, conn_idx, cr[conn_idx].from, cr[conn_idx].to, cr[conn_idx].host, cr[conn_idx].port);
#endif
  }

  fclose(in_fp);

  return conn_idx;
}

