#include <iostream>
#include "parameter.h"

#include <stdexcept>
typedef std::logic_error catch_error;
#pragma GCC diagnostic ignored "-Wsign-compare"

using namespace std;

parameter::parameter() {
    _type = PT_UNDEFINED;
}

parameter::parameter(parameter_type type) {
    _type = type;
}

void parameter::setValueChar(char c) {
    if(_type != PT_CHAR) throw catch_error("invalid type of parameter");
    _value.c = c;
}

void parameter::setValueInteger(int i) {
    if(_type != PT_INTEGER) throw catch_error("invalid type of parameter");
    _value.i = i;
}

void parameter::setValueReal(double r) {
    if(_type != PT_REAL) throw catch_error("invalid type of parameter");
    _value.r = r;
}

void parameter::setValueString(const string& s) {
    if(_type != PT_STRING) throw catch_error("invalid type of parameter");
    _value.s = new char [s.size()+1];
    for(int i=0; i<s.size()+1; i++)
        _value.s[i] = s.c_str()[i];
}

void parameter::set(std::shared_ptr<entity> ent) {
    auto prm = dynamic_pointer_cast<parameter>(ent);

    if(prm->getType() != _type) throw catch_error("incompatible parameters");

    if(_type == PT_CHAR)
        _value.c = prm->getValueChar();
    if(_type == PT_INTEGER)
        _value.i = prm->getValueInteger();
    if(_type == PT_REAL)
        _value.r = prm->getValueReal();
    if(_type == PT_STRING)
        setValueString(prm->getValueString());
}

parameter::~parameter()
{

}

void parameter::print() const {
    cout<<"parameter: "<<_name<<endl;
    cout<<"     type: ";
    if(_type == PT_UNDEFINED) cout<<"none";
    else if(_type == PT_CHAR) cout<<"character";
    else if(_type == PT_INTEGER) cout<<"integer";
    else if(_type == PT_REAL) cout<<"real";
    else if(_type == PT_STRING) cout<<"string";
    cout<<endl;
    cout<<"    value: ";
    if(_type == PT_UNDEFINED) cout<<"0";
    else if(_type == PT_CHAR) cout<<_value.c;
    else if(_type == PT_INTEGER) cout<<_value.i;
    else if(_type == PT_REAL) cout<<_value.r;
    else if(_type == PT_STRING) cout<<_value.s;
    cout<<endl;
}

