LIB_BOOST_TEST:=-lboost_unit_test_framework-mt

test: timsort.hpp test.cpp
	$(CXX) $(LIB_BOOST_TEST) $(CXXFLAGS) -Wall -Wextra -std=c++0x test.cpp -o $@
	./test

bench: timsort.hpp bench.cpp
	$(CXX) $(LIB_BOOST_TEST) $(CXXFLAGS) -DNDEBUG -O2 -Wall -Wextra -std=c++0x bench.cpp -o $@
	./bench
