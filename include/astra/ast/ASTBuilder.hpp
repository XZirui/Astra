#pragma once

#include "astra/parser/AstraParserBaseVisitor.h"
#include "ASTContext.hpp"
#include "Program.hpp"

namespace astra::ast {
    class ASTBuilder : public parser::AstraParserBaseVisitor {
        ASTContext &AstCtx;

        template<typename SubCtx, typename CtxOp, typename Getter>
        std::any buildBinaryExpr(support::SourceRange Range, const std::vector<SubCtx> &Subs, const std::vector<CtxOp> &Ops,
                                 Getter GetterFunction);

    public:
        explicit ASTBuilder(ASTContext &AstCtx);

        Program *build(parser::AstraParser::FileContext *Ctx);

        enum class IntBase { Bin = 2, Oct = 8, Dec = 10, Hex = 16 };

        static IntBase detectBase(std::string_view Text);

        std::any visitFile(parser::AstraParser::FileContext *Ctx) override;
        std::any visitTopLevelObject(parser::AstraParser::TopLevelObjectContext *Ctx) override;
        std::any visitStatement(parser::AstraParser::StatementContext *Ctx) override;
        std::any visitBlockStmt(parser::AstraParser::BlockStmtContext *Ctx) override;
        std::any visitIfStmt(parser::AstraParser::IfStmtContext *Ctx) override;
        std::any visitExprStmt(parser::AstraParser::ExprStmtContext *Ctx) override;
        std::any visitReturnStmt(parser::AstraParser::ReturnStmtContext *Ctx) override;

        std::any visitExpression(parser::AstraParser::ExpressionContext *Ctx) override;
        std::any visitDisjunction(parser::AstraParser::DisjunctionContext *Ctx) override;
        std::any visitConjunction(parser::AstraParser::ConjunctionContext *Ctx) override;
        std::any visitEquality(parser::AstraParser::EqualityContext *Ctx) override;
        std::any visitComparison(parser::AstraParser::ComparisonContext *Ctx) override;
        std::any visitAddition(parser::AstraParser::AdditionContext *Ctx) override;
        std::any visitMultiplication(parser::AstraParser::MultiplicationContext *Ctx) override;
        std::any visitUnaryExpr(parser::AstraParser::UnaryExprContext *Ctx) override;
        std::any visitParenExpr(parser::AstraParser::ParenExprContext *Ctx) override;
        std::any visitLiteral(parser::AstraParser::LiteralContext *Ctx) override;
        std::any visitVariable(parser::AstraParser::VariableContext *Ctx) override;
        // TODO
    };
} // namespace astra::ast
