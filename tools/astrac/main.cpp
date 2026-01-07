#include <ANTLRInputStream.h>
#include <iostream>

#include "astra/parser/AstraLexer.h"
#include "astra/frontend/ASTBuilder.hpp"
#include "astra/parser/AstraParser.h"
#include "astra/tools/ast/ASTDumper.hpp"

int main() {
    auto input = R"(def a() -> void {
    if (x > 0) {
        return 4 < y + 3 * 2 / 4;
    } else {
        return x == 10.0;
        return x + 3.14;
    }
    if(x == null) {
        return true;
    }
    break;
    continue;
}
)";

    antlr4::ANTLRInputStream  input_stream(input);
    astra::parser::AstraLexer lexer(&input_stream);
    antlr4::CommonTokenStream tokens(&lexer);

    astra::parser::AstraParser parser(&tokens);
    auto                       tree = parser.file();

    // std::cout << tree->toStringTree(&parser, true) << std::endl;

    auto                       *astCtx = new astra::frontend::CompilerContext();
    astra::frontend::ASTBuilder builder(*astCtx);
    auto                       *program = builder.build(tree);
    auto                        printer = astra::support::Printer(std::cout);
    astra::tools::ast::ASTDumper dumper(printer);
    dumper.dump(program);

    return 0;
}
