CXX=g++
BOOST_INCLUDE=/opt/homebrew/opt/boost@1.85/include
CXXFLAGS=-Wall -Wextra -pedantic -O2 -std=c++20 -I$(BOOST_INCLUDE)
COMPILE.cpp = $(CXX) $(CXXFLAGS)

%.o: %.cpp
	$(COMPILE.cpp) $<

run: build
	./main < tests/in

build_and_run_random: build build_scripts
	./test_generator 20 > tests/random && ./main < tests/random

run_random: build build_scripts
	./main < tests/random

build_scripts:
	$(COMPILE.cpp) -o test_generator scripts/test_generator.cpp

build:
	$(COMPILE.cpp) -o main src/*.cpp

clean: 
	rm -f main
