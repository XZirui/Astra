
// Generated from D:/Programs/CPP/astra/src/parser/grammar/AstraLexer.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  AstraLexer : public antlr4::Lexer {
public:
  enum {
    DOT = 1, COMMA = 2, SEMICOLON = 3, COLON = 4, LPAREN = 5, RPAREN = 6, 
    LBRACKET = 7, RBRACKET = 8, LBRACE = 9, RBRACE = 10, ADD = 11, SUB = 12, 
    MULT = 13, DIV = 14, MOD = 15, ASSIGNMENT = 16, ADD_ASSIGNMENT = 17, 
    SUB_ASSIGNMENT = 18, MULT_ASSIGNMENT = 19, DIV_ASSIGNMENT = 20, MOD_ASSIGNMENT = 21, 
    ARROW = 22, EQ = 23, NEQ = 24, LE = 25, GE = 26, LT = 27, GT = 28, CONJ = 29, 
    DISJ = 30, IMPOERT = 31, IF = 32, ELSE = 33, FOR = 34, WHILE = 35, BREAK = 36, 
    CONTINUE = 37, RETURN = 38, DEF = 39, CLASS = 40, PUBLIC = 41, PRIVATE = 42, 
    PROTECTED = 43, INTEGER_LITERAL = 44, DEC_INTEGER_LITERAL = 45, HEX_INTEGER_LITERAL = 46, 
    OCT_INTEGER_LITERAL = 47, BIN_INTEGER_LITERAL = 48, IDENTIFIER = 49, 
    WS = 50, COMMENT = 51, LINE_COMMENT = 52, UNICODE_CLASS_LL = 53, UNICODE_CLASS_LM = 54, 
    UNICODE_CLASS_LO = 55, UNICODE_CLASS_LT = 56, UNICODE_CLASS_LU = 57, 
    UNICODE_CLASS_ND = 58, UNICODE_CLASS_NL = 59
  };

  explicit AstraLexer(antlr4::CharStream *input);

  ~AstraLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

