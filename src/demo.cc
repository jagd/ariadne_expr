#include "interface.h"

#include <iostream>
#include <string>
#include <memory>
#include <sstream>

#include <parameter.h> // ariadne code

/// @brief a not very strict read function
/// @note do not handle boolean type, since ariadne does not support yet
std::shared_ptr<parameter> readParameter()
{
    std::string s;
    do {
        std::getline(std::cin, s);
    } while (s.empty());
    std::istringstream ss(s);
    double x;
    if (ss >> x) {
        auto p = std::make_shared<parameter>(PT_REAL);
        p->setValueReal(x);
        return p;
    }
    auto p = std::make_shared<parameter>(PT_STRING);
    p->setValueString(s);
    return p;
}

int main()
{
    std::string str;
    std::getline(std::cin, str);
    Expression e(str);
    if (!e) {
        std::cerr << "Error: " << e.msg() << std::endl;
        return 1;
    }
    Expression::Dict dict;
    for (auto s : e.symbols()) {
        std::cout << s << " = ";
        auto p = readParameter();
        if (!p) {
            std::cerr << "Error: not a valid parameter" << std::endl;
            return 2;
        }
        dict[s] = p;
    }
    auto v = e.eval(dict);
    if (!e) { //  or to write if (!v.first)
        std::cerr << "Error: " << v.second << std::endl;
        return 3;
    }
    v.first->print();
    return 0;
}