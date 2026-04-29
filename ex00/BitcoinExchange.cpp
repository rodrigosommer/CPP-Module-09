#include "BitcoinExchange.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& other) : _database(other._database) {}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other) {
    if (this != &other) {
        _database = other._database;
    }
    return *this;
}

BitcoinExchange::~BitcoinExchange() {}

void BitcoinExchange::loadDatabase(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "Error: could not open database file." << std::endl;
        return;
    }
    
    std::string line;
    std::getline(file, line); // Skip header line
    
    while (std::getline(file, line)) {
        size_t pos = line.find(',');
        if (pos != std::string::npos) {
            std::string date = line.substr(0, pos);
            std::string rateStr = line.substr(pos + 1);
            double rate = std::atof(rateStr.c_str());
            _database[date] = rate;
        }
    }
    file.close();
}

void BitcoinExchange::processFile(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "Error: could not open file." << std::endl;
        return;
    }
    
    std::string line;
    std::getline(file, line); // Skip header line if present
    
    while (std::getline(file, line)) {
        processLine(line);
    }
    file.close();
}

void BitcoinExchange::processLine(const std::string& line) {
    size_t pos = line.find(" | ");
    if (pos == std::string::npos) {
        std::cerr << "Error: bad input => " << line << std::endl;
        return;
    }
    
    std::string dateStr = trim(line.substr(0, pos));
    std::string valueStr = trim(line.substr(pos + 3));
    
    if (!isValidDate(dateStr)) {
        std::cerr << "Error: bad input => " << line << std::endl;
        return;
    }
    
    if (!isValidValue(valueStr)) {
        if (valueStr.find('-') != std::string::npos) {
            std::cerr << "Error: not a positive number." << std::endl;
        } else {
            std::cerr << "Error: too large a number." << std::endl;
        }
        return;
    }
    
    double value = std::atof(valueStr.c_str());
    if (value < 0) {
        std::cerr << "Error: not a positive number." << std::endl;
        return;
    }
    if (value > 1000) {
        std::cerr << "Error: too large a number." << std::endl;
        return;
    }
    
    double rate = findClosestRate(dateStr);
    double result = value * rate;
    
    std::cout << dateStr << " => " << value << " = " << result << std::endl;
}

bool BitcoinExchange::isValidDate(const std::string& date) {
    if (date.length() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;
    
    for (size_t i = 0; i < date.length(); ++i) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(date[i])) return false;
    }
    
    int year = std::atoi(date.substr(0, 4).c_str());
    int month = std::atoi(date.substr(5, 2).c_str());
    int day = std::atoi(date.substr(8, 2).c_str());
    
    if (year < 1000 || year > 9999) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    
    return true;
}

bool BitcoinExchange::isValidValue(const std::string& value) {
    if (value.empty()) return false;
    
    bool hasDecimal = false;
    size_t start = 0;
    
    if (value[0] == '-' || value[0] == '+') {
        start = 1;
        if (value.length() == 1) return false;
    }
    
    for (size_t i = start; i < value.length(); ++i) {
        if (value[i] == '.') {
            if (hasDecimal) return false;
            hasDecimal = true;
        } else if (!std::isdigit(value[i])) {
            return false;
        }
    }
    
    return true;
}

double BitcoinExchange::findClosestRate(const std::string& date) {
    std::map<std::string, double>::iterator it = _database.lower_bound(date);
    
    if (it != _database.end() && it->first == date) {
        return it->second;
    }
    
    if (it == _database.begin()) {
        return _database.begin()->second;
    }
    
    --it;
    return it->second;
}

std::string BitcoinExchange::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}