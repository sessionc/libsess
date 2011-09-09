/**
 * \file
 * This file contains the graph representation of (multiparty) session types
 * and provides functions to build and manipulate session type graphs.
 *
 * \headerfile "st_node.h"
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "st_node.h"
#include "stack.h"

const char *node_type[] = {
  "begin",  // 0
  "!",      // 1
  "?",      // 2
  "branch", // 3
  "select", // 4
  "outwhile",  // 5
  "inwhile",   // 6
  "outbranch", // 7
  "inbranch",  // 8
  "recur",     // 9
};

stackli *stack, *_stack;

int _asyncmsg_compare_st_node(st_node *node, st_node *other);
int _compare_st_node(st_node *node, st_node *other);


/**
 * Sets up node mechanically using given parameters.
 */
st_node *init_st_node(st_node *node, unsigned type,
                      const char *role, const char *datatype)
{
  node->type = type;
  strncpy(node->role, role, 255);
  strncpy(node->datatype, datatype, 255);
  node->role[254] = 0;     // cutoff at 255 char
  node->datatype[254] = 0; // cutoff at 255 char
  node->branchtag[0] = 0;
  
  node->next_sz = 0;
  return node;
}


/**
 * Free the st_node tree by walking the tree and reference counting.
 */
void free_st_node(st_node *node)
{
  unsigned i = 0;

  if (node) {
    for (i=0; i<node->next_sz; ++i)
      free_st_node(node->next[i]);

    free(node);
  }
}


/**
 * Grow the tree by changing the next pointers.
 * If the node already has successors, convert the node to a multi-successor
 * node but will warn if the new successor does not have a unique branchtag.
 */
st_node *append_st_node(st_node *node, st_node *next)
{
  if (node->next_sz <= 0) {
    node->next_sz = 1;
    node->next = (st_node **)malloc(sizeof(st_node *));
  } else {
    node->next_sz++;
    node->next = (st_node **)realloc(node->next,
      node->next_sz * sizeof(st_node *));
  }
      
  node->next[node->next_sz-1] = next;

  return node; // For chaining
}


/**
 * Delete the last child node.
 */
st_node *delete_last_st_node(st_node *node)
{
  st_node *tmp;
  if(node->next_sz > 0) {
    tmp = node->next[node->next_sz-1];
    node->next_sz--;
    free_st_node(tmp);
  }

  return node;
}


/**
 * Pretty-print the ST tree rooted at node.
 */
void print_st_node(st_node *node, unsigned indent)
{
  unsigned i = 0;
  for (i=0; i<indent; ++i) printf("  ");

  if (node) {
    printf(
      "st_node <%p type=%s role=%s datatype=%s tag=%s next_sz=%d>\n",
      (void *)node, node_type[node->type], node->role,
      node->datatype, node->branchtag, node->next_sz);

    for (i=0; i<node->next_sz; ++i)
        print_st_node(node->next[i], indent + 1);
  }
}


/**
 * Recursive step of compare function.
 */
int _compare_st_node(st_node *node, st_node *other)
{
  int i;
  int cmp_result = 0;

  // Corrupted nodes/different ST tree.
  if ((node == NULL && other != NULL) || (node != NULL && other == NULL)) {
    return 0;
  }

  cmp_result = (node->type == other->type
                && (strcmp(node->role, other->role) == 0)
                && (strcmp(node->datatype, other->datatype) == 0)
                && (strcmp(node->branchtag, other->branchtag) == 0)
                && node->next_sz == other->next_sz);

  if (cmp_result) { // Node is identical.

    if (node->type == RECUR_NODE) {

      cmp_result &= _asyncmsg_compare_st_node(node, other);

    } else { // Check child nodes (normal).

      push(stack, node);
      push(_stack, other);

      for (i=0; i<node->next_sz; ++i) {
        cmp_result &= _compare_st_node(node->next[i], other->next[i]);
      }

      pop(stack);
      pop(_stack);

    }

  } else { // Node is NOT identical.

    fprintf(stderr, "Type checking failed at this node:\n");
    print_st_node(node, 0);
    fprintf(stderr, "----------\nAnd this node:\n");
    print_st_node(other, 0);

  }

  return cmp_result;
}


int _asyncmsg_compare_st_node(st_node *node, st_node *other)
{
  int i, j;
  int cmp_result = 0;
  int visited[node->next_sz];

  assert(node->type == RECUR_NODE && other->type == RECUR_NODE);

  // Set bounds for permuted send/recv nodes search space.
  int search_bound = 0;

  for (i=0; i<node->next_sz; ++i) {
    visited[i] = 0;
  }

  // Run through all nodes once.
  for (i=0; i<node->next_sz; ++i) {
    if (node->next[i]->type == SEND_NODE || node->next[i]->type == RECV_NODE) {

      // Search for the last permutable node.
      search_bound = node->next_sz-1;
      for (j=i+1; j<node->next_sz; ++j) {
        if (node->next[j]->type != SEND_NODE && node->next[j]->type != RECV_NODE) {
          search_bound = j;
        }
      }

      // Start node: i, end node: search_bound - 1.

      if (other->next[i]->type == RECV_NODE) {

        //
        // RECV_NODE may overtake SEND_NODE.
        //

        for (j=0; j<=search_bound; ++j) {

          // Find next available node in the same channel.
          if (visited[j] == 0 && strcmp(other->next[i]->role, node->next[j]->role) == 0) {

            // Only look for RECV_NODE.
            if (node->next[j]->type == RECV_NODE) {

              if (strcmp(node->next[j]->datatype, other->next[i]->datatype) == 0
                  && (strcmp(node->next[j]->branchtag, other->next[i]->branchtag) == 0)
                  && node->next[j]->next_sz == other->next[i]->next_sz) {

                // RECV_NODE found.
                visited[j] = 1;
                break; // Next node.

              } else {

                // RECV_NODE found but type error.
                fprintf(stderr, "No matching RECV_NODE for REC child %d\n", i);
                return 0;

              }

            } // RECV_NODE
          } // next node in same channel.

          if (j == search_bound) {
            // No RECV_NODE found.
            fprintf(stderr, "No RECV_NODE found.\n");
            return 0;
          }

        }

      } else { // other->next[i]->type == SEND_NODE

        //
        // SEND_NODE must not overtake RECV_NODE.
        //

        for (j=0; j<=search_bound; ++j) {

          // Find next available node in the same channel.
          if (visited[j] == 0 && strcmp(other->next[i]->role, node->next[j]->role) == 0) {

            // Check for identical SEND_NODE.
            if (node->next[j]->type == SEND_NODE
                && (strcmp(node->next[j]->datatype, other->next[i]->datatype) == 0)
                && (strcmp(node->next[j]->branchtag, other->next[i]->branchtag) == 0)
                && node->next[j]->next_sz == other->next[i]->next_sz) {

              // SEND_NODE found.
              visited[j] = 1;
              break; // Next node.

            } else {

              // SEND_NODE found but type error.
              fprintf(stderr, "No matching SEND_NODE for REC child %d\n", i);
              return 0;

            }

          } // next node in same channel.

          if (j == search_bound) {
            // No SEND_NODE found.
            fprintf(stderr, "No SEND_NODE found.\n");
            return 0;
          }

        }

      } // other->next[i]->type == SEND_NODE | RECV_NODE.

    } else { // Not SEND_NODE/RECV_NODE.

      // TODO: Make sure other->next[i] and node->next[i] are identical.

      // Mark for checking later.
      visited[i] = 2;

    }
  }


  // Passed async message optimisation type-checking.
  cmp_result = 1;

  // Check non SEND_NODE/RECV_NODE.
  for (i=0; i<node->next_sz; ++i) {
    switch (visited[i]) {
      case 0: // Not visited.
        fprintf(stderr, "Some nodes are not mark as visited at %d.\n", i);
        assert(1 /*This can't be correct because node and other have the same next_sz*/);
        return 0;
        break;

      case 1: // Visited.
        break;

      case 2: // Marked for 'normal' type-checking.
        push(stack, node);
        push(_stack, other);

        cmp_result &= _compare_st_node(node->next[i], other->next[i]);

        pop(stack);
        pop(_stack);
        break;
    }
  }

  return cmp_result;
}


/**
 * Compares two session type graph and see if they are the same.
 * This is the toplevel compare function.
 */
int compare_st_node(st_node *node, st_node *other)
{
  int i;
  int cmp_result = 0;

  if ((node == NULL && other != NULL) || (node != NULL && other == NULL)) {
    fprintf(stderr, "One of the st_node tree is NULL.\n");
    return 0;
  }

  // Check if both are root node.
  if (node->type == BEGIN_NODE && other->type == BEGIN_NODE) {

    cmp_result = 1;
    if (node->next_sz == other->next_sz) {

      init_stack(&stack);
      init_stack(&_stack);

      // Root nodes.
      push(stack, node);
      push(_stack, other);

      // Check the child nodes.
      for (i=0; i<node->next_sz; ++i) {
        cmp_result &= _compare_st_node(node->next[i], other->next[i]);

        if (!cmp_result) {
          fprintf(stderr, "Type checking failed at this node:\n");
          print_st_node(node->next[i], 0);
          fprintf(stderr, "----------\nAnd this node:\n");
          print_st_node(other->next[i], 0);
        }
      }

      // Root nodes.
      pop(stack);
      pop(_stack);

      free_stack(stack);
      free_stack(_stack);
    } else {

      // Different next_sz.
      cmp_result = 0;
    }

    return cmp_result;
  }

  // Not BEGIN_NODE.
  return 0;
}

/**
 * Normalise tree.
 * Remove empty or meaningless nodes
 * and get a canonical ST tree from given root.
 *
 * 1. Remove empty branch node 
 * 2. Convert BRANCH-->BRANCH--> OUTBRANCH...  ===>> BRANCH-->OUTBRANCH...
 */
void normalise(st_node *root)
{
  unsigned i, j;
  st_node *node = root;
  st_node *child;

  if (root) {

    // Branch node normalisation.
    if (node->type == BRANCH_NODE) {

      for (i=0; i<node->next_sz; ++i) {
        if (node->next[i]) {
          // BRANCH_NODE -> BRANCH_NODE (w/o intermediate OUTBRANCH/INBRANCH).
          if (node->next[i]->type == BRANCH_NODE) {
            child = node->next[i];
            for (j=0; j<child->next_sz; ++j) {
              // Move st_node
              //  from node->next[i]->next[j]
              //  to   node->next[j+1]
               shiftElementsRight(node, j+1);
              insert_st_node_at(node, child->next[j], j+1);
              child->next[j] = NULL;
              shiftElementsLeft(child, j);
              --j; 
            }
          }
        }
      }

    } else { // Not BRANCH_NODE.

      for (i=0; i<node->next_sz; ++i) {
        normalise(node->next[i]);
      }

    }
        
  }

}


void shiftElementsLeft(st_node *node, int index)
{
  int i;
  for(i=index; i<node->next_sz-1; ++i) {
      node->next[i] = node->next[i+1];
  }
  node->next[node->next_sz-1] = NULL;
  node->next_sz--;
}


void shiftElementsRight(st_node *node, int index)
{
  int i;
  node->next_sz++;
  node->next = (st_node **)realloc(node->next,
                                   node->next_sz * sizeof(st_node *));
  for(i=node->next_sz-1; i>index; --i) {
      node->next[i] = node->next[i-1];
  }
  node->next[index] = NULL;
}


st_node *insert_st_node_at(st_node *node, st_node *next, int index)
{
  node->next[index] = next;
  return node;
}


/**
 * Walk the tree and remove empty branches.
 *
 */
void remove_empty_branch_node(st_node *root)
{
  unsigned i;

  if (root) {
    st_node *node = root;
    st_node *child;
    /* printf("test1     %i\n", node->next_sz); */
    for (i=0; i<node->next_sz; ++i) {
      //printf("test1     %i\n", node->next_sz);
      if(node->next[i]->type == BRANCH_NODE) { //BRANCH_NODE LABEL
        child = node->next[i];
        //print_st_node(child, 1);

        if(child->next_sz<=0) {
          //print_st_node(child, 1);
          node->next[i] = NULL;
          shiftElementsLeft(node, i);
          i--;
          free(child);
        }
      }
    }

    for (i=0; i<node->next_sz; ++i) {
      remove_empty_branch_node(node->next[i]);
    }
  }
}


// Helper function for sort_branches
// Compares node branchtag
int cmp_branchtag(const void *a, const void *b)
{
  st_node **aa = a;
  st_node **bb = b;
  return strcmp((*aa)->branchtag, (*bb)->branchtag);
}


/**
 * Sort branch nodes according to their branchtag.
 *
 */
void sort_branches(st_node *root)
{
  unsigned i;
  st_node *node = root;
  if (node->type == BRANCH_NODE) {
    qsort((void *)node->next, node->next_sz, sizeof(st_node *), cmp_branchtag);
  }

  for (i=0; i<node->next_sz; ++i) {
    sort_branches(node->next[i]);
  }
}
