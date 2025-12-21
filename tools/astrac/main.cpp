#include <iostream>

#include <antlr4-runtime/ANTLRInputStream.h>

#include <astra/ast/ast.h>

int main() {
    std::cout << "Hello World!\n" << std::endl;

    antlr4::ANTLRInputStream input("test input");
    std::cout << "ANTLR input size = " << input.size() << std::endl;

    astra::ast::Node* node = nullptr;
    std::cout << "AST node ptr = " << node << std::endl;
    return 0;
}
