#pragma once

#include "astra/ast/ASTVisitor.hpp"
#include <llvm/ADT/DenseMap.h>

namespace astra::frontend {
    inline auto operator<=>(const llvm::APSInt &LHS, const llvm::APSInt &RHS) {
        int Cmp = llvm::APSInt::compareValues(LHS, RHS);
        if (Cmp < 0) {
            return std::strong_ordering::less;
        }
        if (Cmp > 0) {
            return std::strong_ordering::greater;
        }
        return std::strong_ordering::equal;
    }
    inline auto operator<=>(const llvm::APFloat &LHS,
                            const llvm::APFloat &RHS) {
        switch (LHS.compare(RHS)) {
        case llvm::APFloat::cmpLessThan:
            return std::partial_ordering::less;
        case llvm::APFloat::cmpGreaterThan:
            return std::partial_ordering::greater;
        case llvm::APFloat::cmpEqual:
            return std::partial_ordering::equivalent;
        default:
            return std::partial_ordering::unordered; // NaN case
        }
    }

    class ConstantValue {
    public:
        enum class ValueKind { Null, Int, Float, Bool, String, Array };

    private:
        ValueKind Kind = ValueKind::Null;

        union {
            llvm::APSInt               *IntValue;
            llvm::APFloat              *FloatValue;
            bool                        BoolValue;
            std::vector<ConstantValue> *ArrayValue;
            // TODO support for string
        } Storage{};

        void destroy() {
            switch (Kind) {
            case ValueKind::Int:
                delete Storage.IntValue;
                break;
            case ValueKind::Float:
                delete Storage.FloatValue;
                break;
            case ValueKind::Array:
                delete Storage.ArrayValue;
                break;
            default:
                break; // no need to destroy
            }
            Storage = {};
        }

        ValueKind getCommonKind(const ConstantValue &Other) const {
            if (Kind == Other.Kind) {
                return Kind;
            }

            if (isString() || Other.isString() || isArray() ||
                Other.isArray()) {
                return ValueKind::Null; // Incompatible types
            }

            // Float > Int > Bool
            if (isFloat() || Other.isFloat()) {
                return ValueKind::Float;
            }
            if (isInt() || Other.isInt()) {
                return ValueKind::Int;
            }
            return ValueKind::Bool;
        }

        ConstantValue promoteTo(ValueKind Target) const {
            if (Kind == Target) {
                return *this;
            }

            switch (Target) {
            case ValueKind::Float:
                if (isInt()) {
                    llvm::APFloat FloatValue{llvm::APFloat::IEEEdouble()};
                    FloatValue.convertFromAPInt(
                        *Storage.IntValue, Storage.IntValue->isSigned(),
                        llvm::APFloat::rmNearestTiesToEven);
                    return ConstantValue(std::move(FloatValue));
                }
                if (isBool()) {
                    llvm::APFloat FloatValue(llvm::APFloat::IEEEdouble(),
                                             getBool() ? 1.0 : 0.0);
                    return ConstantValue(std::move(FloatValue));
                }
                break;
            case ValueKind::Int:
                if (isBool()) {
                    return ConstantValue(
                        llvm::APSInt(llvm::APInt(32, getBool() ? 1 : 0)));
                }
                break;
            default:
                break;
            }
            assert(false && "Invalid promotion");
        }

    public:
        ConstantValue() = default;
        ~ConstantValue() { destroy(); }

        explicit ConstantValue(llvm::APSInt IntValue) : Kind(ValueKind::Int) {
            Storage.IntValue = new llvm::APSInt(std::move(IntValue));
        }

        explicit ConstantValue(llvm::APFloat FloatValue)
            : Kind(ValueKind::Float) {
            Storage.FloatValue = new llvm::APFloat(std::move(FloatValue));
        }

        explicit ConstantValue(bool BoolValue) : Kind(ValueKind::Bool) {
            Storage.BoolValue = BoolValue;
        }

        ValueKind getKind() const { return Kind; }
        bool      isInt() const { return Kind == ValueKind::Int; }
        bool      isFloat() const { return Kind == ValueKind::Float; }
        bool      isBool() const { return Kind == ValueKind::Bool; }
        bool      isString() const { return Kind == ValueKind::String; }
        bool      isArray() const { return Kind == ValueKind::Array; }

        const llvm::APSInt &getInt() const {
            assert(isInt());
            return *Storage.IntValue;
        }

        const llvm::APFloat &getFloat() const {
            assert(isFloat());
            return *Storage.FloatValue;
        }

        bool getBool() const {
            assert(isBool());
            return Storage.BoolValue;
        }

        bool toBool() const {
            if (isBool())
                return getBool();
            if (isInt())
                return !getInt().isZero();
            if (isFloat())
                return !getFloat().isZero();
            return false; // null/string/array
            // TODO: string/array should be handled properly when supported
        }

        ConstantValue operator[](ConstantValue Index) const {
            assert(isArray());
            assert(Index.isInt());
            size_t I = Index.getInt().getZExtValue();
            assert(I < Storage.ArrayValue->size());
            return (*Storage.ArrayValue)[I];
        }

#define BINARY_OP(OP)                                                          \
    ValueKind     CommonKind = getCommonKind(Other);                           \
    ConstantValue LHS = promoteTo(CommonKind);                                 \
    ConstantValue RHS = Other.promoteTo(CommonKind);                           \
                                                                               \
    if (CommonKind == ValueKind::Float) {                                      \
        return ConstantValue(LHS.getFloat() OP RHS.getFloat());                \
    }                                                                          \
    return ConstantValue(LHS.getInt() OP RHS.getInt());

        ConstantValue operator+(const ConstantValue &Other) const {
            if (isString() && Other.isString()) {
                // TODO string concatenation
                return ConstantValue(); // placeholder
            }
            BINARY_OP(+);
        }

        ConstantValue operator-(const ConstantValue &Other) const {
            BINARY_OP(-);
        }

        ConstantValue operator*(const ConstantValue &Other) const {
            BINARY_OP(*);
        }

        ConstantValue operator/(const ConstantValue &Other) const {
            BINARY_OP(/);
        }

        ConstantValue operator%(const ConstantValue &Other) const {
            ValueKind CommonKind = getCommonKind(Other);
            if (CommonKind == ValueKind::Float) {
                assert(
                    false &&
                    "Modulo operator is not defined for floating-point types");
            }
            return ConstantValue(getInt() % Other.getInt());
        }

        ConstantValue operator&(const ConstantValue &Other) const {
            assert(isInt() && Other.isInt() &&
                   "Bitwise AND requires integer operands");
            return ConstantValue(getInt() & Other.getInt());
        }

        ConstantValue operator|(const ConstantValue &Other) const {
            assert(isInt() && Other.isInt() &&
                   "Bitwise OR requires integer operands");
            return ConstantValue(getInt() | Other.getInt());
        }

        ConstantValue operator^(const ConstantValue &Other) const {
            assert(isInt() && Other.isInt() &&
                   "Bitwise XOR requires integer operands");
            return ConstantValue(getInt() ^ Other.getInt());
        }

        ConstantValue operator~() const {
            assert(isInt() && "Bitwise NOT requires integer operand");
            return ConstantValue(~getInt());
        }

        ConstantValue operator<<(const ConstantValue &Other) const {
            assert(isInt() && Other.isInt() &&
                   "Shift requires integer operands");
            return ConstantValue(getInt() << Other.getInt().getZExtValue());
        }

        ConstantValue operator>>(const ConstantValue &Other) const {
            assert(isInt() && Other.isInt() &&
                   "Shift requires integer operands");
            return ConstantValue(
                llvm::APSInt(getInt().ashr(Other.getInt().getZExtValue()),
                             getInt().isSigned()));
        }

        ConstantValue operator&&(const ConstantValue &Other) const {
            return ConstantValue(toBool() && Other.toBool());
        }

        ConstantValue operator||(const ConstantValue &Other) const {
            return ConstantValue(toBool() || Other.toBool());
        }

        ConstantValue operator!() const {
            return ConstantValue(!toBool());
        }
    };

    class ConstantEvaluator : public ast::ExprVisitor<ConstantEvaluator> {
        llvm::DenseMap<const ast::Expr *, ConstantValue> ConstantCache;

    public:
        void evaluate(ast::Expr *E) { visit(E); }
        void visitIntLiteral(ast::IntLiteral *IntLiteral);
        void visitBoolLiteral(ast::BoolLiteral *BoolLiteral);
        void visitNullLiteral(ast::NullLiteral *NullLiteral);
        void visitFloatLiteral(ast::FloatLiteral *FloatLiteral);
        void visitVarExpr(ast::VarExpr *VarExpr);
        void visitUnaryExpr(ast::UnaryExpr *UnaryExpr);
        void visitBinaryExpr(ast::BinaryExpr *BinaryExpr);
    };
} // namespace astra::frontend