//
// Created by 黃彥儒 on 2023/1/4.
//

#ifndef CUCKOO_HASHING_CUDA_THREAD_SAFE_NORMAL_HASH_TABLE_STRIP_MUTEX_H
#define CUCKOO_HASHING_CUDA_THREAD_SAFE_NORMAL_HASH_TABLE_STRIP_MUTEX_H

#include <iostream>
#include <mutex>
#include <atomic>


template<typename T>
class HashTableNormalStripMutex{
private:

    std::mutex *g_locks;
    std::atomic_int32_t counter;
    u_int32_t _size;
    struct node{
        T data;
        node *next = nullptr;
    };
    struct hash_unit{
        node *next = nullptr;
    };
    hash_unit *_data;
    size_t _lock_size;
    void rehash();

public:
    HashTableNormalStripMutex(size_t size, size_t lock_num=4): _size(size), _lock_size(lock_num){
        _data = new hash_unit[size]();
        g_locks = new std::mutex[lock_num]();
        counter = 0;
    }
    ~HashTableNormalStripMutex(){
        for(size_t i = 0; i < _size; i++){
            node *cur = _data[i].next;
            while (cur != nullptr){
                node *temp = cur;
                cur = cur->next;
                free(temp);

            }

        }
        delete[] _data;
        _data = nullptr;
        delete[] g_locks;
        g_locks = nullptr;
    };
    void insert_val(const T val);
    bool delete_val(const T val);
    bool lookup_val(const T val);
    void show_content();
};

template <typename T>
void HashTableNormalStripMutex<T>::insert_val(const T val){
    uint32_t pos = val % _size;
    std::lock_guard<std::mutex> lock1(g_locks[pos % _lock_size]);
    node *cur = _data[pos].next;
    while(cur != nullptr){
        if(cur->data == val){
            return;
        }
        cur = cur->next;
    }
    node *new_node = new node();
    new_node->data = val;
    new_node->next = _data[pos].next;
    _data[pos].next = new_node;
    counter++;
    if(counter > _size * 0.8){
        rehash();
    }
}

template <typename T>
bool HashTableNormalStripMutex<T>::delete_val(const T val){
    uint32_t pos = val % _size;
    std::lock_guard<std::mutex> lock1(g_locks[pos % _lock_size]);
    node *cur = _data[pos].next;
    node *prev = nullptr;
    while(cur != nullptr){
        if(cur->data == val){
            if(prev == nullptr){
                _data[pos].next = cur->next;
            }else{
                prev->next = cur->next;
            }
            free(cur);
            counter--;
            return true;
        }
        prev = cur;
        cur = cur->next;
    }
    return false;
}

template <typename T>
bool HashTableNormalStripMutex<T>::lookup_val(const T val){
    uint32_t pos = val % _size;
    std::lock_guard<std::mutex> lock1(g_locks[pos % _lock_size]);
    node *cur = _data[pos].next;
    while(cur != nullptr){
        if(cur->data == val){
            return true;
        }
        cur = cur->next;
    }
    return false;
}

template <typename T>
void HashTableNormalStripMutex<T>::show_content(){
    // 這個不用加鎖，因為改變只會在第一個練結列表
    for(size_t i = 0; i < _size; i++){
        node *cur = _data[i].next;
        while(cur != nullptr){
            std::cout << cur->data << " ";
            cur = cur->next;
        }
    }
    std::cout << std::endl;
}

template <typename T>
void HashTableNormalStripMutex<T>::rehash(){
    for(int i =0; i < _lock_size; i++){
        std::lock_guard<std::mutex> lock1(g_locks[i]);
    }
    size_t new_size = _size * 2;
    hash_unit *new_data = new hash_unit[new_size]();
    for(size_t i = 0; i < _size; i++){
        node *cur = _data[i].next;
        while(cur != nullptr){
            node *temp = cur;
            cur = cur->next;
            uint32_t pos = temp->data % new_size;
            temp->next = new_data[pos].next;
            new_data[pos].next = temp;
        }
    }
    delete[] _data;
    _data = new_data;
    _size = new_size;
}

#endif //CUCKOO_HASHING_CUDA_THREAD_SAFE_NORMAL_HASH_TABLE_STRIP_MUTEX_H
