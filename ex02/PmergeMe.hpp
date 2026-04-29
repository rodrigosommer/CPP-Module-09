#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector>
#include <deque>
#include <string>

class PmergeMe {
private:
    std::vector<int> _vectorData;
    std::deque<int>  _dequeData;

public:
    PmergeMe();
    PmergeMe(const PmergeMe& other);
    PmergeMe& operator=(const PmergeMe& other);
    ~PmergeMe();

    void parseInput(int argc, char** argv);
    void sortAndDisplay();

private:
    template<typename Container>
    void fordJohnsonSort(Container& c);

    void displaySequence(const std::vector<int>& seq, const std::string& label);
    bool isValidPositiveInteger(const std::string& str);
    double getTime();
};

#endif
