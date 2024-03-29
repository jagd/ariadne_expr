#ifndef ARIADNE_PARSER_INTERFACE_H
#define ARIADNE_PARSER_INTERFACE_H

#include <memory>
#include <utility>
#include <string>
#include <set>
#include <map>

#include <parameter.h> // ariadne code

#if defined _WIN32 || defined __CYGWIN__
#ifdef BUILDING_DLL
#ifdef __GNUC__
  #define DLL_EXPORT __attribute__ ((dllexport))
#else
  #define DLL_EXPORT __declspec(dllexport) // Note: actually gcc seems to
  also
  supports this syntax.
#endif
#else
#ifdef __GNUC__
  #define DLL_EXPORT __attribute__ ((dllimport))
#else
  #define DLL_EXPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#define DLL_LOCAL
#else
#if __GNUC__ >= 4
#define DLL_EXPORT __attribute__ ((visibility ("default")))
#else
#define DLL_EXPORT
#endif
#endif

struct ExpressionImpl;
class DLL_EXPORT Expression {
public:
    Expression();
    Expression(const std::string &expr);
    std::set<std::string> symbols() const;
    typedef std::map<std::string, std::shared_ptr<parameter> > Dict;
    std::pair<std::shared_ptr<parameter>, std::string> eval(const Dict &);
    operator bool() const;
    bool parse(const std::string &expr);
    const std::string msg() const;
private:
    std::shared_ptr<ExpressionImpl> impl_;
};

#endif
