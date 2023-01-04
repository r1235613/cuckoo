//
// Created by 黃彥儒 on 2023/1/3.
//



#include <iostream>
#include <mutex>
#include <atomic>

template<typename T>
class HashTableNormalMutexBig{
private:

    mutable std::mutex g_locks;
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
    void rehash();

public:
    HashTableNormalMutexBig(size_t size): _size(size){
        _data = new hash_unit[size]();
        counter = 0;
    }
    ~HashTableNormalMutexBig(){
        for(size_t i = 0; i < _size; i++){
            node *cur = _data[i].next;
            while (cur != nullptr){
                node *temp = cur;
                cur = cur->next;
                free(temp);
            }

        }
        delete[] _data;
    };
    void insert_val(const T val);
    bool delete_val(const T val);
    bool lookup_val(const T val);
    void show_content();
};
template <typename T>
void HashTableNormalMutexBig<T>::insert_val(const T val){
    uint32_t pos = val % _size;
    //std::shared_lock<std::shared_mutex> lock(g_locks);
    std::lock_guard<std::mutex> lock2(g_locks);
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
    return;
}

template <typename T>
bool HashTableNormalMutexBig<T>::delete_val(const T val){
    uint32_t pos = val % _size;
    //std::lock_guard<std::shared_mutex> lock(g_locks);
    std::lock_guard<std::mutex> lock2(g_locks);
    node *cur = _data[pos].next;
    node *prev = nullptr;
    while(cur != nullptr){
        if(cur->data == val){
            if(prev == nullptr){
                _data[pos].next = cur->next;
            }else{
                prev->next = cur->next;
            }
            delete cur;
            counter--;
            return true;
        }
        prev = cur;
        cur = cur->next;
    }
    return false;
}

template <typename T>
bool HashTableNormalMutexBig<T>::lookup_val(const T val){
    uint32_t pos = val % _size;
    //std::lock_guard<std::shared_mutex> lock(g_locks);
    std::lock_guard<std::mutex> lock2(g_locks);
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
void HashTableNormalMutexBig<T>::show_content(){
    for(uint32_t i = 0; i < _size; i++){
        std::cout << "Table " << i << ": ";
        //std::lock_guard<std::shared_mutex> lock(g_locks);
        std::lock_guard<std::mutex> lock2(g_locks);
        node *cur = _data[i].next;
        while(cur != nullptr){
            std::cout << cur->data << " ";
            cur = cur->next;
        }
    }
    std::cout << std::endl;
}

template <typename T>
void HashTableNormalMutexBig<T>::rehash(){
    std::lock_guard<std::mutex> lock2(g_locks);
    uint32_t new_size = _size * 2;
    hash_unit *new_data = new hash_unit[new_size]();
    for(uint32_t i = 0; i < _size; i++){

        node *cur = _data[i].next;
        while(cur != nullptr){
            int pos = cur->data % new_size;
            node *new_node = new node();
            new_node->data = cur->data;
            new_node->next = new_data[pos].next;
            new_data[pos].next = new_node;
            cur = cur->next;
        }
    }
    delete[] _data;
    _data = new_data;
    _size = new_size;
}

