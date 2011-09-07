#include <stdlib.h>
#include <stdio.h>

#include "stack.h"
#include "st_node.h"

extern char *node_type[];

void init_stack(stackli **stack){
  *stack = (stackli *)malloc(sizeof(stackli));
  (*stack)->top = NULL;
}

void free_stack(stackli *stack){
  stack_node *node = stack->top;;
  while(stack->top != NULL){
    stack->top = stack->top->next;
    free(node);
    node = stack->top;
  }
}

int isEmpty(stackli *stack){
  if(stack->top != NULL)
    return 0;
  else
    return 1; 
}

void push(stackli *stack, st_node *node){
  stack_node *tmp;
  tmp = (stack_node *)malloc(sizeof(stack_node));
  tmp->element = node;
  if(isEmpty(stack)){
    tmp->next = NULL;
    stack->top = tmp;
  }else{
    tmp->next =stack->top;
    stack->top =tmp;
  }
}

void pop(stackli *stack){
  if(!isEmpty(stack)){
    stack_node *tmp;
    tmp = stack->top;
    stack->top =tmp->next;
    free(tmp);
  }
}

void top(stackli *stack,st_node **stack_node){
  if(!isEmpty(stack))
  *stack_node = stack->top->element;
}


void _print_st_node(st_node *node){
  printf("st_node <%p type=%s role=%s datatype=%s tag=%s next_sz=%d >\n",(void *)node, node_type[node->type], node->role, node->datatype, node->branchtag, node->next_sz);
}

//for testing 
void _print_stack(stackli *stack){
  if(!isEmpty(stack)){
    printf("stack:\n");
    _print_st_node(stack->top->element);
  }
}

