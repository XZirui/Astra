#include "astra/frontend/TypeContext.hpp"
#include "astra/frontend/CompilerContext.hpp"
#include <llvm/ADT/ArrayRef.h>

namespace astra::frontend {
    TypeContext::TypeContext(CompilerContext &Ctx) : CompilerCtx(Ctx) {
        VoidTy = CompilerCtx.create<sema::VoidType>();
        I32Ty = CompilerCtx.create<sema::IntType>(32u, true);
        I64Ty = CompilerCtx.create<sema::IntType>(64u, true);
        U32Ty = CompilerCtx.create<sema::IntType>(32u, false);
        U64Ty = CompilerCtx.create<sema::IntType>(64u, false);
        F32Ty = CompilerCtx.create<sema::FloatType>(32u);
        F64Ty = CompilerCtx.create<sema::FloatType>(64u);
        BoolTy = CompilerCtx.create<sema::BoolType>();
        ErrorTy = CompilerCtx.create<sema::ErrorType>();
    }

    sema::ArrayType *TypeContext::getArrayType(sema::Type *ElementType,
                                               size_t      Size) {
        assert(Size != 0 && "Array size must be greater than zero");
        return getOrCreateType<sema::ArrayType>(ArrayTypes, ElementType, Size);
    }

    sema::ArrayType *TypeContext::getArrayType(sema::Type *ElementType) {
        return getOrCreateType<sema::ArrayType>(ArrayTypes, ElementType);
    }

    sema::FunctionType *
    TypeContext::getFunctionType(sema::Type                  *ReturnType,
                                 llvm::ArrayRef<sema::Type *> ParamTypes) {
        return getOrCreateType<sema::FunctionType>(FunctionTypes, ReturnType,
                                                   ParamTypes);
    }

    sema::ClassType *TypeContext::getClassType(llvm::StringRef Name) {
        if (!ClassTypes.contains(Name)) {
            // Allow forward reference by creating an incomplete class type
            ClassTypes.insert({Name, sema::ClassType(Name)});
        }
        return &ClassTypes[Name];
    }

    sema::ClassType *
    TypeContext::getClassType(llvm::StringRef                         Name,
                              llvm::ArrayRef<sema::ClassType::Member> Members) {
        if (ClassTypes.contains(Name)) {
            auto &Target = ClassTypes[Name];
            assert(!Target.isComplete() &&
                   "Cannot redefine class with members");
            Target.define(Members);
            return &Target;
        }
        ClassTypes.insert({Name, sema::ClassType(Name, Members)});
        return &ClassTypes[Name];
    }

    bool TypeContext::canImplicitCast(sema::Type *From, sema::Type *To) const {
        if (From == To) {
            return true;
        }

        // Error types can be implicitly cast to any type
        // To avoid cascading errors
        if (From->isErrorType() || To->isErrorType()) {
            return true;
        }

        if (From->isBuiltinType() && To->isBuiltinType()) {
            auto *FromBuiltin = llvm::cast<sema::BuiltinType>(From);
            auto *ToBuiltin = llvm::cast<sema::BuiltinType>(To);
            return FromBuiltin->getTypeRank() <= ToBuiltin->getTypeRank();
        }

        if (From->isClassType() && To->isClassType()) {
            // TODO
        }

        // Not allowing implicit casts between non-builtin and non-class types
        return false;
    }

    sema::Type *TypeContext::getCommonType(sema::Type *T1,
                                           sema::Type *T2) const {
        // TODO support for class derived types
        if (canImplicitCast(T1, T2)) {
            return T2;
        }
        if (canImplicitCast(T2, T1)) {
            return T1;
        }
        return nullptr;
    }

} // namespace astra::frontend