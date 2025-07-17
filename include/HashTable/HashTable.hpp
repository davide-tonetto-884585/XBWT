#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <vector>
#include <string>
#include <memory>
#include <functional>

template <typename KeyType>
struct HashNode {
    KeyType key;
    int count_occ;
    std::shared_ptr<HashNode> next;

    HashNode(const KeyType& k)
        : key(k), count_occ(1), next(nullptr) {}
};

template <typename KeyType>
class HashTable {
public:
    HashTable(int n);
    void print() const;
    std::shared_ptr<HashNode<KeyType>> search(const KeyType& key) const;
    bool insert(const KeyType& key);
    void clear();

private:
    int size;
    int card;
    std::vector<std::shared_ptr<HashNode<KeyType>>> table;
    std::hash<KeyType> hashFunc;
};

#endif // HASHTABLE_HPP