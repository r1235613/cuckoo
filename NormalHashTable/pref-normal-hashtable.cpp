//
// Created by 黃彥儒 on 2023/1/1.
//
#include "thread-safe-normal-hash-table-rwlock.hpp"
#include "thread-safe-normal-hash-table-mutex.hpp"
#include "thread-safe-normal-hash-table-mutex-big.hpp"
#include "thread-safe-normal-hash-table-rwlock-big.hpp"
#include "thread-safe-normal-hash-table-strip-mutex.hpp"
#include "thread-safe-normal-hash-table-strip-rwlock.hpp"



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
gen_rnd_input_op(command * const vals, const int n, const uint32_t limit, int read_ratio) {
    std::map<uint32_t, bool> val_map;
    int count = 0;
    while (count < n) {
        uint32_t val = (rand() % (limit - 1)) + 1;
        if (val_map.find(val) != val_map.end())
            continue;
        val_map[val] = true;
        if (val % 10 >= (10-read_ratio)){
            vals[count].op = Operate::QUERY;
        }else {
            vals[count].op = Operate::INSERT;
        }
        vals[count].val = val;
        count++;
    }
}



int main(){
    omp_set_dynamic(0);
    int size = 10000000;
    bool flag;
    size_t exp_thread[] = {1,2,4,6,8,16,24};
    auto ts = std::chrono::high_resolution_clock::now();
    command *data = new command [size];
    gen_rnd_input_op(data, size, RAND_MAX, 0);
    command *test = new command [size/10];
    gen_rnd_input_op(test, size/10, RAND_MAX, 6);
    auto te = std::chrono::high_resolution_clock::now();
    //std::cout << " 30w 70r Initial time: " << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count() << "ms. start" << std::endl;


    HashTableNormalStripMutex<uint32_t> hash_table(2*size);

    omp_set_num_threads(24);
#pragma omp parallel for
    for(int i = 0; i < size; i++){
        hash_table.insert_val(data[i].val);
    }

    for(size_t exp_idx = 0; exp_idx < sizeof(exp_thread)/sizeof(size_t); exp_idx++){
        size_t thread_num = exp_thread[exp_idx];
        omp_set_num_threads(thread_num);


#pragma omp parallel for
        for(int i = 0; i < size; i++){
            hash_table.insert_val(data[i].val);
        }


        ts = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
        for(int i = 0; i < 1000000; i++){
            if(test[i].op == Operate::QUERY) {
                hash_table.lookup_val(test[i].val);
            }else if(test[i].op == Operate::INSERT) {
                hash_table.insert_val(test[i].val);
            }
        }
        te = std::chrono::high_resolution_clock::now();
        auto command_time = std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count();

        std::cout << "time thread:\t" << command_time << "\t" << thread_num << std::endl;
#pragma omp parallel for
        for(int i = 0; i < 1000000; i++){
            if(test[i].op == Operate::INSERT) {
                hash_table.delete_val(test[i].val);
            }
        }
    }



    /*

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
    }*/



    delete[] data;
    delete[] test;



}