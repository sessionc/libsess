/**
 * \file
 * Example code to invoke parser and generate st_node graph.
 *
 * \headerfile "ScribbleProtocolLexer.h"
 * \headerfile "ScribbleProtocolParser.h"
 * \headerfile "st_node.h"
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <antlr3interfaces.h>

#include "ScribbleProtocolLexer.h"
#include "ScribbleProtocolParser.h"

#include "parser.h"
#include "st_node.h"
#include "stack.h"


st_node *root;
stackli *parents;


void visit_protocol_node(pANTLR3_BASE_TREE node)
{
  pANTLR3_BASE_TREE tmp_node;

  char *protocol_name;
  char *myrole_name;

  int i;
  int child_count = node->getChildCount(node);

  tmp_node = node->getChild(node, 0);
  protocol_name = (char *)tmp_node->getText(tmp_node)->chars;

  tmp_node = node->getChild(node, 2);
  myrole_name = (char *)tmp_node->getText(tmp_node)->chars;

  printf("Protocol %s @ %s\n", protocol_name, myrole_name);

  root = malloc(sizeof(st_node));
  init_st_node(root, BEGIN_NODE, "", "");

  assert(isEmpty(parents));
  push(parents, root);

#ifdef __DEBUG__
  fprintf(stderr, "visit_node: root st_node <%p role=%s type=%s>\n",
          root, protocol_name, myrole_name);

#endif

  for (i=3; i<child_count; ++i) {
    tmp_node = node->getChild(node, i);
    visit_node(tmp_node);
  }
}


void visit_role_decl(pANTLR3_BASE_TREE node)
{
  pANTLR3_BASE_TREE tmp_node;

  char *role_name;

  tmp_node = node->getChild(node, 0);
  role_name = (char *)tmp_node->getText(tmp_node)->chars;

#ifdef __DEBUG__
    fprintf(stderr, "role: %s\n", role_name);
#endif
}


void visit_inbranch_branch_node(pANTLR3_BASE_TREE node)
{
  pANTLR3_BASE_TREE tmp_node;
  st_node *br_node = NULL;
  st_node *parent_node;

  char *branch_label_name;

  int i;
  int child_count = node->getChildCount(node);

  tmp_node = node->getChild(node, 0);
  branch_label_name = (char *)tmp_node->getText(tmp_node)->chars;
  br_node = malloc(sizeof(st_node));
  init_st_node(br_node, BRANCH_NODE, "", branch_label_name);//, "");

  top(parents, &parent_node);
  append_st_node(parent_node, br_node);

  // Set the new parent for branch body.
  push(parents, br_node);

  for (i=1; i<child_count; ++i) {
    tmp_node = node->getChild(node, i);
    visit_node(tmp_node);
  }

  pop(parents);
}


void visit_inbranch_node(pANTLR3_BASE_TREE node)
{
  pANTLR3_BASE_TREE tmp_node;
  st_node *inbr_node = NULL;
  st_node *parent_node;

  char *role_name;
  char *child_node_name;

  int i;
  int child_count;

  tmp_node = node->getChild(node, 0);
  role_name = (char *)tmp_node->getText(tmp_node)->chars;

  // Note: This is INBRANCH_NODE followed by BRANCH_NODEs
  inbr_node = malloc(sizeof(st_node));
  init_st_node(inbr_node, INBRANCH_NODE, role_name, "N/A");//, "");

  top(parents, &parent_node);
  append_st_node(parent_node, inbr_node);

#ifdef __DEBUG__
  fprintf(stderr, "visit_node: inbranch st_node <%p role=%s>\n",
      inbr_node, role_name);
#endif

  // Set the new parent for BRANCH_NODEs
  push(parents, inbr_node);

  child_count = node->getChildCount(node);

  for (i=2; i<child_count; ++i) {
    tmp_node = node->getChild(node, i);
    child_node_name = (char *)tmp_node->getText(tmp_node)->chars;
    if (strcmp(child_node_name, ":") == 0) {
      visit_inbranch_branch_node(tmp_node);
    }
  }

  pop(parents);
}


void visit_outbranch_node(pANTLR3_BASE_TREE node)
{
  pANTLR3_BASE_TREE tmp_node;
  st_node *outbr_node = NULL;
  st_node *parent_node;

  char *role_name;
  char *child_node_name;

  int i;
  int child_count;

  tmp_node = node->getChild(node, 0);
  role_name = (char *)tmp_node->getText(tmp_node)->chars;

  // Note: This is BRANCH_NODE followed by OUTBRANCH_NODEs
  outbr_node = malloc(sizeof(st_node));
  init_st_node(outbr_node, BRANCH_NODE, role_name, "N/A");//, "");

  top(parents, &parent_node);
  append_st_node(parent_node, outbr_node);

#ifdef __DEBUG__
  fprintf(stderr, "visit_node: outbranch st_node <%p role=%s>\n",
      outbr_node, role_name);
#endif

  // Set the new parent for BRANCH_NODEs
  push(parents, outbr_node);

  child_count = node->getChildCount(node);

  for (i=2; i<child_count; ++i) {
    tmp_node = node->getChild(node, i);
    child_node_name = (char *)tmp_node->getText(tmp_node)->chars;
    if (strcmp(child_node_name, ":") == 0) {
      visit_outbranch_branch_node(tmp_node);
    }
  }

  pop(parents);
}


void visit_outbranch_branch_node(pANTLR3_BASE_TREE node)
{
  pANTLR3_BASE_TREE tmp_node;
  st_node *br_node = NULL;
  st_node *parent_node;

  char *branch_label_name;

  int i;
  int child_count = node->getChildCount(node);

  tmp_node = node->getChild(node, 0);
  branch_label_name = (char *)tmp_node->getText(tmp_node)->chars;
  br_node = malloc(sizeof(st_node));
  init_st_node(br_node, OUTBRANCH_NODE, "", branch_label_name);//, "");

  top(parents, &parent_node);
  append_st_node(parent_node, br_node);

  // Set the new parent for branch body.
  push(parents, br_node);

  for (i=1; i<child_count; ++i) {
    tmp_node = node->getChild(node, i);
    visit_node(tmp_node);
  }

  pop(parents);
}


void visit_recv_node(pANTLR3_BASE_TREE node)
{
  pANTLR3_BASE_TREE tmp_node;
  st_node *recv_node;
  st_node *parent_node;

  char *role_name;
  char *type_name;

  tmp_node  = node->getChild(node, 0); // Type name
  type_name = (char *)tmp_node->getText(tmp_node)->chars;

  tmp_node  = node->getChild(node, 1); // Role name
  role_name = (char *)tmp_node->getText(tmp_node)->chars;

  recv_node = malloc(sizeof(st_node));
  init_st_node(recv_node, RECV_NODE, role_name, type_name);//, "");

  top(parents, &parent_node);
  append_st_node(parent_node, recv_node);

#ifdef __DEBUG__
  fprintf(stderr, "visit_node: recv st_node <%p role=%s type=%s>\n",
    recv_node, role_name, type_name);
#endif
}


void visit_send_node(pANTLR3_BASE_TREE node)
{
  pANTLR3_BASE_TREE tmp_node;
  st_node *send_node;
  st_node *parent_node;

  char *role_name;
  char *type_name;

  tmp_node  = node->getChild(node, 0); // Type name
  type_name = (char *)tmp_node->getText(tmp_node)->chars;

  tmp_node  = node->getChild(node, 1); // Role name
  role_name = (char *)tmp_node->getText(tmp_node)->chars;

  send_node = malloc(sizeof(st_node));
  init_st_node(send_node, SEND_NODE, role_name, type_name);//, "");

  top(parents, &parent_node);
  append_st_node(parent_node, send_node);

#ifdef __DEBUG__
  fprintf(stderr, "visit_node: send st_node <%p role=%s type=%s>\n",
    send_node, role_name, type_name);
#endif
}


void visit_rec_node(pANTLR3_BASE_TREE node)
{
  pANTLR3_BASE_TREE tmp_node;
  st_node *rec_node;
  st_node *parent_node;

  char *rec_name;

  int i;
  int child_count = node->getChildCount(node);

  tmp_node  = node->getChild(node, 0);
  rec_name = (char *)tmp_node->getText(tmp_node)->chars;

  rec_node = malloc(sizeof(st_node));
  init_st_node(rec_node, RECUR_NODE, "", "\0"/*rec_name*/);

  top(parents, &parent_node);
  append_st_node(parent_node, rec_node);

  push(parents, rec_node);

#ifdef __DEBUG__
  fprintf(stderr, "visit_node: rec st_node <%p rec(%s)>\n",
    rec_node, rec_name);
#endif

  for (i=1; i<child_count; ++i) {
    tmp_node = node->getChild(node, i);
    if (strcmp((char *)tmp_node->getText(tmp_node)->chars, rec_name) == 0) {
      if (i < child_count - 1) {
        fprintf(stderr,
          "Warning: Reached end of rec %s with %d ignored statements\n",
          rec_name, child_count - i - 1);
      }
      break;
    }
    visit_node(tmp_node);
  }

  pop(parents);
}

void visit_repeat_node(pANTLR3_BASE_TREE node)
{
  pANTLR3_BASE_TREE tmp_node;

  char *repeat_name;

  tmp_node = node->getChild(node, 0);
  repeat_name = (char *)tmp_node->getText(tmp_node)->chars;

  if (strcmp(repeat_name, "from") == 0) { // inwhile

    visit_inwhile_node(node);

  } else if (strcmp(repeat_name, "to") == 0) { // outwhile

    visit_outwhile_node(node);

  } else {

    fprintf(stderr, "Warning: Unknown mode of repeat (%s)\n", repeat_name);

  }

}


void visit_inwhile_node(pANTLR3_BASE_TREE node)
{
  pANTLR3_BASE_TREE tmp_node;
  st_node *repeat_node;
  st_node *parent_node;

  char *child_node_name;
  char role_names[255];

  int i;
  int child_count = node->getChildCount(node);

  role_names[0] = '\0';
  for (i=1; i<child_count; ++i) {
    tmp_node = node->getChild(node, i);
    child_node_name = (char *)tmp_node->getText(tmp_node)->chars;
    // This belongs to loop body.
    if (tmp_node->getChildCount(tmp_node) > 0) break;
    strncat(role_names, child_node_name, 254);
    strncat(role_names, "|", 254);
    // TODO: Sort role_names.
  }
  repeat_node = malloc(sizeof(st_node));
  init_st_node(repeat_node, INWHILE_NODE, role_names, "");//, "");

  top(parents, &parent_node);
  append_st_node(parent_node, repeat_node);

  push(parents, repeat_node);
  for (i-=1; i<child_count; ++i) {
    tmp_node = node->getChild(node, i);
    visit_node(tmp_node);
  }
  pop(parents);
}


void visit_outwhile_node(pANTLR3_BASE_TREE node)
{
  pANTLR3_BASE_TREE tmp_node;
  st_node *repeat_node;
  st_node *parent_node;

  char *child_node_name;
  char role_names[255];

  int i;
  int child_count = node->getChildCount(node);

  role_names[0] = '\0';
  for (i=1; i<child_count; ++i) {
    tmp_node = node->getChild(node, i);
    child_node_name = (char *)tmp_node->getText(tmp_node)->chars;
    // This belongs to loop body.
    if (tmp_node->getChildCount(tmp_node) > 0) break;
    strncat(role_names, child_node_name, 254);
    strncat(role_names, "|", 254);
    // TODO: Sort role_names.
  }
  repeat_node = malloc(sizeof(st_node));
  init_st_node(repeat_node, OUTWHILE_NODE, role_names, "");//, "");

  top(parents, &parent_node);
  append_st_node(parent_node, repeat_node);

  push(parents, repeat_node);
  for (i-=1; i<child_count; ++i) {
    tmp_node = node->getChild(node, i);
    visit_node(tmp_node);
  }
  pop(parents);
}


/**
 * \brief Visitor of node.
 *
 * Since there is no polymorphism in C, this visits all types of (toplevel)
 * nodes and identify their type by their getText() name.
 *
 * @param[in] node Node to visit.
 */
void visit_node(pANTLR3_BASE_TREE node)
{
  pANTLR3_BASE_TREE tmp_node;

  char *node_name = (char *)node->getText(node)->chars;

  int i;
  int child_count = node->getChildCount(node);

#ifdef __DEBUG__
  fprintf(stderr, "Node: %s (%d children)\n", node_name, child_count);
#endif

  if (strcmp(node_name, "protocol") == 0) {

    visit_protocol_node(node);

  } else if (strcmp(node_name, "role") == 0) {

    visit_role_decl(node);

  } else if (strcmp(node_name, "from") == 0) {

    for (i=0; i<child_count; ++i) {
      tmp_node = node->getChild(node, i);
      if (strcmp((char *)tmp_node->getText(tmp_node)->chars, ":") == 0) {
        visit_inbranch_node(node);
        return;
      }
    }

    visit_recv_node(node);

  } else if (strcmp(node_name, "to") == 0) {

    for (i=0; i<child_count; ++i) {
      tmp_node = node->getChild(node, i);
      if (strcmp((char *)tmp_node->getText(tmp_node)->chars, ":") == 0) {
        visit_outbranch_node(node);
        return;
      }
    }

    visit_send_node(node);

  } else if (strcmp(node_name, "rec") == 0) {

    visit_rec_node(node);

  } else if (strcmp(node_name, "repeat") == 0) {

    visit_repeat_node(node);

  } else {

    fprintf(stderr, "Warning: unhandled node '%s'\n", node_name);

  }
}


/**
 * Entry point to parser by ANTLRv3.
 */
st_node *parse(const char *filename)
{
  //
  // Input data structures generated by ANTLR3
  //

  pANTLR3_UINT8  source_filename;
  pANTLR3_INPUT_STREAM  input;
  pANTLR3_COMMON_TOKEN_STREAM  tokens;

  //
  // Output data structures
  //

  pScribbleProtocolLexer  lexer;
  pScribbleProtocolParser  parser;
  ScribbleProtocolParser_description_return  ast;

  //
  // Local variables
  //


  source_filename = (pANTLR3_UINT8)filename;

  //
  // Step 1: Open the source file.
  //

  input = antlr3AsciiFileStreamNew(source_filename);
  if (input == NULL) {
    fprintf(stderr, "Error: Unable to open file '%s'\n",
        (char *)source_filename);
    exit(ANTLR3_ERR_NOMEM);
  }

  //
  // Step 2: Invoke lexer on source stream.
  //

  lexer = ScribbleProtocolLexerNew(input);
  if (lexer == NULL) {
    fprintf(stderr, "Error: Unable to create lexer (malloc() failure)\n");
    exit(ANTLR3_ERR_NOMEM);
  }

  //
  // Step 3: Extract tokens from lexer output.
  //

  tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT,
      TOKENSOURCE(lexer));
  if (tokens == NULL) {
    fprintf(stderr, "Error: Unable to extract tokens (Out of memory)\n");
    exit(ANTLR3_ERR_NOMEM);
  }

  //
  // Step 4: Create parser from token stream.
  //

  parser = ScribbleProtocolParserNew(tokens);
  if (parser == NULL) {
    fprintf(stderr, "Error: Unable to create parser (Out of memory)\n");
    exit(ANTLR3_ERR_NOMEM);
  }

  //
  // Step 5: Get the AST
  //

  ast = parser->description(parser);
  if (parser->pParser->rec->state->errorCount > 0) {
    fprintf(stderr, "Error: Parser returned %d errors\n",
        parser->pParser->rec->state->errorCount);
  } else {
    // We have an AST, so initialise ST-tree
    // with BEGIN_NODE

    init_stack(&parents);

    visit_node(ast.tree);

    print_st_node(root, 0);
  }

  parser->free(parser);
  parser = NULL;

  tokens->free(tokens);
  tokens = NULL;

  lexer->free(lexer);
  lexer = NULL;

  input->close(input);
  input = NULL;

  return root;
}
