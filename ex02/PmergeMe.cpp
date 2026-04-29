#include "PmergeMe.hpp"
#include <iostream>
#include <algorithm>
#include <utility>
#include <sys/time.h>
#include <cstdlib>

PmergeMe::PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe& other)
    : _vectorData(other._vectorData), _dequeData(other._dequeData) {}

PmergeMe& PmergeMe::operator=(const PmergeMe& other) {
    if (this != &other) {
        _vectorData = other._vectorData;
        _dequeData  = other._dequeData;
    }
    return *this;
}

PmergeMe::~PmergeMe() {}

// Jacobsthal sequence: 1, 3, 5, 11, 21, 43, ... (t[n] = t[n-1] + 2*t[n-2])
// Used to determine insertion order for pend elements.
static std::vector<size_t> jacobsthalNumbers(size_t n) {
    std::vector<size_t> jac;
    jac.push_back(1);
    if (n <= 1) return jac;
    jac.push_back(3);
    while (jac.back() < n) {
        size_t next = jac[jac.size() - 1] + 2 * jac[jac.size() - 2];
        jac.push_back(next);
    }
    return jac;
}

// Ford-Johnson (merge-insertion) sort:
// 1. Pair elements; larger of each pair goes to main chain, smaller to pend.
// 2. Recursively sort main chain using Ford-Johnson.
// 3. Insert pend[0] at the front (it is <= its main partner).
// 4. Insert remaining pend elements via binary search in Jacobsthal order,
//    bounding each search by the position of its paired main element.
template<typename Container>
void PmergeMe::fordJohnsonSort(Container& c) {
    size_t n = c.size();
    if (n <= 1)
        return;

    typedef typename Container::value_type T;

    bool hasStraggler = (n % 2 != 0);
    T straggler = hasStraggler ? c[n - 1] : T();

    // Step 1: Form sorted pairs (larger first).
    std::vector<std::pair<T, T> > pairs;
    for (size_t i = 0; i + 1 < n; i += 2) {
        if (c[i] >= c[i + 1])
            pairs.push_back(std::make_pair(c[i], c[i + 1]));
        else
            pairs.push_back(std::make_pair(c[i + 1], c[i]));
    }

    // Step 2: Recursively sort the main chain (larger elements).
    Container mainChain;
    for (size_t i = 0; i < pairs.size(); ++i)
        mainChain.push_back(pairs[i].first);
    fordJohnsonSort(mainChain);

    // Re-order pairs to match sorted main chain order so that
    // pairs[i].first == mainChain[i] and pairs[i].second is its pend partner.
    std::sort(pairs.begin(), pairs.end());

    // Step 3: Build initial sequence [pend[0], main[0], main[1], ..., main[k-1]].
    // pend[0] <= main[0] by the pairing invariant.
    Container result;
    result.push_back(pairs[0].second);
    for (size_t i = 0; i < mainChain.size(); ++i)
        result.push_back(mainChain[i]);

    // Step 4: Insert remaining pend elements using Jacobsthal insertion order.
    // pend[i] <= pairs[i].first, so the binary search upper bound for pend[i]
    // is the position of pairs[i].first in result.
    if (pairs.size() > 1) {
        std::vector<size_t> jac = jacobsthalNumbers(pairs.size());
        std::vector<bool> inserted(pairs.size(), false);
        inserted[0] = true;

        size_t prevBound = 1;
        for (size_t g = 0; g < jac.size(); ++g) {
            size_t curr = jac[g];
            if (curr >= pairs.size())
                curr = pairs.size() - 1;

            // Insert indices from curr down to prevBound (inclusive).
            for (long long k = (long long)curr; k >= (long long)prevBound; --k) {
                size_t idx = (size_t)k;
                if (inserted[idx])
                    continue;

                // Upper bound: just past pairs[idx].first in the current result.
                typename Container::iterator bound =
                    std::upper_bound(result.begin(), result.end(), pairs[idx].first);
                typename Container::iterator pos =
                    std::lower_bound(result.begin(), bound, pairs[idx].second);
                result.insert(pos, pairs[idx].second);
                inserted[idx] = true;
            }

            prevBound = curr + 1;
            if (prevBound >= pairs.size())
                break;
        }

        // Safety fallback for any uninserted pend elements.
        for (size_t i = 1; i < pairs.size(); ++i) {
            if (!inserted[i]) {
                typename Container::iterator bound =
                    std::upper_bound(result.begin(), result.end(), pairs[i].first);
                typename Container::iterator pos =
                    std::lower_bound(result.begin(), bound, pairs[i].second);
                result.insert(pos, pairs[i].second);
            }
        }
    }

    // Insert straggler (odd element) into its sorted position.
    if (hasStraggler) {
        typename Container::iterator pos =
            std::lower_bound(result.begin(), result.end(), straggler);
        result.insert(pos, straggler);
    }

    c = result;
}

void PmergeMe::parseInput(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        if (!isValidPositiveInteger(argv[i])) {
            std::cerr << "Error" << std::endl;
            std::exit(1);
        }
        int num = std::atoi(argv[i]);
        if (num <= 0) {
            std::cerr << "Error" << std::endl;
            std::exit(1);
        }
        _vectorData.push_back(num);
        _dequeData.push_back(num);
    }
    if (_vectorData.empty()) {
        std::cerr << "Error" << std::endl;
        std::exit(1);
    }
}

void PmergeMe::sortAndDisplay() {
    displaySequence(_vectorData, "Before:");

    std::vector<int> vectorCopy = _vectorData;
    double startVec = getTime();
    fordJohnsonSort(vectorCopy);
    double vecTime = (getTime() - startVec) * 1000000.0;

    std::deque<int> dequeCopy = _dequeData;
    double startDeq = getTime();
    fordJohnsonSort(dequeCopy);
    double deqTime = (getTime() - startDeq) * 1000000.0;

    std::vector<int> sortedVec(vectorCopy.begin(), vectorCopy.end());
    displaySequence(sortedVec, "After:");

    std::cout << "Time to process a range of " << _vectorData.size()
              << " elements with std::vector : " << vecTime << " us" << std::endl;
    std::cout << "Time to process a range of " << _dequeData.size()
              << " elements with std::deque : " << deqTime << " us" << std::endl;
}

void PmergeMe::displaySequence(const std::vector<int>& seq, const std::string& label) {
    std::cout << label;
    for (size_t i = 0; i < seq.size() && i < 5; ++i)
        std::cout << " " << seq[i];
    if (seq.size() > 5)
        std::cout << " [...]";
    std::cout << std::endl;
}

bool PmergeMe::isValidPositiveInteger(const std::string& str) {
    if (str.empty()) return false;
    for (size_t i = 0; i < str.length(); ++i) {
        if (!std::isdigit(str[i])) return false;
    }
    return true;
}

double PmergeMe::getTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}
