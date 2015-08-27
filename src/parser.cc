#include "parser.h"
#include "ast.h"

#include <cctype>
#include <istream>

Parser::Parser(std::istream &s) : s_(s), tk_(TK::UNKNOWN), eof_(false)
{
}

Parser::TK Parser::token()
{
    s_ >> std::ws;
    const int peek = static_cast<int>(s_.peek());
    if (peek == std::istream::traits_type::eof()) {
        eof_ = true;
        msg_ = "EOF";
        return TK::END;
    } else if (std::isalpha(peek)) {
        str_.clear();
        return peekAlpha();
    } else if (std::isdigit(peek)) {
        s_ >> num_;
        return TK::NUMBER;
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
            case '%':
                s_.get();
                op_ = Ast::O::MODULO;
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
            case '&':
                return peekAND();
            case '|':
                return peekOR();
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
    msg_ = "operator '=' not understandable, do you mean '==' ?";
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
        while (std::isalnum(peek)) {
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
    } while (std::isalpha(s_.peek()) || s_.peek() == '.');
    if (str_ == "true") {
        return TK::T;
    } else if (str_ == "false") {
        return TK::F;
    }
    return TK::SYMBOL;
}

Parser::TK Parser::pushBrackets()
{
    str_.push_back(s_.get());
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

Ast::Ptr Parser::parseAtomicExpr()
{
    preToken();
    switch (tk_) {
        case TK::END:
            msg_ = "unexpected end";
            return nullptr;
        case TK::SYMBOL:
            swallowToken();
            return Ast::makeSymbol(str_);
        case TK::STRING:
            swallowToken();
            return Ast::makeString(str_);
        case TK::NUMBER:
            swallowToken();
            return Ast::make(num_);
        case TK::T:
            swallowToken();
            return Ast::make(true);
        case TK::F:
            swallowToken();
            return Ast::make(false);
        case TK::BRACKET_OPEN: {
                swallowToken();
                Ast::Ptr root = parseExpr();
                if (!root) {
                    dumpPosition();
                    return nullptr;
                }
                preToken();
                if (tk_ != TK::BRACKET_CLOSE) {
                    msg_ = "expect )";
                    dumpPosition();
                    return nullptr;
                }
                swallowToken();
                return root;
            };
        case TK::ERROR:
            msg_ += "| error";
            dumpPosition();
            return nullptr;
        default:
            msg_ = "expect something";
            dumpPosition();
            return nullptr;
    }
}

void Parser::dumpPosition()
{
    std::string word;
    s_ >> word;
    msg_ += " before '";
    msg_ += word.empty() ? "the end" : word;
    msg_ += '\'';
}

Parser::TK Parser::peekAND()
{
    s_.get();
    const auto next = s_.get();
    if (next == '&') {
        op_ = Ast::O::LOGICAL_AND;
        return TK::OP;
    }
    msg_ = "operator '&' not understandable, do you mean '&&' ?";
    return TK::ERROR;
}

Parser::TK Parser::peekOR()
{
    s_.get();
    const auto next = s_.get();
    if (next == '|') {
        op_ = Ast::O::LOGICAL_OR;
        return TK::OP;
    }
    msg_ = "operator '|' not understandable, do you mean '|| ?";
    return TK::ERROR;
}

Ast::Ptr Parser::genericDeniableExpr(std::function<Ast::Ptr()> f)
{
    preToken();
    if (tk_ != TK::OP) {
        return f();
    }
    Ast::Ptr root;
    switch (op_) {
        case Ast::O::PLUS:
            swallowToken();
            root = f();
            break;
        case Ast::O::MINUS:
            swallowToken();
            root = Ast::make(Ast::O::MINUS);
            root->right = f();
            break;
        case Ast::O::LOGICAL_NOT:
            swallowToken();
            root = Ast::make(Ast::O::LOGICAL_NOT);
            root->right = f();
            break;
        default:
            swallowToken();
            msg_ = "unacceptable operator " + msg_;
            dumpPosition();
            root.release();
    }
    return root;
}

Ast::Ptr Parser::parseDeniableAtomicExpr()
{
    return genericDeniableExpr(std::bind(&Parser::parseAtomicExpr, this));
}

Ast::Ptr Parser::parseDeniablePotExpr()
{
    return genericDeniableExpr(std::bind(&Parser::parsePotExpr, this));
}

void Parser::preToken(bool force)
{
    if (tk_ == TK::UNKNOWN || force) {
        tk_  = token();
    }
}

void Parser::swallowToken()
{
    tk_ = TK::UNKNOWN;
}

Ast::Ptr Parser::parsePotExpr()
{
    Ast::Ptr root = parseAtomicExpr();;
    if (!root) {
        return nullptr;
    }
    preToken();
    if (tk_ == TK::OP && op_ == Ast::O::POWER) {
        swallowToken();
        auto a = std::move(root);
        root = Ast::make(Ast::O::POWER);
        root->left = std::move(a);
        root->right = parseDeniableAtomicExpr();
        if (!root->right) {
            return nullptr;
        }
    }
    return root;
}

Ast::Ptr Parser::parseMulDivModExpr()
{
    auto a = parseDeniablePotExpr();
    if (!a) {
        return nullptr;
    }
    return parseMulDivModExprTail(std::move(a));
}

Ast::Ptr Parser::parsePlusMinusExpr()
{
    auto a = parseMulDivModExpr();
    if (!a) {
        return nullptr;
    }
    return parsePlusMinusExprTail(std::move(a));
}

Ast::Ptr Parser::parseCmpExpr()
{
    preToken();
    auto a = parsePlusMinusExpr();
    if (!a) {
        return nullptr;
    }
    preToken();
    if (tk_ != TK::OP) {
        return a;
    }
    Ast::Ptr root;
    switch (op_) {
        case Ast::O::CMP_EQ:
        case Ast::O::CMP_NE:
        case Ast::O::CMP_GT:
        case Ast::O::CMP_GE:
        case Ast::O::CMP_LT:
        case Ast::O::CMP_LE:
            swallowToken();
            root = Ast::make(op_);
            break;
        default:
            return a;
    }
    root->left = std::move(a);
    root->right = parsePlusMinusExpr();
    if (!root->right) {
        return nullptr;
    }
    return root;
}

Ast::Ptr Parser::parseExpr()
{
    preToken();
    auto a = parseCmpExpr();
    if (!a) {
        return nullptr;
    }
    preToken();
    if (tk_ != TK::OP) {
        return a;
    }
    Ast::Ptr root;
    switch (op_) {
        case Ast::O::LOGICAL_AND:
        case Ast::O::LOGICAL_OR:
            swallowToken();
            root = Ast::make(op_);
            break;
        default:
            return a;
    }
    root->left = std::move(a);
    root->right = parseExpr();
    if (!root->right) {
        return nullptr;
    }
    return root;
}

Ast::Ptr Parser::parsePlusMinusExprTail(Ast::Ptr &&a)
{
    preToken();
    if (tk_ != TK::OP) {
        return std::move(a);
    }
    Ast::Ptr root;
    switch (op_) {
        case Ast::O::PLUS:
        case Ast::O::MINUS:
            swallowToken();
            root = Ast::make(op_);
            break;
        default:
            return std::move(a);
    }
    root->left = std::move(a);
    root->right = parseMulDivModExpr();
    if (!root->right) {
        return nullptr;
    }
    return parsePlusMinusExprTail(std::move(root));
}

Ast::Ptr Parser::parseMulDivModExprTail(Ast::Ptr &&a)
{
    preToken();
    if (tk_ != TK::OP) {
        return std::move(a);
    }
    Ast::Ptr root;
    switch (op_) {
        case Ast::O::MULTIPLY:
        case Ast::O::DIVISION:
        case Ast::O::MODULO:
            swallowToken();
            root = Ast::make(op_);
            break;
        default:
            return std::move(a);
    }
    root->left = std::move(a);
    root->right = parseDeniablePotExpr();
    if (!root->right) {
        return nullptr;
    }
    return parseMulDivModExprTail(std::move(root));

}
