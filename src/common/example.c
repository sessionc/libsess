/**
 * \file
 * Example code to use st_node session type graph builder
 *
 * \headerfile "st_node.h"
 */

#include <stdio.h>
#include <stdlib.h>

#include "st_node.h"


int main(int argc, char *argv[])
{
    st_node *root = (st_node *)malloc(sizeof(st_node));
    init_st_node(root, BEGIN_NODE, "N/A", "Nil", "Nil");

    st_node *node0 = (st_node *)malloc(sizeof(st_node));
    init_st_node(node0, BRANCH_NODE, "N/A", "Nil", "LB1");
    
    st_node *node1 = (st_node *)malloc(sizeof(st_node));
    init_st_node(node1, BRANCH_NODE, "N/A", "Nil", "LB2");

    st_node *node2 = (st_node *)malloc(sizeof(st_node));
    init_st_node(node2, BRANCH_NODE, "N/A", "Nil", "LB3");

    st_node *node3 = (st_node *)malloc(sizeof(st_node));
    init_st_node(node3, BRANCH_NODE, "N/A", "Nil", "LB4");

    st_node *node4 = (st_node *)malloc(sizeof(st_node));
    init_st_node(node4, OUTBRANCH_NODE, "N/A", "Nil", "LB8");


    st_node *node41 = (st_node *)malloc(sizeof(st_node));
    init_st_node(node41, OUTBRANCH_NODE, "N/A", "Nil", "LB1");


    st_node *node42 = (st_node *)malloc(sizeof(st_node));
    init_st_node(node42, OUTBRANCH_NODE, "N/A", "Nil", "LB2");


    st_node *node43 = (st_node *)malloc(sizeof(st_node));
    init_st_node(node43, OUTBRANCH_NODE, "N/A", "Nil", "LB3");


    st_node *node5 = (st_node *)malloc(sizeof(st_node));
    init_st_node(node5, SEND_NODE, "Bob", "Nil", "Nil");
    
    append_st_node(node2, node3);

    append_st_node(node1, node4);
    append_st_node(node1, node41);
    append_st_node(node1, node42);
    append_st_node(node1, node43);
    append_st_node(node1, node2);

    append_st_node(node0, node1);
    append_st_node(root, node0);

    append_st_node(root, node5);

    //print_st_node(root, 2);

    /* printf("***************************************\n"); */
    printf("***************************************\n");

    normalise(root);

    /* print_st_node(root, 2); */

    /* printf("+++++++++++++++++++++++++++++++++++++++\n"); */

    removeEmptyBranchNode(root);
    print_st_node(root, 2);

    printf("***************************************\n");

    sortBranches(root);
    print_st_node(root, 2);
    /* printf("*####################################\n"); */
    /* print_st_node(root, 2); */

    /* if(compString("abce", "abcd")) { */
    /*     printf("b1\n"); */
    /* }else{ */
    /*     printf("b2\n"); */
    /* } */
    
        
        


    return EXIT_SUCCESS;
}
