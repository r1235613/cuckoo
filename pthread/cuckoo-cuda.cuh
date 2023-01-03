#ifndef _CUCKOO_CUDA_HPP_
#define _CUCKOO_CUDA_HPP_


#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <cstdlib>


/** Reserved value for indicating "empty". */
#define EMPTY_CELL (0)


/** Max rehashing depth, and error depth. */
#define MAX_DEPTH (100)
#define ERR_DEPTH (-1)


/** CUDA thread block size. */
#define BLOCK_SIZE (256)


/** Struct of a hash function config. */
typedef struct {
    int rv;     // Randomized XOR value.
    int ss;     // Randomized shift filter start position.
} FuncConfig;


/** Hard code XOR hash functions and all inline helper functions for CUDA kernels' use. */
template <typename T>
static inline __device__ int
do_hash(const T val, const FuncConfig * const hash_func_configs, const int func_idx,
        const int size) {
    FuncConfig fc = hash_func_configs[func_idx];
    return ((val ^ fc.rv) >> fc.ss) % size;
}

template <typename T>
static inline __device__ T
fetch_val(const T data, const int pos_width) {
    return data >> pos_width;
}

template <typename T>
static inline __device__ int
fetch_func(const T data, const int pos_width) {
    return data & ((0x1 << pos_width) - 1);
}

template <typename T>
static inline __device__ T
make_data(const T val, const int func, const int pos_width) {
    return (val << pos_width) ^ func;   // CANNOT handle signed values currently!
}



#endif
