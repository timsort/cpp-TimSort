#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <boost/timer.hpp>

#include "timsort.hpp"

template<typename T>
int compare(T x, T y) {
    if(x == y) {
        return 0;
    }
    else if(x < y) {
        return -1;
    }
    else {
        return 1;
    }
}

template<typename T>
bool std_compare(T x, T y) {
    return x < y;
}

int main() {
    std::srand(std::time(NULL));
    for(int size = 1024; size <= (1024 * 1024); size *= 2) {
        std::cerr << "size\t" << size << std::endl;

        std::vector<int> a;
        for(int i = 0; i < size; ++i) {
            a.push_back((i+1) * 10);
        }

        std::random_shuffle(a.begin(), a.end());

        {
            std::vector<int> b(a);
            boost::timer t;

            for(int i = 0; i < 100; ++i) {
                std::sort(b.begin(), b.end(), &std_compare<int>);
            }

            std::cerr << "std::sort\t" << t.elapsed() << std::endl;
        }


        {
            std::vector<int> b(a);
            boost::timer t;

            for(int i = 0; i < 100; ++i) {
                std::stable_sort(b.begin(), b.end(), &std_compare<int>);
            }

            std::cerr << "std::stable_sort\t" << t.elapsed() << std::endl;
        }


        {
            std::vector<int> b(a);
            boost::timer t;

            for(int i = 0; i < 100; ++i) {
                timsort(b.begin(), b.end(), &compare<int>);
            }

            std::cerr << "timsort\t" << t.elapsed() << std::endl;
        }
    }
}
