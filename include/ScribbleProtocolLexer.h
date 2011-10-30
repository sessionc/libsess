/** \file
 *  This C header file was generated by $ANTLR version 3.2 debian-5
 *
 *     -  From the grammar source file : ScribbleProtocol.g
 *     -                            On : 2011-09-06 14:24:24
 *     -                 for the lexer : ScribbleProtocolLexerLexer *
 * Editing it, at least manually, is not wise. 
 *
 * C language generator and runtime by Jim Idle, jimi|hereisanat|idle|dotgoeshere|ws.
 *
 *
 * The lexer ScribbleProtocolLexer has the callable functions (rules) shown below,
 * which will invoke the code for the associated rule in the source grammar
 * assuming that the input stream is pointing to a token/text stream that could begin
 * this rule.
 * 
 * For instance if you call the first (topmost) rule in a parser grammar, you will
 * get the results of a full parse, but calling a rule half way through the grammar will
 * allow you to pass part of a full token stream to the parser, such as for syntax checking
 * in editors and so on.
 *
 * The parser entry points are called indirectly (by function pointer to function) via
 * a parser context typedef pScribbleProtocolLexer, which is returned from a call to ScribbleProtocolLexerNew().
 *
 * As this is a generated lexer, it is unlikely you will call it 'manually'. However
 * the methods are provided anyway.
 * * The methods in pScribbleProtocolLexer are  as follows:
 *
 *  -  void      pScribbleProtocolLexer->INTERACTION(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->PLUS(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->MINUS(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->MULT(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->DIV(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->FULLSTOP(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->BRANCH(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->RECV(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->SEND(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__21(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__22(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__23(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__24(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__25(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__26(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__27(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__28(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__29(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__30(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__31(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__32(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__33(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__34(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__35(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__36(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__37(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__38(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__39(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__40(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__41(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__42(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__43(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__44(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__45(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->T__46(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->ID(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->NUMBER(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->WHITESPACE(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->DIGIT(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->ANNOTATION(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->ML_COMMENT(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->LINE_COMMENT(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->StringLiteral(pScribbleProtocolLexer)
 *  -  void      pScribbleProtocolLexer->Tokens(pScribbleProtocolLexer)
 *
 * The return type for any particular rule is of course determined by the source
 * grammar file.
 */
// [The "BSD licence"]
// Copyright (c) 2005-2009 Jim Idle, Temporal Wave LLC
// http://www.temporal-wave.com
// http://www.linkedin.com/in/jimidle
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef	_ScribbleProtocolLexer_H
#define _ScribbleProtocolLexer_H
/* =============================================================================
 * Standard antlr3 C runtime definitions
 */
#include    <antlr3.h>

/* End of standard antlr 3 runtime definitions
 * =============================================================================
 */
 
#ifdef __cplusplus
extern "C" {
#endif

// Forward declare the context typedef so that we can use it before it is
// properly defined. Delegators and delegates (from import statements) are
// interdependent and their context structures contain pointers to each other
// C only allows such things to be declared if you pre-declare the typedef.
//
typedef struct ScribbleProtocolLexer_Ctx_struct ScribbleProtocolLexer, * pScribbleProtocolLexer;



#ifdef	ANTLR3_WINDOWS
// Disable: Unreferenced parameter,							- Rules with parameters that are not used
//          constant conditional,							- ANTLR realizes that a prediction is always true (synpred usually)
//          initialized but unused variable					- tree rewrite variables declared but not needed
//          Unreferenced local variable						- lexer rule declares but does not always use _type
//          potentially unitialized variable used			- retval always returned from a rule 
//			unreferenced local function has been removed	- susually getTokenNames or freeScope, they can go without warnigns
//
// These are only really displayed at warning level /W4 but that is the code ideal I am aiming at
// and the codegen must generate some of these warnings by necessity, apart from 4100, which is
// usually generated when a parser rule is given a parameter that it does not use. Mostly though
// this is a matter of orthogonality hence I disable that one.
//
#pragma warning( disable : 4100 )
#pragma warning( disable : 4101 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4189 )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4701 )
#endif

/** Context tracking structure for ScribbleProtocolLexer
 */
struct ScribbleProtocolLexer_Ctx_struct
{
    /** Built in ANTLR3 context tracker contains all the generic elements
     *  required for context tracking.
     */
    pANTLR3_LEXER    pLexer;


     void (*mINTERACTION)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mPLUS)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mMINUS)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mMULT)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mDIV)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mFULLSTOP)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mBRANCH)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mRECV)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mSEND)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__21)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__22)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__23)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__24)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__25)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__26)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__27)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__28)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__29)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__30)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__31)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__32)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__33)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__34)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__35)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__36)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__37)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__38)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__39)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__40)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__41)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__42)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__43)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__44)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__45)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mT__46)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mID)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mNUMBER)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mWHITESPACE)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mDIGIT)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mANNOTATION)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mML_COMMENT)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mLINE_COMMENT)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mStringLiteral)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);
     void (*mTokens)	(struct ScribbleProtocolLexer_Ctx_struct * ctx);    const char * (*getGrammarFileName)();
    void	    (*free)   (struct ScribbleProtocolLexer_Ctx_struct * ctx);
        
};

// Function protoypes for the constructor functions that external translation units
// such as delegators and delegates may wish to call.
//
ANTLR3_API pScribbleProtocolLexer ScribbleProtocolLexerNew         (pANTLR3_INPUT_STREAM instream);
ANTLR3_API pScribbleProtocolLexer ScribbleProtocolLexerNewSSD      (pANTLR3_INPUT_STREAM instream, pANTLR3_RECOGNIZER_SHARED_STATE state);

/** Symbolic definitions of all the tokens that the lexer will work with.
 * \{
 *
 * Antlr will define EOF, but we can't use that as it it is too common in
 * in C header files and that would be confusing. There is no way to filter this out at the moment
 * so we just undef it here for now. That isn't the value we get back from C recognizers
 * anyway. We are looking for ANTLR3_TOKEN_EOF.
 */
#ifdef	EOF
#undef	EOF
#endif
#ifdef	Tokens
#undef	Tokens
#endif 
#define T__29      29
#define T__28      28
#define T__27      27
#define T__26      26
#define T__25      25
#define T__24      24
#define T__23      23
#define ANNOTATION      13
#define T__22      22
#define T__21      21
#define ID      14
#define EOF      -1
#define RECV      11
#define INTERACTION      4
#define ML_COMMENT      19
#define FULLSTOP      9
#define PLUS      5
#define SEND      12
#define DIGIT      17
#define T__42      42
#define T__43      43
#define T__40      40
#define T__41      41
#define T__46      46
#define T__44      44
#define T__45      45
#define LINE_COMMENT      20
#define NUMBER      16
#define WHITESPACE      18
#define MINUS      6
#define MULT      7
#define StringLiteral      15
#define T__30      30
#define T__31      31
#define T__32      32
#define T__33      33
#define T__34      34
#define T__35      35
#define T__36      36
#define T__37      37
#define T__38      38
#define T__39      39
#define BRANCH      10
#define DIV      8
#ifdef	EOF
#undef	EOF
#define	EOF	ANTLR3_TOKEN_EOF
#endif

#ifndef TOKENSOURCE
#define TOKENSOURCE(lxr) lxr->pLexer->rec->state->tokSource
#endif

/* End of token definitions for ScribbleProtocolLexer
 * =============================================================================
 */
/** \} */

#ifdef __cplusplus
}
#endif

#endif

/* END - Note:Keep extra line feed to satisfy UNIX systems */