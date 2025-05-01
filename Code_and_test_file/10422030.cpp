#include <iostream>     // cin, cout
#include <fstream>      // ifstream, ofstream
#include <string>       // string
#include <vector>       // vector
#include <stack>        // stack
#include <stdexcept>    // runtime_error
#include <cctype>       // isdigit, isspace
#include <algorithm>    // reverse, max

using namespace std;

//function prototype
int precedence(const string& op);
vector<string> tokenize(const string& expr);
vector<string> infix_postfix(const vector<string>& tokens);


// fix bug 2: 'calculate_postfix' was not declared (23/4/2025)
// BigInteger: stores and processes large integers (stage 1)
class Big_Integer {
    public:
        vector<int> digits;  // each element is 0-9, stored in reverse order (least significant digit first)
        bool negative = false; // boolean flag to indicate if the number is negative
    
        // fix bug 1: Add default constructor (21/4/2025)
        Big_Integer() {
            digits.push_back(0); // initializes the digits vector with 0
            negative = false; // initializes the negative flag to false
        }
    
        // Intitialize from string
        Big_Integer(const string &s) {
            string str = s; //create a copy of the input
            if (str.empty()) return; // return if input string is empty
            
            // checks if the first character is a minus sign
            if (str[0] == '-') {
                negative = true; // sets the negative flag to true
                str = str.substr(1); // removes the minus sign from the string
            }

            //remove leading zeros
            int i = 0; //index
            // increments i while there are leading zeros
            while (i + 1 < (int)str.size() && str[i] == '0') ++i;
            // loop through the remaining characters of the string
            for (; i < (int)str.size(); ++i) {
                // breaks the loop if a non-digit character
                if (!isdigit(str[i])) break;
                // converts the digit character to an integer and adds it to the digits vector
                digits.push_back(str[i] - '0');
            }
            // if no digits were found, add a single '0'
            if (digits.empty()) digits.push_back(0);
            reverse(digits.begin(), digits.end());  // low digit first
        }
    
        // convert to string
        string to_string() const {
            string s; // initializes an empty string
            // if negative and not zero, add a minus sign
            if (negative && !is_zero()) s.push_back('-');
            // loop through the digits vector in reverse order
            for (int i = (int)digits.size() - 1; i >= 0; --i)
            // converts each digit to its character representation and appends it to the string
                s.push_back(char('0' + digits[i]));
            return s;
        }
        
        // returns true if the number has only one digit which is 0
        bool is_zero() const {
            return digits.size() == 1 && digits[0] == 0;
        }
    
        // add absolute values
        static Big_Integer add_abs(const Big_Integer &a, const Big_Integer &b) {
            Big_Integer res("0");
            res.digits.clear(); // clears the initial digit of the result
            int carry = 0;
            // finds the maximum size of the two Big_Integers' digit vectors
            int n = max(a.digits.size(), b.digits.size());
            // loop up to the maximum size
            for (int i = 0; i < n; ++i) {
                int da = i < (int)a.digits.size() ? a.digits[i] : 0;
                int db = i < (int)b.digits.size() ? b.digits[i] : 0;
                // calculates the sum of the digits and the carry
                int sum = da + db + carry;
                res.digits.push_back(sum % 10);
                carry = sum / 10; // updates the carry for the next iteration
            }
            // if there's a remaining carry, add it to the result's digits
            if (carry) res.digits.push_back(carry);
            return res;
        }
    
        // Subtract absolute values (assuming |a| >= |b|)
        static Big_Integer sub_abs(const Big_Integer &a, const Big_Integer &b) {
            Big_Integer res("0");
            res.digits.clear();
            int carry = 0;
            int n = a.digits.size();
            // loop through the digits of 'a'
            for (int i = 0; i < n; ++i) {
                int da = a.digits[i];
                // gets the digit from 'b' at index i, or 0 if i is out of bounds
                int db = i < (int)b.digits.size() ? b.digits[i] : 0;
                int diff = da - db - carry;
                // if the difference is negative
                if (diff < 0) {
                    diff += 10;
                    carry = 1;
                } else carry = 0;
                res.digits.push_back(diff);
            }
            // remove trailing zeros
            while (res.digits.size() > 1 && res.digits.back() == 0)
                res.digits.pop_back();
            return res;
        }
    
        // Compare absolute values
        static int compare_abs(const Big_Integer &a, const Big_Integer &b) {
            // if the number of digits is different
            if (a.digits.size() != b.digits.size())
                return a.digits.size() < b.digits.size() ? -1 : 1;
            //iterate through the digits from most significant to least significant
                for (int i = a.digits.size() - 1; i >= 0; --i)
                // if the digits at the current position are different
                if (a.digits[i] != b.digits[i])
                    return a.digits[i] < b.digits[i] ? -1 : 1;
            return 0;
        }
    
        // operator "+"
        Big_Integer operator+(const Big_Integer &other) const {
            Big_Integer res;
            // if both numbers have the same sign
            if (negative == other.negative) {
                res = add_abs(*this, other);
                res.negative = negative;
            } else { // if the numbers have different signs
                int cmp = compare_abs(*this, other);
                if (cmp >= 0) {
                    res = sub_abs(*this, other);
                    res.negative = negative;
                } else {
                    res = sub_abs(other, *this);
                    res.negative = other.negative;
                }
            }
            // zero is neither positive nor negative
            if (res.is_zero()) res.negative = false;
            return res;
        }
    
        // operator "-"
        Big_Integer operator-(const Big_Integer &other) const {
            Big_Integer tmp = other;
            // inverts the sign of the temporary Big_Integer
            tmp.negative = !other.negative;
            return *this + tmp;
        }

        // operator "*" (24/4/2025)
        Big_Integer operator*(const Big_Integer& other) const {
            Big_Integer res;
            res.digits.assign(digits.size() + other.digits.size(), 0);
            
            // loop through the digits of the first Big_Integer
            for (size_t i = 0; i < digits.size(); ++i) {
                int carry = 0;
                // loop through the digits of the second Big_Integer and any remaining carry
                for (size_t j = 0; j < other.digits.size() || carry; ++j) {
                    long long cur = res.digits[i + j] +
                        digits[i] * 1LL * (j < other.digits.size() ? other.digits[j] : 0) + carry;
                    res.digits[i + j] = cur % 10; // stores the last digit of the current product in the result
                    carry = cur / 10;
                }
            }
        
            // Remove trailing zeros (if any)
            while (res.digits.size() > 1 && res.digits.back() == 0)
                res.digits.pop_back();
        
            // Determine the sign
            res.negative = (negative != other.negative) && !res.is_zero();
            return res;
        }
        
        // operator "/" (24/4/2025)
        Big_Integer operator/(const Big_Integer& other) const {
            // checks for division by zero
            if (other.is_zero()) {
                throw runtime_error("Division by zero");
            }
        
            Big_Integer dividend = *this;
            Big_Integer divisor = other;
            dividend.negative = divisor.negative = false;
            
            // if the absolute value of the dividend is less than the divisor
            if (compare_abs(dividend, divisor) < 0)
                return Big_Integer("0");
        
            Big_Integer result;
            result.digits.resize(dividend.digits.size(), 0);
        
            Big_Integer current("0");
            // loop through the digits of the dividend from most significant to least significant
            for (int i = dividend.digits.size() - 1; i >= 0; --i) {
                current.digits.insert(current.digits.begin(), dividend.digits[i]);
                // remove leading zeros (if any)
                while (current.digits.size() > 1 && current.digits.back() == 0)
                    current.digits.pop_back();
        
                int x = 0, l = 0, r = 9;
                while (l <= r) {
                    int m = (l + r) / 2;
                    Big_Integer t = divisor * Big_Integer(std::to_string(m)); //fix bug 3: call wrong function (24/4/2025)
                    // if the product is less than or equal to the current part of the dividend
                    if (compare_abs(t, current) <= 0) {
                        x = m;
                        l = m + 1;
                    } else {
                        r = m - 1;
                    }
                }
                
                // stores the found quotient digit in the result
                result.digits[i] = x;
                current = current - (divisor * Big_Integer(std::to_string(x))); // bug 3
            }
        
            // Remove trailing zeros (if any)
            while (result.digits.size() > 1 && result.digits.back() == 0)
                result.digits.pop_back();
            
            // determines the sign of the result    
            result.negative = (negative != other.negative) && !result.is_zero();
            return result;
        }
    
        
};

//fuction prototype for calculate_postfix should be here to avoid bug 2 (24/4/2025)
Big_Integer calculate_postfix(const vector<string>& postfix);

//main function
int main(int argc, char* argv[]) {
    // checks if the number of command-line arguments is not 3
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <input.txt> <output.txt>\n";
        return 1;
    }

    ifstream fin(argv[1]);
    ofstream fout(argv[2]);
    // checks if either file failed to open
    if (!fin || !fout) {
        cerr << "Error opening files.\n";
        return 1;
    }

    // declares a string variable to store each line from the input file
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        try {
            // Step 1: Tokenize
            auto tokens = tokenize(line);
            // checks if tokenization resulted in an error token
            if (tokens.size() == 1 && tokens[0] == "ERROR")
                throw runtime_error("Invalid character in expression");

            // step 2: Infix ➜ Postfix
            auto postfix = infix_postfix(tokens);
            if (postfix.size() == 1 && postfix[0] == "ERROR")
                throw runtime_error("Malformed expression (parentheses mismatch)");

            // step 3: Evaluate
            Big_Integer result = calculate_postfix(postfix);

            fout << result.to_string() << '\n';
            cout << result.to_string() << '\n';
        } catch (const exception& e) {
            fout << "Error: " << e.what() << '\n';
            cout << "Error: " << e.what() << '\n';
        }
    }

    return 0;
}

//function space

// Define operator precedence (stage 2.2.1) 23/4/2025
int precedence(const string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    return 0;
}

// tokensize function to slipt number (stage 2.1) 23/4/2025
vector<string> tokenize(const string& expr) {
    vector<string> tokens;
    int i = 0;
    // Loops through each character of the expression.
    while (i < expr.size()) {
        char ch = expr[i];

        // Checks if the character is a whitespace.
        if (isspace(ch)) {
            ++i;
        }
        // Checks if the character is a digit.
        else if (isdigit(ch)) {
            string num;
            // Loops while the current character is a digit.
            while (i < expr.size() && isdigit(expr[i])) {
                num += expr[i];
                ++i;
            }
            tokens.push_back(num);
        }
        // Checks if the character is a minus sign and it's either the start of the expression or follows an operator/opening parenthesis.
        else if (ch == '-' && (
                    tokens.empty() || 
                    tokens.back() == "(" || 
                    tokens.back() == "+" || 
                    tokens.back() == "-" || 
                    tokens.back() == "*" || 
                    tokens.back() == "/"
                )) {
            string num = "-";
            ++i; // Moves to the next character.
            // Loops while the current character is a digit.
            while (i < expr.size() && isdigit(expr[i])) {
                num += expr[i];
                ++i;
            }
            tokens.push_back(num);
        }
        // Checks if the character is an operator or a parenthesis.
        else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')') {
            tokens.push_back(string(1, ch));
            ++i;
        }
        // If the character is not a whitespace, digit, valid negative sign, or operator/parenthesis.
        else {
            tokens.clear();
            tokens.push_back("ERROR");
            return tokens;
        }
    }
    return tokens;
}

// Convert expression from infix to postfix using Shunting Yard algorithm (stage 2.2.2) 23/4/2025
vector<string> infix_postfix(const vector<string>& tokens) {
    vector<string> output;
    stack<string> ops;

    // Iterates through each token in the input vector.
    for (const string& token : tokens) {
        if (isdigit(token[0]) || (token[0] == '-' && token.size() > 1 && isdigit(token[1]))) {
            // number or negative number (-123)
            output.push_back(token);
        }
        // Checks if the token is an opening parenthesis.
        else if (token == "(") {
            ops.push(token);
        }
        // Checks if the token is a closing parenthesis.
        else if (token == ")") {
            // Loops while the 'ops' stack is not empty and the top element is not an opening parenthesis.
            while (!ops.empty() && ops.top() != "(") {
                output.push_back(ops.top());
                ops.pop();
            }
            if (!ops.empty()) ops.pop(); // Pops the opening parenthesis from the 'ops' stack.
        }
        // Checks if the token is an operator.
        else if (token == "+" || token == "-" || token == "*" || token == "/") {
            // Loops while the 'ops' stack is not empty and the precedence of the top operator is greater than or equal to the current operator's precedence.
            while (!ops.empty() && precedence(ops.top()) >= precedence(token)) {
                // If the top of the stack is an opening parenthesis, break the loop.
                if (ops.top() == "(") break;
                output.push_back(ops.top());
                ops.pop();
            }
            ops.push(token);
        }
        else {
            // invaild token 
            return {"ERROR"};
        }
    }

    // push all to stack
    while (!ops.empty()) {
        if (ops.top() == "(") return {"ERROR"}; // If the top of the stack is an opening parenthesis, it indicates a mismatch, so return an error.
        output.push_back(ops.top());
        ops.pop();
    }

    return output;
}

// Calculate postfix expression using Big_Integer class (stage 2.2.3) 23/4/2025
Big_Integer calculate_postfix(const vector<string>& postfix) {
    stack<Big_Integer> stk;

    // Iterates through each token in the postfix expression.
    for (const string& token : postfix) {
        if (isdigit(token[0]) || (token[0] == '-' && token.size() > 1 && isdigit(token[1]))) {
            // integer (positive or negative)
            stk.push(Big_Integer(token));
        }
        // Checks if the token is an operator.
        else if (token == "+" || token == "-" || token == "*" || token == "/") {
            if (stk.size() < 2) {
                throw runtime_error("Malformed expression"); //bug 5: print wrong (25/4/2025)
            }
            Big_Integer b = stk.top(); stk.pop();
            Big_Integer a = stk.top(); stk.pop();
            Big_Integer res;

            // If the operator is '+', perform addition.
            if (token == "+") res = a + b;
            else if (token == "-") res = a - b;
            else if (token == "*") res = a * b; // bug 5: call * / when add it in Big_Integer (24/4/2025)
            else if (token == "/") {
                // check division by zero 
                if (b.is_zero()) {
                    throw runtime_error("Division by zero"); // bug 6: print wrong (25/4/2025)
                }
                res = a / b; // Perform division.
            }

            stk.push(res);
        }
        // If the token is not a number or a valid operator.
        else {
            throw runtime_error("Error: Invalid token");
        }
    }

    // After processing all tokens, if the stack does not contain exactly one element (the final result).
    if (stk.size() != 1) {
        throw runtime_error("Error: Invalid postfix expression");
    }

    return stk.top();
}