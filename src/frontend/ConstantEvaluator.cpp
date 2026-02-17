#include "astra/frontend/ConstantEvaluator.hpp"
#include "astra/support/Diagnostic.hpp"

namespace astra::frontend {
    void ConstantEvaluator::visitIntLiteral(ast::IntLiteral *IntLiteral) {
        ConstantCache[IntLiteral] = ConstantValue(IntLiteral->Value);
    }

    void ConstantEvaluator::visitBoolLiteral(ast::BoolLiteral *BoolLiteral) {
        ConstantCache[BoolLiteral] = ConstantValue(BoolLiteral->Value);
    }

    void ConstantEvaluator::visitNullLiteral(ast::NullLiteral *NullLiteral) {
        ConstantCache[NullLiteral] = ConstantValue(nullptr);
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
        if (OperandValue.isArray() || OperandValue.isString()) {
            // TODO: implement constant evaluation for unary operations on array and string values.
        }
        switch (UnaryExpr->Operator) {
        case ast::Op::Add:
            break;
        case ast::Op::Sub:
            ConstantCache[UnaryExpr] = -OperandValue;
            break;
        case ast::Op::Not:
            ConstantCache[UnaryExpr] = !OperandValue;
            break;
        case ast::Op::BitNot:
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
        switch (BinaryExpr->Operator) {
        case ast::Op::Add:
            ConstantCache[BinaryExpr] = LHSValue + RHSValue;
            break;
        case ast::Op::Sub:
            ConstantCache[BinaryExpr] = LHSValue - RHSValue;
            break;
        case ast::Op::Mult:
            ConstantCache[BinaryExpr] = LHSValue * RHSValue;
                break;
        case ast::Op::Div:

        default:
            break;
        }
    }
} // namespace astra::frontend