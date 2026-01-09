#pragma once

#include "astra/ast/ASTVisitor.hpp"
#include "astra/support/Printer.hpp"

namespace astra::tools::ast {
    using astra::ast::ASTConstVisitor;
    using astra::ast::ASTNode;
    using astra::ast::Program;
    using astra::ast::TopLevelObject;
    using astra::ast::FunctionDecl;
    using astra::ast::ParamDecl;
    using astra::ast::BlockStmt;
    using astra::ast::IfStmt;
    using astra::ast::ExprStmt;
    using astra::ast::ReturnStmt;
    using astra::ast::BreakStmt;
    using astra::ast::ContinueStmt;
    using astra::ast::IntLiteral;
    using astra::ast::BoolLiteral;
    using astra::ast::FloatingLiteral;
    using astra::ast::NullLiteral;
    using astra::ast::VarExpr;
    using astra::ast::UnaryExpr;
    using astra::ast::BinaryExpr;
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

        void visitASTNode(const ASTNode *) {}
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
} // namespace astra::tools::ast
