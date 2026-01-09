#include "astra/frontend/TypeContext.hpp"
#include "astra/frontend/CompilerContext.hpp"

#include "llvm/ADT/ArrayRef.h"

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
    }

    sema::ArrayType *TypeContext::getArrayType(sema::Type *ElementType,
                                               size_t      Size) {
        assert(Size != 0 && "Array size must be greater than zero");
        return getOrCreateType<sema::ArrayType>(ArrayTypes, ElementType, Size);
    }

    sema::FunctionType *
    TypeContext::getFunctionType(sema::Type                  *ReturnType,
                                 llvm::ArrayRef<sema::Type *> ParamTypes) {
        return getOrCreateType<sema::FunctionType>(FunctionTypes, ReturnType,
                                                   ParamTypes);
    }

} // namespace astra::frontend