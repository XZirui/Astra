#include <ANTLRInputStream.h>
#include <iostream>

#include <astra/ast/ASTBuilder.hpp>

#include "astra/parser/AstraLexer.h"
#include "astra/parser/AstraParser.h"

int main() {
    auto input = R"(
if (x > 0) {
    return 4 < y + 3 * 2 / 4;
} else {
    return x == 10;
}
    )";

    antlr4::ANTLRInputStream input_stream(input);
    astra::parser::AstraLexer lexer(&input_stream);
    antlr4::CommonTokenStream tokens(&lexer);

    astra::parser::AstraParser parser(&tokens);
    auto tree = parser.file();

    std::cout << tree->toStringTree(&parser, true) << std::endl;

    auto *astCtx = new astra::ast::ASTContext();
    astra::ast::ASTBuilder builder(*astCtx);
    return 0;
}
