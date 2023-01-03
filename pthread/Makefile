CC=g++
NVCC=nvcc
CXXFLAGS=-std=c++17 -O3 -pthread -g3
all: perf-test demo

perf-test: perf-test.cpp cuckoo-serial.hpp cuckoo-cuda.cuh cuckoo-pthread.hpp
	${CC} $< -o $@ ${CXXFLAGS}

demo: demo.cpp cuckoo-serial.hpp cuckoo-cuda.cuh cuckoo-pthread.hpp
	${CC} $< -o $@ ${CXXFLAGS}

.PHONY: clean
clean:
	rm -f demo perf-test
