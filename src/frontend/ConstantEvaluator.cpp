#include "astra/frontend/ConstantEvaluator.hpp"
#include <fmt/format.h>

namespace astra::frontend {
    using enum ast::Op;

    void ConstantEvaluator::visitIntLiteral(ast::IntLiteral *IntLiteral) {
        ConstantCache[IntLiteral] = ConstantValue(IntLiteral->Value);
    }

    void ConstantEvaluator::visitBoolLiteral(ast::BoolLiteral *BoolLiteral) {
        ConstantCache[BoolLiteral] = ConstantValue(BoolLiteral->Value);
    }

    void ConstantEvaluator::visitNullLiteral(ast::NullLiteral *NullLiteral) {
        ConstantCache[NullLiteral] = ConstantValue();
    }

    void ConstantEvaluator::visitFloatLiteral(ast::FloatLiteral *FloatLiteral) {
        ConstantCache[FloatLiteral] = ConstantValue(FloatLiteral->Value);
    }

    void ConstantEvaluator::visitVarExpr(ast::VarExpr *VarExpr) {
        // TODO: need to handle constant variables
    }

    void ConstantEvaluator::visitUnaryExpr(ast::UnaryExpr *UnaryExpr) {
        if (!UnaryExpr->Operand->IsConstexpr) {
            return;
        }
        UnaryExpr->IsConstexpr = true;
        const auto &OperandValue = ConstantCache[UnaryExpr->Operand];
        if (OperandValue.isError()) {
            // Avoid cascading errors
            ConstantCache[UnaryExpr] = ConstantValue::error();
            return;
        }
        if (OperandValue.isArray() || OperandValue.isString()) {
            // TODO: implement constant evaluation for unary operations on array
            // and string values.
            return;
        }
        if ((OperandValue.isBool() && UnaryExpr->Operator != Not) ||
            OperandValue.isNull()) {
            ConstantCache[UnaryExpr] = ConstantValue::error();
            auto Message = fmt::format(
                "Operator '{}' cannot be applied to operand of type '{}'",
                UnaryExpr->Operator, OperandValue.getKind());
            DiagEngine.error(UnaryExpr->Range, Message);
            return;
        }
        switch (UnaryExpr->Operator) {
        case Add:
            break;
        case Sub:
            ConstantCache[UnaryExpr] = -OperandValue;
            break;
        case Not:
            ConstantCache[UnaryExpr] = !OperandValue;
            break;
        case BitNot:
            ConstantCache[UnaryExpr] = ~OperandValue;
            break;
        default:
            break;
        }
    }

    void ConstantEvaluator::visitBinaryExpr(ast::BinaryExpr *BinaryExpr) {
        if (!BinaryExpr->LHS->IsConstexpr || !BinaryExpr->RHS->IsConstexpr) {
            return;
        }
        BinaryExpr->IsConstexpr = true;
        const auto &LHSValue = ConstantCache[BinaryExpr->LHS];
        const auto &RHSValue = ConstantCache[BinaryExpr->RHS];
        if (LHSValue.isError() || RHSValue.isError()) {
            // Avoid cascading errors
            ConstantCache[BinaryExpr] = ConstantValue::error();
            return;
        }
        if (LHSValue.isNull() && RHSValue.isNull()) {
            switch (BinaryExpr->Operator) {
            case Eq:
                ConstantCache[BinaryExpr] = ConstantValue(true);
                return;
            case Neq:
                ConstantCache[BinaryExpr] = ConstantValue(false);
                return;
            default:
                auto Message = fmt::format("Operator '{}' cannot be applied to "
                                           "operands of type 'null' and 'null'",
                                           BinaryExpr->Operator);
                DiagEngine.error(BinaryExpr->Range, Message);
                ConstantCache[BinaryExpr] = ConstantValue::error();
                return;
            }
        }
        if (LHSValue.isArray() || RHSValue.isArray() || LHSValue.isNull() ||
            RHSValue.isNull()) {
            auto Message = fmt::format("Operator '{}' cannot be applied to "
                                       "operands of type '{}' and '{}'",
                                       BinaryExpr->Operator, LHSValue.getKind(),
                                       RHSValue.getKind());
            DiagEngine.error(BinaryExpr->Range, Message);
            ConstantCache[BinaryExpr] = ConstantValue::error();
            return;
        }
        if (LHSValue.isString() && RHSValue.isString()) {
            switch (BinaryExpr->Operator) {
            case Add:
                // TODO: implement string concatenation
                ConstantCache[BinaryExpr] = ConstantValue(); // placeholder
                return;
            case Eq:
                // TODO: implement string comparison
                ConstantCache[BinaryExpr] = (LHSValue == RHSValue);
                return;
            case Neq:
                ConstantCache[BinaryExpr] = (LHSValue != RHSValue);
                return;
            case Lt:
                ConstantCache[BinaryExpr] = (LHSValue < RHSValue);
                return;
            case Gt:
                ConstantCache[BinaryExpr] = (LHSValue > RHSValue);
                return;
            case Le:
                ConstantCache[BinaryExpr] = (LHSValue <= RHSValue);
                return;
            case Ge:
                ConstantCache[BinaryExpr] = (LHSValue >= RHSValue);
                return;
            default:
                auto Message =
                    fmt::format("Operator '{}' cannot be applied to operands "
                                "of type 'string' and 'string'",
                                BinaryExpr->Operator);
                DiagEngine.error(BinaryExpr->Range, Message);
                ConstantCache[BinaryExpr] = ConstantValue::error();
                return;
            }
        }
        if (LHSValue.isString() || RHSValue.isString()) {
            auto Message = fmt::format("Operator '{}' cannot be applied to "
                                       "operands of type '{}' and '{}'",
                                       BinaryExpr->Operator, LHSValue.getKind(),
                                       RHSValue.getKind());
            DiagEngine.error(BinaryExpr->Range, Message);
            ConstantCache[BinaryExpr] = ConstantValue::error();
            return;
        }

        if ((LHSValue.isFloat() || RHSValue.isFloat()) &&
            (BinaryExpr->Operator >= LShift &&
             BinaryExpr->Operator <= BitNot)) {
            // Bitwise operators are not defined for floating-point types
            auto Message = fmt::format("Operator '{}' cannot be applied to "
                                       "operands of type '{}' and '{}'",
                                       BinaryExpr->Operator, LHSValue.getKind(),
                                       RHSValue.getKind());
            DiagEngine.error(BinaryExpr->Range, Message);
            ConstantCache[BinaryExpr] = ConstantValue::error();
            return;
        }

        // int, float and bool
        switch (BinaryExpr->Operator) {
        case Add:
            ConstantCache[BinaryExpr] = LHSValue + RHSValue;
            break;
        case Sub:
            ConstantCache[BinaryExpr] = LHSValue - RHSValue;
            break;
        case Mult:
            ConstantCache[BinaryExpr] = LHSValue * RHSValue;
            break;
        case Div:
            if ((RHSValue.isInt() && RHSValue.getInt() == 0) ||
                (RHSValue.isBool() && RHSValue.getBool() == false)) {
                auto Message = fmt::format("Division by zero");
                DiagEngine.error(BinaryExpr->Range, Message);
                ConstantCache[BinaryExpr] = ConstantValue::error();
                return;
            }
            ConstantCache[BinaryExpr] = LHSValue / RHSValue;
            break;
        case Mod:
            if (LHSValue.isFloat() || RHSValue.isFloat()) {
                auto Message =
                    fmt::format("Operator '%' cannot be applied to operands of "
                                "type '{}' and '{}'",
                                LHSValue.getKind(), RHSValue.getKind());
                DiagEngine.error(BinaryExpr->Range, Message);
                ConstantCache[BinaryExpr] = ConstantValue::error();
                return;
            }
            if ((RHSValue.isInt() && RHSValue.getInt() == 0) ||
                (RHSValue.isBool() && RHSValue.getBool() == false)) {
                auto Message = fmt::format("Division by zero");
                DiagEngine.error(BinaryExpr->Range, Message);
                ConstantCache[BinaryExpr] = ConstantValue::error();
                return;
            }
            ConstantCache[BinaryExpr] = LHSValue % RHSValue;
            break;
        case Eq:
            ConstantCache[BinaryExpr] = (LHSValue == RHSValue);
            break;
        case Neq:
            ConstantCache[BinaryExpr] = (LHSValue != RHSValue);
            break;
        case LShift:
            ConstantCache[BinaryExpr] = LHSValue << RHSValue;
            break;
        case RShift:
            ConstantCache[BinaryExpr] = LHSValue >> RHSValue;
            break;
        case BitAnd:
            ConstantCache[BinaryExpr] = LHSValue & RHSValue;
            break;
        case BitXor:
            ConstantCache[BinaryExpr] = LHSValue ^ RHSValue;
            break;
        case BitOr:
            ConstantCache[BinaryExpr] = LHSValue | RHSValue;
            break;
        case BitNot:
            ConstantCache[BinaryExpr] = ~LHSValue;
            break;
        case Lt:
            ConstantCache[BinaryExpr] = (LHSValue < RHSValue);
            break;
        case Gt:
            ConstantCache[BinaryExpr] = (LHSValue > RHSValue);
            break;
        case Le:
            ConstantCache[BinaryExpr] = (LHSValue <= RHSValue);
            break;
        case Ge:
            ConstantCache[BinaryExpr] = (LHSValue >= RHSValue);
            break;
        case Disj:
            ConstantCache[BinaryExpr] = LHSValue || RHSValue;
            break;
        case Conj:
            ConstantCache[BinaryExpr] = LHSValue && RHSValue;
            break;
        default:
            break;
        }
    }
} // namespace astra::frontend