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
        explicit ASTDumper(support::Printer &Out);

        void dump(const Program *Program);

        void visitProgram(const Program *Program);
        void visitTopLevelObject(const TopLevelObject *TopLevelObject);
        void visitFunctionDecl(const FunctionDecl *FunctionDecl);
        void visitParamDecl(const ParamDecl *ParamDecl);
        void visitBlockStmt(const BlockStmt *BlockStmt);
        void visitIfStmt(const IfStmt *IfStmt);
        void visitExprStmt(const ExprStmt *ExprStmt);
        void visitReturnStmt(const ReturnStmt *ReturnStmt);
        void visitBreakStmt(const BreakStmt *BreakStmt);
        void visitContinueStmt(const ContinueStmt *ContinueStmt);
        void visitIntLiteral(const IntLiteral *IntLiteral);
        void visitBoolLiteral(const BoolLiteral *BoolLiteral);
        void visitFloatingLiteral(const FloatingLiteral *FloatingLiteral);
        void visitNullLiteral(const NullLiteral *NullLiteral);
        void visitVarExpr(const VarExpr *VarExpr);
        void visitUnaryExpr(const UnaryExpr *UnaryExpr);
        void visitBinaryExpr(const BinaryExpr *BinaryExpr);

    private:
        support::Printer &Out;
    };
} // namespace astra::ast
