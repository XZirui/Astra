// $antlr-format alignTrailingComments true, columnLimit 150, maxEmptyLinesToKeep 1, reflowComments false, useTab false
// $antlr-format allowShortRulesOnASingleLine true, allowShortBlocksOnASingleLine true, minEmptyLines 0, alignSemicolons ownLine
// $antlr-format alignColons trailing, singleLineOverrulesHangingColon true, alignLexerCommands true, alignLabels true, alignTrailers true

lexer grammar AstraLexer;

import UnicodeClasses;

//
// Operators and Punctuation
//

DOT : '.';
COMMA : ',';
SEMICOLON : ';';
COLON : ':';
LPAREN : '(';
RPAREN : ')';
LBRACKET : '[';
RBRACKET : ']';
LBRACE : '{';
RBRACE : '}';
ADD : '+';
SUB : '-';
MULT : '*';
DIV : '/';
MOD : '%';
ASSIGNMENT : '=';
ADD_ASSIGNMENT : '+=';
SUB_ASSIGNMENT : '-=';
MULT_ASSIGNMENT : '*=';
DIV_ASSIGNMENT : '/=';
MOD_ASSIGNMENT : '%=';
ARROW : '->';

EQ: '==';
NEQ: '!=';
LE: '<=' ;
GE: '>=' ;
LT : '<';
GT : '>';

CONJ: '&&';
DISJ: '||';
// TODO: More

//
// Keywords
//

IMPORT : 'import';
IF : 'if';
ELSE : 'else';
FOR : 'for';
WHILE : 'while';
BREAK : 'break';
CONTINUE : 'continue';
RETURN : 'return';
DEF : 'def';
CLASS : 'class';

//
// Modifiers
//

PUBLIC : 'public';
PRIVATE : 'private';
PROTECTED : 'protected';

//
// Logical Literals
//

BOOLEAN_LITERAL:
    'true' | 'false'
;

NULL_LITERAL:
    'null'
;


//
// Integer Literals
//

INTEGER_LITERAL:
    DEC_INTEGER_LITERAL
    | HEX_INTEGER_LITERAL
    | OCT_INTEGER_LITERAL
    | BIN_INTEGER_LITERAL
;


//
// Floating Point Literals
//

FLOAT_LITERAL:
    DOUBLE_LITERAL [fF]
    | INTEGER_LITERAL [fF]
;

DOUBLE_LITERAL:
    DEC_DIGIT+? DOT DEC_DIGIT+ DOUBLE_EXPONENT?
    | DEC_DIGIT+ DOUBLE_EXPONENT
;

fragment DOUBLE_EXPONENT: [eE] [+-]? DEC_DIGIT+;


// TODO: Separator
DEC_INTEGER_LITERAL:
    DEC_DIGIT_NO_ZERO DEC_DIGIT*
    | '0'
;

fragment DEC_DIGIT: [0-9];
fragment DEC_DIGIT_NO_ZERO: [1-9];

// TODO: Separator
HEX_INTEGER_LITERAL:
    '0' [xX] HEX_DIGIT+
;

fragment HEX_DIGIT: [0-9a-fA-F];

OCT_INTEGER_LITERAL:
    '0' [oO] OCT_DIGIT+
;

fragment OCT_DIGIT: [0-7];

BIN_INTEGER_LITERAL:
    '0' [bB] BIN_DIGIT+
;

fragment BIN_DIGIT: [01];

//
// Identifiers
//

IDENTIFIER:
    (LETTER | '_') (LETTER | UNICODE_DIGIT | '_')*
;

fragment UNICODE_DIGIT:
    UNICODE_CLASS_ND
;

fragment LETTER:
    UNICODE_CLASS_LL
    | UNICODE_CLASS_LM
    | UNICODE_CLASS_LO
    | UNICODE_CLASS_LT
    | UNICODE_CLASS_LU
    | UNICODE_CLASS_NL
;

//
// Whitespace and comments
//

WS: [ \t\r\n\u000C]+ -> skip;

COMMENT: '/*' .*? '*/' -> channel(HIDDEN);
LINE_COMMENT: '//' ~[\r\n]* -> channel(HIDDEN);