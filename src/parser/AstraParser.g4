// $antlr-format alignTrailingComments true, columnLimit 150, maxEmptyLinesToKeep 1, reflowComments false, useTab false
// $antlr-format allowShortRulesOnASingleLine true, allowShortBlocksOnASingleLine true, minEmptyLines 0, alignSemicolons ownLine
// $antlr-format alignColons trailing, singleLineOverrulesHangingColon true, alignLexerCommands true, alignLabels true, alignTrailers true

parser grammar AstraParser;

options {
    tokenVocab=AstraLexer;
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
    : LPAREN type RPAREN
    | IDENTIFIER
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
    : addition (comparisonOperator addition)?
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
    ;

unaryExpr
    : unaryOperator* primaryExpr
    ;

primaryExpr
    : LPAREN expression RPAREN  # ParenExpr
    | IDENTIFIER                # Variable
    | INTEGER_LITERAL           # IntLiteral
    | BOOLEAN_LITERAL           # BoolLiteral
    | NULL_LITERAL              # NullLiteral
    | FLOAT_LITERAL             # FloatLiteral
    | DOUBLE_LITERAL            # DoubleLiteral
    ;