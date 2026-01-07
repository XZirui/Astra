#pragma once

#include "CompilerContext.hpp"
#include "astra/sema/Type.hpp"
#include "astra/support/Identifier.hpp"
#include <unordered_map>

namespace astra::frontend {
    class TypeContext {
        CompilerContext &CompilerCtx;

        std::unordered_map<support::Identifier *, sema::Type *> TypeCache;

        sema::VoidType  *VoidTy;
        sema::IntType   *I32Ty;
        sema::IntType   *I64Ty;
        sema::IntType   *U32Ty;
        sema::IntType   *U64Ty;
        sema::FloatType *F32Ty;
        sema::FloatType *F64Ty;
        sema::BoolType  *BoolTy;

    public:
        explicit TypeContext(CompilerContext &CompilerCtx);

        TypeContext(const TypeContext &) = delete;
        TypeContext &operator=(const TypeContext &) = delete;

        template <typename T, typename... Args> T *createType(Args &&...args) {
            auto *TypeInstance =
                CompilerCtx.create<T>(std::forward<Args>(args)...);
            TypeCache[CompilerCtx.getIdentifier(TypeInstance->toString())] =
                TypeInstance;
            return TypeInstance;
        }

        sema::VoidType     *getVoidType() const { return VoidTy; }
        sema::IntType      *getIntType() const { return I32Ty; }
        sema::IntType      *getLongType() const { return I64Ty; }
        sema::IntType      *getUIntType() const { return U32Ty; }
        sema::IntType      *getULongType() const { return U64Ty; }
        sema::FloatType    *getFloatType() const { return F32Ty; }
        sema::FloatType    *getDoubleType() const { return F64Ty; }
        sema::BoolType     *getBoolType() const { return BoolTy; }
        sema::ArrayType    *getArrayType(sema::Type *ElementType, size_t Size);
        sema::FunctionType *getFunctionType(sema::Type *ReturnType,
                                            size_t      Size);
        sema::Type         *getTypeByName(support::Identifier *Name);
    };
} // namespace astra::frontend
