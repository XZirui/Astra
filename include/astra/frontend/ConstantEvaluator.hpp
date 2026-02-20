#pragma once

#include "astra/ast/ASTVisitor.hpp"
#include "astra/frontend/CompilerContext.hpp"
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
        enum class ValueKind { Error, Null, Int, Float, Bool, String, Array };

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
                    llvm::APFloat FloatValue(getBool() ? 1.0 : 0.0);
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

        static ConstantValue error() {
            ConstantValue Result;
            Result.Kind = ValueKind::Error;
            return Result;
        }

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
        bool      isError() const { return Kind == ValueKind::Error; }
        bool      isNull() const { return Kind == ValueKind::Null; }
        bool      isInt() const { return Kind == ValueKind::Int; }
        bool      isFloat() const { return Kind == ValueKind::Float; }
        bool      isBool() const { return Kind == ValueKind::Bool; }
        bool      isString() const { return Kind == ValueKind::String; }
        bool      isArray() const { return Kind == ValueKind::Array; }

        llvm::APSInt getInt() const {
            assert(isInt());
            return *Storage.IntValue;
        }

        llvm::APSInt toInt() const {
            if (isInt()) {
                return getInt();
            }
            if (isBool()) {
                return llvm::APSInt(llvm::APInt(32, getBool() ? 1 : 0));
            }
            assert(false && "Cannot convert to int");
        }

        llvm::APFloat getFloat() const {
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
            assert(Index.isInt() || Index.isBool());
            size_t I = Index.toInt().getZExtValue();
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
    return ConstantValue(LHS.toInt() OP RHS.toInt());

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
            return ConstantValue(toInt() % Other.toInt());
        }

        ConstantValue operator==(const ConstantValue &Other) const {
            ValueKind CommonKind = getCommonKind(Other);
            if (CommonKind == ValueKind::String) {
                // TODO string comparison
                return ConstantValue(false); // placeholder
            }
            if (CommonKind == ValueKind::Null) {
                return ConstantValue(true);
            }
            ConstantValue LHS = promoteTo(CommonKind);
            ConstantValue RHS = Other.promoteTo(CommonKind);
            if (CommonKind == ValueKind::Float) {
                return ConstantValue(LHS.getFloat() == RHS.getFloat());
            }
            if (CommonKind == ValueKind::Int) {
                return ConstantValue(LHS.getInt() == RHS.getInt());
            }
            return ConstantValue(LHS.getBool() == RHS.getBool());
        }

        ConstantValue operator!=(const ConstantValue &Other) const {
            return !(*this == Other);
        }

        ConstantValue operator<(const ConstantValue &Other) const {
            ValueKind CommonKind = getCommonKind(Other);
            if (CommonKind == ValueKind::String) {
                // TODO string comparison
                return ConstantValue(false); // placeholder
            }
            ConstantValue LHS = promoteTo(CommonKind);
            ConstantValue RHS = Other.promoteTo(CommonKind);
            if (CommonKind == ValueKind::Float) {
                return ConstantValue(LHS.getFloat() < RHS.getFloat());
            }
            if (CommonKind == ValueKind::Int) {
                return ConstantValue(LHS.getInt() < RHS.getInt());
            }
            return ConstantValue(LHS.getBool() < RHS.getBool());
        }

        ConstantValue operator>(const ConstantValue &Other) const {
            return Other < *this;
        }

        ConstantValue operator<=(const ConstantValue &Other) const {
            return !(Other < *this);
        }

        ConstantValue operator>=(const ConstantValue &Other) const {
            return !(*this < Other);
        }

        ConstantValue operator&(const ConstantValue &Other) const {
            return ConstantValue(toInt() & Other.toInt());
        }

        ConstantValue operator|(const ConstantValue &Other) const {
            return ConstantValue(toInt() | Other.toInt());
        }

        ConstantValue operator^(const ConstantValue &Other) const {
            return ConstantValue(toInt() ^ Other.toInt());
        }

        ConstantValue operator~() const { return ConstantValue(~toInt()); }

        ConstantValue operator<<(const ConstantValue &Other) const {
            return ConstantValue(toInt() << Other.toInt().getZExtValue());
        }

        ConstantValue operator>>(const ConstantValue &Other) const {
            return ConstantValue(
                llvm::APSInt(toInt().ashr(Other.toInt().getZExtValue()),
                             toInt().isSigned()));
        }

        ConstantValue operator&&(const ConstantValue &Other) const {
            return ConstantValue(toBool() && Other.toBool());
        }

        ConstantValue operator||(const ConstantValue &Other) const {
            return ConstantValue(toBool() || Other.toBool());
        }

        ConstantValue operator+() const { return *this; }
        ConstantValue operator-() const {
            assert(assert((isInt() || isFloat() || isBool()) &&
                   "Unary minus requires numeric operand");
            return isInt() ? ConstantValue(-toInt())
                           : ConstantValue(-getFloat());
        }

        ConstantValue operator!() const { return ConstantValue(!toBool()); }
    };

    class ConstantEvaluator : public ast::ExprVisitor<ConstantEvaluator> {
        llvm::DenseMap<const ast::Expr *, ConstantValue> ConstantCache;
        support::DiagnosticEngine                       &DiagEngine;

    public:
        explicit ConstantEvaluator(const CompilerContext &CompilerCtx)
            : DiagEngine(CompilerCtx.getDiagEngine()) {}

        void evaluate(ast::Expr *E) { visit(E); }

        ConstantValue *getConstantValue(ast::Expr *E) {
            auto It = ConstantCache.find(E);
            if (It != ConstantCache.end()) {
                return &It->second;
            }
            return nullptr;
        }
        void visitIntLiteral(ast::IntLiteral *IntLiteral);
        void visitBoolLiteral(ast::BoolLiteral *BoolLiteral);
        void visitNullLiteral(ast::NullLiteral *NullLiteral);
        void visitFloatLiteral(ast::FloatLiteral *FloatLiteral);
        void visitVarExpr(ast::VarExpr *VarExpr);
        void visitUnaryExpr(ast::UnaryExpr *UnaryExpr);
        void visitBinaryExpr(ast::BinaryExpr *BinaryExpr);
    };
} // namespace astra::frontend

template <> struct fmt::formatter<astra::frontend::ConstantValue::ValueKind> {
    constexpr auto parse(format_parse_context &Ctx) { return Ctx.begin(); }

    template <typename FormatContext>
    auto format(const astra::frontend::ConstantValue::ValueKind &Kind,
                FormatContext                                   &Ctx) {
        using enum astra::frontend::ConstantValue::ValueKind;
        switch (Kind) {
        case Error:
            return fmt::format_to(Ctx.out(), "error");
        case Null:
            return fmt::format_to(Ctx.out(), "null");
        case Int:
            return fmt::format_to(Ctx.out(), "int");
        case Float:
            return fmt::format_to(Ctx.out(), "float");
        case Bool:
            return fmt::format_to(Ctx.out(), "bool");
        case String:
            return fmt::format_to(Ctx.out(), "string");
        case Array:
            return fmt::format_to(Ctx.out(), "array");
        }
    }
};