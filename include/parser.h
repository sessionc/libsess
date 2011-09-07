#ifndef __PARSER_H__
#define __PARSER_H__
/**
 * \file
 * Header file for parser entry point.
 *
 * \headerfile "st_node.h"
 */

#include <antlr3interfaces.h>
#include "st_node.h"

void visit_protocol_node(pANTLR3_BASE_TREE node);
void visit_role_decl(pANTLR3_BASE_TREE node);
void visit_inbranch_node(pANTLR3_BASE_TREE node);
void visit_inbranch_branch_node(pANTLR3_BASE_TREE node);
void visit_outbranch_node(pANTLR3_BASE_TREE node);
void visit_outbranch_branch_node(pANTLR3_BASE_TREE node);
void visit_send_node(pANTLR3_BASE_TREE node);
void visit_recv_node(pANTLR3_BASE_TREE node);
void visit_rec_node(pANTLR3_BASE_TREE node);
void visit_node(pANTLR3_BASE_TREE node);
void visit_inwhile_node(pANTLR3_BASE_TREE node);
void visit_outwhile_node(pANTLR3_BASE_TREE node);

/**
 * \brief Parse the scribble file for a st_node.
 *
 * @param[in] filename Scribble filename.
 * 
 * \returns parsed st_node.
 */
st_node *parse(const char *filename);

#endif // __PARSER_H__
