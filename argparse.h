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

/** Argument Parser Namespace */
namespace argparse {
  using std::regex_constants::icase;

  /** Indicator of "Variable Arguments" */
  constexpr int16_t variable_args = -1;

  /** String element */
  typedef std::string arg;
  /** An array of string elements */
  typedef std::vector<argparse::arg> args;

  /**
   * @brief Types of values recognized by Argument Parser.
   */
  enum class value_type {
    Null,    /**< Default type (should not be used) */
    Bool,    /**< Boolean type */
    Integer, /**< Integer type */
    Float,   /**< Float type */
    String   /**< String type */
  };

  /**
   * @brief A container of an element.
   *
   * This class provides a container of an element. A value of the
   * container is assiened by a string with its type. The value is
   * automatically converted form the string into the assigned type
   * when the value is requested.
   */
  class value {
  public:
    /**
     * @brief Initialize a container with a type and an empty value
     * @param[in] type The type of a value.
     * @exception std::runtime_error is thrown if the type is wrong.
     */
    value(const value_type type): value(t, "") { }
    /**
     * @brief Initialize a container with a type and an element
     * @param[in] type The type of value.
     * @param[in] s The value in a form of C++-type string.
     * @exception std::runtime_error is thrown if the type is wrong.
     */
    value(const value_type type, const arg& s): _type(type),_value(s)
    { assert_argument_type(); }

    /**
     * @brief Return the current `value_type`.
     * @return The current `value_type`.
     */
    const value_type& type(void) const { return _type; }

    /**
     * @brief Return the current `value_type` as a text.
     * @return A C-type text explaining the current `value_type`.
     * @exception std::runtime_error is thrown if the type is wrong.
     */
    const char* describe_type(void) const;

    /**
     * @brief Assign a value from a C++-type string.
     * @param[in] s An input element in a C++-type string.
     * @return The reference to this instance.
     * @exception std::runtime_error is thrown if the type is wrong.
     */
    value& operator=(const arg& s) {
      _value = s;
      assert_argument_type();
      return *this;
    }
    /**
     * @brief Assign a value from a C-type string.
     * @param[in] c An input element in a C-type string.
     * @return The reference to this instance.
     * @exception std::runtime_error is thrown if a wrong type of
     * element is assigned.
     */
    value& operator=(const char* c) {
      _value = arg(c);
      assert_argument_type();
      return *this;
    }

    /**
     * @brief Obtain a value converted in a requested type.
     * @tparam T A type of the element.
     *
     * @note Acceptable types are as follows: `bool`, `int16_t`, `int32_t`,
     * `int64_t`, `uint16_t`, `uint32_t`, `uint64_t`, `float`, `double`,
     * and `std::string`.
     * @exception std::runtime_error is thrown if the type is wrong.
     */
    template <class T> const T get(void) const;
  private:
    value_type _type;  /**< The type of the value */
    arg _value;        /**< The value in a C++-type string */
    /** Check wheather the value is convertible to the requested type  */
    void assert_argument_type(void) const;
    /** Convert the value into Bool */
    const bool convert_bool(void) const;
    /** Convert the value into Integer */
    const int64_t convert_integer(void) const;
    /** Convert the value into Float */
    const double convert_float(void) const;
    /** Convert the value into String */
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
    case value_type::Integer : return "integer"; break;
    case value_type::Float   : return "float"; break;
    case value_type::String  : return "string"; break;
    default: throw std::runtime_error("wrong argument type.");
    }
  }

  void
  value::assert_argument_type(void) const
  {
    switch (_type) {
    case value_type::Null :
      throw std::runtime_error("argument type is null.");
      break;
    case value_type::Bool :
      convert_bool();
      break;
    case value_type::Integer :
      convert_integer();
      break;
    case value_type::Float   :
      convert_float();
      break;
    case value_type::String  :
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

  /**
   * @brief An abstract class for the definitions of the argument classes.
   */
  class abstract_argument {
  public:
    /**
     * @brief Initialize the argument instance.
     * @param[in] name The name of the instance.
     * @param[in] type The type of arguments.
     * @param[in] n The number of arguments.
     * @param[in] com The description of the instance.
     */
    abstract_argument(const arg& name, const value_type type,
                      const int16_t n, const arg& com)
      : _type(type),_nargs(n),_name(name),_comment(com) {}

    /**
     * @brief Return the current `value_type`.
     * @return The current `value_type`.
     */
    const value_type& type(void) const { return _type; }

    /**
     * @brief Return the name of this instance.
     * @return The name of the instance.
     */
    const arg& name(void) const { return _name; }

    /**
     * @brief Return the number of the arguments in the instance.
     * @return The number of the arguments.
     */
    const int16_t& nargs(void) const { return _nargs; }

    /**
     * @brief Return the description of the instance.
     * @return The description of the instance.
     */
    const arg& comment(void) const { return _comment; }

    /**
     * @brief Return the current `value_type` as a text.
     * @return A C-type text explaining the current `value_type`.
     */
    const char* describe_type(void) const = 0;

    /**
     * @brief Check the equality of two instances.
     * @return Zero if the instances are identical.
     */
    virtual int operator==(const arg&) const = 0;
    /**
     * @brief Display a format of the instance.
     * @param[in] output A file descriptor for output. [default: `stdout`]
     */
    virtual void format(FILE* output) const = 0;
    /**
     * @brief Display an explanation of the instance.
     * @param[in] output A file descriptor for output. [default: `stdout`]
     */
    virtual void explain(FILE* output) const = 0;

  private:
    value_type _type; /**< The type of arguments */
    int16_t _nargs;   /**< The number of arguments */
    arg _name;        /**< The name of the instance */
    arg _comment;     /**< The description of the instance */
  };

  const char*
  abstract_argument::describe_type(void) const
  {
    switch (_type) {
    case value_type::Integer : return "integer"; break;
    case value_type::Float   : return "float"; break;
    case value_type::String  : return "string"; break;
    default: throw std::runtime_error("wrong argument type.");
    }
  }

  /**
   * @brief A definition of a positional argument.
   *
   * This class provides an instance for a positional argument. Multiple
   * and variable number of elements are acceptable for this instance.
   * The elements are stored in an array of argparse::value's.
   */
  class positional_argument : public abstract_argument {
  public:
    /**
     * @brief Create an instance for multiple elements without a comment.
     * @param[in] name The name of the instance.
     * @param[in] type The type of elements.
     * @param[in] n The number of elements.
     * @exception std::runtime_error is thrown if the type is wrong.
     */
    positional_argument(const arg& name, const value_type type,
                        const int16_t n=1)
      : abstract_argument(name, t, n, "") {}
    /**
     * @brief Create an instance for multiple elements with a comment.
     * @param[in] name The name of the instance.
     * @param[in] type The type of elements.
     * @param[in] n The number of elements.
     * @param[in] com A line of description of the instance.
     * @exception std::runtime_error is thrown if the type is wrong.
     */
    positional_argument(const arg& name, const value_type type,
                        const int16_t n, const arg& com)
      : abstract_argument(name, t, n, com) {}
    /**
     * @brief Create an instance for a single element with a comment.
     * @param[in] name The name of the instance.
     * @param[in] type The type of elements.
     * @param[in] com A line of description of the instance.
     * @exception std::runtime_error is thrown if the type is wrong.
     */
    positional_argument(const arg& name, const value_type type,
                        const arg& com)
      : abstract_argument(name, t, 1, com) {}

    /**
     * @brief Check the equality of two argparse::positional_argument's.
     * @return Zero if the instances are identical.
     *
     * @note Instances which have the same name are recognized as the
     * identical. The element's values in the instances are not checked.
     */
    int operator==(const arg& str) const { return name().compare(str); }

    /**
     * @brief Display a format of the instance.
     * @param[in] output A file descriptor for output. [default: `stdout`]
     */
    void format(FILE* output=stdout) const;

    /**
     * @brief Display an explanation of the instance.
     * @param[in] output A file descriptor for output. [default: `stdout`]
     */
    void explain(FILE* output=stdout) const;
  private:
  };

  void
  positional_argument::format(FILE* output=stdout) const
  {
    if (nargs() != 0) {
      if (nargs() > 1) {
        fprintf(output,"%s(%d)", name().c_str(), 0);
        for (auto i=1; i<nargs(); i++) {
          fprintf(output," %s(%d)", name().c_str(), i);
        }
      } else if (nargs() == 1){
        fprintf(output,"%s", name().c_str());
      } else {
        fprintf(output,"%s...", name().c_str());
      }
    }
    fprintf(output," ");
  }

  void
  positional_argument::explain(FILE* output=stdout) const
  {
    fprintf(output,"  %s [%s", name().c_str(), describe_type());
    for (auto i=1; i<nargs(); i++)
      fprintf(output,",%s", describe_type());
    if (nargs() == variable_args) fprintf(output,",...");
    fprintf(output,"]:\n");
    if (comment().size()>0) {
      size_t n = 0;
      int64_t N = comment().size();
      for (auto i=0; i<N; i++) {
        if (n==0) {
          fprintf(output,"        ");
          n += 8;
        }
        fprintf(output,"%c", comment()[i]);
        n++;
        if (n==80) { n=0; fprintf(output,"\n"); }
      }
      if (n!=0) fprintf(output,"\n");
    }
  }

  /**
   * @brief A definition of an option.
   *
   * This class provides an instance for an option. Multiple and variable
   * number of optional elements are acceptable for this instance.
   * The elements are stored in an array of argparse::value's.
   */
  class optional_argument : public abstract_argument {
  public:
    /**
     * @brief Create an instance for a boolean option.
     * @param[in] dir The directive string of the option.
     * @param[in] name The name of the option.
     * @param[in] com The description of the option.
     * @exception std::runtime_error is thrown if the type is wrong.
     */
    optional_argument(const arg& dir, const arg& name, const arg& com = "")
      : optional_argument(dir, name, value_type::Bool, 0, com)
    { }

    /**
     * @brief Create an instance for an boolean option.
     * @param[in] dirs The directive strings of the option.
     * @param[in] name The name of the option.
     * @param[in] com The description of the option.
     * @exception std::runtime_error is thrown if the type is wrong.
     */
    optional_argument(const std::vector<arg>& dirs,
                      const arg& name, const arg& com = "")
      : optional_argument(dirs, name, value_type::Bool, 0, com)
    { }

    /**
     * @brief Create an instance for an option with elements.
     * @param[in] dir The directive string of the option.
     * @param[in] name The name of the option.
     * @param[in] type The type of elements.
     * @param[in] n The number of elements.
     * @exception std::runtime_error is thrown if the type is wrong.
     */
    optional_argument(const arg& dir, const arg& name,
                      const value_type type, const int16_t n = 1)
      : optional_argument(dir, name, t, n, "")
    { }

    /**
     * @brief Create an instance for an option with elements.
     * @param[in] dirs The directive strings of the option.
     * @param[in] name The name of the option.
     * @param[in] type The type of elements.
     * @param[in] n The number of elements.
     * @exception std::runtime_error is thrown if the type is wrong.
     */
    optional_argument(const std::vector<arg>& dirs,
           const arg& name, const value_type type, const int16_t n = 1)
      : optional_argument(dirs, name, t, n, "")
    { }

    /**
     * @brief Create an instance for an option with elements.
     * @param[in] dir The directive string of the option.
     * @param[in] name The name of the option.
     * @param[in] type The type of elements.
     * @param[in] n The number of elements.
     * @param[in] com The description of the option.
     * @exception std::runtime_error is thrown if the type is wrong.
     */
    optional_argument(const arg& dir,
           const arg& name, const value_type type, const int16_t n,
           const arg& com)
      : _type(type),_nargs(n),_name(name),_comment(com)
    { _optseqs.push_back(dir); }

    /**
     * @brief Create an instance for an option with elements.
     * @param[in] dirs The directive strings of the option.
     * @param[in] name The name of the option.
     * @param[in] type The type of elements.
     * @param[in] n The number of elements.
     * @param[in] com The description of the option.
     * @exception std::runtime_error is thrown if the type is wrong.
     */
    optional_argument(const std::vector<arg>& dirs,
           const arg& name, const value_type type, const int16_t n,
           const arg& com)
      : _type(type),_nargs(n),_name(name),_comment(com)
    { for (auto d : dirs) _optseqs.push_back(d); }

    /**
     * @brief Return the list of the directives.
     */
    const std::vector<arg>& options(void) const { return _optseqs; }

    /**
     * @brief Check the equality of two argparse::optional_argument's.
     * @return Zero if the instances are identical.
     *
     * @note Instances which have the same directives are recognized
     * as the identical. The values in the instances are not checked.
     */
    bool operator==(const arg& str) const {
      for (auto d : _optseqs)
        if (d.compare(str) == 0) return true;
      return false;
    }

    /**
     * @brief Display a format of the instance.
     * @param[in] output A file descriptor for output. [default: `stdout`]
     */
    void format(FILE* output=stdout) const;

    /**
     * @brief Display an explanation of the instance.
     * @param[in] output A file descriptor for output. [default: `stdout`]
     */
    void explain(FILE* output=stdout) const;
  private:
    std::vector<arg> _optseqs;     /**< The list of the directives */

    /** A help function to display the usage */
    void show_option(void) const;
  };

  void
  optional_argument::show_option(void) const
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
  optional_argument::format(FILE* output=stdout) const
  {
    fprintf(output,"[");
    if (_optseqs.size()>1) fprintf(output,"{");
    show_option();
    if (_optseqs.size()>1) fprintf(output,"}");
    if (nargs() != 0) {
      if (nargs() > 1) {
        for (auto i=0; i<nargs(); i++) {
          fprintf(output," %s(%d)", name().c_str(), i);
        }
      } else if (nargs() == 1){
        fprintf(output," %s", name().c_str());
      } else {
        fprintf(output," %s...", name().c_str());
      }
    }
    fprintf(output,"] ");
  }

  void
  optional_argument::explain(FILE* output=stdout) const
  {
    fprintf(output,"  ");
    show_option();
    if (nargs() != 0) {
      if (nargs() > 1) {
        fprintf(output," [%s(%d):%s", name().c_str(), 0, describe_type());
        for (auto i=1; i<nargs(); i++) {
          fprintf(output,",%s(%d):%s", name().c_str(), i, describe_type());
        }
      } else if (nargs() == 1) {
        fprintf(output," [%s:%s", name().c_str(), describe_type());
      } else if (nargs() == variable_args) {
        fprintf(output," [%s:%s,...", name().c_str(), describe_type());
      }
      fprintf(output,"]");
    }
    fprintf(output,":\n");
    if (comment().size()>0) {
      size_t n = 0;
      int64_t N = comment().size();
      for (auto i=0; i<N; i++) {
        if (n==0) {
          fprintf(output,"        ");
          n += 8;
        }
        fprintf(output,"%c", comment()[i]);
        n++;
        if (n==80) { n=0; fprintf(output,"\n"); }
      }
      if (n!=0) fprintf(output,"\n");
    }
  }

  /** An array of argparse::value's */
  typedef std::vector<value> values;
  /** A pair of argparse::arg and argparse::values */
  typedef std::pair<arg,values> argument;

  /**
   * @brief An argument parser class
   */
  class argparse {
  public:
    /**
     * @brief Create an argument parser instance.
     * @param[in] nargs `nargs` given in the main function.
     * @param[in] argv `argv` given in the main function.
     * @param[in] desc The description of the main program.
     */
    argparse(const int nargs, const char** argv, arg desc="")
      : _description(desc),_completed(false),_varargs(false),_appname(argv[0])
    {
      for (int i=1; i<nargs; i++)
        _arguments.push_back(arg(argv[i]));
    }
    /**
     * @brief Create an argument parser instance.
     * @param[in] nargs `nargs` given in the main function.
     * @param[in] argv `argv` given in the main function.
     * @param[in] desc The description of the main program.
     */
    argparse(int nargs, char** argv, arg desc="")
      : argparse((const int)nargs, (const char**)argv, desc)
    { }

    /**
     * @brief Parse the input arguments and store values.
     */
    void parse(const bool help_on_error = true,
               const bool show_help_and_exit = true);

    /**
     * @brief Show the current status of the parser.
     * @param[in] output A file descriptor for output. [default: `stdout`]
     */
    void display_status(FILE* output=stdout) const;

    /**
     * @brief List the formats of the registered arguments.
     * @param[in] output A file descriptor for output. [default: `stdout`]
     */
    void format(FILE* output=stdout) const;

    /**
     * @brief List the descriptions of the registered arguments.
     * @param[in] output A file descriptor for output. [default: `stdout`]
     */
    void explain(FILE* output=stdout) const;

    /**
     * @brief Display the auto-configured help message.
     * @param[in] output A file descriptor for output. [default: `stdout`]
     * @param[in] simple A simplified help message is displayed if true.
     */
    void show_help(FILE* output=stdout, const bool simple=false) const;

    /**
     * @brief Update the description of the program.
     * @param[in] desc The description of the program.
     */
    void set_description(const arg& desc)
    { _description = desc; }

    /**
     * @brief Obtain all the values associated with the given name.
     * @param[in] name The name of the positional or optional argument.
     * @return An array of the arguments.
     * @exception std::runtime_error is thrown when the name is not found.
     */
    template <class T>
    const std::vector<T> getall(const arg& name) const;

    /**
     * @brief Obtain all the values associated with the given name.
     * @param[in] name The name of the positional or optional argument.
     * @param[in] dummy A dummy value returned if an argument is not found.
     * @return An array of the arguments.
     * @note When the name is not found, an array which contains a `dummy`
     * value is returned.
     */
    template <class T>
    const std::vector<T> getall(const arg& name, const T& dummy) const;

    /**
     * @brief Obtain the first value associated with the given name.
     * @param[in] name The name of the positional or optional argument.
     * @return The first element of the arguments.
     * @exception std::runtime_error is thrown when the name is not found.
     * @note Only the first element will be returned even if there are
     * multiple items stored in the parameter instance.
     */
    template <class T>
    const T get(const arg& name) const;
    /**
     * @brief Obtain the first value associated with the given name.
     * @param[in] name The name of the positional or optional argument.
     * @param[in] dummy A dummy value returned if an argument is not found.
     * @return The first element of the arguments.
     * @note When the name is not found, a `dummy` value is returned.
     * @note Only the first element will be returned even if there are
     * multiple items stored in the parameter instance.
     */
    template <class T>
    const T get(const arg& name, const T& dummy) const;

    /**
     * @brief Check an optional argument is defined or not.
     * @param[in] name The name of the argument in question.
     */
    const bool find(const arg& name) const
    { return (_map.find(name) != _map.end()); }

    /**
     * @brief Add a positional argument with an element without a comment.
     * @param[in] name The name of the argument.
     * @param[in] type The type of the elements.
     */
    void add_argument(const arg& name, const value_type type)
    { add_argument(name, type, 1, ""); }
    /**
     * @brief Add a positional argument with an element with a comment.
     * @param[in] name The name of the argument.
     * @param[in] type The type of elements.
     * @param[in] com The description of the argument.
     */
    void add_argument(const arg& name, const value_type type,
                      const arg& com)
    { add_argument(name, type, 1, com); }
    /**
     * @brief Add a positional argument with multiple elements with a comment.
     * @param[in] name The name of the argument.
     * @param[in] type The type of elements.
     * @param[in] n The number of elements.
     * @param[in] com The description of the argument.
     * @exception std::runtime_error
     */
    void add_argument(const arg& name, const value_type type,
                      const int16_t n, const arg& com="") {
      if (_varargs)
        throw std::runtime_error("cannot add any argument after varargs.");
      if (n<0) _varargs = true;
      _parse_parameters.push_back(positional_argument(name, type, n, com));
      _completed = false;
    }

    /**
     * @brief Add an optional switch argument.
     * @param[in] dir The directive string of the argument.
     * @param[in] name The name of the elements.
     * @param[in] com The description of the argument.
     */
    void add_option(const arg& dir, const arg& name,
                    const arg& com = "") {
      add_option(dir, name, value_type::Bool, 0, com);
    }
    /**
     * @brief Add an optional switch argument.
     * @param[in] dirs The directive strings of the argument.
     * @param[in] name The name of the elements.
     * @param[in] com The description of the argument.
     */
    void add_option(const std::vector<arg>& dirs, const arg& name,
                    const arg& com = "") {
      add_option(dirs, name, value_type::Bool, 0, com);
    }
    /**
     * @brief Add an optional argument with a element.
     * @param[in] dir The directive string of the argument.
     * @param[in] name The type of the argument.
     * @param[in] type The type of elements.
     * @param[in] com The description of the argument.
     */
    void add_option(const arg& dir, const arg& name,
                    const value_type type, const arg& com="") {
      add_option(dir, name, t, 1, com);
    }
    /**
     * @brief Add an optional argument with a element.
     * @param[in] dirs The directive strings of the argument.
     * @param[in] name The type of the argument.
     * @param[in] type The number of elements.
     * @param[in] com The description of the argument.
     */
    void add_option(const std::vector<arg>& dirs, const arg& name,
                    const value_type type, const arg& com="") {
      add_option(dirs, name, t, 1, com);
    }
    /**
     * @brief Add an optional argument with elements.
     * @param[in] dir The directive string of the argument.
     * @param[in] name The type of the argument.
     * @param[in] type The type of elements.
     * @param[in] n The number of elements.
     * @param[in] com The description of the argument.
     */
    void add_option(const arg& dir,
                    const arg& name, const value_type type,
                    const int16_t n, const arg& com="") {
      if (_varargs)
        throw std::runtime_error("cannot add any argument after varargs.");
      if (n<0) _varargs = true;
      _parse_options.push_back(optional_argument(dir, name, type, n, com));
      _completed = false;
    }
    /**
     * @brief Add an optional argument with elements.
     * @param[in] dirs The directive strings of the argument.
     * @param[in] name The name of the argument.
     * @param[in] type The type of elements.
     * @param[in] n The number of elements.
     * @param[in] com The description of the argument.
     */
    void add_option(const std::vector<arg>& dirs,
                    const arg& name, const value_type type,
                    const int16_t n, const arg& com="") {
      if (_varargs)
        throw std::runtime_error("cannot add any argument after varargs.");
      if (n<0) _varargs = true;
      _parse_options.push_back(optional_argument(dirs, name, type, n, com));
      _completed = false;
    }
  private:
    arg _description;             /**< The description of the application */
    bool _completed;              /**< True if `parse` is successfully done */
    bool _varargs;                /**< True if vararg is defined */
    int32_t _nargs;               /**< The number of arguments */
    std::string _appname;         /**< The name of the application */
    std::map<arg, values> _map;   /**< The map of (name, values) */
    std::vector<arg> _arguments;  /**< The array of arguments */
    std::vector<positional_argument> _parse_parameters;
    std::vector<optional_argument> _parse_options;
  };


  void
  argparse::format(FILE* output=stdout) const
  {
    fprintf(output,"%s ", _appname.c_str());
    for (auto o : _parse_options) o.format();
    for (auto p : _parse_parameters) p.format();
    fprintf(output,"\n");
  }

  void
  argparse::explain(FILE* output=stdout) const
  {
    if (_parse_parameters.size()>0) {
      fprintf(output,"\nArguments\n");
      for (auto p : _parse_parameters) p.explain();
    }
    if (_parse_options.size()>0) {
      fprintf(output,"\nOptions\n");
      for (auto o : _parse_options) o.explain();
    }
  }

  void
  argparse::show_help(FILE* output=stdout
                      const bool simple = false) const
  {
    if (_description.size() > 0)
      fprintf(output,"%s\n\n", _description.c_str());
    fprintf(output,"usage:\n  ");
    format();
    if (!simple) explain();
  }

  void
  argparse::display_status(FILE* output=stdout) const
  {
    fprintf(output,"# input arguments:");
    for (auto s : _arguments) fprintf(output," %s", s.c_str());
    fprintf(output,"\n");
    fprintf(output,"# defined options: ");
    for (auto o : _parse_options) o.format();
    fprintf(output,"\n");
    fprintf(output,"# named arguments: ");
    for (auto p : _parse_parameters) p.format();
    fprintf(output,"\n");
    fprintf(output,"# parsed arguments:\n");
    for (auto m : _map) {
      auto& name = m.first;
      auto& val  = m.second;
      fprintf(output,"    %s:", name.c_str());
      for (auto v : val) {
        switch (v.type()) {
        case value_type::Null:
          fprintf(output," null"); break;
        case value_type::Bool:
          fprintf(output," %s", (v.get<bool>()==true?"true":"false")); break;
        case value_type::Integer:
          fprintf(output," %ld", v.get<int64_t>()); break;
        case value_type::Float:
          fprintf(output," %lf", v.get<double>()); break;
        case value_type::String:
          fprintf(output," %s", v.get<arg>().c_str()); break;
        }
      }
      fprintf(output,"\n");
    }
  }

  void
  argparse::parse(const bool help_on_error = true,
                  const bool show_help_and_exit = true)
  {
    std::vector<arg> _remaining;

    try {
      {
        /**
         * At the beginning, all the optional arguments are processed.
         * When the conversion of an element is failed, it throws
         * std::runtime_error immediately.
         */
        auto vp = _arguments.begin();
        while (vp != _arguments.end()) {
          bool _updated(false);
          for (auto o : _parse_options) {
            const auto& size = o.nargs();
            const auto& name = o.name();
            const auto& type = o.type();
            const auto& vtrue = values{value(value_type::Bool, "true")};
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
        /**
         * The remaining elements are processed as positional arguments.
         * When the conversion of an element is failed, it throws
         * std::runtime_error immediately.
         */
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
      /**
       * `_completed` flag is set `true` when all the conversion is
       * successfully completed. After that `get()` and `getall()` functions
       * are enabled.
       */
      _completed = true;
    } catch (std::exception e) {
      /**
       * If `help_on_error` is set `true`, `parse()` function catches any
       * exception, displays a detailed help message, and exits.
       */
      if (help_on_error) {
        argparse::show_help(stderr, true);
        fprintf(stderr, "error: %s\n", e.what());
        exit(EXIT_FAILURE);
      }
      throw;
    }
    /**
     * If `show_help_and_exit` is set `true` and `help` argument is defined,
     * it displays a detailed help message and exits normally.
     *
     * @note The value of the `help` argument is not checked here. If you
     * defined a `help` argument as a positional argument, this function
     * always displays a help message and exits. Thus, using a positional
     * argument with the name of `help` is not recommended.
     */
    if (show_help_and_exit && argparse::get<bool>("help", false)) {
      argparse::show_help(stderr, true);
      exit(EXIT_SUCCESS);
    }

  }

  template <class T>
  T argparse::get(const arg& name) const
  { return argparse::getall<T>(name)[0]; }
  template <> const std::vector<bool>  argparse::get(const arg&) const;
  template <> const std::vector<int64_t>  argparse::get(const arg&) const;
  template <> const std::vector<double>  argparse::get(const arg&) const;
  template <> const std::vector<arg>  argparse::get(const arg&) const;

  template <class T>
  T argparse::get(const arg& name, const T& dummy) const
  {
    try {
      return argparse::getall<T>(name)[0];
    } catch (std::runtime_error e) {
      return dummy;
    }
  }
  template <> const std::vector<bool>
  argparse::get(const arg&,const T&) const;
  template <> const std::vector<int64_t>
  argparse::get(const arg&,const T&) const;
  template <> const std::vector<double>
  argparse::get(const arg&,const T&) const;
  template <> const std::vector<arg>
  argparse::get(const arg&,const T&) const;

  template <class T>
  const std::vector<T>
  argparse::getall(const arg& name) const
  {
    if (!_completed)
      throw std::runtime_error("arguments are not parsed.");

    std::vector<T> retval;
    if (_map.find(name) == _map.end())
      throw std::runtime_error("argument not found.");
    auto& varr = _map.at(name);
    for (auto& p : varr)
      retval.push_back(p.get<T>());
    return retval;
  }
  template <class T>
  const std::vector<T>
  argparse::getall(const arg& name, const T& dummy) const
  {
    if (!_completed)
      throw std::runtime_error("arguments are not parsed.");

    std::vector<T> retval;
    if (_map.find(name) == _map.end())
      return std::vector<T>({dummy});
    auto& varr = _map.at(name);
    for (auto& p : varr)
      retval.push_back(p.get<T>());
    return retval;
  }
  template <> const std::vector<bool>  argparse::getall(const arg&) const;
  template <> const std::vector<int64_t>  argparse::getall(const arg&) const;
  template <> const std::vector<double>  argparse::getall(const arg&) const;
  template <> const std::vector<arg>  argparse::getall(const arg&) const;
}

#endif
