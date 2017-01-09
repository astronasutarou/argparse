# Argparse
This repository contains a header file that provides classes to parse command-line arguments in C++ in a similar manner in Python's argparse library.

## How to Use
Copy `argparse.h` into your project directory and include it. Remember that the code should be compiled with the C++11-mode. The following code is a sample program which require one integer argument.

``` c++
#include "argparse.h"
using argparse::value_type;

int
main(int argc, char** argv)
{
  argparse::argparse parser(argc, argv, "Sample");
  parser.add_argument("arg1", value_type::arg_integer, "store integer.");
  parser.add_option("-h", "help", "show this help message.");

  try {
    parser.parse();
  } catch (std::exception& e) {
    if (parser.find("help")) {
      parser.show_help(false);
      exit(0);
    } else {
      parser.show_help();
      std::cerr << std::endl << e.what() << std::endl;
      exit(1);
    }
  }

  auto v_arg1 = parser.get<int64_t>("arg1");
  for (auto& v: v_arg1) {
    std::cout << "  arg1: " << v << std::endl;
  }
  return 0;
}
```


``` sh
g++ -std=c++11 -o sample sample.cc

./sample 1
#  arg1: 1

./sample a
#Sample
#
#usage:
#  ./sample [-h] arg1
#
#  value is not convertible to integer-type

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
The codes in this repository are licensed under a [Creative Commons Attribution-ShareAlike 4.0 International License](http://creativecommons.org/licenses/by-sa/4.0/).
