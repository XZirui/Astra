#include "astra/tools/ast/ASTDumper.hpp"

#include "astra/sema/Type.hpp"
#include "astra/support/SourceRangeFmt.hpp"
#include <llvm/ADT/SmallString.h>

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
        Out.print("Name={}\n", FunctionDecl->Name.str());
        Out.print("Params:\n");
        Out.push();
        for (auto *const Param : FunctionDecl->Params) {
            visit(Param);
        }
        Out.pop();
        Out.print("ReturnType: \n");
        Out.push();
        visit(FunctionDecl->ReturnType);
        Out.pop();
        Out.print("Body:\n");
        Out.push();
        visit(FunctionDecl->Body);
        Out.pop();
        Out.pop();
    }

    void ASTDumper::visitParamDecl(const ParamDecl *ParamDecl) {
        Out.print("ParamDecl {}:\n", ParamDecl->Range);
        Out.push();
        Out.print("Name={}\n", ParamDecl->Name.str());
        Out.print("Type:\n");
        Out.push();
        visit(ParamDecl->ParamType);
        Out.pop();
        Out.pop();
    }

    void ASTDumper::visitVoidTypeRef(const VoidTypeRef *VoidTypeRef) {
        Out.print("Void\n");
    }

    void ASTDumper::visitBoolTypeRef(const BoolTypeRef *BoolTypeRef) {
        Out.print("Bool\n");
    }

    void ASTDumper::visitIntTypeRef(const IntTypeRef *IntTypeRef) {
        Out.print("Int\n");
    }

    void ASTDumper::visitLongTypeRef(const LongTypeRef *LongTypeRef) {
        Out.print("Long\n");
    }

    void ASTDumper::visitFloatTypeRef(const FloatTypeRef *FloatTypeRef) {
        Out.print("Float\n");
    }

    void ASTDumper::visitDoubleTypeRef(const DoubleTypeRef *DoubleTypeRef) {
        Out.print("Double\n");
    }

    void ASTDumper::visitArrayTypeRef(const ArrayTypeRef *ArrayTypeRef) {
        Out.print("Array:\n");
        Out.push();
        Out.print("ElementType:\n");
        Out.push();
        visit(ArrayTypeRef->ElementType);
        Out.pop();
        Out.print("Size:\n");
        visit(ArrayTypeRef->Size);
        Out.pop();
    }

    void
    ASTDumper::visitFunctionTypeRef(const FunctionTypeRef *FunctionTypeRef) {
        Out.print("Function:\n");
        Out.push();
        Out.print("ReturnType:\n");
        Out.push();
        visit(FunctionTypeRef->ReturnType);
        Out.pop();
        for (size_t Index = 0; Index < FunctionTypeRef->ParamTypes.size();
             ++Index) {
            Out.print("Param {}:\n", Index);
            Out.push();
            visit(FunctionTypeRef->ParamTypes[Index]);
            Out.pop();
        }
        Out.pop();
    }

    void ASTDumper::visitClassTypeRef(const ClassTypeRef *ClassTypeRef) {
        Out.print("Class: {}\n", ClassTypeRef->ClassName.str());
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
        llvm::SmallString<64> Buffer;
        IntLiteral->Value.toString(Buffer);
        Out.print("Value={}\n", Buffer.c_str());
        Out.pop();
    }

    void ASTDumper::visitBoolLiteral(const BoolLiteral *BoolLiteral) {
        Out.print("BoolLiteral {}:\n", BoolLiteral->Range);
        Out.push();
        Out.print("Value={}\n", BoolLiteral->Value ? "true" : "false");
        Out.pop();
    }

    void ASTDumper::visitFloatLiteral(const FloatLiteral *FloatLiteral) {
        Out.print("FloatLiteral {}:\n", FloatLiteral->Range);
        Out.push();
        Out.print("Kind={}\n", FloatLiteral->FloatKind);
        llvm::SmallString<64> Buffer;
        FloatLiteral->Value.toString(Buffer);
        Out.print("Value={}\n", Buffer.c_str());
        Out.pop();
    }

    void ASTDumper::visitNullLiteral(const NullLiteral *NullLiteral) {
        Out.print("NullLiteral {}\n", NullLiteral->Range);
    }

    void ASTDumper::visitVarExpr(const VarExpr *VarExpr) {
        Out.print("VarExpr {}:\n", VarExpr->Range);
        Out.push();
        Out.print("Name={}\n", VarExpr->Name.str());
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
