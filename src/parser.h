#ifndef HEADER_086303CA18754744903657E6B3A52B68
#define HEADER_086303CA18754744903657E6B3A52B68

#include <iosfwd>

class Parser
{
public:
    Parser(std::istream &s);
private:
    std::istream &s_;
};

#endif

