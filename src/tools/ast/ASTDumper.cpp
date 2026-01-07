#include "astra/tools/ast/ASTDumper.hpp"

#include "astra/sema/Type.hpp"
#include "astra/support/SourceRangeFmt.hpp"

template <> struct fmt::formatter<astra::ast::Op> {
    constexpr auto parse(format_parse_context &Ctx) { return Ctx.begin(); }

    template <typename FormatContext>
    auto format(const astra::ast::Op &Operator, FormatContext &Ctx) {
        switch (Operator) {
            using enum astra::ast::Op;
        case Add:
            return fmt::format_to(Ctx.out(), "+");
        case Sub:
            return fmt::format_to(Ctx.out(), "-");
        case Mult:
            return fmt::format_to(Ctx.out(), "*");
        case Div:
            return fmt::format_to(Ctx.out(), "/");
        case Mod:
            return fmt::format_to(Ctx.out(), "%");
        case Eq:
            return fmt::format_to(Ctx.out(), "==");
        case Neq:
            return fmt::format_to(Ctx.out(), "!=");
        case Lt:
            return fmt::format_to(Ctx.out(), "<");
        case Gt:
            return fmt::format_to(Ctx.out(), ">");
        case Le:
            return fmt::format_to(Ctx.out(), "<=");
        case Ge:
            return fmt::format_to(Ctx.out(), ">=");
        case Disj:
            return fmt::format_to(Ctx.out(), "||");
        case Conj:
            return fmt::format_to(Ctx.out(), "&&");
        default:
            return fmt::format_to(Ctx.out(), "UnknownOp");
        }
    }
};

template <> struct fmt::formatter<astra::ast::FloatType> {
    constexpr auto parse(format_parse_context &Ctx) { return Ctx.begin(); }

    template <typename FormatContext>
    auto format(const astra::ast::FloatType &Kind, FormatContext &Ctx) {
        switch (Kind) {
            using enum astra::ast::FloatType;
        case Float:
            return fmt::format_to(Ctx.out(), "float");
        case Double:
            return fmt::format_to(Ctx.out(), "double");
        default:
            return fmt::format_to(Ctx.out(), "UnknownFloatType");
        }
    }
};

namespace astra::tools::ast {
    ASTDumper::ASTDumper(support::Printer &Out) : Out(Out) {}

    void ASTDumper::dump(const Program *Program) {
        Out.print("AST Dump:\n");

        visit(Program);
    }

    void ASTDumper::visitProgram(const Program *Program) {
        Out.print("Program {}:\n", Program->Range);
        Out.push();
        for (auto *const Object : Program->Objects) {
            visit(Object);
        }
        Out.pop();
    }

    void ASTDumper::visitTopLevelObject(const TopLevelObject *TopLevelObject) {
        visit(TopLevelObject->Declaration);
    }

    void ASTDumper::visitFunctionDecl(const FunctionDecl *FunctionDecl) {
        Out.print("FunctionDecl {}:\n", FunctionDecl->Range);
        Out.push();
        Out.print("Name={}\n", FunctionDecl->Name->getName());
        Out.print("Params:\n");
        Out.push();
        for (auto *const Param : FunctionDecl->Params) {
            visit(Param);
        }
        Out.pop();
        Out.print("ReturnType: {}\n",
                  FunctionDecl->ReturnType->Name->getName());
        Out.print("Body:\n");
        Out.push();
        visit(FunctionDecl->Body);
        Out.pop();
        Out.pop();
    }

    void ASTDumper::visitParamDecl(const ParamDecl *ParamDecl) {
        Out.print("ParamDecl {}:\n", ParamDecl->Range);
        Out.push();
        Out.print("Name={}\n", ParamDecl->Name->getName());
        Out.print("Type={}\n", ParamDecl->ParamType->Name->getName());
        Out.pop();
    }

    void ASTDumper::visitBlockStmt(const BlockStmt *BlockStmt) {
        Out.print("BlockStmt {}:\n", BlockStmt->Range);
        Out.push();
        for (auto *const Stmt : BlockStmt->Statements) {
            visit(Stmt);
        }
        Out.pop();
    }

    void ASTDumper::visitIfStmt(const IfStmt *IfStmt) {
        Out.print("IfStmt {}:\n", IfStmt->Range);
        Out.push();
        Out.print("Condition:\n");
        Out.push();
        visit(IfStmt->Condition);
        Out.pop();
        Out.print("Then:\n");
        Out.push();
        visit(IfStmt->ThenBranch);
        Out.pop();
        if (IfStmt->ElseBranch) {
            Out.print("Else:\n");
            Out.push();
            visit(IfStmt->ElseBranch);
            Out.pop();
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

    void ASTDumper::visitBreakStmt(const BreakStmt *BreakStmt) {
        Out.print("BreakStmt {}\n", BreakStmt->Range);
    }

    void ASTDumper::visitContinueStmt(const ContinueStmt *ContinueStmt) {
        Out.print("ContinueStmt {}\n", ContinueStmt->Range);
    }

    void ASTDumper::visitIntLiteral(const IntLiteral *IntLiteral) {
        Out.print("IntLiteral {}:\n", IntLiteral->Range);
        Out.push();
        Out.print("Value={}\n", IntLiteral->Value);
        Out.pop();
    }

    void ASTDumper::visitBoolLiteral(const BoolLiteral *BoolLiteral) {
        Out.print("BoolLiteral {}:\n", BoolLiteral->Range);
        Out.push();
        Out.print("Value={}\n", BoolLiteral->Value ? "true" : "false");
        Out.pop();
    }

    void
    ASTDumper::visitFloatingLiteral(const FloatingLiteral *FloatingLiteral) {
        Out.print("FloatingLiteral {}:\n", FloatingLiteral->Range);
        Out.push();
        Out.print("Kind={}\n", FloatingLiteral->FloatKind);
        Out.print("Value={}\n", FloatingLiteral->Value);
        Out.pop();
    }

    void ASTDumper::visitNullLiteral(const NullLiteral *NullLiteral) {
        Out.print("NullLiteral {}\n", NullLiteral->Range);
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
} // namespace astra::tools::ast
