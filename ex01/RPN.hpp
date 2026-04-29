#ifndef RPN_HPP
#define RPN_HPP

#include <stack>
#include <string>

class RPN {
private:
    std::stack<double> _stack;
    
public:
    RPN();
    RPN(const RPN& other);
    RPN& operator=(const RPN& other);
    ~RPN();
    
    double calculate(const std::string& expression);
    
private:
    bool isOperator(const std::string& token);
    bool isNumber(const std::string& token);
    double performOperation(const std::string& op, double a, double b);
};

#endif