CXX ?= g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -fopenmp -I../../Assignment_01/common

SRC = source/arraysearch.cpp
HDR = ../../Assignment_01/common/omp_bench.hpp
BIN = bin/task1_arraysearch

all: $(BIN)

$(BIN): $(SRC) $(HDR)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

check: $(BIN)
	./$(BIN) --sizes 1000000 --threads 1,2,4 --repeats 1

bench: $(BIN)
	mkdir -p output
	./$(BIN) | tee output/results.md

clean:
	rm -rf bin

.PHONY: all check bench clean
