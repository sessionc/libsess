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
 *
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
 *
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

  /* if (next->type == BRANCH_NODE && strcmp(next->branchtag, "Nil") == 0) */
  /*     fprintf(stderr, "Warn: Branch node w/o branch tag\n"); */

  /* for (i=0; i<node->next_sz-1; ++i) { */
  /*     if (strcmp(node->next[i]->branchtag, next->branchtag) == 0) */
  /*         fprintf(stderr, "Warn: branchtag clash\n"); */
  /* } */

  return node; // For chaining
}


/**
 *
 *
 */
st_node *delete_last_st_node(st_node *node)
{
  if(node->next_sz > 0) {
    st_node *tmp;
    tmp = node->next[node->next_sz-1];
    node->next_sz--;
    free_st_node(tmp);
  }
  return node;
}


/**
 *
 *
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


/// Compares 1 node only (ignoring successors)
int _compare_st_node(st_node *node, st_node *other)
{
  return (node->type == other->type
          && (strcmp(node->role, other->role) == 0)
          && (strcmp(node->datatype, other->datatype) == 0)
          && (strcmp(node->branchtag, other->branchtag) == 0)
          && node->next_sz == other->next_sz) ||

          // Special case: BEGIN_NODE, stores role name in datatype
          (node->type == BEGIN_NODE
          && (strcmp(node->datatype, other->datatype) == 0)
          && node->next_sz == other->next_sz);
}


/**
 * Compare two session type graph (successor > 0)
 * 
 */
int _visit_and_compare(st_node *root, st_node *other)
{
  int i;
  st_node *node;
  st_node *other_node;
  push(stack, root);
  push(_stack, other);

  while (!isEmpty(stack) || !isEmpty(_stack)) {
    top(stack, &node);
    top(_stack, &other_node);
    pop(stack);
    pop(_stack);

    if(!_compare_st_node(node, other_node))
      return 0;

    for (i=node->next_sz-1; 0<=i; --i) {
      push(stack, node->next[i]);
    }
    for (i=other_node->next_sz-1; 0<=i; --i) {
      push(_stack, other_node->next[i]);
    }
  }
  free_stack(stack);
  free_stack(_stack);

  return 1;
}


/**
 * Compares two session type graph and see if they are the same.
 * Also takes care of unique branches in different order.
 */
int compare_st_node(st_node *node, st_node *other)
{
  init_stack(&stack);
  init_stack(&_stack);

  if ((node == NULL && other != NULL) || (node != NULL && other == NULL)) {
    fprintf(stderr, "One of the st_node tree is NULL.\n");
    return 0;
  }

    // Identical nodes
    /*if (!_compare_st_node(node, other)) {
#ifdef __DEBUG__
        fprintf(stderr, "<%p type=%s role=%s datatype=%s tag=%s next_sz=%d> != <%p type=%s role=%s datatype=%s tag=%s next_sz=%d>\n", 
                (void *)node, node_type[node->type], node->role,
                node->datatype, node->branchtag, node->next_sz,
                (void *)other, node_type[other->type], other->role,
                other->datatype, other->branchtag, other->next_sz);
#endif
        return 0;
    }*/
    // No successor
    if (node->next_sz == 0) return 1;
    else if (_visit_and_compare(node, other)) return 1;
    else return 0;	    

    /* // 1 successor */
    /* if (node->next_sz == 1) { */
    /*     return compare_st_node(node->next[0], other->next[0]); */
    /* } */


    /* // Disallow tagclash or multitag */
    /* for (i=0; i<node->next_sz; ++i) { */
    /*     tagcount = 0; */
    /*     for (j=0; j<node->next_sz; ++j) { */
    /*         if (node->next[i]->branchtag == other->next[j]->branchtag) { */
    /*             ++tagcount; */
    /*         } */
    /*     } */
    /*     if (tagcount != 1) return 0; */
    /* } */

    /* // >1 successors: find matching tag */
    /* for (i=0; i<node->next_sz; ++i) { */
    /*     for (j=0; j<node->next_sz; ++j) { */
    /*         if (node->next[i]->branchtag == other->next[j]->branchtag) { */
    /*             if (!compare_st_node(node->next[i], other->next[j])) return 0; */
    /*             break; */
    /*         } */
    /*     } */
    /* } */

    /* return 1; */
}


/**
 * Normalise tree by 
 *
 */
void normalise(st_node *root) {
    if(root) {
        unsigned i, j;
        st_node *node = root;
        st_node *child;

        if(node->type == 3) {
        
            /* printf(" next_sz = %i\n", root->next_sz); */
            for (i=0; i<node->next_sz; ++i) {
                /* printf("traverse children\n"); */
                if(node->next[i]) {
                    if(node->next[i]->type == 3) { //BRANCH_NODE LABEL
                        /* printf(" child is BRANCH NODE\n");  */
                        child = node->next[i];
                        for (j=0; j<child->next_sz; ++j) {
                            shiftElementsRight(node, j+1);
                            insert_st_node_at(node, child->next[j], j+1);
                            /* append_st_node(node, child->next[j]); */
                            /* print_st_node(child, 2); */
                            child->next[j] = NULL;
                            shiftElementsLeft(child, j);
                            j--; 
                        }
                    }
                }
            }
        } else {

            for (i=0; i<node->next_sz; ++i) {
                /* printf("recursion \n =============================== \n"); */
                normalise(node->next[i]);
            }
        }
        
    }
}


void shiftElementsLeft(st_node *node, int index)
{
    int i;
    for(i=index; i<node->next_sz-1; i++) {
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
    for(i=node->next_sz-1; i>index; i--) {
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
