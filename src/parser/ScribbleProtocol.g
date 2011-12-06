grammar ScribbleProtocol;

options {
    output       = AST;
    language     = C;
    ASTLabelType = pANTLR3_BASE_TREE;
}

tokens {
    INTERACTION = 'interaction' ;
    PLUS        = '+' ;
    MINUS       = '-' ;
    MULT        = '*' ;
    DIV         = '/' ;
    FULLSTOP    = '.' ;
    BRANCH      ='branch' ;

    RECV        = 'from' ;
    SEND        = 'to' ;
}

/*------------------------------------------------------------------
 * PARSER RULES
 *------------------------------------------------------------------*/

description: ( ( ANNOTATION )* ( importProtocolStatement | importTypeStatement ) )* ( ANNOTATION )* protocolDef ;

importProtocolStatement: 'import' 'protocol' importProtocolDef ( ','! importProtocolDef )* ';'! ;

importProtocolDef: ID 'from'! StringLiteral;
						
importTypeStatement: 'import' ( simpleName )? importTypeDef ( ','! importTypeDef )* ( 'from'! StringLiteral )? ';'! ;

importTypeDef: ( dataTypeDef 'as'! )? ID ;

dataTypeDef: StringLiteral ;

simpleName: ID ;

protocolDef: 'protocol'^ protocolName ( 'at' roleName )? ( parameterDefs )? blockDef ;

protocolName: ID ;

parameterDefs: '('! parameterDef ( ','! parameterDef )* ')'! ;

parameterDef: ( typeReferenceDef | 'role'^ ) simpleName ;

blockDef: '{'! activityListDef '}'! ;

activityListDef: ( ( ANNOTATION )* activityDef )* ;

activityDef: ( introducesDef | interactionDef | includeDef | runDef | recursionDef | endDef ) ';'! | 
			choiceDef | directedChoiceDef | parallelDef | repeatDef | unorderedDef |
			recBlockDef | globalEscapeDef | protocolDef ;

introducesDef: roleDef 'introduces' roleDef ( ','! roleDef )* ;

roleDef: ID ;

roleName: ID ;

typeReferenceDef: ID ;

interactionSignatureDef: ( typeReferenceDef | ID^ '('! ( typeReferenceDef ( ','! typeReferenceDef )* )? ')'! ) ;

interactionDef: interactionSignatureDef ( 'from'^ roleName ( 'to' roleName ( ','! roleName )* )? |
							'to'^ roleName ( ','! roleName )* ) ;


choiceDef: 'choice'^ ( 'at' roleName )? blockDef ( 'or' blockDef )* ;

directedChoiceDef: ( 'from'^ roleName )? ( 'to'^ roleName ( ','! roleName )* )? '{' ( onMessageDef )+ '}';

onMessageDef: interactionSignatureDef ':'^ activityList ; 

activityList: ( ( ANNOTATION )* activityDef )* ;

// repeatDef: 'repeat'^ ( 'at' roleName ( ','! roleName )* )? blockDef ;
repeatDef: 'repeat'^ ( 'from' roleName ( ','! roleName )* |
				 		'to' roleName ( ','! roleName )* ) blockDef;

recBlockDef: 'rec'^ labelName blockDef ;

labelName: ID ;

recursionDef: labelName ;

endDef: 'end'^ ;

runDef: 'run'^ protocolRefDef ( '('! parameter ( ','! parameter )* ')'! )? 'from' roleName ;

protocolRefDef: ID ( '@' roleName )? ;

declarationName: ID ;

parameter: declarationName ;

includeDef: 'include'^ protocolRefDef ( '('! parameter ( ','! parameter )* ')'! )? ;

parallelDef: 'parallel'^ blockDef ( 'and' blockDef )* ;

globalEscapeDef: 'do'^ blockDef ( interruptDef )+ ;

interruptDef: 'interrupt'^ blockDef ;

unorderedDef: 'unordered'^ blockDef ;


/*-----------------------------------------------
TO DO:
Declaration (variables) possibly - but that may need
lookahead to avoid conflict with interactions.
-------------------------------------------------*/

expr	: term ( ( PLUS | MINUS )  term )* ;

term	: factor ( ( MULT | DIV ) factor )* ;

factor	: NUMBER ;


/*------------------------------------------------------------------
 * LEXER RULES
 *------------------------------------------------------------------*/

ID : ('a'..'z'|'A'..'Z'|'_')('a'..'z'|'A'..'Z'|'0'..'9'|'_')* ;

NUMBER	: (DIGIT)+ ;

WHITESPACE : ( '\t' | ' ' | '\r' | '\n'| '\u000C' )+ 	{ $channel = HIDDEN; } ;

fragment DIGIT	: '0'..'9' ;

ANNOTATION : '[[' (options {greedy=false;} : .)* ']]' ;

ML_COMMENT
    :   '/*' (options {greedy=false;} : .)* '*/' {$channel=HIDDEN;}
    ;

LINE_COMMENT : '//' (options {greedy=false;} : .)* '\n' {$channel=HIDDEN;} ;

StringLiteral: '"' ( ~('\\'|'"') )* '"' ;
