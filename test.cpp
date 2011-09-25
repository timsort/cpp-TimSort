#include <iostream>
#include <vector>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TimSortTest
#include <boost/test/unit_test.hpp>

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

BOOST_AUTO_TEST_CASE( t01_simple10 ) {
    std::vector<int> a;
    a.push_back(60);
    a.push_back(50);
    a.push_back( 0);
    a.push_back(40);
    a.push_back(80);
    a.push_back(20);
    a.push_back(30);
    a.push_back(70);
    a.push_back(10);
    a.push_back(90);

    timsort(a.begin(), a.end(), &compare<int>);

    BOOST_CHECK_EQUAL( a[0],  0 );
    BOOST_CHECK_EQUAL( a[1], 10 );
    BOOST_CHECK_EQUAL( a[2], 20 );
    BOOST_CHECK_EQUAL( a[3], 30 );
    BOOST_CHECK_EQUAL( a[4], 40 );
    BOOST_CHECK_EQUAL( a[5], 50 );
    BOOST_CHECK_EQUAL( a[6], 60 );
    BOOST_CHECK_EQUAL( a[7], 70 );
    BOOST_CHECK_EQUAL( a[8], 80 );
    BOOST_CHECK_EQUAL( a[9], 90 );
}

BOOST_AUTO_TEST_CASE( t02_shuffle32 ) {
    std::vector<int> a;
    const int size = 32;
    for(int i = 1; i <= size; ++i) {
        a.push_back(i * 10);
    }
    std::random_shuffle(a.begin(), a.end());

    timsort(a.begin(), a.end(), &compare<int>);

    for(int i = 1; i <= size; ++i) {
        BOOST_CHECK_EQUAL( a[i], i * 10 );
    }
}

BOOST_AUTO_TEST_CASE( t03_shuffle33 ) {
    std::vector<int> a;
    const int size = 33;
    for(int i = 1; i <= size; ++i) {
        a.push_back(i * 10);
    }
    std::random_shuffle(a.begin(), a.end());

    timsort(a.begin(), a.end(), &compare<int>);

    for(int i = 1; i <= size; ++i) {
        BOOST_CHECK_EQUAL( a[i], i * 10 );
    }
}
