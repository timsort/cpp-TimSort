LIB_BOOST_TEST:=-lboost_unit_test_framework-mt

all:
	@echo This library is a C++ header file only.

.bin:
	mkdir -p .bin

test: test/test.cpp timsort.hpp .bin
	$(CXX) -I. $(LIB_BOOST_TEST) $(CXXFLAGS) -Wall -Wextra $< -o .bin/$@
	./.bin/test

bench: example/bench.cpp timsort.hpp .bin
	$(CXX) -I. $(CXXFLAGS) -DNDEBUG -O2 -Wall -Wextra $< -o .bin/$@
	./.bin/bench

.PHONY: test bench
