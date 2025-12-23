#include <charconv>

#include "astra/ast/ASTBuilder.hpp"

namespace astra::ast {
    Program *ASTBuilder::build(parser::AstraParser::FileContext *ctx) {
        return std::any_cast<Program *>(visit(ctx));
    }

    std::any ASTBuilder::visitFile(parser::AstraParser::FileContext *ctx) {
        auto *program = astCtx.create<Program>(SourceRange::rangeOf(ctx));

        for (auto *objCtx: ctx->topLevelObject()) {
            program->objects.emplace_back(std::any_cast<TopLevelObject *>(visit(objCtx)));
        }

        return program;
    }

    std::any ASTBuilder::visitTopLevelObject(parser::AstraParser::TopLevelObjectContext *ctx) {
        // TODO
        return astCtx.create<TopLevelObject>(SourceRange::rangeOf(ctx), std::any_cast<Stmt *>(visit(ctx->statement())));
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
        auto *blockStmt = astCtx.create<BlockStmt>(SourceRange::rangeOf(ctx));

        for (auto *stmtCtx: ctx->statement()) {
            blockStmt->statements.emplace_back(std::any_cast<Stmt *>(visit(stmtCtx)));
        }

        return blockStmt;
        // TODO merge source range
    }

    std::any ASTBuilder::visitIfStmt(parser::AstraParser::IfStmtContext *ctx) {
        auto *condition = std::any_cast<Expr *>(visit(ctx->expression()));
        auto *thenStmt = std::any_cast<Stmt *>(visit(ctx->statement(0)));
        Stmt *elseStmt = nullptr;
        if (ctx->statement().size() > 1) {
            elseStmt = std::any_cast<Stmt *>(visit(ctx->statement(1)));
        }

        auto *ifStmt = astCtx.create<IfStmt>(SourceRange::rangeOf(ctx), condition, thenStmt, elseStmt);
        // TODO merge source range
        return ifStmt;
    }

    std::any ASTBuilder::visitExprStmt(parser::AstraParser::ExprStmtContext *ctx) {
        auto *expr = std::any_cast<Expr *>(visit(ctx->expression()));
        auto *exprStmt = astCtx.create<ExprStmt>(SourceRange::rangeOf(ctx), expr);

        // TODO merge source range
        return exprStmt;
    }

    std::any ASTBuilder::visitReturnStmt(parser::AstraParser::ReturnStmtContext *ctx) {
        Expr *expr = nullptr;
        if (ctx->expression()) {
            expr = std::any_cast<Expr *>(visit(ctx->expression()));
        }

        auto *returnStmt = astCtx.create<ReturnStmt>(SourceRange::rangeOf(ctx), expr);
        // TODO merge source range
        return returnStmt;
    }

    std::any ASTBuilder::visitExpression(parser::AstraParser::ExpressionContext *ctx) {
        return visit(ctx->disjunction());
    }

    std::any ASTBuilder::visitDisjunction(parser::AstraParser::DisjunctionContext *ctx) {
        return buildBinaryExpr(
            SourceRange::rangeOf(ctx),
            ctx->conjunction(),
            std::vector{ctx->DISJ()},
            [](antlr4::tree::TerminalNode *) { return BinaryExpr::Op::Disj; }
        );
    }

    std::any ASTBuilder::visitConjunction(parser::AstraParser::ConjunctionContext *ctx) {
        return buildBinaryExpr(
            SourceRange::rangeOf(ctx),
            ctx->equality(),
            std::vector{ctx->CONJ()},
            [](antlr4::tree::TerminalNode *) { return BinaryExpr::Op::Conj; }
        );
    }

    std::any ASTBuilder::visitEquality(parser::AstraParser::EqualityContext *ctx) {
        return buildBinaryExpr(
            SourceRange::rangeOf(ctx),
            ctx->comparison(),
            std::vector{ctx->equalityOperator()},
            [](parser::AstraParser::EqualityOperatorContext *ctx) {
                if (ctx->EQ()) {
                    return BinaryExpr::Op::Eq;
                }
                return BinaryExpr::Op::Neq;
            }
        );
    }

    std::any ASTBuilder::visitComparison(parser::AstraParser::ComparisonContext *ctx) {
        return buildBinaryExpr(
            SourceRange::rangeOf(ctx),
            ctx->addition(),
            std::vector{ctx->comparisonOperator()},
            [](parser::AstraParser::ComparisonOperatorContext *ctx) {
                if (ctx->LT()) {
                    return BinaryExpr::Op::Lt;
                }
                if (ctx->LE()) {
                    return BinaryExpr::Op::Le;
                }
                if (ctx->GT()) {
                    return BinaryExpr::Op::Gt;
                }
                return BinaryExpr::Op::Ge;
            }
        );
    }

    std::any ASTBuilder::visitAddition(parser::AstraParser::AdditionContext *ctx) {
        return buildBinaryExpr(
            SourceRange::rangeOf(ctx),
            ctx->multiplication(),
            ctx->additionOperator(),
            [](parser::AstraParser::AdditionOperatorContext *ctx) {
                if (ctx->ADD()) {
                    return BinaryExpr::Op::Add;
                }
                return BinaryExpr::Op::Sub;
            }
        );
    }

    std::any ASTBuilder::visitMultiplication(parser::AstraParser::MultiplicationContext *ctx) {
        return buildBinaryExpr(
            SourceRange::rangeOf(ctx),
            ctx->unaryExpr(),
            ctx->multiplicationOperator(),
            [](parser::AstraParser::MultiplicationOperatorContext *ctx) {
                if (ctx->MULT()) {
                    return BinaryExpr::Op::Mult;
                }
                if (ctx->DIV()) {
                    return BinaryExpr::Op::Div;
                }
                return BinaryExpr::Op::Mod;
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
                return UnaryExpr::Op::Add;
            }
            return UnaryExpr::Op::Sub;
        };
        const auto &ops = ctx->unaryOperator();
        for (auto it = ops.rbegin(); it != ops.rend(); ++it) {
            result = astCtx.create<UnaryExpr>(SourceRange::rangeOf(ctx), getter(*it), result);
        }
        return result;
    }

    std::any ASTBuilder::visitParenExpr(parser::AstraParser::ParenExprContext *ctx) {
        return visit(ctx->expression());
    }

    std::any ASTBuilder::visitVariable(parser::AstraParser::VariableContext *ctx) {
        return astCtx.create<VarExpr>(SourceRange::rangeOf(ctx), astCtx.getIdentifier(ctx->getText()));
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

        return astCtx.create<LiteralExpr>(SourceRange::rangeOf(ctx), value);
    }

    template<typename SubCtx, typename CtxOp, typename Getter>
    std::any ASTBuilder::buildBinaryExpr(SourceRange range, const std::vector<SubCtx> &subs, const std::vector<CtxOp> &ops,
                                         Getter getter) {
        if (subs.size() == 1) {
            return visit(subs[0]);
        }

        auto *result = std::any_cast<Expr *>(visit(subs[0]));
        for (size_t i = 1; i < subs.size(); ++i) {
            result = astCtx.create<BinaryExpr>(
                range,
                std::forward<Getter>(getter)(ops[i - 1]),
                result,
                std::any_cast<Expr *>(visit(subs[i]))
            );
        }

        return result;
    }
}
