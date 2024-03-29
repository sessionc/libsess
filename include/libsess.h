#ifndef __LIBSESS_H__
#define __LIBSESS_H__
/**
 * \file
 * This file is the header file of session C runtime library (libsess)
 * 
 */

#include <stdarg.h>
#include <zmq.h>

#define _Others_idx -1
#define _Others(sess) _Others_idx, sess

typedef void role; ///< Type representing a participant/role

typedef struct {
  char *role_name;
  role *role_ptr;
  char uri[6+255+7]; // tcp:// + FQDN + :port + \0
} endpoint_t;

struct session_t {
  endpoint_t **endpoints; // Array of endpoint pointers.
  unsigned endpoints_count;

  role *(*get_role)(struct session_t *, char *); // lookup function.
  char *all_roles[255];
  unsigned all_roles_count;
  void *ctx; // Extra data.
};
typedef struct session_t session;


/**
 * \brief Create and join a session.
 *
 * @param[in,out] argc     Command line argument count
 * @param[in,out] argv     Command line argument list
 * @param[out]    s        Pointer to session varible to create
 * @param[in]     scribble Endpoint Scribble file path for this session
 *                         (Must be constant string)
 */
void join_session(int *argc, char ***argv, session **s, const char *scribble);


/**
 * \brief Dump content of an established session.
 *
 * @param[in] s Session to dump
 */
void dump_session(const session *s);

/**
 * \brief Terminate a session.
 *
 * @param[in] s Session to terminate
 */
void end_session(session *s);


/**
 * \brief Session initiation for the server-side of the communication.
 * \deprecated 
 *  Use join_session and end_session instead of explicit channel creation.
 *
 * @param[in] ctx      ZeroMQ context from a zmq_init()
 * @param[in] type     ZeroMQ connection type (eg. ZMQ_PAIR)
 * @param[in] uri      Server URI (eg. tcp://\*:4242)
 * @param[in] scribble Scribble file for this session
 *                     Note that the file must be in the same directory of the
 *                     executable
 *
 * \returns Role handle of server session.
 */
role *sess_server(void *ctx, int type, const char *uri, const char *scribble);


/**
 * \brief Session initiation for the client-side of the communication.
 * \deprecated 
 *  Use join_session and end_session instead of explicit channel creation.
 *
 * @param[in] ctx      ZeroMQ context from a zmq_init()
 * @param[in] type     ZeroMQ connection type (eg. ZMQ_PAIR)
 * @param[in] uri      Server URI (eg. tcp://localhost:4242)
 * @param[in] scribble Scribble file for this session
 *                     Note that the file must be in the same directory of the
 *                     executable
 *
 * \returns Role handle of client session.
 */
role *sess_client(void *ctx, int type, const char *uri, const char *scribble);


/**
 * \brief Send an integer.
 *
 * @param[in] r   Role to send to
 * @param[in] val Value to send
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_send)
 */
int send_int(role *r, int val);


/**
 * \brief Send an integer array.
 *
 * @param[in] r      Role to send to
 * @param[in] arr    Array to send
 * @param[in[ length Size of array
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_send)
 */
int send_int_array(role *r, const int arr[], size_t length);


/**
 * \brief Send a char.
 *
 * @param[in] r   Role to send to
 * @param[in] val Value to send
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_send)
 */
int send_char(role *r, char val);


/**
 * \brief Send a string.
 *
 * @param[in] r   Role to send to
 * @param[in] str NULL-terminated string to send
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_send)
 */
int send_string(role *r, const char *str);


/**
 * \brief Send a double.
 *
 * @param[in] r   Role to send to
 * @param[in] val Value to send
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_send)
 */
int send_double(role *r, double val);


/**
 * \brief Send a double array.
 *
 * @param[in] r      Role to send to
 * @param[in] arr    Array to send
 * @param[in[ length Size of array
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_send)
 */
int send_double_array(role *r, const double arr[], size_t length);


/**
 * \brief Send a float.
 *
 * @param[in] r   Role to send to
 * @param[in] val Value to send
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_send)
 */
int send_float(role *r, float val);


/**
 * \brief Send a float array.
 *
 * @param[in] r      Role to send to
 * @param[in] arr    Array to send
 * @param[in[ length Size of array
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_send)
 */
int send_float_array(role *r, const float arr[], size_t length);

int __send_blob(role *r, const void *blob, size_t length);
int __receive_blob(role *r, void **dst, size_t *length);
int __recv_blob(role *r, void *dst, size_t *length);


/**
 * \brief Receive an integer.
 *
 * @param[in]  r   Role to send to
 * @param[out] dst Pointer-of-pointer to variable storing recevied value
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_recv)
 */
int receive_int(role *r, int **dst);


/**
 * \brief Receive an integer (pre-allocated).
 *
 * @param[in]  r   Role to send to
 * @param[out] dst Pointer to variable storing recevied value
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_recv)
 */
int recv_int(role *r, int *dst);


/**
 * \brief Receive an integer array.
 *
 * @param[in]  r      Role to send to
 * @param[out] arr    Pointer-of-pointer to array storing recevied value
 * @param[out] length Variable storing size of received array
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_recv)
 */
int receive_int_array(role *r, int **arr, size_t *length);


/**
 * \brief Receive an integer array (pre-allocated).
 *
 * @param[in]     r        Role to send to
 * @param[out]    arr      Pointer to array storing recevied value
 * @param[in,out] arr_size Pointer to allocated size of array,
 *                         stores size of received array after execution
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_recv)
 */
int recv_int_array(role *r, int *arr, size_t *arr_size);


/**
 * \brief Receive a char.
 *
 * @param[in]  r   Role to send to
 * @param[out] dst Pointer-of-pointer to variable storing recevied value
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_recv)
 */
int receive_char(role *r, char **dst);


/**
 * \brief Receive a char (pre-allocated).
 *
 * @param[in]  r   Role to send to
 * @param[out] dst Pointer to variable storing recevied value
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_recv)
 */
int recv_char(role *r, char *dst);


/**
 * \brief Receive a string.
 *
 * @param[in]  r   Role to send to
 * @param[out] dst Pointer to variable storing received value
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_send)
 */
int receive_string(role *r, char **dst);


/**
 * \brief Receive a double.
 *
 * @param[in]  r   Role to send to
 * @param[out] dst Pointer-of-pointer to variable storing recevied value
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_recv)
 */
int receive_double(role *r, double **dst);


/**
 * \brief Receive a double (pre-allocated).
 *
 * @param[in]  r   Role to send to
 * @param[out] dst Pointer to variable storing recevied value
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_recv)
 */
int recv_double(role *r, double *dst);


/**
 * \brief Receive a double array.
 *
 * @param[in]  r      Role to send to
 * @param[out] arr    Pointer-of-pointer to array storing recevied value
 * @param[out] length Variable storing size of received array
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_recv)
 */
int receive_double_array(role *r, double **arr, size_t *length);


/**
 * \brief Receive a double array (pre-allocated).
 *
 * @param[in]     r        Role to send to
 * @param[out]    arr      Pointer to array storing recevied value
 * @param[in,out] arr_size Pointer to allocated size of array,
 *                         stores size of received array after execution
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_recv)
 */
int recv_double_array(role *r, double *arr, size_t *arr_size);


/**
 * \brief Receive a float.
 *
 * @param[in]  r   Role to send to
 * @param[out] dst Pointer-of-pointer to variable storing recevied value
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_recv)
 */
int receive_float(role *r, float **dst);


/**
 * \brief Receive a float (pre-allocated).
 *
 * @param[in]  r   Role to send to
 * @param[out] dst Pointer to variable storing recevied value
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_recv)
 */
int recv_float(role *r, float *dst);


/**
 * \brief Receive a float array.
 *
 * @param[in]  r      Role to send to
 * @param[out] arr    Pointer-of-pointer to array storing recevied value
 * @param[out] length Variable storing size of received array
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_recv)
 */
int receive_float_array(role *r, float **arr, size_t *length);


/**
 * \brief Receive a float array (pre-allocated).
 *
 * @param[in]     r        Role to send to
 * @param[out]    arr      Pointer to array storing recevied value
 * @param[in,out] arr_size Pointer to allocated size of array,
 *                         stores size of received array after execution
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_recv)
 */
int recv_float_array(role *r, float *arr, size_t *arr_size);


/**
 * \brief Send an integer to multiple roles.
 *
 * @param[in] val         Value to send
 * @param[in] nr_of_roles Number of roles to send to 
 * @param[in] ...         Variable number (subject to nr_of_roles)
 *                        of role variables
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_recv)
 */
int msend_int(int val, int nr_of_roles, ...);


/**
 * \brief Receive an integer from multiple roles.
 *
 * @param[out] dst         Pointer-of-pointer to array storing received value
 *                         This has to be at least the size of nr_of_roles.
 * @param[in]  nr_of_roles Number of roles to receive from
 * @param[in]  ...         Variable number (subject to nr_of_roles)
 *                         of role variables
 *
 * \returns 0 if successful, -1 otherwise and set errno
 *          (See man page of zmq_recv)
 */
int mrecv_int(int *dst, int nr_of_roles, ...);


int outbranch(role *r, int choice);


int inbranch(role *r, int **choice);


/**
 * \brief Outward iteration primitive.
 *
 * @param[in] cond        Condition of iteration
 * @param[in] nr_of_roles Number of roles to synchronise
 * @param[in] ...         Variable number (subject to nr_of_roles)
 *                        of role variables to synchronise
 *
 * \returns cond
 */
int outwhile(int cond, int nr_of_roles, ...);


/**
 * \brief Passive iterative primitive.
 *
 * @param[in] nr_of_roles Number of roles to synchronise
 * @param[in] ...         Variable number (subject to nr_of_roles)
 *                        of role variables to synchronise
 *
 * \returns Loop condition
 */
int inwhile(int nr_of_roles, ...);


/**
 * \brief Outward iteration primitive (synchronised).
 *
 * @param[in] cond        Condition of iteration
 * @param[in] nr_of_roles Number of roles to synchronise
 * @param[in] ...         Variable number (subject to nr_of_roles)
 *                        of role variables to synchronise
 *
 * \returns cond 
 */
int s_outwhile(int cond, int nr_of_roles, ...);


/**
 * \brief Passive iterative primitive (synchronised).
 *
 * @param[in] nr_of_roles Number of roles to synchronise
 * @param[in] ...         Variable number (subject to nr_of_roles)
 *                        of role variables to synchronise
 *
 * \returns Loop condition
 */
int s_inwhile(int nr_of_roles, ...);

#endif // __LIBSESS_H__
