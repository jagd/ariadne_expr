#ifndef HEADER_086303CA18754744903657E6B3A52B68
#define HEADER_086303CA18754744903657E6B3A52B68

#include "ast.h"
#include <iosfwd>

class Parser
{
public:
    enum class TK {
        UNKNOWN, ERROR, END, SYMBOL, STRING, REAL, T, F, OP,
        BRACKET_OPEN, BRACKET_CLOSE
    };
    Parser(std::istream &s);
    TK token();
    Ast::Ptr parseAtomicExpr();
private:
    std::istream &s_;
    std::string msg_;
    std::string str_;
    Ast::O op_;
    double num_;
    void dumpPosition();
    TK peekEQ();
    TK peekLT();
    TK peekGT();
    TK peekNot();
    TK peekAlpha();
    Parser::TK pushBrackets();
    Parser::TK peekQuote();

    Ast::Ptr parseExpr();
};

#endif

