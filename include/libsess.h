#ifndef __LIBSESS_H__
#define __LIBSESS_H__
/**
 * \file
 * This file is the header file of session C runtime library (libsess)
 * 
 */

#include <stdarg.h>
#include <zmq.h>

typedef void role; ///< Type representing a participant/role


/**
 * \brief Session initiation for the server-side of the communication.
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
