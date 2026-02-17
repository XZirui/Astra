#pragma once

#include "astra/ast/ASTVisitor.hpp"
#include "astra/support/Printer.hpp"

namespace astra::tools::ast {
    using astra::ast::ArrayTypeRef;
    using astra::ast::ASTConstVisitor;
    using astra::ast::ASTNode;
    using astra::ast::BinaryExpr;
    using astra::ast::BlockStmt;
    using astra::ast::BoolLiteral;
    using astra::ast::BoolTypeRef;
    using astra::ast::BreakStmt;
    using astra::ast::ClassTypeRef;
    using astra::ast::ContinueStmt;
    using astra::ast::DoubleTypeRef;
    using astra::ast::ExprStmt;
    using astra::ast::FloatLiteral;
    using astra::ast::FloatTypeRef;
    using astra::ast::FunctionDecl;
    using astra::ast::FunctionTypeRef;
    using astra::ast::IfStmt;
    using astra::ast::IntLiteral;
    using astra::ast::IntTypeRef;
    using astra::ast::LongTypeRef;
    using astra::ast::NullLiteral;
    using astra::ast::ParamDecl;
    using astra::ast::Program;
    using astra::ast::ReturnStmt;
    using astra::ast::TopLevelObject;
    using astra::ast::UnaryExpr;
    using astra::ast::VarExpr;
    using astra::ast::VoidTypeRef;

    class ASTDumper final : public ASTConstVisitor<ASTDumper> {
    public:
        explicit ASTDumper(support::Printer &Out);

        void dump(const Program *Program);

        void visitASTNode(const ASTNode *) {}
        void visitProgram(const Program *Program);
        void visitTopLevelObject(const TopLevelObject *TopLevelObject);
        void visitFunctionDecl(const FunctionDecl *FunctionDecl);
        void visitParamDecl(const ParamDecl *ParamDecl);
        void visitVoidTypeRef(const VoidTypeRef *VoidTypeRef);
        void visitBoolTypeRef(const BoolTypeRef *BoolTypeRef);
        void visitIntTypeRef(const IntTypeRef *IntTypeRef);
        void visitLongTypeRef(const LongTypeRef *LongTypeRef);
        void visitFloatTypeRef(const FloatTypeRef *FloatTypeRef);
        void visitDoubleTypeRef(const DoubleTypeRef *DoubleTypeRef);
        void visitArrayTypeRef(const ArrayTypeRef *ArrayTypeRef);
        void visitFunctionTypeRef(const FunctionTypeRef *FunctionTypeRef);
        void visitClassTypeRef(const ClassTypeRef *ClassTypeRef);
        void visitBlockStmt(const BlockStmt *BlockStmt);
        void visitIfStmt(const IfStmt *IfStmt);
        void visitExprStmt(const ExprStmt *ExprStmt);
        void visitReturnStmt(const ReturnStmt *ReturnStmt);
        void visitBreakStmt(const BreakStmt *BreakStmt);
        void visitContinueStmt(const ContinueStmt *ContinueStmt);
        void visitIntLiteral(const IntLiteral *IntLiteral);
        void visitBoolLiteral(const BoolLiteral *BoolLiteral);
        void visitFloatLiteral(const FloatLiteral *FloatLiteral);
        void visitNullLiteral(const NullLiteral *NullLiteral);
        void visitVarExpr(const VarExpr *VarExpr);
        void visitUnaryExpr(const UnaryExpr *UnaryExpr);
        void visitBinaryExpr(const BinaryExpr *BinaryExpr);

    private:
        support::Printer &Out;
    };
} // namespace astra::tools::ast
