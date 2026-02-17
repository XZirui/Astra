#pragma once

#include "CompilerContext.hpp"
#include "astra/ast/Program.hpp"
#include "astra/parser/AstraParserBaseVisitor.h"

namespace astra::frontend {
    class ASTBuilder : public parser::AstraParserBaseVisitor {
        CompilerContext &CompilerCtx;

        template <typename SubCtx, typename CtxOp, typename Getter>
        std::any buildBinaryExpr(support::SourceRange       Range,
                                 const std::vector<SubCtx> &Subs,
                                 const std::vector<CtxOp>  &Ops,
                                 Getter                     GetterFunction);

    public:
        explicit ASTBuilder(CompilerContext &Ctx);

        ast::Program *build(parser::AstraParser::FileContext *Ctx);

        enum class IntBase { Bin = 2, Oct = 8, Dec = 10, Hex = 16 };

        static IntBase detectBase(std::string_view Text);

        std::any visitFile(parser::AstraParser::FileContext *Ctx) override;
        std::any visitTopLevelObject(
            parser::AstraParser::TopLevelObjectContext *Ctx) override;
        std::any
        visitDeclaration(parser::AstraParser::DeclarationContext *Ctx) override;
        std::any visitFunctionDecl(
            parser::AstraParser::FunctionDeclContext *Ctx) override;
        std::any
        visitParamList(parser::AstraParser::ParamListContext *Ctx) override;
        std::any
        visitParameter(parser::AstraParser::ParameterContext *Ctx) override;
        std::any visitType(parser::AstraParser::TypeContext *Ctx) override;
        std::any
        visitParenType(parser::AstraParser::ParenTypeContext *Ctx) override;
        std::any
        visitTypeRef(parser::AstraParser::TypeRefContext *Ctx) override;
        std::any visitFunctionType(
            parser::AstraParser::FunctionTypeContext *Ctx) override;
        std::any visitParamTypeList(
            parser::AstraParser::ParamTypeListContext *Ctx) override;
        std::any
        visitVoidType(parser::AstraParser::VoidTypeContext *Ctx) override;
        std::any
        visitBoolType(parser::AstraParser::BoolTypeContext *Ctx) override;
        std::any
        visitIntType(parser::AstraParser::IntTypeContext *Ctx) override;
        std::any
        visitLongType(parser::AstraParser::LongTypeContext *Ctx) override;
        std::any
        visitFloatType(parser::AstraParser::FloatTypeContext *Ctx) override;
        std::any
        visitDoubleType(parser::AstraParser::DoubleTypeContext *Ctx) override;
        std::any
        visitStatement(parser::AstraParser::StatementContext *Ctx) override;
        std::any
        visitBlockStmt(parser::AstraParser::BlockStmtContext *Ctx) override;
        std::any visitIfStmt(parser::AstraParser::IfStmtContext *Ctx) override;
        std::any
        visitExprStmt(parser::AstraParser::ExprStmtContext *Ctx) override;
        std::any
        visitReturnStmt(parser::AstraParser::ReturnStmtContext *Ctx) override;
        std::any
        visitBreakStmt(parser::AstraParser::BreakStmtContext *Ctx) override;
        std::any visitContinueStmt(
            parser::AstraParser::ContinueStmtContext *Ctx) override;
        std::any
        visitExpression(parser::AstraParser::ExpressionContext *Ctx) override;
        std::any
        visitDisjunction(parser::AstraParser::DisjunctionContext *Ctx) override;
        std::any
        visitConjunction(parser::AstraParser::ConjunctionContext *Ctx) override;
        std::any
        visitEquality(parser::AstraParser::EqualityContext *Ctx) override;
        std::any
        visitComparison(parser::AstraParser::ComparisonContext *Ctx) override;
        std::any
        visitBitwiseOr(parser::AstraParser::BitwiseOrContext *Ctx) override;
        std::any
        visitBitwiseXor(parser::AstraParser::BitwiseXorContext *Ctx) override;
        std::any
        visitBitwiseAnd(parser::AstraParser::BitwiseAndContext *Ctx) override;
        std::any visitBitwiseShift(
            parser::AstraParser::BitwiseShiftContext *Ctx) override;
        std::any
        visitAddition(parser::AstraParser::AdditionContext *Ctx) override;
        std::any visitMultiplication(
            parser::AstraParser::MultiplicationContext *Ctx) override;
        std::any
        visitUnaryExpr(parser::AstraParser::UnaryExprContext *Ctx) override;
        std::any
        visitParenExpr(parser::AstraParser::ParenExprContext *Ctx) override;
        std::any
        visitIntLiteral(parser::AstraParser::IntLiteralContext *Ctx) override;
        std::any
        visitBoolLiteral(parser::AstraParser::BoolLiteralContext *Ctx) override;
        std::any
        visitNullLiteral(parser::AstraParser::NullLiteralContext *Ctx) override;
        std::any visitFloatLiteral(
            parser::AstraParser::FloatLiteralContext *Ctx) override;
        std::any visitDoubleLiteral(
            parser::AstraParser::DoubleLiteralContext *Ctx) override;
        std::any
        visitVariable(parser::AstraParser::VariableContext *Ctx) override;
        // TODO
    };
} // namespace astra::frontend
