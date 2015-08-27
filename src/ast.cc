#include "ast.h"
#include <memory>
#include <string>
#include <cassert>
#include <sstream>
#include <cmath>

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
    std::string &msg
)
{
    assert(l && r);
    const char *opDesc = "apply ^ on";
    std::ostringstream os;
    if (l->t == Ast::T::NUMBER && r->t == Ast::T::NUMBER) {
        return Ast::make(std::pow(l->num, r->num));
    }
    msg = opError(l,r, opDesc);
    return nullptr;
}

static Ast::Ptr land(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    std::string &msg
)
{
    assert(l && r);
    const char *opDesc = "apply && on";
    std::ostringstream os;
    if (l->t == Ast::T::BOOLEAN && r->t == Ast::T::BOOLEAN) {
        return Ast::make(l->b && r->b);
    }
    msg = opError(l,r, opDesc);
    return nullptr;
}

static Ast::Ptr lor(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    std::string &msg
)
{
    assert(l && r);
    const char *opDesc = "apply || on";
    std::ostringstream os;
    if (l->t == Ast::T::BOOLEAN && r->t == Ast::T::BOOLEAN) {
        return Ast::make(l->b || r->b);
    }
    msg = opError(l,r, opDesc);
    return nullptr;
}

static Ast::Ptr ceq(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    std::string &msg
)
{
    assert(l && r);
    const char *opDesc = "apply == on";
    std::ostringstream os;
    if (l->t == Ast::T::BOOLEAN && r->t == Ast::T::BOOLEAN) {
        return Ast::make(l->b == r->b);
    }
    if (l->t == Ast::T::STRING && r->t == Ast::T::STRING) {
        return Ast::make(l->str == r->str);
    }
    if (l->t == Ast::T::NUMBER && r->t == Ast::T::NUMBER) {
        return Ast::make(l->num == r->num);
    }
    msg = opError(l,r, opDesc);
    return nullptr;
}

static Ast::Ptr cne(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    std::string &msg
)
{
    assert(l && r);
    const char *opDesc = "apply != on";
    std::ostringstream os;
    if (l->t == Ast::T::BOOLEAN && r->t == Ast::T::BOOLEAN) {
        return Ast::make(l->b != r->b);
    }
    if (l->t == Ast::T::STRING && r->t == Ast::T::STRING) {
        return Ast::make(l->str != r->str);
    }
    if (l->t == Ast::T::NUMBER && r->t == Ast::T::NUMBER) {
        return Ast::make(l->num != r->num);
    }
    msg = opError(l,r, opDesc);
    return nullptr;
}

#define CMP_COMMON(X) do {\
        assert(l && r);\
        const char *opDesc = "apply " #X "on";\
        std::ostringstream os;\
        if (l->t == Ast::T::STRING && r->t == Ast::T::STRING) {\
        return Ast::make(l->str X r->str);\
        }\
        if (l->t == Ast::T::NUMBER && r->t == Ast::T::NUMBER) {\
        return Ast::make(l->num X r->num);\
        }\
        msg = opError(l,r, opDesc);\
        return nullptr;\
    } while(false);

static Ast::Ptr cgt(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    std::string &msg
)
{
    CMP_COMMON(>);
}

static Ast::Ptr cge(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    std::string &msg
)
{
    CMP_COMMON(>=);
}

static Ast::Ptr clt(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    std::string &msg
)
{
    CMP_COMMON(<);
}

static Ast::Ptr cle(
    const Ast::Ptr &l,
    const Ast::Ptr &r,
    std::string &msg
)
{
    CMP_COMMON(<=);
}

static std::string uniError(const Ast::Ptr &root)
{
    std::string msg = "cannot apply uni-operand operator ";
    msg += toString(root->t);
    msg += " on ";
    msg += toString(root->right->t);
    return msg;
}

static Ast::Ptr
uniOperand(const Ast::Ptr &root, std::string &msg)
{
    switch (root->op)  {
        case Ast::O::PLUS:
            if (root->right->t == Ast::T::NUMBER) {
                return root->right->clone();
            }
            break;
        case Ast::O::MINUS:
            if (root->right->t == Ast::T::NUMBER) {
                return Ast::make(-root->right->num);
            }
            break;
        case Ast::O::LOGICAL_NOT:
            if (root->right->t == Ast::T::BOOLEAN) {
                return Ast::make(!root->right->b);
            }
            break;
        default:
            break;
    }
    msg = uniError(root);
    return nullptr;

}

static Ast::Ptr
opEval(const Ast::Ptr &root, const Ast::Dict &dict,  std::string &msg)
{
    assert(root->t == Ast::T::OPERATOR);
    auto r = eval(root->right, dict, msg);
    if (!r) {
        return nullptr;
    }
    if (!root->left) {
        auto rt = Ast::make(root->op);
        rt->right = std::move(r);
        return uniOperand(rt, msg);
    }
    auto l = eval(root->left, dict, msg);
    if (!l) {
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
            return amod(l,r,msg);
        case Ast::O::POWER:
            return apow(l,r,msg);
        case Ast::O::LOGICAL_AND:
            // no short circuit
            return land(l,r,msg);
        case Ast::O::LOGICAL_OR:
            // no short circuit
            return lor(l,r,msg);
        case Ast::O::CMP_EQ:
            return ceq(l,r,msg);
        case Ast::O::CMP_NE:
            return cne(l,r,msg);
        case Ast::O::CMP_GT:
            return cgt(l,r,msg);
        case Ast::O::CMP_GE:
            return cge(l,r,msg);
        case Ast::O::CMP_LT:
            return clt(l,r,msg);
        case Ast::O::CMP_LE:
            return cle(l,r,msg);
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
