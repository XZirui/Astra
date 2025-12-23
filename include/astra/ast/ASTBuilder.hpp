#pragma once

#include "astra/parser/AstraParserBaseVisitor.h"
#include "ASTContext.hpp"
#include "Program.hpp"

namespace astra::ast {
    class ASTBuilder : public parser::AstraParserBaseVisitor {
        ASTContext &astCtx;

        template<typename SubCtx, typename CtxOp, typename Getter>
        std::any buildBinaryExpr(SourceRange range, const std::vector<SubCtx> &subs, const std::vector<CtxOp> &ops,
                                             Getter getter);

    public:
        explicit ASTBuilder(ASTContext &astCtx) : astCtx(astCtx) {
        }

        Program *build(parser::AstraParser::FileContext *ctx);

        enum class IntBase { Bin = 2, Oct = 8, Dec = 10, Hex = 16 };

        static IntBase detectBase(std::string_view text);

        std::any visitFile(parser::AstraParser::FileContext *ctx) override;
        std::any visitTopLevelObject(parser::AstraParser::TopLevelObjectContext *ctx) override;
        std::any visitStatement(parser::AstraParser::StatementContext *ctx) override;
        std::any visitBlockStmt(parser::AstraParser::BlockStmtContext *ctx) override;
        std::any visitIfStmt(parser::AstraParser::IfStmtContext *ctx) override;
        std::any visitExprStmt(parser::AstraParser::ExprStmtContext *ctx) override;
        std::any visitReturnStmt(parser::AstraParser::ReturnStmtContext *ctx) override;

        std::any visitExpression(parser::AstraParser::ExpressionContext *ctx) override;
        std::any visitDisjunction(parser::AstraParser::DisjunctionContext *ctx) override;
        std::any visitConjunction(parser::AstraParser::ConjunctionContext *ctx) override;
        std::any visitEquality(parser::AstraParser::EqualityContext *ctx) override;
        std::any visitComparison(parser::AstraParser::ComparisonContext *ctx) override;
        std::any visitAddition(parser::AstraParser::AdditionContext *ctx) override;
        std::any visitMultiplication(parser::AstraParser::MultiplicationContext *ctx) override;
        std::any visitUnaryExpr(parser::AstraParser::UnaryExprContext *ctx) override;
        std::any visitParenExpr(parser::AstraParser::ParenExprContext *ctx) override;
        std::any visitLiteral(parser::AstraParser::LiteralContext *ctx) override;
        std::any visitVariable(parser::AstraParser::VariableContext *ctx) override;
        // TODO
    };
}
