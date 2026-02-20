#pragma once

#include "ConstantEvaluator.hpp"
#include "astra/ast/ASTVisitor.hpp"

namespace astra::frontend {
    class CompilerContext;
    class TypeContext;
    class SymbolContext;

    class SymbolCollector : public ast::DeclVisitor<SymbolCollector> {
        CompilerContext           &CompilerCtx;
        TypeContext               &TypeCtx;
        SymbolContext             &SymbolCtx;
        support::DiagnosticEngine &DiagEngine;
        ConstantEvaluator         &ConstEval;

    public:
        SymbolCollector(CompilerContext   &CompilerCtx,
                        ConstantEvaluator &ConstEval);

        void collect(ast::Program *Program) { visit(Program); }
        void visitProgram(ast::Program *Program);
        void visitTopLevelObject(ast::TopLevelObject *TopLevelObject);
        void visitFunctionDecl(ast::FunctionDecl *FunctionDecl);
        void visitParamDecl(ast::ParamDecl *ParamDecl);
        void visitVoidTypeRef(ast::VoidTypeRef *VoidTypeRef);
        void visitBoolTypeRef(ast::BoolTypeRef *BoolTypeRef);
        void visitIntTypeRef(ast::IntTypeRef *IntTypeRef);
        void visitLongTypeRef(ast::LongTypeRef *LongTypeRef);
        void visitFloatTypeRef(ast::FloatTypeRef *FloatTypeRef);
        void visitDoubleTypeRef(ast::DoubleTypeRef *DoubleTypeRef);
        void visitArrayTypeRef(ast::ArrayTypeRef *ArrayTypeRef);
        void visitFunctionTypeRef(ast::FunctionTypeRef *FunctionTypeRef);
        void visitClassTypeRef(ast::ClassTypeRef *ClassTypeRef);
    };
} // namespace astra::frontend