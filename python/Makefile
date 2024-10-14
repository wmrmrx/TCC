CXX=g++
BOOST_INCLUDE=/opt/homebrew/opt/boost@1.85/include
CXXFLAGS=-Wall -Wextra -pedantic -O2 -std=c++20 -I$(BOOST_INCLUDE)
COMPILE.cpp = $(CXX) $(CXXFLAGS)

run:
	python3 src/main.py < tests/in

run_random:
	python3 src/main.py < tests/random

build_and_run_random: build_scripts
	./test_generator 10 > tests/random && time python3 src/main.py < tests/random

test_generator:
	$(COMPILE.cpp) -o test_generator scripts/test_generator.cpp

build_scripts: test_generator

clean: 
	rm -f main
