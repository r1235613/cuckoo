//
// Created by 黃彥儒 on 2023/1/1.
//
#include "thread-safe-normal-hash-table.hpp"
#include <iostream>
#include <stdlib.h>
#include <omp.h>
#include <map>

static void
gen_rnd_input(uint32_t * const vals, const int n, const uint32_t limit) {
    std::map<uint32_t, bool> val_map;
    int count = 0;
    while (count < n) {
        uint32_t val = (rand() % (limit - 1)) + 1;
        if (val_map.find(val) != val_map.end())
            continue;
        val_map[val] = true;
        vals[count] = val;
        count++;
    }
}


int main(){
    omp_set_dynamic(1);
    int size = 0x1 << 25;
    bool flag;
    HashTableNormalStrip<int> hash_table(size>>1);
    HashTableNormalStrip<int> hash_table_serial(size>>1);
    auto ts = std::chrono::high_resolution_clock::now();
    uint32_t *data = new uint32_t [size];
    gen_rnd_input(data, size, RAND_MAX);
    auto te = std::chrono::high_resolution_clock::now();
    std::cout << "Initial time: " << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count() << "ms. start" << std::endl;
    ts = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
    for(int i = 0; i < size; i++){
        hash_table.insert_val(data[i]);
    }
    te = std::chrono::high_resolution_clock::now();
    std::cout << "insert time: " << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count() << "ms" << std::endl;

    flag = false;
    ts = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
    for(int i = 0; i < size; i++){
        if(!hash_table.lookup_val(data[i]))
            flag = true;
    }
    te = std::chrono::high_resolution_clock::now();
    if(flag)
        std::cout << "lookup error | ";
    std::cout << "lookup time: " << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count() << "ms" << std::endl;

    std::cout << "start serial" << std::endl;
    ts = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < size; i++){
        hash_table_serial.insert_val(data[i]);
    }
    te = std::chrono::high_resolution_clock::now();
    std::cout << "insert time: " << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count() << "ms" << std::endl;

    flag = false;
    ts = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < size; i++){
        if(!hash_table_serial.lookup_val(data[i]))
            flag = true;
    }
    te = std::chrono::high_resolution_clock::now();
    if(flag)
        std::cout << "lookup error | ";
    std::cout << "lookup time: " << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count() << "ms" << std::endl;

    delete[] data;



}