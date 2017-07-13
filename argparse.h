/***
 * @brief A Simple Argument Parser Library for C++
 * @author Ryou Ohsawa
 * @date 2017
 *
 * This code is licensed under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>
#include <map>
#include <regex>
#include <stdexcept>

#ifndef __ARGPARSE_H_INCLUDE
#define __ARGPARSE_H_INCLUDE

namespace argparse {
  using std::regex_constants::icase;
  constexpr int16_t variable_args = -1;
  typedef std::string arg;
  typedef std::vector<argparse::arg> args;

  enum class value_type {
    arg_null, arg_boolean, arg_integer, arg_float, arg_string };

  class value {
  public:
    value(const value_type t): value(t, "") { }
    value(const value_type t, const arg& s): _type(t),_value(s)
    { assert_argument_type(); }

    const value_type& type(void) const { return _type; }
    const char* describe_type(void) const;

    value& operator=(const arg& s) {
      _value = s;
      assert_argument_type();
      return *this;
    }
    value& operator=(const char* c) {
      _value = arg(c);
      assert_argument_type();
      return *this;
    }
    template <class T> const T get(void) const;
  private:
    value_type _type;
    arg _value;
    void assert_argument_type(void) const;
    const bool convert_bool(void) const;
    const int64_t convert_integer(void) const;
    const double convert_float(void) const;
    const arg convert_string(void) const;
  };

  template <> inline
  const bool value::get<bool>(void) const {
    return convert_bool();
  }
  template <> inline
  const int16_t value::get<int16_t>(void) const {
    return (int16_t)convert_integer();
  }
  template <> inline
  const int32_t value::get<int32_t>(void) const {
    return (int32_t)convert_integer();
  }
  template <> inline
  const int64_t value::get<int64_t>(void) const {
    return convert_integer();
  }
  template <> inline
  const uint16_t value::get<uint16_t>(void) const {
    return (uint16_t)convert_integer();
  }
  template <> inline
  const uint32_t value::get<uint32_t>(void) const {
    return (uint32_t)convert_integer();
  }
  template <> inline
  const uint64_t value::get<uint64_t>(void) const {
    return (uint64_t)convert_integer();
  }
  template <> inline
  const float value::get<float>(void) const {
    return convert_float();
  }
  template <> inline
  const double value::get<double>(void) const {
    return convert_float();
  }
  template <> inline
  const arg value::get<arg>(void) const {
    return convert_string();
  }

  const char*
  value::describe_type(void) const
  {
    switch (_type) {
    case value_type::arg_integer : return "integer"; break;
    case value_type::arg_float   : return "float"; break;
    case value_type::arg_string  : return "string"; break;
    default: throw std::runtime_error("wrong argument type.");
    }
  }

  void
  value::assert_argument_type(void) const
  {
    switch (_type) {
    case value_type::arg_null :
      throw std::runtime_error("argument type is null.");
      break;
    case value_type::arg_boolean :
      convert_bool();
      break;
    case value_type::arg_integer :
      convert_integer();
      break;
    case value_type::arg_float   :
      convert_float();
      break;
    case value_type::arg_string  :
      convert_string();
      break;
    default:
      throw std::runtime_error("wrong argument type is set.");
    }
  }

  const bool
  value::convert_bool(void) const
  {
    if (std::regex_match(_value, std::regex("true", icase))) {
      return true;
    } else if (std::regex_match(_value, std::regex("false", icase))) {
      return false;
    } else {
      try {
        return (std::stol(_value) != 0);
      } catch (std::exception& e) {
        throw std::runtime_error("value is not convertible to boolean-type");
      }
    }
  }

  const int64_t
  value::convert_integer(void) const
  {
    try {
      return std::stol(_value);
    } catch (std::exception& e) {
      throw std::runtime_error("value is not convertible to integer-type");
    }
  }
  const double
  value::convert_float(void) const
  {
    try {
      return std::stod(_value);
    } catch (std::exception& e) {
      throw std::runtime_error("value is not convertible to float-type");
    }
  }

  const arg
  value::convert_string(void) const
  {
    try {
      return _value;
    } catch (std::exception& e) {
      throw std::runtime_error("value is not convertible to string-type");
    }
  }


  class parameter {
  public:
    parameter(const arg& name, const value_type t,
              const int16_t n=1)
      : _type(t),_nargs(n),_name(name),_comment("") {}
    parameter(const arg& name, const value_type t,
              const int16_t n, const arg& com)
      : _type(t),_nargs(n),_name(name),_comment(com) {}
    parameter(const arg& name, const value_type t,
              const arg& com)
      : _type(t),_nargs(1),_name(name),_comment(com) {}

    const value_type& type(void) const { return _type; }
    const char* describe_type(void) const;
    const arg& name(void) const { return _name; }
    const int16_t& nargs(void) const { return _nargs; }
    const arg& comment(void) const { return _comment; }

    int operator==(const arg& str) const { return name().compare(str); }

    void format(void) const;
    void explain(void) const;
  private:
    value_type _type;
    int16_t _nargs;
    arg _name;
    arg _comment;
  };

  const char*
  parameter::describe_type(void) const
  {
    switch (_type) {
    case value_type::arg_integer : return "integer"; break;
    case value_type::arg_float   : return "float"; break;
    case value_type::arg_string  : return "string"; break;
    default: throw std::runtime_error("wrong argument type.");
    }
  }

  void
  parameter::format(void) const
  {
    if (nargs() != 0) {
      if (nargs() > 1) {
        printf("%s(%d)", name().c_str(), 0);
        for (auto i=1; i<nargs(); i++) {
          printf(" %s(%d)", name().c_str(), i);
        }
      } else if (nargs() == 1){
        printf("%s", name().c_str());
      } else {
        printf("%s...", name().c_str());
      }
    }
    printf(" ");
  }

  void
  parameter::explain(void) const
  {
    printf("  %s [%s", name().c_str(), describe_type());
    for (auto i=1; i<nargs(); i++)
      printf(",%s", describe_type());
    if (nargs() == variable_args) printf(",...");
    printf("]:\n");
    if (comment().size()>0) {
      size_t n = 0;
      int64_t N = comment().size();
      for (auto i=0; i<N; i++) {
        if (n==0) {
          printf("        ");
          n += 8;
        }
        printf("%c", comment()[i]);
        n++;
        if (n==80) { n=0; printf("\n"); }
      }
      if (n!=0) printf("\n");
    }
  }


  class option: public parameter {
  public:
    option(const arg& dir, const arg& name, const arg& com = "")
      : option(dir, name, value_type::arg_boolean, 0, com)
    { }
    option(const std::vector<arg>& dirs, const arg& name, const arg& com = "")
      : option(dirs, name, value_type::arg_boolean, 0, com)
    { }
    option(const arg& dir,
           const arg& name, const value_type t, const int16_t n = 1)
      : option(dir, name, t, n, "")
    { }
    option(const std::vector<arg>& dirs,
           const arg& name, const value_type t, const int16_t n = 1)
      : option(dirs, name, t, n, "")
    { }
    option(const arg& dir,
           const arg& name, const value_type t, const int16_t n,
           const arg& com)
      : parameter(name, t, n, com)
    { _optseqs.push_back(dir); }
    option(const std::vector<arg>& dirs,
           const arg& name, const value_type t, const int16_t n,
           const arg& com)
      : parameter(name, t, n, com)
    { for (auto d : dirs) _optseqs.push_back(d); }

    const std::vector<arg>& options(void) const { return _optseqs; }

    bool operator==(const arg& str) const {
      for (auto d : _optseqs)
        if (d.compare(str) == 0) return true;
      return false;
    }

    void format(void) const;
    void explain(void) const;
  private:
    std::vector<arg> _optseqs;
    void show_option(void) const;
  };

  void
  option::show_option(void) const
  {
    if (_optseqs.size() > 1) {
      printf("%s", _optseqs[0].c_str());
      for (size_t i=1; i<_optseqs.size(); i++)
        printf("|%s", _optseqs[i].c_str());
    } else {
      printf("%s", _optseqs[0].c_str());
    }
  }

  void
  option::format(void) const
  {
    printf("[");
    if (_optseqs.size()>1) printf("{");
    show_option();
    if (_optseqs.size()>1) printf("}");
    if (nargs() != 0) {
      if (nargs() > 1) {
        for (auto i=0; i<nargs(); i++) {
          printf(" %s(%d)", name().c_str(), i);
        }
      } else if (nargs() == 1){
        printf(" %s", name().c_str());
      } else {
        printf(" %s...", name().c_str());
      }
    }
    printf("] ");
  }

  void
  option::explain(void) const
  {
    printf("  ");
    show_option();
    if (nargs() != 0) {
      if (nargs() > 1) {
        printf(" [%s(%d):%s", name().c_str(), 0, describe_type());
        for (auto i=1; i<nargs(); i++) {
          printf(",%s(%d):%s", name().c_str(), i, describe_type());
        }
      } else if (nargs() == 1) {
        printf(" [%s:%s", name().c_str(), describe_type());
      } else if (nargs() == variable_args) {
        printf(" [%s:%s,...", name().c_str(), describe_type());
      }
      printf("]");
    }
    printf(":\n");
    if (comment().size()>0) {
      size_t n = 0;
      int64_t N = comment().size();
      for (auto i=0; i<N; i++) {
        if (n==0) {
          printf("        ");
          n += 8;
        }
        printf("%c", comment()[i]);
        n++;
        if (n==80) { n=0; printf("\n"); }
      }
      if (n!=0) printf("\n");
    }
  }

  typedef std::vector<value> values;
  typedef std::pair<arg,values> argument;

  class argparse {
  public:
    argparse(int nargs, char** argv, arg desc="")
      : argparse((const int)nargs, (const char**)argv, desc)
    { }
    argparse(const int nargs, const char** argv, arg desc="")
      : _description(desc),_completed(false),_varargs(false),_appname(argv[0])
    {
      for (int i=1; i<nargs; i++)
        _arguments.push_back(arg(argv[i]));
    }
    void display_status(void) const;

    void format(void) const;
    void explain(void) const;
    void show_help(const bool) const;
    void parse(void);

    void set_description(const arg& desc)
    { _description = desc; }

    template <class T>
    const std::vector<T> get(const arg&) const;

    const bool find(const arg& s) const
    { return (_map.find(s) != _map.end()); }

    void add_argument(const arg& name, const value_type type)
    { add_argument(name, type, 1, ""); }
    void add_argument(const arg& name, const value_type type,
                      const arg& com)
    { add_argument(name, type, 1, com); }
    void add_argument(const arg& name, const value_type type,
                      const int16_t n, const arg& com="") {
      if (_varargs)
        throw std::runtime_error("cannot add any argument after varargs.");
      if (n<0) _varargs = true;
      _parse_parameters.push_back(parameter(name, type, n, com));
      _completed = false;
    }

    void add_option(const arg& dir, const arg& name,
                    const arg& com = "") {
      add_option(dir, name, value_type::arg_boolean, 0, com);
    }
    void add_option(const std::vector<arg>& dirs, const arg& name,
                    const arg& com = "") {
      add_option(dirs, name, value_type::arg_boolean, 0, com);
    }
    void add_option(const arg& dir, const arg& name,
                    const value_type t, const arg& com="") {
      add_option(dir, name, t, 1, com);
    }
    void add_option(const std::vector<arg>& dirs, const arg& name,
                    const value_type t, const arg& com="") {
      add_option(dirs, name, t, 1, com);
    }
    void add_option(const arg& dir,
                    const arg& name, const value_type type,
                    const int16_t n, const arg& com="") {
      if (_varargs)
        throw std::runtime_error("cannot add any argument after varargs.");
      if (n<0) _varargs = true;
      _parse_options.push_back(option(dir, name, type, n, com));
      _completed = false;
    }
    void add_option(const std::vector<arg>& dirs,
                    const arg& name, const value_type type,
                    const int16_t n, const arg& com="") {
      if (_varargs)
        throw std::runtime_error("cannot add any argument after varargs.");
      if (n<0) _varargs = true;
      _parse_options.push_back(option(dirs, name, type, n, com));
      _completed = false;
    }
  private:
    arg _description;
    bool _completed, _varargs;
    int32_t _nargs;
    std::string _appname;
    std::map<arg, values> _map;
    std::vector<arg> _arguments;
    std::vector<parameter> _parse_parameters;
    std::vector<option> _parse_options;
  };


  void
  argparse::format(void) const
  {
    printf("%s ", _appname.c_str());
    for (auto o : _parse_options) o.format();
    for (auto p : _parse_parameters) p.format();
    printf("\n");
  }

  void
  argparse::explain(void) const
  {
    if (_parse_parameters.size()>0) {
      printf("\nArguments\n");
      for (auto p : _parse_parameters) p.explain();
    }
    if (_parse_options.size()>0) {
      printf("\nOptions\n");
      for (auto o : _parse_options) o.explain();
    }
  }

  void
  argparse::show_help(const bool simple = true) const
  {
    if (_description.size() > 0)
      printf("%s\n\n", _description.c_str());
    printf("usage:\n  ");
    format();
    if (!simple) explain();
  }

  void
  argparse::display_status(void) const
  {
    printf("# input arguments:");
    for (auto s : _arguments) printf(" %s", s.c_str());
    printf("\n");
    printf("# defined options: ");
    for (auto o : _parse_options) o.format();
    printf("\n");
    printf("# named arguments: ");
    for (auto p : _parse_parameters) p.format();
    printf("\n");
    printf("# parsed arguments:\n");
    for (auto m : _map) {
      auto& name = m.first;
      auto& val  = m.second;
      printf("    %s:", name.c_str());
      for (auto v : val) {
        switch (v.type()) {
        case value_type::arg_null:
          printf(" null"); break;
        case value_type::arg_boolean:
          printf(" %s", (v.get<bool>()==true?"true":"false")); break;
        case value_type::arg_integer:
          printf(" %ld", v.get<int64_t>()); break;
        case value_type::arg_float:
          printf(" %lf", v.get<double>()); break;
        case value_type::arg_string:
          printf(" %s", v.get<arg>().c_str()); break;
        }
      }
      printf("\n");
    }
  }

  void
  argparse::parse(void)
  {
    std::vector<arg> _remaining;

    {
      auto vp = _arguments.begin();
      while (vp != _arguments.end()) {
        bool _updated(false);
        for (auto o : _parse_options) {
          const auto& size = o.nargs();
          const auto& name = o.name();
          const auto& type = o.type();
          const auto& vtrue = values{value(value_type::arg_boolean, "true")};
          values v;

          if (vp == _arguments.end()) break;
          if (o==*vp) {
            _updated = true;
            vp++;
            if (size == 0) {
              _map.insert(argument(name,vtrue));
            } else if (size >= 1) {
              for (auto i=0; i<size; i++) {
                v.push_back(value(type, *vp)); vp++;
              }
              _map.insert(argument(name,v));
            } else if (size == variable_args) {
              while (vp != _arguments.end()) {
                v.push_back(value(type, *vp)); vp++;
              }
              _map.insert(argument(name,v));
            }
          }
        }

        if (!_updated) {
          _remaining.push_back(*vp);
          vp++;
        }
      }
    }
    {
      auto vp = _remaining.begin();
      auto ip = _parse_parameters.begin();
      while (ip != _parse_parameters.end()) {
        if (vp == _remaining.end())
          throw std::runtime_error("insufficient number of arguments");

        const auto& size = ip->nargs();
        const auto& name = ip->name();
        const auto& type = ip->type();
        values v;

        if (size >= 1) {
          for (auto i=0; i<size; i++) {
            if (vp == _remaining.end())
              throw std::runtime_error("insufficient number of arguments");
            v.push_back(value(type, *vp)); vp++;
          }
        } else if (size == variable_args) {
          while (vp != _remaining.end()) {
            v.push_back(value(type, *vp)); vp++;
          }
        }
        _map.insert(argument(name,v));
        ip++;
      }
    }
    _completed = true;
  }

  template <class T>
  const std::vector<T>
  argparse::get(const arg& s) const
  {
    if (!_completed)
      throw std::runtime_error("arguments are not parsed.");

    std::vector<T> retval;
    if (_map.find(s) == _map.end())
      throw std::runtime_error("argument not found.");
    auto& varr = _map.at(s);
    for (auto& p : varr)
      retval.push_back(p.get<T>());
    return retval;
  }
  template const std::vector<bool>  argparse::get(const arg&) const;
  template const std::vector<int64_t>  argparse::get(const arg&) const;
  template const std::vector<double>  argparse::get(const arg&) const;
  template const std::vector<arg>  argparse::get(const arg&) const;
}

#endif
