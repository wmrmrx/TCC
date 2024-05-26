BOOST_DIR = boost_1_85_0
BOOST_URL = https://boostorg.jfrog.io/artifactory/main/release/1.85.0/source/boost_1_85_0.tar.gz

default: build

artifacts/boost.tar.gz:
	wget $(BOOST_URL) -O artifacts/boost.tar.gz

artifacts/boost: artifacts/boost.tar.gz
	tar xf artifacts/boost.tar.gz -C artifacts
	mv artifacts/$(BOOST_DIR) artifacts/boost

artifacts/boost_bootstrapped: artifacts/boost
	cd artifacts/boost && \
		./bootstrap.sh --includedir=../../include --libdir=../../lib && \
		./b2 install
	touch artifacts/boost_bootstrapped

include/boost: artifacts/boost_bootstrapped
lib/boost: artifacts/boost_bootstrapped

boost: include/boost lib/boost
.PHONY: boost

CXX=g++-14
CXXFLAGS=-Wall -Wextra -pedantic -O2 -std=c++20 -Iinclude
COMPILE.cpp = $(CXX) $(CXXFLAGS)

%.o: %.cpp
	$(COMPILE.cpp) $<

build: boost
	$(COMPILE.cpp) -o main src/main.cpp

clean: 
	rm -f main
