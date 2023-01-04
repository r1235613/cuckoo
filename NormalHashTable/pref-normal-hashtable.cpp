//
// Created by 黃彥儒 on 2023/1/1.
//
#include "thread-safe-normal-hash-table-rwlock.hpp"
#include "thread-safe-normal-hash-table-mutex.hpp"
#include <iostream>
#include <stdlib.h>
#include <omp.h>
#include <map>

enum class Operate{
    INSERT,
    QUERY,
    DELETE
};

struct command{
    Operate op;
    uint32_t val;
};

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
static void
gen_rnd_input_op(command * const vals, const int n, const uint32_t limit) {
    std::map<uint32_t, bool> val_map;
    int count = 0;
    while (count < n) {
        uint32_t val = (rand() % (limit - 1)) + 1;
        if (val_map.find(val) != val_map.end())
            continue;
        val_map[val] = true;
        if (val % 10 < 3){
            vals[count].op = Operate::INSERT;
        }else {
            vals[count].op = Operate::QUERY;
        }
        vals[count].val = val;
        count++;
    }
}



int main(){
    omp_set_dynamic(0);
    int size = 0x1 << 25;
    bool flag;

    auto ts = std::chrono::high_resolution_clock::now();
    command *data = new command [size];
    gen_rnd_input_op(data, size, RAND_MAX);
    auto te = std::chrono::high_resolution_clock::now();
    std::cout << " 30w 70r Initial time: " << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count() << "ms. start" << std::endl;

    std::cout << "Rwlock" << std::endl;
    HashTableNormalRwlock<int> hash_table_rwlock(0x1<<24);
#pragma omp parallel for
    for(int i=0; i<size; i++){
        hash_table_rwlock.insert_val(data[i].val);
    }
    for(int core = 24; core >=1; core--){

        omp_set_num_threads(core);
        int thread_total;

        ts = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
        for(int i = 0; i < size; i++){
            if(data->op == Operate::INSERT){
                hash_table_rwlock.insert_val(data[i].val);
            }else
                hash_table_rwlock.lookup_val(data[i].val);
            thread_total = omp_get_num_threads();
        }
        te = std::chrono::high_resolution_clock::now();
        std::cout  << thread_total << " Threads, "  << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count() << "ms" << std::endl;
    }


    std::cout << "Mutex" << std::endl;
    HashTableNormalMutex<int> hash_table_mutex(0x1<<24);
#pragma omp parallel for
    for(int i=0; i<size; i++){
        hash_table_mutex.insert_val(data[i].val);
    }
    for(int core = 24; core >=1; core--){

        omp_set_num_threads(core);
        int thread_total;
        for(int i=0; i<size; i++){
            hash_table_mutex.insert_val(data[i].val);
        }
        ts = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
        for(int i = 0; i < size; i++){
            if(data->op == Operate::INSERT)
                hash_table_mutex.insert_val(data[i].val);
            else
                hash_table_mutex.lookup_val(data[i].val);
            thread_total = omp_get_num_threads();
        }
        te = std::chrono::high_resolution_clock::now();
        std::cout  << thread_total << " Threads, "  << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count() << "ms" << std::endl;
    }



    delete[] data;



}