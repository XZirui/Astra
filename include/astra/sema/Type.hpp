#pragma once

#include <string>

#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/ArrayRef.h"

namespace astra::sema {
    class Type {
    public:
        enum class Kind {
            Builtin,
            Array,
            Function,
            Class,
        };

        Kind getKind() const { return TypeKind; }

    protected:
        Kind TypeKind;

        explicit Type(Kind K) : TypeKind(K) {}

    public:
        virtual ~Type() = default;

        [[nodiscard]]
        virtual std::string toString() const = 0;
    };

    class BuiltinType : public Type {
    public:
        enum class Kind {
            Void,
            Int,
            Float,
            Bool,
        };

        BuiltinType(Kind K) : Type(Type::Kind::Builtin), BuiltinTypeKind(K) {}

    protected:
        Kind BuiltinTypeKind;
    };

    class VoidType final : public BuiltinType {
    public:
        VoidType() : BuiltinType(Kind::Void) {}

        std::string toString() const override { return "void"; }
    };

    class IntType final : public BuiltinType {
        size_t BitWidth;
        bool   IsSigned;

    public:
        explicit IntType(size_t Width = 32, bool Signed = true)
            : BuiltinType(Kind::Int), BitWidth(Width), IsSigned(Signed) {}

        std::string toString() const override {
            return (IsSigned ? "i" : "u") + std::to_string(BitWidth);
        }

        size_t getBitWidth() const { return BitWidth; }
        bool   isSigned() const { return IsSigned; }
    };

    class FloatType final : public BuiltinType {
        size_t BitWidth;

    public:
        explicit FloatType(size_t Width = 64)
            : BuiltinType(Kind::Float), BitWidth(Width) {}

        std::string toString() const override {
            return "f" + std::to_string(BitWidth);
        }
    };

    class BoolType final : public BuiltinType {
    public:
        BoolType() : BuiltinType(Kind::Bool) {}

        std::string toString() const override { return "bool"; }
    };

    class ArrayType final : public Type, public llvm::FoldingSetNode {
        Type  *ElementType;
        size_t Size;

    public:
        ArrayType(Type *ElementType, size_t Size)
            : Type(Kind::Array), ElementType(ElementType), Size(Size) {}

        Type  *getElementType() const { return ElementType; }
        size_t getSize() const { return Size; }

        std::string toString() const override {
            return ElementType->toString() + "[" + std::to_string(Size) + "]";
        }

        static void profile(llvm::FoldingSetNodeID &ID, Type *ElementType,
                            size_t Size) {
            ID.AddInteger(static_cast<unsigned>(Kind::Array));
            ID.AddPointer(ElementType);
            ID.AddInteger(Size);
        }

        void Profile(llvm::FoldingSetNodeID &ID) const {
            profile(ID, ElementType, Size);
        }
    };

    class FunctionType final : public Type, public llvm::FoldingSetNode {
        Type                        *ReturnType;
        llvm::SmallVector<Type *, 4> ParamTypes;

    public:
        FunctionType(Type *ReturnType, llvm::ArrayRef<Type *> ParamTypes)
            : Type(Kind::Function), ReturnType(ReturnType),
              ParamTypes(ParamTypes) {}

        Type                  *getReturnType() const { return ReturnType; }
        llvm::ArrayRef<Type *> getParamTypes() const { return ParamTypes; }
        size_t getParamCount() const { return ParamTypes.size(); }
        Type  *getParamType(size_t Index) const { return ParamTypes[Index]; }

        std::string toString() const override {
            std::string Result = "(";
            for (size_t Index = 0; Index < ParamTypes.size(); ++Index) {
                if (Index > 0) {
                    Result += ", ";
                }
                Result += ParamTypes[Index]->toString();
            }
            Result += ") -> " + ReturnType->toString();
            return Result;
        }

        static void profile(llvm::FoldingSetNodeID &ID, Type *ReturnType,
                            llvm::ArrayRef<Type *> ParamTypes) {
            ID.AddInteger(static_cast<unsigned>(Kind::Function));
            ID.AddPointer(ReturnType);
            ID.AddInteger(ParamTypes.size());
            for (auto *ParamType : ParamTypes) {
                ID.AddPointer(ParamType);
            }
        }

        void Profile(llvm::FoldingSetNodeID &ID) const {
            profile(ID, ReturnType, ParamTypes);
        }
    };
} // namespace astra::sema
