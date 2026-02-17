// $antlr-format alignTrailingComments true, columnLimit 150, minEmptyLines 1, maxEmptyLinesToKeep 1, reflowComments false, useTab false
// $antlr-format allowShortRulesOnASingleLine false, allowShortBlocksOnASingleLine true, alignSemicolons hanging, alignColons hanging

parser grammar AstraParser;

options {
    tokenVocab = AstraLexer;
}

file
    : topLevelObject* EOF
    ;

// TODO: Adjust top level objects
topLevelObject
    : declaration
    ;

declaration
    : functionDecl
    ;

functionDecl
    : DEF IDENTIFIER LPAREN paramList? RPAREN ARROW type blockStmt
    ;

paramList
    : parameter (COMMA parameter)*
    ;

parameter
    : IDENTIFIER COLON type (ASSIGNMENT expression)?
    ;

// TODO: more types such as function types
type
    : (parenType | typeRef | builtinType) (LBRACKET expression RBRACKET)*
    | functionType
    ;

parenType
    : LPAREN type RPAREN
    ;

typeRef
    : IDENTIFIER
    ;

functionType
    : LPAREN paramTypeList? RPAREN ARROW type
    ;

paramTypeList
    : type (COMMA type)*
    ;

builtinType
    : VOID   # VoidType
    | BOOL   # BoolType
    | INT    # IntType
    | LONG   # LongType
    | FLOAT  # FloatType
    | DOUBLE # DoubleType
    ;

// TODO assignment
statement
    : blockStmt
    | exprStmt
    | ifStmt
    | returnStmt
    | breakStmt
    | continueStmt
    ;

blockStmt
    : LBRACE statement* RBRACE
    ;

exprStmt
    : expression SEMICOLON
    ;

ifStmt
    : IF LPAREN expression RPAREN statement (ELSE statement)?
    ;

returnStmt
    : RETURN expression? SEMICOLON
    ;

// TODO: support labels
breakStmt
    : BREAK SEMICOLON
    ;

continueStmt
    : CONTINUE SEMICOLON
    ;

expression
    : disjunction
    ;

disjunction
    : conjunction (DISJ conjunction)*
    ;

conjunction
    : equality (CONJ equality)*
    ;

equalityOperator
    : EQ
    | NEQ
    ;

equality
    : comparison (equalityOperator comparison)?
    ;

comparisonOperator
    : LT
    | GT
    | LE
    | GE
    ;

comparison
    : bitwiseOr (comparisonOperator bitwiseOr)?
    ;

bitwiseOr
    : bitwiseXor (BIT_OR bitwiseXor)*
    ;

bitwiseXor
    : bitwiseAnd (BIT_XOR bitwiseAnd)*
    ;

bitwiseAnd
    : bitwiseShift (BIT_AND bitwiseShift)*
    ;

bitwiseShiftOperator
    : LSHIFT
    | RSHIFT
    ;

bitwiseShift
    : addition (bitwiseShiftOperator addition)*
    ;

additionOperator
    : ADD
    | SUB
    ;

addition
    : multiplication (additionOperator multiplication)*
    ;

multiplicationOperator
    : MULT
    | DIV
    | MOD
    ;

multiplication
    : unaryExpr (multiplicationOperator unaryExpr)*
    ;

// TODO more
unaryOperator
    : ADD
    | SUB
    | NOT
    | BIT_NOT
    ;

unaryExpr
    : unaryOperator* primaryExpr
    ;

primaryExpr
    : LPAREN expression RPAREN # ParenExpr
    | IDENTIFIER               # Variable
    | INTEGER_LITERAL          # IntLiteral
    | BOOLEAN_LITERAL          # BoolLiteral
    | NULL_LITERAL             # NullLiteral
    | FLOAT_LITERAL            # FloatLiteral
    | DOUBLE_LITERAL           # DoubleLiteral
    ;