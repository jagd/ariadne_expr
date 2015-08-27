#ifndef HEADER_82DEFF939A154BFA8787C84C8BB5CD66
#define HEADER_82DEFF939A154BFA8787C84C8BB5CD66

#include <memory>
#include <string>
#include <set>
#include <map>

struct Ast
{
    typedef std::unique_ptr<Ast> Ptr;
    typedef std::map<std::string, Ast::Ptr> Dict;
    enum class T {UNKNOWN, SYMBOL, NUMBER, STRING, OPERATOR, BOOLEAN};
    enum class O {
        PLUS, MINUS, MULTIPLY, DIVISION, MODULO, POWER,
        LOGICAL_AND,  LOGICAL_OR, LOGICAL_NOT,
        CMP_EQ, CMP_NE, CMP_GT, CMP_GE, CMP_LT, CMP_LE,
    };
    Ast();
    Ast(const Ast &);
    explicit Ast(bool b);
    explicit Ast(double v);
    explicit Ast(const std::string &s);
    Ast(O o);
    std::unique_ptr<Ast> clone() const;
    static std::unique_ptr<Ast> make(double v);
    static std::unique_ptr<Ast> make(O o);
    static std::unique_ptr<Ast> make(bool b);
    static std::unique_ptr<Ast> makeString(const std::string &s);
    static std::unique_ptr<Ast> makeSymbol(const std::string &s);
    T t;
    union {
        O op;
        double num;
        bool b;
    };
    std::string str;
    std::unique_ptr<Ast> left;
    std::unique_ptr<Ast> right;
};

std::set<std::string> symbols(const Ast::Ptr &p);
Ast::Ptr eval(
    const Ast::Ptr &,
    const Ast::Dict &dict,
    std::string &msg
);

#endif
