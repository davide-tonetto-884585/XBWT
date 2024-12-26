#ifndef XBWT_HPP
#define XBWT_HPP

#include <memory>
#include <vector>
#include "../LabeledTree/LabeledTree.hpp"

template <typename T1, typename T2, typename T3>
struct Triplet
{
    T1 first;
    T2 second;
    T3 third;

    Triplet(T1 f, T2 s, T3 t) : first(f), second(s), third(t) {}

    T1& operator[](std::size_t index) {
        if (index == 0) return first;
        else if (index == 1) return second;
        else if (index == 2) return third;
        else throw std::out_of_range("Index out of range");
    }

    const T1& operator[](std::size_t index) const {
        if (index == 0) return first;
        else if (index == 1) return second;
        else if (index == 2) return third;
        else throw std::out_of_range("Index out of range");
    }
};

class XBWT
{
private:
    struct MyImpl;
    // std::unique_ptr<MyImpl> pImpl;

    void createXBWT(const LabeledTree<unsigned int> &tree);
    void pathSort(LabeledTree<unsigned int> &cTree, bool dummyRoot = false, bool firstIt = true, unsigned int rem = 0, unsigned int maxName = 0);
    std::vector<Triplet<unsigned int, int, int>> computeIntNodes(const Node<unsigned int> &root);

public:
    XBWT(const LabeledTree<unsigned int> &tree);
    // ~XBWT() = default;
};

#endif // XBWT_HPP
