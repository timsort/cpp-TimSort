#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <boost/rational.hpp>

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


template <typename value_t>
void bench(int const size) {
    std::cerr << "size\t" << size << std::endl;

    std::vector<value_t> a;
    for(int i = 0; i < size; ++i) {
        a.push_back((i+1) * 10);
    }

    std::random_shuffle(a.begin(), a.end());

    {
        std::vector<value_t> b(a);
        boost::timer t;

        for(int i = 0; i < 100; ++i) {
            std::sort(b.begin(), b.end(), &std_compare<value_t>);
        }

        std::cerr << "std::sort\t" << t.elapsed() << std::endl;
    }


    {
        std::vector<value_t> b(a);
        boost::timer t;

        for(int i = 0; i < 100; ++i) {
            std::stable_sort(b.begin(), b.end(), &std_compare<value_t>);
        }

        std::cerr << "std::stable_sort\t" << t.elapsed() << std::endl;
    }


    {
        std::vector<value_t> b(a);
        boost::timer t;

        for(int i = 0; i < 100; ++i) {
            timsort(b.begin(), b.end(), &compare<value_t>);
        }

        std::cerr << "timsort\t" << t.elapsed() << std::endl;
    }
}

int main() {
    typedef boost::rational<int> value_t;

    std::srand(std::time(NULL));

    std::cerr << "int" << std::endl;
    bench<int>(100 * 1000);

    std::cerr << "double" << std::endl;
    bench<double>(100 * 1000);

    std::cerr << "boost::rational" << std::endl;
    bench< boost::rational<int> >(100 * 1000);
}
