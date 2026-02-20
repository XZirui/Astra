#pragma once

#include "CompilerContext.hpp"
#include "astra/sema/Type.hpp"
#include <llvm/ADT/StringMap.h>
#include <llvm/Support/Casting.h>

namespace astra::frontend {
    class TypeContext {
        CompilerContext &CompilerCtx;

        llvm::FoldingSet<sema::ArrayType>    ArrayTypes;
        llvm::FoldingSet<sema::FunctionType> FunctionTypes;
        llvm::StringMap<sema::ClassType>     ClassTypes;

        sema::VoidType  *VoidTy;
        sema::IntType   *I32Ty;
        sema::IntType   *I64Ty;
        sema::IntType   *U32Ty;
        sema::IntType   *U64Ty;
        sema::FloatType *F32Ty;
        sema::FloatType *F64Ty;
        sema::BoolType  *BoolTy;
        sema::ErrorType *ErrorTy;

        template <typename T, typename FoldingSetType, typename... Args>
        T *getOrCreateType(FoldingSetType &Set, Args &&...Arguments) {
            llvm::FoldingSetNodeID ID;

            // Call the static profile method to populate the ID
            T::profile(ID, std::forward<Args>(Arguments)...);

            void *InsertPos = nullptr;
            if (auto *Existing = Set.FindNodeOrInsertPos(ID, InsertPos)) {
                return llvm::cast<T>(Existing);
            }

            auto *NewType =
                CompilerCtx.create<T>(std::forward<Args>(Arguments)...);
            Set.InsertNode(NewType, InsertPos);
            return NewType;
        }

    public:
        explicit TypeContext(CompilerContext &CompilerCtx);

        TypeContext(const TypeContext &) = delete;
        TypeContext &operator=(const TypeContext &) = delete;

        sema::VoidType  *getVoidType() const { return VoidTy; }
        sema::IntType   *getIntType() const { return I32Ty; }
        sema::IntType   *getLongType() const { return I64Ty; }
        sema::IntType   *getUIntType() const { return U32Ty; }
        sema::IntType   *getULongType() const { return U64Ty; }
        sema::FloatType *getFloatType() const { return F32Ty; }
        sema::FloatType *getDoubleType() const { return F64Ty; }
        sema::BoolType  *getBoolType() const { return BoolTy; }
        sema::ErrorType *getErrorType() const { return ErrorTy; }
        sema::ArrayType *getArrayType(sema::Type *ElementType, size_t Size);
        sema::ArrayType *getArrayType(sema::Type *ElementType);
        sema::FunctionType *
        getFunctionType(sema::Type                  *ReturnType,
                        llvm::ArrayRef<sema::Type *> ParamTypes);
        sema::ClassType *getClassType(llvm::StringRef Name);
        sema::ClassType *getClassType(llvm::StringRef Name, llvm::ArrayRef<sema::ClassType::Member> Members);

        bool        canImplicitCast(sema::Type *From, sema::Type *To) const;
        sema::Type *getCommonType(sema::Type *T1, sema::Type *T2) const;
    };
} // namespace astra::frontend
