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

BOOST_AUTO_TEST_CASE( test_timsort_simple ) {
    std::vector<int> a;
    a.push_back(6);
    a.push_back(5);
    a.push_back(0);
    a.push_back(4);
    a.push_back(8);
    a.push_back(2);
    a.push_back(3);
    a.push_back(7);
    a.push_back(1);
    a.push_back(9);

    timsort(a.begin(), a.end(), &compare<int>);

    BOOST_CHECK_EQUAL( a[0], 0 );
    BOOST_CHECK_EQUAL( a[0], 1 );
    BOOST_CHECK_EQUAL( a[0], 2 );
    BOOST_CHECK_EQUAL( a[0], 3 );
    BOOST_CHECK_EQUAL( a[0], 4 );
    BOOST_CHECK_EQUAL( a[0], 5 );
    BOOST_CHECK_EQUAL( a[0], 6 );
    BOOST_CHECK_EQUAL( a[0], 7 );
    BOOST_CHECK_EQUAL( a[0], 8 );
    BOOST_CHECK_EQUAL( a[0], 9 );
}

