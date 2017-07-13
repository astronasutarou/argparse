# Argparse in C++
This repository contains a header file that provides classes to parse command-line arguments in C++ in a similar manner in Python's argparse library.

## How to Use
Copy `argparse.h` into your project directory and include it. Remember that the code should be compiled with the C++11-mode. The following code is a sample program which require one integer argument.

``` c++
#include "argparse.h"
using argparse::value_type;

int
main(int argc, char** argv)
{
  // Create a parser object.
  //
  // - argc (int): the number of arguments.
  // - argv (char**): a pointer to command-line arguments.
  // - description (str): a brief description.
  argparse::argparse
    parser(argc, argv, "Sample program for a demonstration.");

  // Define a help option.

  // Define an argument.
  //
  // - name (str): the name of the argument.
  // - type (value_type): choose the type from the followings,
  //     arg_boolean, arg_integer, arg_float, arg_string.
  // - description (str): a brief description of the argument.
  parser.add_argument("arg1", value_type::Integer, "store integer.");

  // Define an optional argument.
  //
  // - option (str): the flag name of the option.
  // - name (str): the name of the option.
  // - description (str): a brief description of the option.
  parser.add_option("-v", "varg", value_type::Integer,
                    argparse::variable_args, "store variable args.");
  parser.add_option("-h", "help", "show this help message.");

  // Parse the command-line arguments.
  // If failed, display a help and exit.
  parser.parse();

  // Display the current status of the argument parser for debugging.
  // parser.display_status();

  // The parsed argument is available via get() method.
  // get() method always return the first element.
  // std::runtime_error is returned if an undefined item is requested.
  auto arg1 = parser.get<int32_t>("arg1");
  std::cout << "       arg1: " << arg1 << std::endl;

  // A default value can be specified in get() method.
  // The default value is returned if an undefined item is requested.
  auto undef = parser.get<int32_t>("undefined", 9999);
  std::cout << "  undefined: " << undef << std::endl;

  // getall() method is available to get multiple items.
  // The argument is always given as a std::vector<T> object.
  if (parser.find("varg")) {
    auto varg = parser.getall<int32_t>("varg");
    for (auto& v: varg)
      std::cout << "       varg: " << v << std::endl;
  }

  return 0;
}
```

The compiled program accepts a single integer. Additional arguments are ignored. Conversion from a string to an integer is carried out with `std::itol`. When a given argument is not convertible into the specified value-type, the parser returns an exception. No scientific numeric expressions like `3.2e3` are accepted. The `show_help()` method print a help message that is automatically generated.

``` sh
g++ -std=c++11 -o sample sample.cc

./sample 1
#        arg1: 1
#   undefined: 9999

./sample 1 2
#        arg1: 1
#   undefined: 9999

./sample 1 -v 1 2 3
#        arg1: 1
#   undefined: 9999
#        varg: 1
#        varg: 2
#        varg: 3

./sample a
# Sample program for a demonstration.
#
# usage:
#
# error: value is not convertible to integer-type
# ./hoge [-h] arg1 [-v varg...]

./sample -h
# Sample program for a demonstration.
#
# usage:
#   ./hoge [{-h|--help}] arg1 [-v varg...]
#
# Arguments
#   arg1 [integer]:
#         store integer.
#
# Options
#   -h|--help:
#         Show a help message
#   -v [varg:integer,...]:
#         store variable args.
```


## License
The codes in this repository are licensed under the [MIT License](https://opensource.org/licenses/mit-license.php).
