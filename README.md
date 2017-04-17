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
  argparse::argparse parser(argc, argv, "Sample");

  // Define an argument.
  //
  // - name (str): the name of the argument.
  // - type (value_type): choose the type from the followings,
  //     arg_boolean, arg_integer, arg_float, arg_string.
  // - description (str): a brief description of the argument.
  parser.add_argument("arg1", value_type::arg_integer, "store integer.");

  // Define an optional argument.
  //
  // - option (str): the flag name of the option.
  // - name (str): the name of the option.
  // - description (str): a brief description of the option.
  parser.add_option("-h", "help", "show this help message.");

  // Parse the command-line arguments.
  try {
    parser.parse();
    if (parser.find("help")) {
      parser.show_help(false); // show a full help message.
      exit(0);
    }
  } catch (std::exception& e) {
    if (parser.find("help")) {
      parser.show_help(false); // show a full help message.
      exit(0);
    } else {
      parser.show_help();      // show a brief usage message.
      std::cerr << std::endl << e.what() << std::endl;
      exit(1);
    }
  }

  // The parsed argument is available via get() method.
  // The argument is always given as a std::vector<T> object.
  auto v_arg1 = parser.get<int64_t>("arg1");
  for (auto& v: v_arg1) {
    std::cout << "  arg1: " << v << std::endl;
  }

  return 0;
}
```

The compiled program accepts a single integer. Additional arguments are ignored. Conversion from a string to an integer is carried out with `std::itol`. When a given argument is not convertible into the specified value-type, the parser returns an exception. No scientific numeric expressions like `3.2e3` are accepted. The `show_help()` method print a help message that is automatically generated.

``` sh
g++ -std=c++11 -o sample sample.cc

./sample 1
#  arg1: 1

./sample 1 2
#  arg1: 1

./sample a
# Sample
#
# usage:
#   ./sample [-h] arg1
#
#   value is not convertible to integer-type

./sample -h
# Sample
#
# usage:
#   ./sample [-h] arg1
#
# Arguments
#   arg1 [integer]:
#         store integer.
#
# Options
#   -h:
#         show this help message.
```


## License
The codes in this repository are licensed under the [MIT License](https://opensource.org/licenses/mit-license.php).
