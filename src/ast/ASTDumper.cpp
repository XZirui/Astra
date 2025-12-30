#include "astra/ast/ASTDumper.hpp"
#include "astra/support/SourceRangeFmt.hpp"

template<>
struct fmt::formatter<astra::ast::Op> {
    constexpr auto parse(format_parse_context &Ctx) { return Ctx.begin(); }

    template<typename FormatContext>
    auto format(const astra::ast::Op &Operator, FormatContext &Ctx) {
        switch (Operator) {
                using enum astra::ast::Op;
            case Add: return fmt::format_to(Ctx.out(), "+");
            case Sub: return fmt::format_to(Ctx.out(), "-");
            case Mult: return fmt::format_to(Ctx.out(), "*");
            case Div: return fmt::format_to(Ctx.out(), "/");
            case Mod: return fmt::format_to(Ctx.out(), "%");
            case Eq: return fmt::format_to(Ctx.out(), "==");
            case Neq: return fmt::format_to(Ctx.out(), "!=");
            case Lt: return fmt::format_to(Ctx.out(), "<");
            case Gt: return fmt::format_to(Ctx.out(), ">");
            case Le: return fmt::format_to(Ctx.out(), "<=");
            case Ge: return fmt::format_to(Ctx.out(), ">=");
            case Disj: return fmt::format_to(Ctx.out(), "||");
            case Conj: return fmt::format_to(Ctx.out(), "&&");
            default: return fmt::format_to(Ctx.out(), "UnknownOp");
        }
    }
};

namespace astra::ast {
    ASTDumper::ASTDumper(support::Printer &Out) : Out(Out) {
    }

    void ASTDumper::dump(const Program *Program) {
        Out.print("AST Dump:\n");

        visit(Program);
    }

    void ASTDumper::visitProgram(const Program *Program) {
        Out.print("Program {}:\n", Program->Range);
        Out.push();
        for (auto *const Object: Program->Objects) {
            visit(Object);
        }
        Out.pop();
    }

    void ASTDumper::visitTopLevelObject(const TopLevelObject *TopLevelObject) {
        visit(TopLevelObject->Statement);
    }

    void ASTDumper::visitBlockStmt(const BlockStmt *BlockStmt) {
        Out.print("BlockStmt {}:\n", BlockStmt->Range);
        Out.push();
        for (auto *const Stmt: BlockStmt->Statements) {
            visit(Stmt);
        }
        Out.pop();
    }

    void ASTDumper::visitIfStmt(const IfStmt *IfStmt) {
        Out.print("IfStmt {}:\n", IfStmt->Range);
        Out.push();
        Out.print("Condition:\n");
        visit(IfStmt->Condition);
        Out.print("Then:\n");
        visit(IfStmt->ThenBranch);
        if (IfStmt->ElseBranch) {
            Out.print("Else:\n");
            visit(IfStmt->ElseBranch);
        }
        Out.pop();
    }

    void ASTDumper::visitExprStmt(const ExprStmt *ExprStmt) {
        Out.print("ExprStmt {}:\n", ExprStmt->Range);
        Out.push();
        visit(ExprStmt->Expression);
        Out.pop();
    }

    void ASTDumper::visitReturnStmt(const ReturnStmt *ReturnStmt) {
        Out.print("ReturnStmt {}:\n", ReturnStmt->Range);
        Out.push();
        if (ReturnStmt->Expression) {
            visit(ReturnStmt->Expression);
        }
        Out.pop();
    }

    void ASTDumper::visitLiteralExpr(const LiteralExpr *LiteralExpr) {
        Out.print("LiteralExpr {}:\n", LiteralExpr->Range);
        Out.push();
        Out.print("Value={}\n", LiteralExpr->Value);
        Out.pop();
    }

    void ASTDumper::visitVarExpr(const VarExpr *VarExpr) {
        Out.print("VarExpr {}:\n", VarExpr->Range);
        Out.push();
        Out.print("Name={}\n", VarExpr->Name->getName());
        Out.pop();
    }

    void ASTDumper::visitUnaryExpr(const UnaryExpr *UnaryExpr) {
        Out.print("UnaryExpr {}:\n", UnaryExpr->Range);
        Out.push();
        Out.print("Op: {}\n", UnaryExpr->Operator);
        visit(UnaryExpr->Operand);
        Out.pop();
    }

    void ASTDumper::visitBinaryExpr(const BinaryExpr *BinaryExpr) {
        Out.print("BinaryExpr {}:\n", BinaryExpr->Range);
        Out.push();
        Out.print("Op: {}\n", BinaryExpr->Operator);
        Out.print("LHS:\n");
        Out.push();
        visit(BinaryExpr->LHS);
        Out.pop();
        Out.print("RHS:\n");
        Out.push();
        visit(BinaryExpr->RHS);
        Out.pop();
        Out.pop();
    }
} // namespace astra::ast
