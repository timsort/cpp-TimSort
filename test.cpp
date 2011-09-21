#include <iostream>
#include <vector>

#include "timsort.hpp"


template<typename T>
int compare(T x, T y) {
    if(x == y) {
        return 0;
    }
    else if(x < 0) {
        return -1;
    }
    else {
        return 1;
    }
}

int main() {
    std::cout << "# C++ version: " << __cplusplus << std::endl;

    std::vector<int> a;
    a.push_back(6);
    a.push_back(5);
    a.push_back(10);
    a.push_back(4);
    a.push_back(8);
    a.push_back(2);
    a.push_back(3);
    a.push_back(7);
    a.push_back(1);
    a.push_back(9);

    timsort(a.begin(), a.end(), &compare<int>);

    for(std::vector<int>::const_iterator i = a.begin();
            i != a.end();
            ++i ) {
        std::cout << *i << std::endl;
    }
    return 0;
}

