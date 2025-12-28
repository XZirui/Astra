#include <charconv>

#include "astra/ast/ASTBuilder.hpp"

namespace astra::ast {
    Program *ASTBuilder::build(parser::AstraParser::FileContext *ctx) {
        return std::any_cast<Program *>(visit(ctx));
    }

    std::any ASTBuilder::visitFile(parser::AstraParser::FileContext *ctx) {
        auto *program = astCtx.create<Program>(support::SourceRange::rangeOf(ctx));

        for (auto *objCtx: ctx->topLevelObject()) {
            program->objects.emplace_back(std::any_cast<TopLevelObject *>(visit(objCtx)));
        }

        return program;
    }

    std::any ASTBuilder::visitTopLevelObject(parser::AstraParser::TopLevelObjectContext *ctx) {
        // TODO
        auto result = astCtx.create<TopLevelObject>(support::SourceRange::rangeOf(ctx));
        result->stmt = std::any_cast<Stmt *>(visit(ctx->statement()));
        return result;
    }

    std::any ASTBuilder::visitStatement(parser::AstraParser::StatementContext *ctx) {
        if (ctx->blockStmt()) {
            return visit(ctx->blockStmt());
        }
        if (ctx->ifStmt()) {
            return visit(ctx->ifStmt());
        }
        if (ctx->exprStmt()) {
            return visit(ctx->exprStmt());
        }
        if (ctx->returnStmt()) {
            return visit(ctx->returnStmt());
        }

        assert(false && "Unknown statement type");
    }

    std::any ASTBuilder::visitBlockStmt(parser::AstraParser::BlockStmtContext *ctx) {
        auto *blockStmt = astCtx.create<BlockStmt>(support::SourceRange::rangeOf(ctx));

        for (auto *stmtCtx: ctx->statement()) {
            blockStmt->statements.emplace_back(std::any_cast<Stmt *>(visit(stmtCtx)));
        }

        return static_cast<Stmt *>(blockStmt);
    }

    std::any ASTBuilder::visitIfStmt(parser::AstraParser::IfStmtContext *ctx) {
        Stmt *elseStmt = nullptr;
        if (ctx->statement().size() > 1) {
            elseStmt = std::any_cast<Stmt *>(visit(ctx->statement(1)));
        }

        auto *ifStmt = astCtx.create<IfStmt>(support::SourceRange::rangeOf(ctx));
        ifStmt->condition = std::any_cast<Expr *>(visit(ctx->expression()));
        ifStmt->thenBranch = std::any_cast<Stmt *>(visit(ctx->statement(0)));
        ifStmt->elseBranch = elseStmt;
        return static_cast<Stmt *>(ifStmt);
    }

    std::any ASTBuilder::visitExprStmt(parser::AstraParser::ExprStmtContext *ctx) {
        auto *exprStmt = astCtx.create<ExprStmt>(support::SourceRange::rangeOf(ctx));
        exprStmt->expr = std::any_cast<Expr *>(visit(ctx->expression()));
        return static_cast<Stmt *>(exprStmt);
    }

    std::any ASTBuilder::visitReturnStmt(parser::AstraParser::ReturnStmtContext *ctx) {
        Expr *expr = nullptr;
        if (ctx->expression()) {
            expr = std::any_cast<Expr *>(visit(ctx->expression()));
        }

        auto *returnStmt = astCtx.create<ReturnStmt>(support::SourceRange::rangeOf(ctx));
        returnStmt->expr = expr;
        return static_cast<Stmt *>(returnStmt);
    }

    std::any ASTBuilder::visitExpression(parser::AstraParser::ExpressionContext *ctx) {
        return visit(ctx->disjunction());
    }

    std::any ASTBuilder::visitDisjunction(parser::AstraParser::DisjunctionContext *ctx) {
        return buildBinaryExpr(
            support::SourceRange::rangeOf(ctx),
            ctx->conjunction(),
            std::vector{ctx->DISJ()},
            [](antlr4::tree::TerminalNode *) { return Op::Disj; }
        );
    }

    std::any ASTBuilder::visitConjunction(parser::AstraParser::ConjunctionContext *ctx) {
        return buildBinaryExpr(
            support::SourceRange::rangeOf(ctx),
            ctx->equality(),
            std::vector{ctx->CONJ()},
            [](antlr4::tree::TerminalNode *) { return Op::Conj; }
        );
    }

    std::any ASTBuilder::visitEquality(parser::AstraParser::EqualityContext *ctx) {
        return buildBinaryExpr(
            support::SourceRange::rangeOf(ctx),
            ctx->comparison(),
            std::vector{ctx->equalityOperator()},
            [](parser::AstraParser::EqualityOperatorContext *ctx) {
                if (ctx->EQ()) {
                    return Op::Eq;
                }
                return Op::Neq;
            }
        );
    }

    std::any ASTBuilder::visitComparison(parser::AstraParser::ComparisonContext *ctx) {
        return buildBinaryExpr(
            support::SourceRange::rangeOf(ctx),
            ctx->addition(),
            std::vector{ctx->comparisonOperator()},
            [](parser::AstraParser::ComparisonOperatorContext *ctx) {
                if (ctx->LT()) {
                    return Op::Lt;
                }
                if (ctx->LE()) {
                    return Op::Le;
                }
                if (ctx->GT()) {
                    return Op::Gt;
                }
                return Op::Ge;
            }
        );
    }

    std::any ASTBuilder::visitAddition(parser::AstraParser::AdditionContext *ctx) {
        return buildBinaryExpr(
            support::SourceRange::rangeOf(ctx),
            ctx->multiplication(),
            ctx->additionOperator(),
            [](parser::AstraParser::AdditionOperatorContext *ctx) {
                if (ctx->ADD()) {
                    return Op::Add;
                }
                return Op::Sub;
            }
        );
    }

    std::any ASTBuilder::visitMultiplication(parser::AstraParser::MultiplicationContext *ctx) {
        return buildBinaryExpr(
            support::SourceRange::rangeOf(ctx),
            ctx->unaryExpr(),
            ctx->multiplicationOperator(),
            [](parser::AstraParser::MultiplicationOperatorContext *ctx) {
                if (ctx->MULT()) {
                    return Op::Mult;
                }
                if (ctx->DIV()) {
                    return Op::Div;
                }
                return Op::Mod;
            }
        );
    }

    std::any ASTBuilder::visitUnaryExpr(parser::AstraParser::UnaryExprContext *ctx) {
        if (!ctx->unaryOperator().size()) {
            return visit(ctx->primaryExpr());
        }

        auto *result = std::any_cast<Expr *>(visit(ctx->primaryExpr()));
        const auto getter = [](parser::AstraParser::UnaryOperatorContext *ctx) {
            if (ctx->ADD()) {
                return Op::Add;
            }
            return Op::Sub;
        };
        const auto &ops = ctx->unaryOperator();
        for (auto it = ops.rbegin(); it != ops.rend(); ++it) {
            auto *temp = astCtx.create<UnaryExpr>(support::SourceRange::rangeOf(ctx));
            temp->op = getter(*it);
            temp->operand = result;
            result = temp;
        }
        return static_cast<Expr *>(result);
    }

    std::any ASTBuilder::visitParenExpr(parser::AstraParser::ParenExprContext *ctx) {
        return visit(ctx->expression());
    }

    std::any ASTBuilder::visitVariable(parser::AstraParser::VariableContext *ctx) {
        auto *result = astCtx.create<VarExpr>(support::SourceRange::rangeOf(ctx));
        result->name = astCtx.getIdentifier(ctx->getText());
        return static_cast<Expr *>(result);
    }

    ASTBuilder::IntBase ASTBuilder::detectBase(std::string_view text) {
        if (text.size() > 2 && text[0] == '0') {
            switch (text[1]) {
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


    std::any ASTBuilder::visitLiteral(parser::AstraParser::LiteralContext *ctx) {
        auto text = ctx->getText();
        auto base = detectBase(text);

        auto *begin = text.data();
        auto *end = begin + text.size();
        if (base != IntBase::Dec) {
            begin += 2; // skip prefix
        }

        uint64_t value;
        auto [ptr, ec] = std::from_chars(begin, end, value, static_cast<int>(base));

        if (ec == std::errc::result_out_of_range) {
            // TODO: handle error: overflow
        } else if (ec != std::errc{}) {
            // TODO: handle error: invalid format
        }

        auto *result = astCtx.create<LiteralExpr>(support::SourceRange::rangeOf(ctx));
        result->value = value;
        return static_cast<Expr *>(result);
    }

    template<typename SubCtx, typename CtxOp, typename Getter>
    std::any ASTBuilder::buildBinaryExpr(support::SourceRange range, const std::vector<SubCtx> &subs,
                                         const std::vector<CtxOp> &ops,
                                         Getter getter) {
        if (subs.size() == 1) {
            return visit(subs[0]);
        }

        auto *result = std::any_cast<Expr *>(visit(subs[0]));
        for (size_t i = 1; i < subs.size(); ++i) {
            auto *temp = astCtx.create<BinaryExpr>(range);
            temp->op = getter(ops[i - 1]);
            temp->lhs = result;
            temp->rhs = std::any_cast<Expr *>(visit(subs[i]));
            result = temp;
        }

        return result;
    }
}
