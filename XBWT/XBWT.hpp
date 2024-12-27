#ifndef XBWT_HPP
#define XBWT_HPP

#include <memory>
#include <vector>
#include <string>
#include "../LabeledTree/LabeledTree.hpp"

template <typename T1, typename T2, typename T3>
struct Triplet
{
    T1 first;
    T2 second;
    T3 third;

    Triplet(T1 f, T2 s, T3 t) : first(f), second(s), third(t) {}
    Triplet(const Triplet &other) : first(other.first), second(other.second), third(other.third) {}

    std::string join(const std::string &sep) const
    {
        return std::to_string(first) + sep + std::to_string(second) + sep + std::to_string(third);
    }
};

class XBWT
{
private:
    struct MyImpl;
    // std::unique_ptr<MyImpl> pImpl;

    void createXBWT(const LabeledTree<unsigned int> &tree);
    void pathSort(const LabeledTree<unsigned int> &cTree, bool dummyRoot = false, bool firstIt = true, unsigned int rem = 0);
    LabeledTree<unsigned int> contractTree(std::vector<Triplet<unsigned int, int, int>> intNodes, short int j, std::vector<std::pair<unsigned int, Triplet<unsigned int, int, int>>> *tripletsSorted = nullptr);
    std::vector<Triplet<unsigned int, int, int>> computeIntNodes(const Node<unsigned int> &root);

    void radixSort(std::vector<std::pair<unsigned int, Triplet<unsigned int, int, int>>> &arr);
public:
    XBWT(const LabeledTree<unsigned int> &tree);
    // ~XBWT() = default;
};

#endif // XBWT_HPP
