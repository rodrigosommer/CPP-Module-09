#include "RPN.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

RPN::RPN() {}

RPN::RPN(const RPN& other) : _stack(other._stack) {}

RPN& RPN::operator=(const RPN& other) {
    if (this != &other) {
        _stack = other._stack;
    }
    return *this;
}

RPN::~RPN() {}

double RPN::calculate(const std::string& expression) {
    std::istringstream iss(expression);
    std::string token;
    
    while (iss >> token) {
        if (isNumber(token)) {
            double num = std::atof(token.c_str());
            _stack.push(num);
        } else if (isOperator(token)) {
            if (_stack.size() < 2) {
                throw std::runtime_error("Error");
            }
            
            double b = _stack.top();
            _stack.pop();
            double a = _stack.top();
            _stack.pop();
            
            double result = performOperation(token, a, b);
            _stack.push(result);
        } else {
            throw std::runtime_error("Error");
        }
    }
    
    if (_stack.size() != 1) {
        throw std::runtime_error("Error");
    }
    
    return _stack.top();
}

bool RPN::isOperator(const std::string& token) {
    return token == "+" || token == "-" || token == "*" || token == "/";
}

bool RPN::isNumber(const std::string& token) {
    if (token.empty()) return false;
    
    size_t start = 0;
    if (token[0] == '-' || token[0] == '+') {
        start = 1;
        if (token.length() == 1) return false;
    }
    
    for (size_t i = start; i < token.length(); ++i) {
        if (!std::isdigit(token[i])) return false;
    }
    
    double num = std::atof(token.c_str());
    return num >= 0 && num < 10;
}

double RPN::performOperation(const std::string& op, double a, double b) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
        if (b == 0) throw std::runtime_error("Error");
        return a / b;
    }
    throw std::runtime_error("Error");
}