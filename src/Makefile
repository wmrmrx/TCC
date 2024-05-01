BOOST_DIR = boost_1_85_0
BOOST_URL = https://boostorg.jfrog.io/artifactory/main/release/1.85.0/source/boost_1_85_0.tar.gz

boost: include/boost lib/boost
.PHONY: boost

artifacts/boost.tar.gz:
	wget $(BOOST_URL) -O artifacts/boost.tar.gz

artifacts/boost: artifacts/boost.tar.gz
	tar xf artifacts/boost.tar.gz -C artifacts
	mv artifacts/$(BOOST_DIR) artifacts/boost

bootstrap-boost: artifacts/boost
	cd artifacts/boost && \
		./bootstrap.sh --includedir=../../include --libdir=../../lib && \
		./b2 install

include/boost: bootstrap-boost
lib/boost: bootstrap-boost

CXX=g++
CXXFLAGS=-Wall -pedantic -O2 -std=c++17 -Iinclude

COMPILE.cpp = $(CXX) $(CXXFLAGS)

%.o: %.cpp
	$(COMPILE.cpp) $<