#include "astra/frontend/SymbolCollector.hpp"
#include "astra/frontend/CompilerContext.hpp"
#include "astra/frontend/SymbolContext.hpp"
#include "astra/frontend/TypeContext.hpp"

namespace astra::frontend {
    SymbolCollector::SymbolCollector(CompilerContext        &CompilerCtx,
                                     ConstantEvaluator &ConstEval)
        : CompilerCtx(CompilerCtx), TypeCtx(CompilerCtx.getTypeContext()),
          SymbolCtx(CompilerCtx.getSymbolContext()), ConstEval(ConstEval) {}

    void SymbolCollector::visitProgram(ast::Program *Program) {
        for (auto *Object : Program->Objects) {
            visit(Object);
        }
    }

    void
    SymbolCollector::visitTopLevelObject(ast::TopLevelObject *TopLevelObject) {
        visit(TopLevelObject->Declaration);
    }

    void SymbolCollector::visitFunctionDecl(ast::FunctionDecl *FunctionDecl) {
        auto Name = FunctionDecl->Name;
        if (SymbolCtx.existsCurrent(Name)) {
            // TODO: handle error: redeclaration
            assert(false);
        }

        visit(FunctionDecl->ReturnType);
        std::vector<sema::Type *> ParamTypes{};
        for (auto *Param : FunctionDecl->Params) {
            visit(Param);
            ParamTypes.emplace_back(Param->ParamType->ResolvedType);
        }
        FunctionDecl->ResolvedFunctionType = TypeCtx.getFunctionType(
            FunctionDecl->ReturnType->ResolvedType, ParamTypes);
    }

    void SymbolCollector::visitParamDecl(ast::ParamDecl *ParamDecl) {
        visit(ParamDecl->ParamType);
    }

    void SymbolCollector::visitVoidTypeRef(ast::VoidTypeRef *VoidTypeRef) {
        VoidTypeRef->ResolvedType = TypeCtx.getVoidType();
    }

    void SymbolCollector::visitBoolTypeRef(ast::BoolTypeRef *BoolTypeRef) {
        BoolTypeRef->ResolvedType = TypeCtx.getBoolType();
    }

    void SymbolCollector::visitIntTypeRef(ast::IntTypeRef *IntTypeRef) {
        IntTypeRef->ResolvedType = TypeCtx.getIntType();
    }

    void SymbolCollector::visitLongTypeRef(ast::LongTypeRef *LongTypeRef) {
        LongTypeRef->ResolvedType = TypeCtx.getLongType();
    }

    void SymbolCollector::visitFloatTypeRef(ast::FloatTypeRef *FloatTypeRef) {
        FloatTypeRef->ResolvedType = TypeCtx.getFloatType();
    }

    void
    SymbolCollector::visitDoubleTypeRef(ast::DoubleTypeRef *DoubleTypeRef) {
        DoubleTypeRef->ResolvedType = TypeCtx.getDoubleType();
    }

    void SymbolCollector::visitArrayTypeRef(ast::ArrayTypeRef *ArrayTypeRef) {
        visit(ArrayTypeRef->ElementType);
        // TODO calculate constant expression for size
        // ArrayTypeRef->ResolvedType = TypeCtx.getArrayType(
        //     ArrayTypeRef->ElementType->ResolvedType, ArrayTypeRef->Size);
    }

    void SymbolCollector::visitFunctionTypeRef(
        ast::FunctionTypeRef *FunctionTypeRef) {
        visit(FunctionTypeRef->ReturnType);
        std::vector<sema::Type *> ParamTypes{};
        for (auto *ParamType : FunctionTypeRef->ParamTypes) {
            visit(ParamType);
            ParamTypes.emplace_back(ParamType->ResolvedType);
        }
        FunctionTypeRef->ResolvedType = TypeCtx.getFunctionType(
            FunctionTypeRef->ReturnType->ResolvedType, ParamTypes);
    }

    void SymbolCollector::visitClassTypeRef(ast::ClassTypeRef *ClassTypeRef) {
        ClassTypeRef->ResolvedType =
            TypeCtx.getClassType(ClassTypeRef->ClassName);
    }

} // namespace astra::frontend