#include "astra/frontend/TypeContext.hpp"
#include "astra/frontend/CompilerContext.hpp"

namespace astra::frontend {
    TypeContext::TypeContext(CompilerContext &Ctx) : CompilerCtx(Ctx) {
        VoidTy = createType<sema::VoidType>();
        I32Ty = createType<sema::IntType>(32u, true);
        I64Ty = createType<sema::IntType>(64u, true);
        U32Ty = createType<sema::IntType>(32u, false);
        U64Ty = createType<sema::IntType>(64u, false);
        F32Ty = createType<sema::FloatType>(32u);
        F64Ty = createType<sema::FloatType>(64u);
        BoolTy = createType<sema::BoolType>();

        // Aliases for built-in types
        TypeCache[CompilerCtx.getIdentifier("int")] = I32Ty;
        TypeCache[CompilerCtx.getIdentifier("long")] = I64Ty;
        TypeCache[CompilerCtx.getIdentifier("uint")] = U32Ty;
        TypeCache[CompilerCtx.getIdentifier("ulong")] = U64Ty;
        TypeCache[CompilerCtx.getIdentifier("float")] = F32Ty;
        TypeCache[CompilerCtx.getIdentifier("double")] = F64Ty;
    }

} // namespace astra::frontend