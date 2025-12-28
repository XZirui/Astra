#pragma once

#include "ASTVisitor.hpp"
#include "astra/support/Printer.hpp"

namespace astra::ast {
    // class ASTDumper final : public ASTVisitor {
    // public:
    //     explicit ASTDumper(support::Printer &out);
    //
    //     void dump(const Program &program);
    //
    //     void visit(const Program &program) override;

    //
    // private:
    //     support::Printer &out;
    // };

    class ASTDumper final : public ASTConstVisitor<ASTDumper> {
    public:
        explicit ASTDumper(support::Printer &out);

        void dump(const Program *program);

        void visitProgram(const Program *program);
        void visitTopLevelObject(const TopLevelObject *top_level_object);
        void visitBlockStmt(const BlockStmt *block_stmt);
        void visitIfStmt(const IfStmt *if_stmt);
        void visitExprStmt(const ExprStmt *expr_stmt);
        void visitReturnStmt(const ReturnStmt *return_stmt);
        void visitLiteralExpr(const LiteralExpr *literal_expr);
        void visitVarExpr(const VarExpr *var_expr);
        void visitUnaryExpr(const UnaryExpr *unary_expr);
        void visitBinaryExpr(const BinaryExpr *binary_expr);

    private:
        support::Printer &out;
    };
}
