//
// Created by 黃彥儒 on 2023/1/1.
//
#include "thread-safe-normal-hash-table.hpp"
#include "thread-safe-normal-hash-table-mutex.hpp"
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
    omp_set_dynamic(0);
    int size = 0x1 << 25;
    bool flag;

    auto ts = std::chrono::high_resolution_clock::now();
    uint32_t *data = new uint32_t [size];
    gen_rnd_input(data, size, RAND_MAX);
    auto te = std::chrono::high_resolution_clock::now();
    std::cout << "Initial time: " << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count() << "ms. start" << std::endl;

    std::cout << "Rwlock" << std::endl;
    for(int core = 24; core >=1; core++){
        HashTableNormalRwlock<int> hash_table_rwlock(size>>1);
        omp_set_num_threads(core);
        int thread_total;
        ts = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
        for(int i = 0; i < size; i++){
            hash_table_rwlock.insert_val(data[i]);
            thread_total = omp_get_num_threads();
        }
        te = std::chrono::high_resolution_clock::now();
        auto time_1s = std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count();
        ts = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
        for(int i = 0; i < size; i++){
            hash_table_rwlock.lookup_val(data[i]);
            thread_total = omp_get_num_threads();
        }
        te = std::chrono::high_resolution_clock::now();
        std::cout  << thread_total << "Threads " << "time: " << time_1s << "ms, " << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count() << "ms" << std::endl;
    }

    std::cout << "Mutex" << std::endl;
    for(int core = 24; core >=1; core++){
        HashTableNormalMutex<int> hash_table_mutex(size>>1);
        omp_set_num_threads(core);
        int thread_total;
        ts = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
        for(int i = 0; i < size; i++){
            hash_table_mutex.insert_val(data[i]);
            thread_total = omp_get_num_threads();
        }
        te = std::chrono::high_resolution_clock::now();
        auto time_1s = std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count();
        ts = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
        for(int i = 0; i < size; i++){
            hash_table_mutex.lookup_val(data[i]);
        }
        te = std::chrono::high_resolution_clock::now();
        std::cout  << thread_total << "Threads " << "time: " << time_1s  << "ms, "  << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count() << "ms" << std::endl;
    }

    delete[] data;



}