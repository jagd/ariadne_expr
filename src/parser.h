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
    TK token();
    Ast::Ptr parseAtomicExpr();
    Ast::Ptr parseDeniableAtomicExpr();
    Ast::Ptr parsePotExpr();
    Ast::Ptr parseDeniablePotExpr();
    Ast::Ptr genericDeniableExpr(std::function<Ast::Ptr()>);
    Ast::Ptr parseMulDivModExpr();
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
    Ast::Ptr parseExpr();
};

#endif

