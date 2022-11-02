#ifndef NODE_H
#define NODE_H

#include <cstring>

// 定义节点类模板
template<typename K, typename V>
class Node {
public:
    Node() {}
    Node(K k, V v, int);
    
    ~Node();

    K get_key() const;
    V get_value() const;
    void set_value(V);
    Node<K, V> **forward;

    int Node_level; //表示从[0~Node_level]层都有该节点

private:
    K key;
    V value;
};

// 有参构造函数
template<typename K, typename V>
Node<K, V>::Node(const K k, const V v, const int level) : key(k), value(v), Node_level(level) {
    this->forward = new Node<K, V>*[level+1];
    memset(this->forward, 0, sizeof(Node<K, V>*)*(level+1));
}

template<typename K, typename V>
K Node<K, V>::get_key() const {
    return this->key;
}

template<typename K, typename V>
V Node<K, V>::get_value() const {
    return this->value;
}

template<typename K, typename V>
void Node<K, V>::set_value(V value) {
    this->value = value;
}

template<typename K, typename V> 
Node<K, V>::~Node() {
    delete []forward;
}

#endif  // NODE_H