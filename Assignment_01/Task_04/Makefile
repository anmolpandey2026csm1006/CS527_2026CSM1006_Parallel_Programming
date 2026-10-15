CXX ?= g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -fopenmp -I../common

SRC = source/matadd.cpp
HDR = ../common/omp_bench.hpp
BIN = bin/task4_matadd

all: $(BIN)

$(BIN): $(SRC) $(HDR)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

check: $(BIN)
	./$(BIN) --sizes 8,256 --threads 1,2,4 --repeats 1

bench: $(BIN)
	mkdir -p output
	./$(BIN) | tee output/results.md

clean:
	rm -rf bin

.PHONY: all check bench clean
