#include <charconv>

#include "astra/ast/AstBuilder.hpp"

namespace astra::ast {
    Program *ASTBuilder::build(parser::AstraParser::FileContext *ctx) {
        return std::any_cast<Program *>(visit(ctx));
    }

    std::any ASTBuilder::visitFile(parser::AstraParser::FileContext *ctx) {
        auto *program = astCtx.create<Program>();

        for (auto *objCtx: ctx->topLevelObject()) {
            program->objects.emplace_back(std::any_cast<TopLevelObject *>(visit(objCtx)));
        }

        return program;
    }

    std::any ASTBuilder::visitTopLevelObject(parser::AstraParser::TopLevelObjectContext *ctx) {
        // TODO
        auto *object = astCtx.create<TopLevelObject>();
        object->stmt = std::any_cast<Stmt *>(visit(ctx->statement()));
        return object;
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
        auto *blockStmt = astCtx.create<BlockStmt>();

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

        auto *ifStmt = astCtx.create<IfStmt>(condition, thenStmt, elseStmt);
        // TODO merge source range
        return ifStmt;
    }

    std::any ASTBuilder::visitExprStmt(parser::AstraParser::ExprStmtContext *ctx) {
        auto *expr = std::any_cast<Expr *>(visit(ctx->expression()));
        auto *exprStmt = astCtx.create<ExprStmt>(expr);

        // TODO merge source range
        return exprStmt;
    }

    std::any ASTBuilder::visitReturnStmt(parser::AstraParser::ReturnStmtContext *ctx) {
        Expr *expr = nullptr;
        if (ctx->expression()) {
            expr = std::any_cast<Expr *>(visit(ctx->expression()));
        }

        auto *returnStmt = astCtx.create<ReturnStmt>(expr);
        // TODO merge source range
        return returnStmt;
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
        auto text = ctx->INTEGER_LITERAL()->getText();
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

        return astCtx.create<LiteralExpr>(value);
    }
}
