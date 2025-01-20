#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cstring>
#include <cstdlib>

#include "HashTable.hpp"

template <typename KeyType>
HashTable<KeyType>::HashTable(int n) {
    size = static_cast<int>(1.2 * n + 13); // Load factor ~ 80%
    card = 0;
    table.resize(size, nullptr);
}

template <typename KeyType>
void HashTable<KeyType>::print() const {
    std::cout << "\n\n================== Hash Table ====================\n";
    std::cout << "Table size = " << size << ", number of stored objects = " << card << "\n\n";

    for (const auto& head : table) {
        for (auto p = head; p; p = p->next) {
            std::cout << "key = \"" << p->key << "\", count_occ= " << p->count_occ << "\n";
        }
    }
}

template <typename KeyType>
std::shared_ptr<HashNode<KeyType>> HashTable<KeyType>::search(const KeyType& key) const {
    int index = hashFunc(key) % size;
    for (auto p = table[index]; p; p = p->next) {
        if (key == p->key) {
            return p;
        }
    }
    return nullptr;
}

template <typename KeyType>
bool HashTable<KeyType>::insert(const KeyType& key) {
    int index = hashFunc(key) % size;
    auto node = search(key);

    if (node) {
        node->count_occ += 1;

        // Move-to-front the searched key
        if (table[index] != node) {
            auto pred = table[index];
            while (pred->next != node) {
                pred = pred->next;
            }
            pred->next = node->next;
            node->next = table[index];
            table[index] = node;
        }
        return false;
    } else {
        auto newNode = std::make_shared<HashNode<KeyType>>(key);
        newNode->next = table[index];
        table[index] = newNode;
        card += 1;
        return true;
    }
}

template <typename KeyType>
void HashTable<KeyType>::clear() {
    for (auto& head : table) {
        while (head) {
            head = head->next;
        }
    }
    card = 0;
    size = 0;
    table.clear();
}
