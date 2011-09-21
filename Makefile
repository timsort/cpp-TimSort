LIB_BOOST_TEST:=-lboost_unit_test_framework-mt

test: timsort.hpp test.cpp
	$(CXX) $(LIB_BOOST_TEST) -Wall -Wextra -std=c++0x test.cpp -o $@
