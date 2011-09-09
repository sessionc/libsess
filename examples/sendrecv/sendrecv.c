#include <stdio.h>
#include <stdlib.h>
#include <libsess.h>
#include <zmq.h>

int main(int argc, char *argv[])
{
    int step_counter = 0;

    void *ctx = zmq_init(0);
    role *Ego = sess_server(ctx, ZMQ_PAIR,"inproc://#1", "sendrecv_Ego.spr");
    role *AlterEgo = sess_client(ctx, ZMQ_PAIR, "inproc://#1", "sendrecv_AlterEgo.spr");

    // As Ego
    int *value = NULL;
    char *str = NULL; 
    send_int(AlterEgo, 100); receive_int(Ego, &value);
    printf("%d: Received [%d] = 100\n", ++step_counter, *value);
    free(value);


    send_int(Ego, 242); receive_int(AlterEgo, &value);
    printf("%d: Received [%d] = 242\n", ++step_counter, *value);
    free(value);

    send_int(AlterEgo, 314); receive_int(Ego, &value);
    printf("%d: Received [%d] = 314\n", ++step_counter, *value);
    free(value);

    send_string(AlterEgo, "Say what?"); receive_string(Ego, &str);
    printf("%d: Received [str:%s] = Say what?\n", ++step_counter, str);
    free(str);


    int arr[] = { 42, 2, 3, 4 };
    int *arr_rcvd;
    size_t arr_rcvd_size = 4;
    send_int_array(AlterEgo, arr, 4); recv_int_array(Ego, &arr_rcvd, &arr_rcvd_size);
    printf("%d: Received [int_arr[0]:%d/%zu]\n", ++step_counter, *arr_rcvd, arr_rcvd_size);
    free(arr_rcvd);

    send_string(Ego, "End of conversation"); receive_string(AlterEgo, &str);
    printf("%d: Received [str:%s] = End of conversation\n", ++step_counter, str);
    free(str);

    zmq_close(AlterEgo);
    zmq_close(Ego);
    zmq_term(ctx);
    return EXIT_SUCCESS;
}
