#include "ast.h"
#include <memory>
#include <string>
#include <cassert>
#include <sstream>

Ast::Ast() : t(Ast::T::UNKNOWN)
{
}

Ast::Ast(double v) : t(Ast::T::NUMBER), num(v)
{
}

Ast::Ast(const std::string &s) : t(Ast::T::UNKNOWN), str(s)
{
}

Ast::Ast(Ast::O o) : t(Ast::T::OPERATOR), op(o)
{
}

Ast::Ast(bool b) : t(Ast::T::BOOLEAN), b(b)
{
}

std::unique_ptr<Ast> Ast::make(Ast::O o)
{
    return std::unique_ptr<Ast>(new Ast(o));
}

std::unique_ptr<Ast> Ast::make(double v)
{
    return std::unique_ptr<Ast>(new Ast(v));
}

std::unique_ptr<Ast> Ast::makeString(const std::string &s)
{
    auto r = std::unique_ptr<Ast>(new Ast(s));
    r->t =Ast::T::STRING;
    return std::unique_ptr<Ast>(std::move(r));
}

std::unique_ptr<Ast> Ast::makeSymbol(const std::string &s)
{
    auto r = std::unique_ptr<Ast>(new Ast(s));
    r->t =Ast::T::SYMBOL;
    return std::unique_ptr<Ast>(std::move(r));
}

std::unique_ptr<Ast> Ast::make(bool b)
{
    return std::unique_ptr<Ast>(new Ast(b));
}

void symbolsImpl(const Ast::Ptr &p, std::set<std::string> &s)
{
    if (!p) {
        return;
    }
    if (p->t == Ast::T::SYMBOL) {
        s.insert(p->str);
    }
    symbolsImpl(p->left, s);
    symbolsImpl(p->right, s);
}

std::set<std::string> symbols(const Ast::Ptr &p)
{
    std::set<std::string> s;
    symbolsImpl(p, s);
    return s;
}

static const char *toString(Ast::T t)
{
    switch (t) {
        case Ast::T::SYMBOL:
            return "symbol";
        case Ast::T::NUMBER:
            return "number";
        case Ast::T::STRING:
            return "string";
        case Ast::T::OPERATOR:
            return "operator";
        case Ast::T::BOOLEAN:
            return "boolean";
        default:
            return "unknown";
    }
}

static std::string opError(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    const char *opDesc
)
{
    std::string msg = "cannot ";
    msg += opDesc;
    msg += " ";
    msg += toString(l->t);
    msg += " and ";
    msg += toString(r->t);
    return msg;
}

static Ast::Ptr aadd(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    std::string &msg
)
{
    assert(l && r);
    const char *opDesc = "add";
    std::ostringstream os;
    switch (l->t) {
        case Ast::T::NUMBER:
            switch (r->t) {
                case Ast::T::NUMBER:
                    return Ast::make(l->num + r->num);
                case Ast::T::STRING:
                    os << l->num;
                    os << r->str;
                    return Ast::makeString(os.str());
                default:
                    break;
            }
        case Ast::T::STRING:
            os << l->str;
            switch (r->t) {
                case Ast::T::NUMBER:
                    os << r->num;
                    return Ast::makeString(os.str());
                case Ast::T::STRING:
                    os << r->str;
                    return Ast::makeString(os.str());
                default:
                    break;
            }
        default:
            msg = opError(l,r, opDesc);
            return nullptr;
    }
}

static Ast::Ptr asub(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    std::string &msg
)
{
    assert(l && r);
    const char *opDesc = "subtract";
    std::ostringstream os;
    if (l->t == Ast::T::NUMBER && r->t == Ast::T::NUMBER) {
        return Ast::make(l->num - r->num);
    }
    msg = opError(l,r, opDesc);
    return nullptr;
}

static Ast::Ptr amul(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    std::string &msg
)
{
    assert(l && r);
    const char *opDesc = "add";
    std::ostringstream os;
    switch (l->t) {
        case Ast::T::NUMBER:
            switch (r->t) {
                case Ast::T::NUMBER:
                    return Ast::make(l->num * r->num);
                case Ast::T::STRING:
                    for (int i = 0; i < static_cast<int>(l->num); ++i) {
                        os << r->str;
                    }
                    return Ast::makeString(os.str());
                default:
                    break;
            }
        case Ast::T::STRING:
            switch (r->t) {
                case Ast::T::NUMBER:
                    for (int i = 0; i < static_cast<int>(r->num); ++i) {
                        os << l->str;
                    }
                    return Ast::makeString(os.str());
                default:
                    break;
            }
        default:
            msg = opError(l,r, opDesc);
            return nullptr;
    }
}

static Ast::Ptr adiv(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    std::string &msg
)
{
    assert(l && r);
    const char *opDesc = "divide";
    std::ostringstream os;
    if (l->t == Ast::T::NUMBER && r->t == Ast::T::NUMBER) {
        if (r->num == 0) {
            msg = "divide by 0";
            return nullptr;
        }
        return Ast::make(l->num / r->num);
    }
    msg = opError(l,r, opDesc);
    return nullptr;
}

static Ast::Ptr amod(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    const Ast::Dict &dict,
    std::string &msg
)
{
    assert(l && r);
    const char *opDesc = "modulo";
    std::ostringstream os;
    if (l->t == Ast::T::NUMBER && r->t == Ast::T::NUMBER) {
        int n = static_cast<int>(r->num);
        if (n == 0) {
            msg = "modulo by 0";
            return nullptr;
        }
        return Ast::make(static_cast<double>(static_cast<int>(l->num) % n));
    }
    msg = opError(l,r, opDesc);
    return nullptr;
}

static Ast::Ptr apow(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    const Ast::Dict &dict,
    std::string &msg
)
{}

static Ast::Ptr land(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    const Ast::Dict &dict,
    std::string &msg
)
{}

static Ast::Ptr lor(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    const Ast::Dict &dict,
    std::string &msg
)
{}

static Ast::Ptr lnot(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    const Ast::Dict &dict,
    std::string &msg
)
{}

static Ast::Ptr ceq(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    const Ast::Dict &dict,
    std::string &msg
)
{}

static Ast::Ptr cne(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    const Ast::Dict &dict,
    std::string &msg
)
{}

static Ast::Ptr cgt(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    const Ast::Dict &dict,
    std::string &msg
)
{}

static Ast::Ptr cge(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    const Ast::Dict &dict,
    std::string &msg
)
{}

static Ast::Ptr clt(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    const Ast::Dict &dict,
    std::string &msg
)
{}

static Ast::Ptr cle(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    const Ast::Dict &dict,
    std::string &msg
)
{}

static Ast::Ptr
opEval(const Ast::Ptr &root, const Ast::Dict &dict,  std::string &msg)
{
    assert(root->t == Ast::T::OPERATOR);
    auto l = eval(root->left, dict, msg);
    if (!l) {
        return nullptr;
    }
    auto r = eval(root->right, dict, msg);
    if (!r) {
        return nullptr;
    }
    switch (root->op) {
        case Ast::O::PLUS:
            return aadd(l,r,msg);
        case Ast::O::MINUS:
            return asub(l,r,msg);
        case Ast::O::MULTIPLY:
            return amul(l,r,msg);
        case Ast::O::DIVISION:
            return adiv(l,r,msg);
        case Ast::O::MODULO:
            return amod(l,r,dict,msg);
        case Ast::O::POWER:
            return apow(l,r,dict,msg);
        case Ast::O::LOGICAL_AND:
            return land(l,r,dict,msg);
        case Ast::O::LOGICAL_OR:
            return lor(l,r,dict,msg);
        case Ast::O::LOGICAL_NOT:
            return lnot(l,r,dict,msg);
        case Ast::O::CMP_EQ:
            return ceq(l,r,dict,msg);
        case Ast::O::CMP_NE:
            return cne(l,r,dict,msg);
        case Ast::O::CMP_GT:
            return cgt(l,r,dict,msg);
        case Ast::O::CMP_GE:
            return cge(l,r,dict,msg);
        case Ast::O::CMP_LT:
            return clt(l,r,dict,msg);
        case Ast::O::CMP_LE:
            return cle(l,r,dict,msg);
    }
    return nullptr;
}

Ast::Ptr eval(const Ast::Ptr &root, const Ast::Dict &dict, std::string &msg)
{
    if (!root) {
        return nullptr;
    }
    switch (root->t) {
        case Ast::T::BOOLEAN:
        case Ast::T::NUMBER:
        case Ast::T::STRING:
            return root->clone();
        case Ast::T::SYMBOL: {
            const auto i = dict.find(root->str);
            if (i == dict.cend()) {
                msg = "unsolvable symbol ";
                msg += root->str;
                return nullptr;
            }
            return i->second->clone();
        }
        case Ast::T::OPERATOR:
            return opEval(root, dict, msg);
        default:
            return nullptr;
    }
}

Ast::Ptr Ast::clone() const
{
    return Ast::Ptr(new Ast(*this));
}

Ast::Ast(const Ast &root)
{
    t = root.t;
    switch (t) {
        case T::STRING:
        case T::SYMBOL:
            str = root.str;
            break;
        case T::NUMBER:
            num = root.num;
            break;
        case T::OPERATOR:
            op = root.op;
            break;
        case T::BOOLEAN:
            b = root.b;
            break;
        default:
            assert(false /* unreachable */);
    }
    if (root.left) {
        left.reset(new Ast(*(root.left)));
    }
    if (root.right) {
        right.reset(new Ast(*(root.right)));
    }
}
