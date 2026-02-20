#pragma once

#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/FoldingSet.h>
#include <llvm/ADT/StringRef.h>
#include <string>

namespace astra::sema {
    class Type {
    public:
        enum class Kind {
            BuiltinVoid,
            BuiltinBool,
            BuiltinInt,
            BuiltinFloat,
            Array,
            Function,
            Class,
            Error,
        };

        Kind getKind() const { return TypeKind; }
        bool isBuiltinType() const {
            return TypeKind >= Kind::BuiltinVoid &&
                   TypeKind <= Kind::BuiltinFloat;
        }
        bool isArrayType() const { return TypeKind == Kind::Array; }
        bool isFunctionType() const { return TypeKind == Kind::Function; }
        bool isClassType() const { return TypeKind == Kind::Class; }
        bool isErrorType() const { return TypeKind == Kind::Error; }

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
        explicit BuiltinType(Kind K) : Type(K) {}

        struct TypeRank {
            size_t Rank = 0;
            size_t AdditionalInfo = 0; // e.g., bit width

            auto operator<=>(const TypeRank &) const = default;
        };

        virtual TypeRank getTypeRank() const = 0;

        static bool classof(const Type *T) { return T->isBuiltinType(); }
    };

    class VoidType final : public BuiltinType {
    public:
        VoidType() : BuiltinType(Kind::BuiltinVoid) {}

        std::string toString() const override { return "void"; }
        TypeRank    getTypeRank() const override {
            return {static_cast<size_t>(Kind::BuiltinVoid)};
        }

        static bool classof(const Type *T) {
            return T->getKind() == Kind::BuiltinVoid;
        }
    };

    class IntType final : public BuiltinType {
        size_t BitWidth;
        bool   IsSigned;

    public:
        explicit IntType(size_t Width = 32, bool Signed = true)
            : BuiltinType(Kind::BuiltinInt), BitWidth(Width), IsSigned(Signed) {
        }

        std::string toString() const override {
            return (IsSigned ? "i" : "u") + std::to_string(BitWidth);
        }

        size_t   getBitWidth() const { return BitWidth; }
        bool     isSigned() const { return IsSigned; }
        TypeRank getTypeRank() const override {
            return {static_cast<size_t>(Kind::BuiltinInt), BitWidth};
        }

        static bool classof(const Type *T) {
            return T->getKind() == Kind::BuiltinInt;
        }
    };

    class FloatType final : public BuiltinType {
        size_t BitWidth;

    public:
        explicit FloatType(size_t Width = 64)
            : BuiltinType(Kind::BuiltinFloat), BitWidth(Width) {}

        std::string toString() const override {
            return "f" + std::to_string(BitWidth);
        }
        size_t   getBitWidth() const { return BitWidth; }
        TypeRank getTypeRank() const override {
            return {static_cast<size_t>(Kind::BuiltinFloat), BitWidth};
        }

        static bool classof(const Type *T) {
            return T->getKind() == Kind::BuiltinFloat;
        }
    };

    class BoolType final : public BuiltinType {
    public:
        BoolType() : BuiltinType(Kind::BuiltinBool) {}

        std::string toString() const override { return "bool"; }
        TypeRank    getTypeRank() const override {
            return {static_cast<size_t>(Kind::BuiltinBool)};
        }

        static bool classof(const Type *T) {
            return T->getKind() == Kind::BuiltinBool;
        }
    };

    class ArrayType final : public Type, public llvm::FoldingSetNode {
        Type  *ElementType;
        size_t Size = 0; // 0 means array of unknown size

    public:
        explicit ArrayType(Type *ElementType)
            : Type(Kind::Array), ElementType(ElementType) {}
        ArrayType(Type *ElementType, size_t Size)
            : Type(Kind::Array), ElementType(ElementType), Size(Size) {}

        Type  *getElementType() const { return ElementType; }
        size_t getSize() const { return Size; }

        std::string toString() const override {
            return ElementType->toString() + "[" + std::to_string(Size) + "]";
        }

        static void profile(llvm::FoldingSetNodeID &ID, const Type *ElementType,
                            size_t Size = 0) {
            ID.AddInteger(static_cast<unsigned>(Kind::Array));
            ID.AddPointer(ElementType);
            ID.AddInteger(Size);
        }

        void Profile(llvm::FoldingSetNodeID &ID) const {
            profile(ID, ElementType, Size);
        }

        static bool classof(const Type *T) { return T->isArrayType(); }
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

        static void profile(llvm::FoldingSetNodeID &ID, const Type *ReturnType,
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

        static bool classof(const Type *T) { return T->isFunctionType(); }
    };

    class ErrorType final : public Type {
    public:
        ErrorType() : Type(Kind::Error) {}

        std::string toString() const override { return "error"; }

        static bool classof(const Type *T) { return T->isErrorType(); }
    };

    class ClassType final : public Type {
    public:
        struct Member {
            llvm::StringRef Name;
            Type           *Type;
            size_t          Index;
        };

    private:
        llvm::StringRef                         Name;
        std::vector<Member>                     Members;
        llvm::DenseMap<llvm::StringRef, size_t> MemberIndices{};
        bool                                    IsComplete;

    public:
        ClassType() : Type(Kind::Class), IsComplete(false) {}

        explicit ClassType(llvm::StringRef Name)
            : Type(Kind::Class), Name(Name), IsComplete(false) {}

        ClassType(llvm::StringRef Name, llvm::ArrayRef<Member> Members)
            : Type(Kind::Class), Name(Name), Members(Members),
              IsComplete(true) {
            for (size_t Index = 0; Index < Members.size(); ++Index) {
                MemberIndices[Members[Index].Name] = Index;
            }
        }

        void define(llvm::ArrayRef<Member> NewMembers) {
            Members = NewMembers;
            IsComplete = true;
            // MemberIndices should be empty
            // guaranteed by TypeContext
            for (size_t Index = 0; Index < Members.size(); ++Index) {
                MemberIndices[Members[Index].Name] = Index;
            }
        }

        std::string toString() const override { return "class " + Name.str(); }

        Member *getMember(llvm::StringRef Name) {
            auto It = MemberIndices.find(Name);
            if (It != MemberIndices.end()) {
                return &Members[It->second];
            }
            return nullptr;
        }

        bool   isComplete() const { return IsComplete; }
        size_t getMemberCount() const { return Members.size(); }
        size_t getMemberIndex(llvm::StringRef Name) {
            return MemberIndices[Name]; // assuming member exists, should be
                                        // checked by caller
        }

        bool addMember(llvm::StringRef Name, Type *Type) {
            if (MemberIndices.contains(Name)) {
                return false; // member already exists
            }
            Members.emplace_back(Name, Type, Members.size());
            MemberIndices[Name] = Members.size() - 1;
            return true;
        }
    };
} // namespace astra::sema
