#ifndef SKIPLIST_H
#define SKIPLIST_H

#include "Node.h"
#include <mutex>
#include <shared_mutex>
#include <iostream>
#include <fstream>
#include <string>

#define STORE_FILE "./store/dumpFile"

std::shared_mutex mtx;
std::string delimiter = ":";

// 定义skiplistd的类模板
template<typename K, typename V>
class SkipList {
public:
    SkipList(int);
    ~SkipList();
    int get_random_level();
    Node<K, V>* create_node(const K&, const V&, int);
    int insert_element(const K&, const V&);
    void display_list();
    bool search_element(K, char[], int&);
    void delete_element(K);
    void dump_file();
    void load_file();
    int size();
    bool isupdate;

private:
    void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
    bool is_valid_string(const std::string& str);

    // Maximum level of the skip list 
    int _max_level;

    // current level of skip list 
    int _curr_level;

    // pointer to header node 
    Node<K, V> *_header;

    // file operator
    std::ofstream _file_writer;
    std::ifstream _file_reader;

    // skiplist current element count
    int _element_count;
};

// skiplist构造函数
template<typename K, typename V> 
SkipList<K, V>::SkipList(int max_level) {
    this->_max_level = max_level;
    this->_curr_level = 0;
    this->_element_count = 0;
    this->isupdate = false;

    K k;
    V v;
    this->_header = new Node<K, V>(k, v, _max_level);
}


template<typename K, typename V>
Node<K, V>* SkipList<K, V>::create_node(const K& k, const V& v, int level) {
    Node<K, V> * node = new Node<K, V>(k, v, level);
    return node;
}


template<typename K, typename V> 
int SkipList<K, V>::size() { 
    std::shared_lock<std::shared_mutex> lock(mtx);
    auto res = this->_element_count;
    return res;
}


// 抛硬币, 决定节点插入多少层
template<typename K, typename V> 
int SkipList<K, V>::get_random_level() {
    int k = 1;
    while (rand() % 2) {
        k++;
    }
    k = (k < _max_level) ? k : _max_level;
    return k;
}


// 插入元素
template<typename K, typename V>
int SkipList<K, V>::insert_element(const K & key, const V & value) {

    // std::unique_lock<std::shared_mutex> lock(mtx);  //排它锁
    auto current = this->_header;
    Node<K, V> * update[_max_level+1];  
    memset(update, 0, sizeof(Node<K, V>*) * (_max_level));
    
    // 扫描链表, 寻找插入的位置
    for(int i = _curr_level; i >= 0; i--) {
        while(current->forward[i] != NULL && current->forward[i]->get_key() < key) {
            // 没有找到插入位置, 继续向前搜索
            current = current->forward[i];
        }
        // 找到了插入位置, 记录下当前节点, 因为它的下一个可能更新为插入节点
        update[i] = current;
    }

    // 这时, 插入节点应在update[0] 和 current之间
    current = current->forward[0];

    if(current != NULL && current->get_key() == key) {
        // lock.unlock();
        return 1;
    }

    if(current == NULL || current->get_key() != key) {
        int random_level = this->get_random_level();

        // 如果随机层数高于当前跳表的层数, 插入节点的当前层数之上的层 的上一个节点应该是头结点
        if(random_level > _curr_level) {
            for(int i = _curr_level + 1; i <= random_level; i++) {
                update[i] = _header;
            }
            _curr_level = random_level;
        }

        // 生成一个节点, 该节点在[0~randon_level]插入
        Node<K, V>* insert_node = create_node(key, value, random_level);

        // 插入节点
        for(int i = 0; i <= random_level; i++) {
            insert_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = insert_node;
        }
        _element_count++;
    }
    this->isupdate = true;
    // unique_lock 自动释放锁
    return 0;
}


// 查询
template<typename K, typename V>
bool SkipList<K, V>::search_element(const K key, char buf[], int& l) {

    std::shared_lock<std::shared_mutex> lock(mtx); //读锁, 共享

    Node<K, V>* current = _header;
    for(int i = _curr_level; i >= 0; i--) {
        if(current->forward[i] != NULL && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
    }

    current = current->forward[0];

    if(current != NULL && current->get_key() == key) {
        auto str = current->get_value() + '\n';
        l = str.length();
        strcpy(buf, str.c_str());
        //std::cout << key << " : " << current->get_value() << std::endl;
        return true;
    }

    //std::cout << "key note found" << std::endl;
    return false;
}


// 删除
template<typename K, typename V>
void SkipList<K, V>::delete_element(K key) {

    std::unique_lock<std::shared_mutex> lock(mtx); // 写锁

    Node<K, V>* current = this->_header; 
    Node<K, V> *update[_curr_level+1];
    memset(update, 0, sizeof(Node<K, V>*)*(_max_level+1));
    for(int i = _curr_level; i>= 0; i--) {
        while(current->forward[i] != NULL && current->forward[i]->get_key() < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];

    if(current != NULL && current->get_key() == key) {
        // 逐层删除
        for(int i = 0; i <= _curr_level; i++) {
            update[i]->forward[i] = current->forward[i];
        }
    }

    // 删除没有元素的层
    while(_curr_level > 0 && _header->forward[_curr_level] == 0) {
        _curr_level--;
    }
    _element_count--;
    this->isupdate = true;
    std::cout << "successfully deleted" << std::endl;
    return;
}


 // 打印跳表
template<typename K, typename V>
void SkipList<K, V>::display_list() {
    std::shared_lock<std::shared_mutex> lock(mtx);
    std::cout << "\n*****Skip List*****"<<"\n"; 
    for(int i = _curr_level; i >= 0; i--) {
        Node<K, V>* node = this->_header->forward[i];
        std::cout << "Level " << i << ": ";
        while(node != NULL) {
            std::cout << node->get_key() << ":" << node->get_value() << ";";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}


template<typename K, typename V>
bool SkipList<K, V>::is_valid_string(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    if (str.find(delimiter) == std::string::npos) {
        return false;
    }
    return true;
}


template<typename K, typename V>
void SkipList<K, V>::get_key_value_from_string(const std::string& str, std::string* key, std::string* value) {
    // key 和 value 是传出参数
    if(!is_valid_string(str)) {
        return;
    }
    *key = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter)+1, str.length());
}


// 持久化 
template<typename K, typename V> 
void SkipList<K, V>::dump_file() {
    std::cout << "dump_file-----------------" << std::endl;
    
    _file_writer.open(STORE_FILE);
    Node<K, V> *node = this->_header->forward[0]; 
    while (node != NULL) {
        auto node_key = node->get_key(), node_val = node->get_value();
        _file_writer << node_key << ":" << node_val << "\n";
        //std::cout << node_key << ":" << node_val << std::endl;
        node = node->forward[0];
    }

    _file_writer.flush();
    _file_writer.close();
    
    return ;
}


// 从磁盘加载数据
template<typename K, typename V> 
void SkipList<K, V>::load_file() {

    _file_reader.open(STORE_FILE);
    std::cout << "load_file-----------------" << std::endl;
    std::string line;
    std::string* key = new std::string();
    std::string* value = new std::string();
    while (getline(_file_reader, line)) {
        get_key_value_from_string(line, key, value);
        if (key->empty() || value->empty()) {
            continue;
        }
        insert_element(stoi(*key), *value);
        std::cout << "key:" << *key << "value:" << *value << std::endl;
    }
    _file_reader.close();
}

// 析构函数
template<typename K, typename V> 
SkipList<K, V>::~SkipList() {

    if (_file_writer.is_open()) {
        _file_writer.close();
    }
    if (_file_reader.is_open()) {
        _file_reader.close();
    }
    delete _header;
}

#endif  // SKIPLIST_H