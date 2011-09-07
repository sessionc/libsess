#ifndef STACK_H__
#define STACK_H__
#include "st_node.h"

struct __stack_node {
  st_node *element;
  struct __stack_node *next;
};
typedef struct __stack_node stack_node;

struct __stack {
  stack_node *top;
};
typedef struct __stack stackli;

void init_stack(stackli **stack);

void free_stack(stackli *stack);

int isEmpty(stackli *stack);

void push(stackli *stack, st_node *node);

void pop(stackli *stack);

void top(stackli *stack,st_node **top_node);

#endif // STACK_H__
