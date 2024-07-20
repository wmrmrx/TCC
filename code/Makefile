CXX=g++
CXXFLAGS=-Wall -Wextra -pedantic -O2 -std=c++20
COMPILE.cpp = $(CXX) $(CXXFLAGS)

%.o: %.cpp
	$(COMPILE.cpp) $<

run: build
	./main < tests/in

build:
	$(COMPILE.cpp) -o main src/*.cpp

clean: 
	rm -f main
