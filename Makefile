
test: timsort.hpp test.cpp
	$(CXX) -Wall -Wextra -std=c++0x test.cpp -o $@
