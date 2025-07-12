# Large Integer Arithmetic Expression - ADS Lab 1

## Overview

This repository contains the implementation for Lab 1 of the Advanced Data Structures (ADS) course: Large Integer Arithmetic Expression. The main goal of this project is to compute arithmetic expressions involving large integers that exceed the default data types' capacity in most programming languages.

## Features

- **Support for Large Integers:** Handles arithmetic operations for integers larger than standard data types (`int`, `long`, etc.).
- **Expression Evaluation:** Parses and evaluates arithmetic expressions including addition, subtraction, multiplication, and division.
- **Input Validation:** Checks for valid input expressions and provides error messages for invalid syntax or unsupported operations.
- **Efficient Algorithms:** Utilizes efficient data structures and algorithms to optimize computation for large numbers.

## Getting Started

### Prerequisites

- A C++ compiler (e.g., GCC, Clang, MSVC)
- Basic knowledge of C++ programming
- C++ Standard Library

### Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/Nguyen-Lam-Minh-Hoa/-ADS-Lab-1-Large-Integer-Arithmetic-Expression.git
   cd -ADS-Lab-1-Large-Integer-Arithmetic-Expression
   ```

2. **Compile the source code:**
   ```bash
   g++ -o largeint main.cpp
   ```

3. **Run the program:**
   ```bash
   ./largeint
   ```

## Usage

- **Input:** Enter arithmetic expressions with large integers (e.g., `12345678901234567890 + 98765432109876543210`).
- **Output:** The program will display the computed result.

Example:
```
Input: 99999999999999999999 * 88888888888888888888
Output: 8888888888888888888711111111111111111112
```

## Algorithms

- **BigInteger Class:** Implements addition, subtraction, multiplication, and division for large integers using arrays or vectors to store digits.
- **Expression Parsing:** Uses stack-based parsing or recursive descent to evaluate arithmetic expressions.

## Contributing

Contributions are welcome! Please fork the repository, create a pull request, and describe your changes clearly.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Author

- [Nguyen Lam Minh Hoa](https://github.com/Nguyen-Lam-Minh-Hoa)

## Acknowledgements

- Advanced Data Structures course materials
- Open-source C++ resources and tutorials
