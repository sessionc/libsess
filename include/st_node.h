#ifndef COMMON__ST_NODE_H__
#define COMMON__ST_NODE_H__
/**
 * \file
 * This file contains the graph representation of (multiparty) session types
 * and provides functions to build and manipulate session type graphs.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#define BEGIN_NODE  0
#define SEND_NODE   1
#define RECV_NODE   2
#define BRANCH_NODE 3
#define SELECT_NODE 4
#define OUTWHILE_NODE 5
#define INWHILE_NODE  6
#define OUTBRANCH_NODE 7
#define INBRANCH_NODE 8
#define RECUR_NODE    9

/**
 * A node in the session type flow graph (internal use).
 */
struct __st_node {
    unsigned type;
    char role[255]; // XXX 255 chars enough for type name?
    char datatype[255]; // XXX 255 chars enough for type name?
    char branchtag[255];
    unsigned next_sz; // Number of elements in next[]
    /* unsigned ref_cnt; */
    struct __st_node **next;
};


typedef struct __st_node st_node; ///< Alias to the struct.


/**
 * \brief Convenient function to initialise session type node.
 *
 * @param[in] node     Node to initialise.
 * @param[in] type     Type of node.
 * @param[in] role     Target role of node.
 * @param[in] datatype Datatype of node.
 *
 * \returns Initialised node.
 */
st_node *init_st_node(st_node *node, unsigned type,
                      const char *role, const char *datatype);//, const char *tag);


/**
 * \brief Convenient function to free a st_node tree.
 * 
 * @param[in] node Root of session tree.
 */
void free_st_node(st_node *node);


/**
 * \brief Convenient function to append a new node to an existing one.
 *
 * @param[in] node Parent node.
 * @param[in] next New child node.
 *
 * \returns extended node tree.
 */
st_node *append_st_node(st_node *node, st_node *next);


/**
 * \brief Convenient function to delete the last child node of given node
 *
 * @param[in] node Parent node.
 *
 * \returns modified node.
 */
st_node *delete_last_st_node(st_node *node);


/**
 * \brief Pretty-print the given node in a debugging format.
 *
 * @param[in] node   Node to print.
 * @param[in] indent Indentation (number of spaces).
 */
void print_st_node(st_node *node, unsigned indent);


/**
 * \brief Compare two st_nodes and check if they are the same.
 *
 * @param[in] node  First node to compare. 
 * @param[in] other The other node to compare. 
 *
 * \returns 1 if same, 0 otherwise.
 */
int compare_st_node(st_node *node, st_node *other);


/**
 * \brief Normalise AST tree. 
 *
 * @param[in,out] root Root of tree to normalise.
 */
void normalise(st_node *root);

/**
 * \brief Shift elements Started from given index one position left. 
 * 
 * @param[in] st_node  
 * 
 */
void shiftElementsLeft(st_node *node, int index);


/**
 * \brief Shift elements Started from given index one position right. 
 * 
 * @param[in] st_node  
 * 
 */
void shiftElementsRight(st_node *node, int index);


/**
 * \brief Convenient function to insert a node  at specified position.
 *
 * @param[in] node Parent node.
 * @param[in] next New child node.
 * @param[in] index to insert
 * \returns extended node tree.
 */
st_node *insert_st_node_at(st_node *node, st_node *next, int index);


/**
 * \brief Remove empty branch node in the tree.
 * 
 * @param[in,out] st_node  
 * 
 */
void remove_empty_branch_node(st_node *root);

// int cmp_branchtag(const void *a, const void *b);

/**
 * \brief Sort branches by their string branchtag.
 *
 * @param[in,out] st_node
 *
 */
void sort_branches(st_node *root);

#ifdef __cplusplus
}
#endif

#endif // COMMON__ST_NODE_H__
