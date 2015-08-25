#ifndef HEADER_82DEFF939A154BFA8787C84C8BB5CD66
#define HEADER_82DEFF939A154BFA8787C84C8BB5CD66

#include <memory>

struct Ast
{
    enum class T {UNKNOWN, SYMBOL, REAL, STRING, OPERATOR};
    enum class O {
        PLUS, MINUS, MULTIPLY, DIVISION, POTENTIAL, LOGICAL_AND,  LOGICAL_OR,
        LOGICAL_NOT, CMP_EQ, CMP_NE, CMP_GT, CMP_GE, CMP_LT, CMP_LE
    };
    Ast();
    Ast(double v);
    Ast(const std::string &s);
    Ast(O o);
    static std::unique_ptr<Ast> make(double v);
    static std::unique_ptr<Ast> make(O o);
    static std::unique_ptr<Ast> makeString(const std::string &s);
    static std::unique_ptr<Ast> makeSymbol(const std::string &s);
    T t;
    O op;
    double val;
    std::string str;
    std::unique_ptr<Ast> left;
    std::unique_ptr<Ast> right;
};
#endif
