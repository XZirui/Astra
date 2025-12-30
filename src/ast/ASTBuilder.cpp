#include <charconv>

#include "astra/ast/ASTBuilder.hpp"

namespace astra::ast {
    template<typename SubCtx, typename CtxOp, typename Getter>
    std::any ASTBuilder::buildBinaryExpr(support::SourceRange Range, const std::vector<SubCtx> &Subs,
                                         const std::vector<CtxOp> &Ops,
                                         Getter GetterFunction) {
        if (Subs.size() == 1) {
            return visit(Subs[0]);
        }

        auto *Result = std::any_cast<Expr *>(visit(Subs[0]));
        for (size_t I = 1; I < Subs.size(); ++I) {
            auto *Temp = AstCtx.create<BinaryExpr>(Range);
            Temp->Operator = GetterFunction(Ops[I - 1]);
            Temp->LHS = Result;
            Temp->RHS = std::any_cast<Expr *>(visit(Subs[I]));
            Result = Temp;
        }

        return Result;
    }

    ASTBuilder::ASTBuilder(ASTContext &AstCtx) : AstCtx(AstCtx) {
    }

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
        auto *Result = AstCtx.create<Program>(support::SourceRange::rangeOf(Ctx));

        for (auto *ObjCtx: Ctx->topLevelObject()) {
            Result->Objects.emplace_back(std::any_cast<TopLevelObject *>(visit(ObjCtx)));
        }

        return Result;
    }

    std::any ASTBuilder::visitTopLevelObject(parser::AstraParser::TopLevelObjectContext *Ctx) {
        // TODO
        auto *Result = AstCtx.create<TopLevelObject>(support::SourceRange::rangeOf(Ctx));
        Result->Statement = std::any_cast<Stmt *>(visit(Ctx->statement()));
        return Result;
    }

    std::any ASTBuilder::visitStatement(parser::AstraParser::StatementContext *Ctx) {
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

        assert(false && "Unknown statement type");
    }

    std::any ASTBuilder::visitBlockStmt(parser::AstraParser::BlockStmtContext *Ctx) {
        auto *Result = AstCtx.create<BlockStmt>(support::SourceRange::rangeOf(Ctx));

        for (auto *StmtCtx: Ctx->statement()) {
            Result->Statements.emplace_back(std::any_cast<Stmt *>(visit(StmtCtx)));
        }

        return static_cast<Stmt *>(Result);
    }

    std::any ASTBuilder::visitIfStmt(parser::AstraParser::IfStmtContext *Ctx) {
        Stmt *ElseStmt = nullptr;
        if (Ctx->statement().size() > 1) {
            ElseStmt = std::any_cast<Stmt *>(visit(Ctx->statement(1)));
        }

        auto *Result = AstCtx.create<IfStmt>(support::SourceRange::rangeOf(Ctx));
        Result->Condition = std::any_cast<Expr *>(visit(Ctx->expression()));
        Result->ThenBranch = std::any_cast<Stmt *>(visit(Ctx->statement(0)));
        Result->ElseBranch = ElseStmt;
        return static_cast<Stmt *>(Result);
    }

    std::any ASTBuilder::visitExprStmt(parser::AstraParser::ExprStmtContext *Ctx) {
        auto *Result = AstCtx.create<ExprStmt>(support::SourceRange::rangeOf(Ctx));
        Result->Expression = std::any_cast<Expr *>(visit(Ctx->expression()));
        return static_cast<Stmt *>(Result);
    }

    std::any ASTBuilder::visitReturnStmt(parser::AstraParser::ReturnStmtContext *Ctx) {
        Expr *ReturnValue = nullptr;
        if (Ctx->expression()) {
            ReturnValue = std::any_cast<Expr *>(visit(Ctx->expression()));
        }

        auto *Result = AstCtx.create<ReturnStmt>(support::SourceRange::rangeOf(Ctx));
        Result->Expression = ReturnValue;
        return static_cast<Stmt *>(Result);
    }

    std::any ASTBuilder::visitExpression(parser::AstraParser::ExpressionContext *Ctx) {
        return visit(Ctx->disjunction());
    }

    std::any ASTBuilder::visitDisjunction(parser::AstraParser::DisjunctionContext *Ctx) {
        return buildBinaryExpr(
            support::SourceRange::rangeOf(Ctx),
            Ctx->conjunction(),
            std::vector{Ctx->DISJ()},
            [](antlr4::tree::TerminalNode *) { return Op::Disj; }
        );
    }

    std::any ASTBuilder::visitConjunction(parser::AstraParser::ConjunctionContext *Ctx) {
        return buildBinaryExpr(
            support::SourceRange::rangeOf(Ctx),
            Ctx->equality(),
            std::vector{Ctx->CONJ()},
            [](antlr4::tree::TerminalNode *) { return Op::Conj; }
        );
    }

    std::any ASTBuilder::visitEquality(parser::AstraParser::EqualityContext *Ctx) {
        return buildBinaryExpr(
            support::SourceRange::rangeOf(Ctx),
            Ctx->comparison(),
            std::vector{Ctx->equalityOperator()},
            [](parser::AstraParser::EqualityOperatorContext *Ctx) {
                if (Ctx->EQ()) {
                    return Op::Eq;
                }
                return Op::Neq;
            }
        );
    }

    std::any ASTBuilder::visitComparison(parser::AstraParser::ComparisonContext *Ctx) {
        return buildBinaryExpr(
            support::SourceRange::rangeOf(Ctx),
            Ctx->addition(),
            std::vector{Ctx->comparisonOperator()},
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
            }
        );
    }

    std::any ASTBuilder::visitAddition(parser::AstraParser::AdditionContext *Ctx) {
        return buildBinaryExpr(
            support::SourceRange::rangeOf(Ctx),
            Ctx->multiplication(),
            Ctx->additionOperator(),
            [](parser::AstraParser::AdditionOperatorContext *Ctx) {
                if (Ctx->ADD()) {
                    return Op::Add;
                }
                return Op::Sub;
            }
        );
    }

    std::any ASTBuilder::visitMultiplication(parser::AstraParser::MultiplicationContext *Ctx) {
        return buildBinaryExpr(
            support::SourceRange::rangeOf(Ctx),
            Ctx->unaryExpr(),
            Ctx->multiplicationOperator(),
            [](parser::AstraParser::MultiplicationOperatorContext *Ctx) {
                if (Ctx->MULT()) {
                    return Op::Mult;
                }
                if (Ctx->DIV()) {
                    return Op::Div;
                }
                return Op::Mod;
            }
        );
    }

    std::any ASTBuilder::visitUnaryExpr(parser::AstraParser::UnaryExprContext *Ctx) {
        if (!Ctx->unaryOperator().size()) {
            return visit(Ctx->primaryExpr());
        }

        auto *Result = std::any_cast<Expr *>(visit(Ctx->primaryExpr()));
        const auto Getter = [](parser::AstraParser::UnaryOperatorContext *Ctx) {
            if (Ctx->ADD()) {
                return Op::Add;
            }
            return Op::Sub;
        };
        const auto &Ops = Ctx->unaryOperator();
        for (auto It = Ops.rbegin(); It != Ops.rend(); ++It) {
            auto *Temp = AstCtx.create<UnaryExpr>(support::SourceRange::rangeOf(Ctx));
            Temp->Operator = Getter(*It);
            Temp->Operand = Result;
            Result = Temp;
        }
        return static_cast<Expr *>(Result);
    }

    std::any ASTBuilder::visitParenExpr(parser::AstraParser::ParenExprContext *Ctx) {
        return visit(Ctx->expression());
    }

    std::any ASTBuilder::visitVariable(parser::AstraParser::VariableContext *Ctx) {
        auto *Result = AstCtx.create<VarExpr>(support::SourceRange::rangeOf(Ctx));
        Result->Name = AstCtx.getIdentifier(Ctx->getText());
        return static_cast<Expr *>(Result);
    }

    std::any ASTBuilder::visitLiteral(parser::AstraParser::LiteralContext *Ctx) {
        auto Text = Ctx->getText();
        auto Base = detectBase(Text);

        auto *Begin = Text.data();
        auto *End = Begin + Text.size();
        if (Base != IntBase::Dec) {
            Begin += 2; // skip prefix
        }

        uint64_t Value;
        auto [ptr, ec] = std::from_chars(Begin, End, Value, static_cast<int>(Base));

        if (ec == std::errc::result_out_of_range) {
            // TODO: handle error: overflow
        } else if (ec != std::errc{}) {
            // TODO: handle error: invalid format
        }

        auto *Result = AstCtx.create<LiteralExpr>(support::SourceRange::rangeOf(Ctx));
        Result->Value = Value;
        return static_cast<Expr *>(Result);
    }
} // namespace astra::ast
