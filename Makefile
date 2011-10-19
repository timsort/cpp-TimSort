LIB_BOOST_TEST:=-lboost_unit_test_framework-mt

test: timsort.hpp test.cpp
	$(CXX) $(LIB_BOOST_TEST) $(CXXFLAGS) -Wall -Wextra test.cpp -o $@
	./test

bench: timsort.hpp bench.cpp
	$(CXX) $(CXXFLAGS) -DNDEBUG -O2 -Wall -Wextra bench.cpp -o $@
	./bench
