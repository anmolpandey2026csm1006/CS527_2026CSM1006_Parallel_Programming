CXX ?= g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -fopenmp -I../../Assignment_01/common

SRC = source/convolution.cpp
HDR = ../../Assignment_01/common/omp_bench.hpp
BIN = bin/task5_convolution

all: $(BIN)

$(BIN): $(SRC) $(HDR)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

check: $(BIN)
	./$(BIN) --sizes 256,512 --threads 1,2,4 --repeats 1 --filter all

bench: $(BIN)
	mkdir -p output
	./$(BIN) | tee output/results.md

clean:
	rm -rf bin

.PHONY: all check bench clean
