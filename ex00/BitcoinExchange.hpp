#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <map>
#include <string>

class BitcoinExchange {
private:
    std::map<std::string, double> _database;
    
public:
    BitcoinExchange();
    BitcoinExchange(const BitcoinExchange& other);
    BitcoinExchange& operator=(const BitcoinExchange& other);
    ~BitcoinExchange();
    
    void loadDatabase(const std::string& filename);
    void processFile(const std::string& filename);
    
private:
    bool isValidDate(const std::string& date);
    bool isValidValue(const std::string& value);
    double findClosestRate(const std::string& date);
    void processLine(const std::string& line);
    std::string trim(const std::string& str);
};

#endif