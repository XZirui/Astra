#include "astra/frontend/ASTBuilder.hpp"
#include "astra/sema/Type.hpp"
#include <llvm/Support/Error.h>

using namespace astra::ast;
using astra::support::SourceRange;

namespace astra::frontend {
    template <typename SubCtx, typename CtxOp, typename Getter>
    std::any ASTBuilder::buildBinaryExpr(const std::vector<SubCtx> &Subs,
                                         const std::vector<CtxOp>  &Ops,
                                         Getter GetterFunction) {
        if (Subs.size() == 1) {
            return visit(Subs[0]);
        }

        auto *Result = std::any_cast<Expr *>(visit(Subs[0]));
        for (size_t I = 1; I < Subs.size(); ++I) {
            auto *Temp = CompilerCtx.create<BinaryExpr>(
                Result->Range.extend(SourceRange::rangeOf(Subs[I])));
            Temp->Operator = GetterFunction(Ops[I - 1]);
            Temp->LHS = Result;
            Temp->RHS = std::any_cast<Expr *>(visit(Subs[I]));
            Result = Temp;
        }

        return Result;
    }

    ASTBuilder::ASTBuilder(CompilerContext &Ctx)
        : CompilerCtx(Ctx), DiagEngine(Ctx.getDiagEngine()) {}

    Program *ASTBuilder::build(parser::AstraParser::FileContext *Ctx) {
        return std::any_cast<Program *>(visit(Ctx));
    }

    ASTBuilder::IntBase ASTBuilder::detectBase(std::string_view Text) {
        if (Text.size() > 2 && Text[0] == '0') {
            switch (Text[1]) {
            case 'b':
            case 'B':
                return IntBase::Bin;
            case 'o':
            case 'O':
                return IntBase::Oct;
            case 'x':
            case 'X':
                return IntBase::Hex;
            default:
                assert(false && "Invalid integer literal prefix");
            }
        }
        return IntBase::Dec;
    }

    std::any ASTBuilder::visitFile(parser::AstraParser::FileContext *Ctx) {
        auto *Result = CompilerCtx.create<Program>(SourceRange::rangeOf(Ctx));

        for (auto *ObjCtx : Ctx->topLevelObject()) {
            Result->Objects.emplace_back(
                std::any_cast<TopLevelObject *>(visit(ObjCtx)));
        }

        return Result;
    }

    std::any ASTBuilder::visitTopLevelObject(
        parser::AstraParser::TopLevelObjectContext *Ctx) {
        // TODO
        auto *Result =
            CompilerCtx.create<TopLevelObject>(SourceRange::rangeOf(Ctx));
        Result->Declaration = std::any_cast<Decl *>(visit(Ctx->declaration()));
        return Result;
    }

    std::any
    ASTBuilder::visitDeclaration(parser::AstraParser::DeclarationContext *Ctx) {
        if (Ctx->functionDecl()) {
            return visit(Ctx->functionDecl());
        }

        assert(false && "Unknown declaration type");
    }

    std::any ASTBuilder::visitFunctionDecl(
        parser::AstraParser::FunctionDeclContext *Ctx) {
        auto *Result =
            CompilerCtx.create<FunctionDecl>(SourceRange::rangeOf(Ctx));
        Result->Name = CompilerCtx.getIdentifier(Ctx->IDENTIFIER()->getText());
        if (Ctx->paramList()) {
            Result->Params = std::any_cast<std::vector<ParamDecl *>>(
                visit(Ctx->paramList()));
        }
        Result->ReturnType = std::any_cast<TypeRef *>(visit(Ctx->type()));
        Result->Body = std::any_cast<Stmt *>(visit(Ctx->blockStmt()));
        return static_cast<Decl *>(Result);
    }

    std::any
    ASTBuilder::visitParamList(parser::AstraParser::ParamListContext *Ctx) {
        auto Result = std::vector<ParamDecl *>{};
        for (auto *ParamCtx : Ctx->parameter()) {
            Result.emplace_back(std::any_cast<ParamDecl *>(visit(ParamCtx)));
        }
        return Result;
    }

    std::any
    ASTBuilder::visitParameter(parser::AstraParser::ParameterContext *Ctx) {
        auto *Result = CompilerCtx.create<ParamDecl>(SourceRange::rangeOf(Ctx));
        Result->ParamKind =
            ParamType::Default; // TODO: support other param types
        Result->Name = CompilerCtx.getIdentifier(Ctx->IDENTIFIER()->getText());
        Result->ParamType = std::any_cast<TypeRef *>(visit(Ctx->type()));

        if (Ctx->expression()) {
            Result->DefaultValue =
                std::any_cast<Expr *>(visit(Ctx->expression()));
        }
        return static_cast<ParamDecl *>(Result);
    }

    std::any ASTBuilder::visitType(parser::AstraParser::TypeContext *Ctx) {
        if (Ctx->functionType()) {
            return visit(Ctx->functionType());
        }

        TypeRef *Result;
        if (Ctx->parenType()) {
            Result = std::any_cast<TypeRef *>(visit(Ctx->parenType()));
        } else if (Ctx->typeRef()) {
            Result = std::any_cast<TypeRef *>(visit(Ctx->typeRef()));
        } else {
            Result = std::any_cast<TypeRef *>(visit(Ctx->builtinType()));
        }

        for (size_t I = 0; I < Ctx->expression().size(); ++I) {
            auto *Temp =
                CompilerCtx.create<ArrayTypeRef>(SourceRange::rangeOf(Ctx));
            Temp->ElementType = Result;
            Temp->Size = std::any_cast<Expr *>(visit(Ctx->expression(I)));
            Result = Temp;
        }

        return Result;
    }

    std::any
    ASTBuilder::visitParenType(parser::AstraParser::ParenTypeContext *Ctx) {
        return visit(Ctx->type());
    }

    std::any
    ASTBuilder::visitTypeRef(parser::AstraParser::TypeRefContext *Ctx) {
        auto *Result =
            CompilerCtx.create<ClassTypeRef>(SourceRange::rangeOf(Ctx));
        Result->ClassName =
            CompilerCtx.getIdentifier(Ctx->IDENTIFIER()->getText());
        return static_cast<TypeRef *>(Result);
    }

    std::any ASTBuilder::visitFunctionType(
        parser::AstraParser::FunctionTypeContext *Ctx) {
        auto *Result =
            CompilerCtx.create<FunctionTypeRef>(SourceRange::rangeOf(Ctx));
        std::vector<TypeRef *> ParamTypes{};
        if (auto *ParamListCtx = Ctx->paramTypeList()) {
            ParamTypes =
                std::any_cast<std::vector<TypeRef *>>(visit(ParamListCtx));
        }
        Result->ParamTypes = std::move(ParamTypes);
        Result->ReturnType = std::any_cast<TypeRef *>(visit(Ctx->type()));
        return static_cast<TypeRef *>(Result);
    }

    std::any ASTBuilder::visitParamTypeList(
        parser::AstraParser::ParamTypeListContext *Ctx) {
        auto Result = std::vector<TypeRef *>{};
        for (auto *TypeCtx : Ctx->type()) {
            Result.emplace_back(std::any_cast<TypeRef *>(visit(TypeCtx)));
        }
        return Result;
    }

    std::any
    ASTBuilder::visitVoidType(parser::AstraParser::VoidTypeContext *Ctx) {
        return static_cast<TypeRef *>(
            CompilerCtx.create<VoidTypeRef>(SourceRange::rangeOf(Ctx)));
    }
    std::any
    ASTBuilder::visitBoolType(parser::AstraParser::BoolTypeContext *Ctx) {
        return static_cast<TypeRef *>(
            CompilerCtx.create<BoolTypeRef>(SourceRange::rangeOf(Ctx)));
    }
    std::any
    ASTBuilder::visitIntType(parser::AstraParser::IntTypeContext *Ctx) {
        return static_cast<TypeRef *>(
            CompilerCtx.create<IntTypeRef>(SourceRange::rangeOf(Ctx)));
    }
    std::any
    ASTBuilder::visitLongType(parser::AstraParser::LongTypeContext *Ctx) {
        return static_cast<TypeRef *>(
            CompilerCtx.create<LongTypeRef>(SourceRange::rangeOf(Ctx)));
    }
    std::any
    ASTBuilder::visitFloatType(parser::AstraParser::FloatTypeContext *Ctx) {
        return static_cast<TypeRef *>(
            CompilerCtx.create<FloatTypeRef>(SourceRange::rangeOf(Ctx)));
    }
    std::any
    ASTBuilder::visitDoubleType(parser::AstraParser::DoubleTypeContext *Ctx) {
        return static_cast<TypeRef *>(
            CompilerCtx.create<DoubleTypeRef>(SourceRange::rangeOf(Ctx)));
    }

    std::any
    ASTBuilder::visitStatement(parser::AstraParser::StatementContext *Ctx) {
        if (Ctx->blockStmt()) {
            return visit(Ctx->blockStmt());
        }
        if (Ctx->ifStmt()) {
            return visit(Ctx->ifStmt());
        }
        if (Ctx->exprStmt()) {
            return visit(Ctx->exprStmt());
        }
        if (Ctx->returnStmt()) {
            return visit(Ctx->returnStmt());
        }
        if (Ctx->breakStmt()) {
            return visit(Ctx->breakStmt());
        }
        if (Ctx->continueStmt()) {
            return visit(Ctx->continueStmt());
        }

        assert(false && "Unknown statement type");
    }

    std::any
    ASTBuilder::visitBlockStmt(parser::AstraParser::BlockStmtContext *Ctx) {
        auto *Result = CompilerCtx.create<BlockStmt>(SourceRange::rangeOf(Ctx));

        for (auto *StmtCtx : Ctx->statement()) {
            Result->Statements.emplace_back(
                std::any_cast<Stmt *>(visit(StmtCtx)));
        }

        return static_cast<Stmt *>(Result);
    }

    std::any ASTBuilder::visitIfStmt(parser::AstraParser::IfStmtContext *Ctx) {
        Stmt *ElseStmt = nullptr;
        if (Ctx->statement().size() > 1) {
            ElseStmt = std::any_cast<Stmt *>(visit(Ctx->statement(1)));
        }

        auto *Result = CompilerCtx.create<IfStmt>(SourceRange::rangeOf(Ctx));
        Result->Condition = std::any_cast<Expr *>(visit(Ctx->expression()));
        Result->ThenBranch = std::any_cast<Stmt *>(visit(Ctx->statement(0)));
        Result->ElseBranch = ElseStmt;
        return static_cast<Stmt *>(Result);
    }

    std::any
    ASTBuilder::visitExprStmt(parser::AstraParser::ExprStmtContext *Ctx) {
        auto *Result = CompilerCtx.create<ExprStmt>(SourceRange::rangeOf(Ctx));
        Result->Expression = std::any_cast<Expr *>(visit(Ctx->expression()));
        return static_cast<Stmt *>(Result);
    }

    std::any
    ASTBuilder::visitReturnStmt(parser::AstraParser::ReturnStmtContext *Ctx) {
        Expr *ReturnValue = nullptr;
        if (Ctx->expression()) {
            ReturnValue = std::any_cast<Expr *>(visit(Ctx->expression()));
        }

        auto *Result =
            CompilerCtx.create<ReturnStmt>(SourceRange::rangeOf(Ctx));
        Result->Expression = ReturnValue;
        return static_cast<Stmt *>(Result);
    }

    std::any
    ASTBuilder::visitBreakStmt(parser::AstraParser::BreakStmtContext *Ctx) {
        return static_cast<Stmt *>(
            CompilerCtx.create<BreakStmt>(SourceRange::rangeOf(Ctx)));
    }

    std::any ASTBuilder::visitContinueStmt(
        parser::AstraParser::ContinueStmtContext *Ctx) {
        return static_cast<Stmt *>(
            CompilerCtx.create<ContinueStmt>(SourceRange::rangeOf(Ctx)));
    }

    std::any
    ASTBuilder::visitExpression(parser::AstraParser::ExpressionContext *Ctx) {
        return visit(Ctx->disjunction());
    }

    std::any
    ASTBuilder::visitDisjunction(parser::AstraParser::DisjunctionContext *Ctx) {
        return buildBinaryExpr(
            Ctx->conjunction(), std::vector{Ctx->DISJ()},
            [](antlr4::tree::TerminalNode *) { return Op::Disj; });
    }

    std::any
    ASTBuilder::visitConjunction(parser::AstraParser::ConjunctionContext *Ctx) {
        return buildBinaryExpr(
            Ctx->equality(), std::vector{Ctx->CONJ()},
            [](antlr4::tree::TerminalNode *) { return Op::Conj; });
    }

    std::any
    ASTBuilder::visitEquality(parser::AstraParser::EqualityContext *Ctx) {
        return buildBinaryExpr(
            Ctx->comparison(), std::vector{Ctx->equalityOperator()},
            [](parser::AstraParser::EqualityOperatorContext *Ctx) {
                if (Ctx->EQ()) {
                    return Op::Eq;
                }
                return Op::Neq;
            });
    }

    std::any
    ASTBuilder::visitComparison(parser::AstraParser::ComparisonContext *Ctx) {
        return buildBinaryExpr(
            Ctx->bitwiseOr(), std::vector{Ctx->comparisonOperator()},
            [](parser::AstraParser::ComparisonOperatorContext *Ctx) {
                if (Ctx->LT()) {
                    return Op::Lt;
                }
                if (Ctx->LE()) {
                    return Op::Le;
                }
                if (Ctx->GT()) {
                    return Op::Gt;
                }
                return Op::Ge;
            });
    }

    std::any
    ASTBuilder::visitBitwiseOr(parser::AstraParser::BitwiseOrContext *Ctx) {
        return buildBinaryExpr(
            Ctx->bitwiseXor(), Ctx->BIT_OR(),
            [](antlr4::tree::TerminalNode *) { return Op::BitOr; });
    }

    std::any
    ASTBuilder::visitBitwiseXor(parser::AstraParser::BitwiseXorContext *Ctx) {
        return buildBinaryExpr(
            Ctx->bitwiseAnd(), Ctx->BIT_XOR(),
            [](antlr4::tree::TerminalNode *) { return Op::BitXor; });
    }

    std::any
    ASTBuilder::visitBitwiseAnd(parser::AstraParser::BitwiseAndContext *Ctx) {
        return buildBinaryExpr(
            Ctx->bitwiseShift(), Ctx->BIT_AND(),
            [](antlr4::tree::TerminalNode *) { return Op::BitAnd; });
    }

    std::any ASTBuilder::visitBitwiseShift(
        parser::AstraParser::BitwiseShiftContext *Ctx) {
        return buildBinaryExpr(
            Ctx->addition(), Ctx->bitwiseShiftOperator(),
            [](parser::AstraParser::BitwiseShiftOperatorContext *Ctx) {
                if (Ctx->LSHIFT()) {
                    return Op::LShift;
                }
                return Op::RShift;
            });
    }

    std::any
    ASTBuilder::visitAddition(parser::AstraParser::AdditionContext *Ctx) {
        return buildBinaryExpr(
            Ctx->multiplication(), Ctx->additionOperator(),
            [](parser::AstraParser::AdditionOperatorContext *Ctx) {
                if (Ctx->ADD()) {
                    return Op::Add;
                }
                return Op::Sub;
            });
    }

    std::any ASTBuilder::visitMultiplication(
        parser::AstraParser::MultiplicationContext *Ctx) {
        return buildBinaryExpr(
            Ctx->unaryExpr(), Ctx->multiplicationOperator(),
            [](parser::AstraParser::MultiplicationOperatorContext *Ctx) {
                if (Ctx->MULT()) {
                    return Op::Mult;
                }
                if (Ctx->DIV()) {
                    return Op::Div;
                }
                return Op::Mod;
            });
    }

    std::any
    ASTBuilder::visitUnaryExpr(parser::AstraParser::UnaryExprContext *Ctx) {
        if (!Ctx->unaryOperator().size()) {
            return visit(Ctx->primaryExpr());
        }

        auto      *Result = std::any_cast<Expr *>(visit(Ctx->primaryExpr()));
        const auto Getter = [](parser::AstraParser::UnaryOperatorContext *Ctx) {
            if (Ctx->ADD()) {
                return Op::Add;
            }
            if (Ctx->SUB()) {
                return Op::Sub;
            }
            if (Ctx->NOT()) {
                return Op::Not;
            }
            return Op::BitNot;
        };
        const auto &Ops = Ctx->unaryOperator();
        for (auto It = Ops.rbegin(); It != Ops.rend(); ++It) {
            auto *Temp =
                CompilerCtx.create<UnaryExpr>(SourceRange::rangeOf(Ctx));
            Temp->Operator = Getter(*It);
            Temp->Operand = Result;
            Result = Temp;
        }
        return static_cast<Expr *>(Result);
    }

    std::any
    ASTBuilder::visitParenExpr(parser::AstraParser::ParenExprContext *Ctx) {
        return visit(Ctx->expression());
    }

    std::any
    ASTBuilder::visitVariable(parser::AstraParser::VariableContext *Ctx) {
        auto *Result = CompilerCtx.create<VarExpr>(SourceRange::rangeOf(Ctx));
        Result->Name = CompilerCtx.getIdentifier(Ctx->getText());
        return static_cast<Expr *>(Result);
    }

    std::any
    ASTBuilder::visitIntLiteral(parser::AstraParser::IntLiteralContext *Ctx) {
        auto Text = Ctx->getText();
        auto Base = detectBase(Text);

        if (Base != IntBase::Dec) {
            Text = Text.substr(2); // skip prefix
        }

        llvm::APSInt Value(llvm::APInt(64, Text, static_cast<unsigned>(Base)),
                           false);

        if (Value.getActiveBits() > 64) {
            // TODO: handle warning: integer literal is larger than 64-bit
        }

        auto *Result =
            CompilerCtx.create<IntLiteral>(SourceRange::rangeOf(Ctx));
        Result->Value = Value;
        return static_cast<Expr *>(Result);
    }

    std::any
    ASTBuilder::visitBoolLiteral(parser::AstraParser::BoolLiteralContext *Ctx) {
        auto *Result =
            CompilerCtx.create<BoolLiteral>(SourceRange::rangeOf(Ctx));
        Result->Value = (Ctx->getText() == "true");
        return static_cast<Expr *>(Result);
    }

    std::any
    ASTBuilder::visitNullLiteral(parser::AstraParser::NullLiteralContext *Ctx) {
        auto *Result =
            CompilerCtx.create<NullLiteral>(SourceRange::rangeOf(Ctx));
        return static_cast<Expr *>(Result);
    }

    std::any ASTBuilder::visitFloatLiteral(
        parser::AstraParser::FloatLiteralContext *Ctx) {
        auto *Result =
            CompilerCtx.create<FloatLiteral>(SourceRange::rangeOf(Ctx));

        Result->FloatKind = FloatType::Float;

        auto Text = Ctx->getText();
        Text.pop_back(); // remove suffix 'f' or 'F'
        Result->Value = llvm::APFloat(llvm::APFloat::IEEEsingle());
        auto Status =
            Result->Value
                .convertFromString(Text, llvm::APFloat::rmNearestTiesToEven)
                .get();

        // There are no possible errors in invalid format
        // since the lexer guarantees the format is correct.
        if (Status & llvm::APFloat::opOverflow) {
            DiagEngine.warning(
                Result->Range,
                "float literal is too large and will be converted to infinity");
        } else if (Status & llvm::APFloat::opUnderflow) {
            DiagEngine.warning(
                Result->Range,
                "float literal is too small and will be converted to zero");
        }
        // Inexact conversion is ignored since it is common for decimal literals
        // to be inexact in binary representation.

        return static_cast<Expr *>(Result);
    }

    std::any ASTBuilder::visitDoubleLiteral(
        parser::AstraParser::DoubleLiteralContext *Ctx) {
        auto *Result =
            CompilerCtx.create<FloatLiteral>(SourceRange::rangeOf(Ctx));
        Result->FloatKind = FloatType::Double;
        Result->Value = llvm::APFloat(llvm::APFloat::IEEEdouble());
        auto Status = Result->Value
                          .convertFromString(Ctx->getText(),
                                             llvm::APFloat::rmNearestTiesToEven)
                          .get();

        // There are no possible errors in invalid format
        // since the lexer guarantees the format is correct.
        if (Status & llvm::APFloat::opOverflow) {
            DiagEngine.warning(
                Result->Range,
                "float literal is too large and will be converted to infinity");
        } else if (Status & llvm::APFloat::opUnderflow) {
            DiagEngine.warning(
                Result->Range,
                "float literal is too small and will be converted to zero");
        }
        // Inexact conversion is ignored since it is common for decimal literals
        // to be inexact in binary representation.

        return static_cast<Expr *>(Result);
    }
} // namespace astra::frontend
