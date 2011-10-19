#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <boost/rational.hpp>

#include <boost/timer.hpp>

#include "timsort.hpp"


template <typename value_t>
void bench(int const size) {
    std::cerr << "size\t" << size << std::endl;

    std::less<value_t> lt;

    std::vector<value_t> a;
    for(int i = 0; i < size; ++i) {
        a.push_back((i+1) * 10);
    }

    std::random_shuffle(a.begin(), a.end());

    {
        boost::timer t;

        for(int i = 0; i < 100; ++i) {
            std::vector<value_t> b(a);
            std::sort(b.begin(), b.end(), lt);
        }

        std::cerr << "std::sort        " << t.elapsed() << std::endl;
    }


    {
        boost::timer t;

        for(int i = 0; i < 100; ++i) {
            std::vector<value_t> b(a);
            std::stable_sort(b.begin(), b.end(), lt);
        }

        std::cerr << "std::stable_sort " << t.elapsed() << std::endl;
    }


    {
        boost::timer t;

        for(int i = 0; i < 100; ++i) {
            std::vector<value_t> b(a);
            timsort(b.begin(), b.end(), lt);
        }

        std::cerr << "timsort          " << t.elapsed() << std::endl;
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
    bench< boost::rational<long long> >(100 * 1000);
}
