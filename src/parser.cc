#include "parser.h"
#include "ast.h"

#include <istream>
#include <curses.h>

Parser::Parser(std::istream &s) : s_(s)
{
}

Parser::TK Parser::token()
{
    s_ >> std::ws;
    const int peek = static_cast<int>(s_.peek());
    if (peek == std::istream::traits_type::eof()) {
        msg_ = "EOF";
        return TK::END;
    } else if (std::isalpha(peek)) {
        str_.clear();
        return peekAlpha();
    } else if (std::isdigit(peek)) {
        s_ >> num_;
        return TK::REAL;
    } else switch (peek) {
            case '-':
                s_.get();
                op_ = Ast::O::MINUS;
                return TK::OP;
            case '+':
                s_.get();
                op_ = Ast::O::PLUS;
                return TK::OP;
            case '*':
                s_.get();
                op_ = Ast::O::MULTIPLY;
                return TK::OP;
            case '/':
                s_.get();
                op_ = Ast::O::DIVISION;
                return TK::OP;
            case '^':
                s_.get();
                op_ = Ast::O::POWER;
                return TK::OP;
            case '(':
                s_.get();
                return TK::BRACKET_OPEN;
            case ')':
                s_.get();
                return TK::BRACKET_CLOSE;
            case '=':
                return peekEQ();
            case '<':
                return peekLT();
            case '>':
                return peekGT();
            case '!':
                return peekNot();
            case '"':
                return peekQuote();
            default:
                msg_ = "invalid symbol ";
                msg_ += static_cast<char>(peek);
                return TK::ERROR;
    }
}

Parser::TK Parser::peekEQ()
{
    s_.get();
    const auto next = s_.get();
    if (next == '=') {
        op_ = Ast::O::CMP_EQ;
        return TK::OP;
    }
    msg_ = "operator '=' not understandable";
    return TK::ERROR;
}

Parser::TK Parser::peekLT()
{
    s_.get();
    const auto next = s_.peek();
    if (next == '=') {
        op_ = Ast::O::CMP_LE;
        s_.get();
    } else {
        op_ = Ast::O::CMP_LT;
    }
    return TK::OP;
}

Parser::TK Parser::peekGT()
{
    s_.get();
    const auto next = s_.peek();
    if (next == '=') {
        op_ = Ast::O::CMP_GE;
        s_.get();
    } else {
        op_ = Ast::O::CMP_GT;
    }
    return TK::OP;
}

Parser::TK Parser::peekNot()
{
    s_.get();
    const auto next = s_.peek();
    if (next == '=') {
        op_ = Ast::O::CMP_NE;
        s_.get();
    } else {
        op_ = Ast::O::LOGICAL_NOT;
    }
    return TK::OP;
}

Parser::TK Parser::peekAlpha()
{
    long peek;
    do {
        str_.push_back(s_.get());
        peek = s_.peek();
        while (std::isalnum(peek) && peek == std::istream::traits_type::eof()) {
            str_.push_back(s_.get());
            peek = s_.peek();
        }
        s_ >> std::ws;
        if (peek == '.') {
            str_.push_back(s_.get());
            return peekAlpha();
        }
        if (peek == '(') {
            if (pushBrackets() == TK::ERROR) {
                return TK::ERROR;
            }
        }
    } while (std::isalpha(s_.peek()));
    if (str_ == "true") {
        return TK::T;
    } else if (str_ == "FALSE") {
        return TK::F;
    }
    return TK::SYMBOL;
}

Parser::TK Parser::pushBrackets()
{
    s_.get();
    while (s_.peek() != ')') {
        if (s_.peek() == std::istream::traits_type::eof()) {
            msg_ = "unmatched parethenses";
            return TK::ERROR;
        }
        if (s_.peek() == '"') {
            std::string t;
            t.swap(str_);
            const auto r = peekQuote();
            if (r != TK::STRING) {
                return r;
            }
            t += '"';
            t += str_;
            t += '"';
            str_.swap(t);
        } else if (s_.peek() == '(') {
            if (pushBrackets() == TK::ERROR) {
                return TK::ERROR;
            }
        } else {
            str_.push_back(s_.get());
        }
    }
    str_.push_back(s_.get());
    return TK::SYMBOL;
}

Parser::TK Parser::peekQuote()
{
    s_.get();
    while (s_.peek() != '"') {
        if (s_.peek() == std::istream::traits_type::eof()) {
            msg_ = "unmatched quote";
            return TK::ERROR;
        }
        str_.push_back(s_.get());
    }
    s_.get(); // the tail quote
    return TK::STRING;
}
