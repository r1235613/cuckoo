//
// Created by 黃彥儒 on 2023/1/1.
//

#ifndef THREAD_SAFE_NORMAL_HASH_TABLE_HPP
#define THREAD_SAFE_NORMAL_HASH_TABLE_HPP

#include <shared_mutex>

template<typename T>
class HashTableNormalStrip{
private:
    hash_unit *_data;
    mutable std::shared_mutex g_locks;
    mutable std::shared_mutex counter_lock;
    long
    int _size;
    struct hash_unit{
        mutable std::shared_mutex lock;
        node *next = nullptr;
    };
    struct node{
        T data;
        node *next = nullptr;
    };
    void rehash();

public:
    HashTableNormalStrip(size_t size): _size(size){
        _data = new hash_unit[size]();
    }
    ~HashTableNormalStrip(){
        delete[] _data;
    };
    void insert_val(const T val);
    bool delete_val(const T val);
    bool lookup_val(const T val);
    void show_content();
};
template <typename T>
void HashTableNormalStrip<T>::insert_val(const T val){
    int pos = val % _size;
    std::unique_lock<std::shared_mutex> lock(g_locks);
    std::unique_lock<std::shared_mutex> lock2(_data[pos].lock);
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
    return;
}

template <typename T>
bool HashTableNormalStrip<T>::delete_val(const T val){
    int pos = val % _size;
    std::unique_lock<std::shared_mutex> lock(g_locks);
    std::unique_lock<std::shared_mutex> lock2(_data[pos].lock);
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
            return true;
        }
        prev = cur;
        cur = cur->next;
    }
    return false;
}

template <typename T>
bool HashTableNormalStrip<T>::lookup_val(const T val){
    int pos = val % _size;
    std::shared_lock<std::shared_mutex> lock(g_locks);
    std::shared_lock<std::shared_mutex> lock2(_data[pos].lock);
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
void HashTableNormalStrip<T>::show_content(){
    for(int i = 0; i < _size; i++){
        std::cout << "Table " << i << ": ";
        std::shared_lock<std::shared_mutex> lock(g_locks);
        std::shared_lock<std::shared_mutex> lock2(_data[i].lock);
        node *cur = _data[i].next;
        while(cur != nullptr){
            std::cout << cur->data << " ";
            cur = cur->next;
        }
    }
    std::cout << std::endl;
}

template <typename T>
void HashTableNormalStrip<T>::rehash(){
    std::unique_lock<std::shared_mutex> lock(g_locks);
    std::unique_lock<std::shared_mutex> lock2(counter_lock);
    int new_size = _size * 2;
    hash_unit *new_data = new hash_unit[new_size]();
    for(int i = 0; i < _size; i++){
        std::unique_lock<std::shared_mutex> lock3(_data[i].lock);
        node *cur = _data[i].next;
        while(cur != nullptr){
            int pos = cur->data % new_size;
            std::unique_lock<std::shared_mutex> lock4(new_data[pos].lock);
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

#endif //THREAD_SAFE_NORMAL_HASH_TABLE_HPP
