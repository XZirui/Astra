#include "astra/ast/ASTDumper.hpp"
#include "astra/support/SourceRangeFmt.hpp"

template<>
struct fmt::formatter<astra::ast::Op> {
    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const astra::ast::Op &op, FormatContext &ctx) {
        switch (op) {
            using astra::ast::Op;
            case Op::Add: return fmt::format_to(ctx.out(), "+");
            case Op::Sub: return fmt::format_to(ctx.out(), "-");
            case Op::Mult: return fmt::format_to(ctx.out(), "*");
            case Op::Div: return fmt::format_to(ctx.out(), "/");
            case Op::Mod: return fmt::format_to(ctx.out(), "%");
            case Op::Eq: return fmt::format_to(ctx.out(), "==");
            case Op::Neq: return fmt::format_to(ctx.out(), "!=");
            case Op::Lt: return fmt::format_to(ctx.out(), "<");
            case Op::Gt: return fmt::format_to(ctx.out(), ">");
            case Op::Le: return fmt::format_to(ctx.out(), "<=");
            case Op::Ge: return fmt::format_to(ctx.out(), ">=");
            case Op::Disj: return fmt::format_to(ctx.out(), "||");
            case Op::Conj: return fmt::format_to(ctx.out(), "&&");
            default: return fmt::format_to(ctx.out(), "UnknownOp");
        }
    }
};

namespace astra::ast {
    ASTDumper::ASTDumper(support::Printer &out) : out(out) {
    }

    void ASTDumper::dump(const Program *program) {
        out.print("AST Dump:\n");

        visit(program);
    }

    void ASTDumper::visitProgram(const Program *program) {
        out.print("Program {}:\n", program->range);
        out.push();
        for (const auto top_level_object: program->objects) {
            visit(top_level_object);
        }
        out.pop();
    }

    void ASTDumper::visitTopLevelObject(const TopLevelObject *top_level_object) {
        visit(top_level_object->stmt);
    }

    void ASTDumper::visitBlockStmt(const BlockStmt *block_stmt) {
        out.print("BlockStmt {}:\n", block_stmt->range);
        out.push();
        for (const auto s: block_stmt->statements) {
            visit(s);
        }
        out.pop();
    }

    void ASTDumper::visitIfStmt(const IfStmt *if_stmt) {
        out.print("IfStmt {}:\n", if_stmt->range);
        out.push();
        out.print("Condition:\n");
        visit(if_stmt->condition);
        out.print("Then:\n");
        visit(if_stmt->thenBranch);
        if (if_stmt->elseBranch) {
            out.print("Else:\n");
            visit(if_stmt->elseBranch);
        }
        out.pop();
    }

    void ASTDumper::visitExprStmt(const ExprStmt *expr_stmt) {
        out.print("ExprStmt {}:\n", expr_stmt->range);
        out.push();
        visit(expr_stmt->expr);
        out.pop();
    }

    void ASTDumper::visitReturnStmt(const ReturnStmt *return_stmt) {
        out.print("ReturnStmt {}:\n", return_stmt->range);
        out.push();
        if (return_stmt->expr) {
            visit(return_stmt->expr);
        }
        out.pop();
    }

    void ASTDumper::visitLiteralExpr(const LiteralExpr *literal_expr) {
        out.print("LiteralExpr {}:\n", literal_expr->range);
        out.push();
        out.print("Value={}\n", literal_expr->value);
        out.pop();
    }

    void ASTDumper::visitVarExpr(const VarExpr *var_expr) {
        out.print("VarExpr {}:\n", var_expr->range);
        out.push();
        out.print("Name={}\n", var_expr->name->getName());
        out.pop();
    }

    void ASTDumper::visitUnaryExpr(const UnaryExpr *unary_expr) {
        out.print("UnaryExpr {}:\n", unary_expr->range);
        out.push();
        out.print("Op: {}\n", unary_expr->op);
        visit(unary_expr->operand);
        out.pop();
    }

    void ASTDumper::visitBinaryExpr(const BinaryExpr *binary_expr) {
        out.print("BinaryExpr {}:\n", binary_expr->range);
        out.push();
        out.print("Op: {}\n", binary_expr->op);
        out.print("LHS:\n");
        out.push();
        visit(binary_expr->lhs);
        out.pop();
        out.print("RHS:\n");
        out.push();
        visit(binary_expr->rhs);
        out.pop();
        out.pop();
    }
}
