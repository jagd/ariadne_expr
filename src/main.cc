#include "interface.h"

#include <iostream>
#include <string>

int main()
{
    std::string str;
    std::getline(std::cin, str);
    Expression e(str);
    if (!e) {
        std::cerr << "Error: " << e.msg() << std::endl;
        return 1;
    }
    for (auto s : e.symbols()) {
        std::cout << s << '\n';
    }
    return 0;
}