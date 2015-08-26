#ifndef HEADER_086303CA18754744903657E6B3A52B68
#define HEADER_086303CA18754744903657E6B3A52B68

#include "ast.h"
#include <iosfwd>
#include <functional>

class Parser
{
public:
    enum class TK {
        UNKNOWN, ERROR, END, SYMBOL, STRING, NUMBER, T, F, OP,
        BRACKET_OPEN, BRACKET_CLOSE
    };
    Parser(std::istream &s);
    bool eof() const { return eof_; }
    TK token();
    Ast::Ptr parseAtomicExpr();
    Ast::Ptr parseDeniableAtomicExpr();
    Ast::Ptr parsePotExpr();
    Ast::Ptr parseDeniablePotExpr();
    Ast::Ptr genericDeniableExpr(std::function<Ast::Ptr()>);
    Ast::Ptr parseMulDivModExpr();
    Ast::Ptr parsePlusMinusExpr();
    Ast::Ptr parseCmpExpr();
    Ast::Ptr parseExpr();
    const std::string &msg() const { return msg_; }
private:
    std::istream &s_;
    std::string msg_;
    std::string str_;
    Ast::O op_;
    TK tk_;
    double num_;
    void dumpPosition();
    void preToken(bool force = false);
    void swallowToken();
    TK peekEQ();
    TK peekLT();
    TK peekGT();
    TK peekNot();
    TK peekAlpha();
    TK peekAND();
    TK peekOR();
    Parser::TK pushBrackets();
    Parser::TK peekQuote();
    bool eof_;
    Ast::Ptr parsePlusMinusExprTail(Ast::Ptr);
};

#endif

