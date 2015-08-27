#ifndef PARAMETER_H
#define PARAMETER_H
#include <string>
#include "entity.h"

enum parameter_type {PT_UNDEFINED, PT_CHAR, PT_INTEGER, PT_REAL, PT_STRING};

class parameter : public entity {
public:
    parameter();
    parameter(parameter_type type);
    ~parameter();

    void setValueChar(char c);
    void setValueInteger(int i);
    void setValueReal(double r);
    void setValueString(const std::string& s);

    void set(std::shared_ptr<entity> ent);
    int getValueInteger() const { return _value.i; }
    double getValueReal() const { return _value.r; }
    char getValueChar() const { return _value.c; }
    std::string getValueString() const { return _value.s; }

    parameter_type getType() const { return _type; }

    void print() const;

private:

    parameter_type _type;

    union _value {
        char c;
        int i;
        double r;
        char *s;
    } _value;
};


#endif // PARAMETER_H
